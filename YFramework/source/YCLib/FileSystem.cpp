﻿/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r2406
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2015-06-26 17:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileSystem
#include YFM_YCLib_Debug
#include YFM_YCLib_NativeAPI
#include YFM_YCLib_Reference // for unique_ptr;
#include <cstring> // for std::strchr;
#include <fcntl.h>
#include <numeric> // for std::accumulate;
#include <ystdex/cstdint.hpp> // for ystdex::read_uint_le,
//	ystdex::write_uint_le;
#if YCL_DS
#	include YFM_CHRLib_CharacterProcessing

//! \since build 341
extern "C" int
_EXFUN(fileno, (FILE*));
//! \since build 566
extern "C" int
_EXFUN(pclose, (FILE*));
//! \since build 566
extern "C" FILE*
_EXFUN(popen, (const char*, const char*));

//! \since build 475
using namespace CHRLib;
#elif YCL_Win32
#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// At least one headers of <stdlib.h>, <stdio.h>, <Windows.h>, <Windef.h>
//	(and probably more) should have been included to make the MinGW-W64 macro
//	available if it is really being used.
//! \since build 464
//@{
extern "C" _CRTIMP int __cdecl __MINGW_NOTHROW
_fileno(::FILE*);

extern "C" _CRTIMP ::FILE* __cdecl __MINGW_NOTHROW
_wfopen(const wchar_t*, const wchar_t*);
//@}
#	endif
#	include YFM_MinGW32_YCLib_MinGW32 // for platform_ex::UTF8ToWCS,
//	platform_ex::ConvertTime;
#	include <time.h> // for ::localtime_s;

//! \since build 540
using platform_ex::UTF8ToWCS;
//! \since build 549
using platform_ex::DirectoryFindData;
#elif YCL_API_POSIXFileSystem
#	include YFM_CHRLib_CharacterProcessing
#	include <dirent.h>
#	include <sys/stat.h>
#	include <time.h> // for ::localtime_r;

//! \since build 475
using namespace CHRLib;
#endif

namespace platform
{

namespace
{

std::string
ensure_str(const char* s)
{
	return s;
}
std::string
ensure_str(const char16_t* s)
{
#if YCL_Win32
	return platform_ex::WCSToMBCS(reinterpret_cast<const wchar_t*>(s));
#elif YCL_API_POSIXFileSystem
	return MakeMBCS(s);
#else
#	error "Unsupported platform found."
#endif
}

//! \since build 544
template<typename _tChar>
std::chrono::nanoseconds
GetFileModificationTimeOfImpl(const _tChar* filename)
{
	if(const unique_ptr<int, FileDescriptorDeleter>
		fdw{platform::uopen(filename, O_RDONLY)})
		return GetFileModificationTimeOf(*fdw.get());
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file time of \"" + ensure_str(filename) + "\".");
}


//! \since build 560
template<typename _type>
NodeCategory
FetchNodeCategoryFromStat(_type& st)
{
	auto res(NodeCategory::Empty);
	const auto m(st.st_mode & S_IFMT);

	if(m & S_IFDIR)
		res |= NodeCategory::Directory;
#if !YCL_Win32
	if(m & S_IFLNK)
		res |= NodeCategory::Link;
#endif
	if(m & S_IFREG)
		res |= NodeCategory::Regular;
	if(YB_UNLIKELY(m & S_IFCHR))
		res |= NodeCategory::Character;
	else if(YB_UNLIKELY(m & S_IFCHR))
		res |= NodeCategory::Block;
	if(YB_UNLIKELY(m & S_IFIFO))
		res |= NodeCategory::FIFO;
#if !YCL_Win32
	if(m & S_IFSOCK)
		res |= NodeCategory::Socket;
#endif
	return res;;
}

} // unnamed namespace;


FileDescriptor::FileDescriptor(std::FILE* fp) ynothrow
#if YCL_Win32
	: desc(fp ? ::_fileno(fp) : -1)
#else
	: desc(fp ? fileno(fp) : -1)
#endif
{}

int
FileDescriptor::SetMode(int mode) const ynothrow
{
#if YCL_Win32
	return ::_setmode(desc, mode);
#elif defined(_NEWLIB_VERSION) && defined(__SCLE)
	// TODO: Other platforms.
	return ::setmode(desc, mode);
#else
	// NOTE: No effect.
	yunused(mode);
	return 0;
#endif
}


void
FileDescriptorDeleter::operator()(FileDescriptorDeleter::pointer p) ynothrow
{
	if(p)
		::close(*p);
}


void
SetBinaryIO(std::FILE* stream) ynothrow
{
#if YCL_Win32
	FileDescriptor(Nonnull(stream)).SetMode(_O_BINARY);
#else
	// NOTE: No effect.
	Nonnull(stream);
#endif
}


// XXX: Catch %std::bad_alloc?
#define YCL_Impl_RetTryCatchAll(...) \
	TryRet(__VA_ARGS__) \
	CatchIgnore(...)

int
uaccess(const char* path, int amode) ynothrow
{
	YAssertNonnull(path);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_waccess(UTF8ToWCS(path).c_str(), amode))
	return -1;
#else
	return ::access(path, amode);
#endif
}
int
uaccess(const char16_t* path, int amode) ynothrow
{
	YAssertNonnull(path);
#if YCL_Win32
	return ::_waccess(reinterpret_cast<const wchar_t*>(path), amode);
#else
	YCL_Impl_RetTryCatchAll(::access(MakeMBCS(path).c_str(), amode))
	return -1;
#endif
}

int
uopen(const char* filename, int oflag) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wopen(UTF8ToWCS(filename).c_str(), oflag))
	return -1;
#else
	return ::open(filename, oflag);
#endif
}
int
uopen(const char* filename, int oflag, int pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wopen(UTF8ToWCS(filename).c_str(), oflag, pmode))
	return -1;
#else
	return ::open(filename, oflag, pmode);
#endif
}
int
uopen(const char16_t* filename, int oflag) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag);
#else
	YCL_Impl_RetTryCatchAll(::open(MakeMBCS(filename).c_str(), oflag))
	return -1;
#endif
}
int
uopen(const char16_t* filename, int oflag, int pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag, pmode);
#else
	YCL_Impl_RetTryCatchAll(::open(MakeMBCS(filename).c_str(), oflag, pmode))
	return -1;
#endif
}

std::FILE*
ufopen(const char* filename, const char* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wfopen(UTF8ToWCS(filename).c_str(),
		UTF8ToWCS(mode).c_str()))
	return {};
#else
	return std::fopen(filename, mode);
#endif
}
std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	return ::_wfopen(reinterpret_cast<const wchar_t*>(filename),
		reinterpret_cast<const wchar_t*>(mode));
#else
	YCL_Impl_RetTryCatchAll(std::fopen(MakeMBCS(filename).c_str(),
		MakeMBCS(mode).c_str()))
	return {};
#endif
}

bool
ufexists(const char* filename) ynothrow
{
#if YCL_Win32
	YAssertNonnull(filename);
	if(const auto file = ufopen(filename, "rb"))
	{
		std::fclose(file);
		return true;
	}
	return {};
#else
	return ystdex::fexists(filename);
#endif
}
bool
ufexists(const char16_t* filename) ynothrow
{
	if(const auto file = ufopen(filename, u"rb"))
	{
		std::fclose(file);
		return true;
	}
	return {};
}

int
upclose(std::FILE* fp) ynothrow
{
	YAssertNonnull(fp);
#if YCL_Win32
	return ::_pclose(fp);
#else
	return ::pclose(fp);
#endif
}

std::FILE*
upopen(const char* filename, const char* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wpopen(UTF8ToWCS(filename).c_str(),
		UTF8ToWCS(mode).c_str()))
	return {};
#else
	return ::popen(filename, mode);
#endif
}
std::FILE*
upopen(const char16_t* filename, const char16_t* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	return ::_wpopen(reinterpret_cast<const wchar_t*>(filename),
		reinterpret_cast<const wchar_t*>(mode));
#else
	YCL_Impl_RetTryCatchAll(::popen(MakeMBCS(filename).c_str(),
		MakeMBCS(mode).c_str()))
	return {};
#endif
}

char16_t*
u16getcwd_n(char16_t* buf, size_t size) ynothrow
{
	if(YB_UNLIKELY(!buf || size == 0))
		errno = EINVAL;
	else
	{
		using namespace std;

#if YCL_Win32
		return reinterpret_cast<char16_t*>(
			::_wgetcwd(reinterpret_cast<wchar_t*>(buf), int(size)));
#else
		if(const auto cwd = ::getcwd(reinterpret_cast<char*>(buf), size))
			try
			{
				const auto res(MakeUCS2LE(cwd));
				const auto len(res.length());

				if(size < len + 1)
					errno = ERANGE;
				else
					return ystdex::ntctscpy(buf, res.data(), len);
			}
			CatchExpr(std::bad_alloc&, errno = ENOMEM)
#endif
	}
	return {};
}

//! \since build 476
#define YCL_Impl_FileSystem_ufunc_1(_n) \
bool \
_n(const char* path) ynothrow \
{ \
	YAssertNonnull(path); \

#if YCL_Win32
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	YCL_Impl_RetTryCatchAll(_wfn(UTF8ToWCS(path).c_str()) == 0) \
	return false; \
}
#else
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	return _fn(path) == 0; \
}
#endif

#define YCL_Impl_FileSystem_ufunc(_n, _fn, _wfn) \
	YCL_Impl_FileSystem_ufunc_1(_n) \
	YCL_Impl_FileSystem_ufunc_2(_fn, _wfn)

YCL_Impl_FileSystem_ufunc(uchdir, ::chdir, ::_wchdir)

YCL_Impl_FileSystem_ufunc_1(umkdir)
#if YCL_Win32
	YCL_Impl_FileSystem_ufunc_2(_unused_, ::_wmkdir)
#else
	return ::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0;
}
#endif

YCL_Impl_FileSystem_ufunc(urmdir, ::rmdir, ::_wrmdir)

YCL_Impl_FileSystem_ufunc(uunlink, ::unlink, ::_wunlink)

YCL_Impl_FileSystem_ufunc(uremove, std::remove, ::_wremove)

#undef YCL_Impl_FileSystem_ufunc_1
#undef YCL_Impl_FileSystem_ufunc_2
#undef YCL_Impl_FileSystem_ufunc

bool
truncate(std::FILE* fp, size_t size) ynothrow
{
#if YCL_Win32
	return ::_chsize(::_fileno(fp), long(size)) == 0;
#else
	return ::ftruncate(fileno(fp), ::off_t(size)) == 0;
#endif
}


std::chrono::nanoseconds
GetFileModificationTimeOf(int fd)
{
#if YCL_Win32
	// NOTE: The %::FILETIME has resolution of 100 nanoseconds.
	::FILETIME file_time;

	// XXX: Error handling for indirect calls.
	YCL_CallWin32(GetFileTime, "GetFileModificationTimeOf",
		::HANDLE(::_get_osfhandle(fd)), {}, {}, &file_time);
	TryRet(platform_ex::ConvertTime(file_time))
	CatchThrow(std::system_error& e, FileOperationFailure(e.code(), std::string(
		"Failed querying file modification time: ") + e.what() + "."))
#else
	// TODO: Get more precise time count.
	struct ::stat st;

	if(::fstat(fd, &st) == 0)
		return std::chrono::seconds(st.st_mtime);
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file size.");
#endif
}
std::chrono::nanoseconds
GetFileModificationTimeOf(std::FILE* fp)
{
	YAssertNonnull(fp);
#if YCL_Win32
	return GetFileModificationTimeOf(::_fileno(fp));
#else
	return GetFileModificationTimeOf(fileno(fp));
#endif
}
std::chrono::nanoseconds
GetFileModificationTimeOf(const char* filename)
{
	return GetFileModificationTimeOfImpl(filename);
}
std::chrono::nanoseconds
GetFileModificationTimeOf(const char16_t* filename)
{
	return GetFileModificationTimeOfImpl(filename);
}

std::uint64_t
GetFileSizeOf(int fd)
{
#if YCL_Win32
	::LARGE_INTEGER sz;

	// XXX: Error handling for indirect calls.
	if(::GetFileSizeEx(::HANDLE(::_get_osfhandle(fd)), &sz) != 0
		&& YB_LIKELY(sz.QuadPart >= 0))
		return std::uint64_t(sz.QuadPart);
	// TODO: Get correct error condition.
#else
	struct ::stat st;

	if(::fstat(fd, &st) == 0)
		// TODO: Use YSLib::CheckNonnegativeScalar<std::uint64_t>?
		// XXX: No negative file size should be found. See also:
		//	http://stackoverflow.com/questions/12275831/why-is-the-st-size-field-in-struct-stat-signed . 
		return std::uint64_t(st.st_size);
#endif
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file size.");
}
std::uint64_t
GetFileSizeOf(std::FILE* fp)
{
	YAssertNonnull(fp);
#if YCL_Win32
	return GetFileSizeOf(::_fileno(fp));
#else
	return GetFileSizeOf(fileno(fp));
#endif
}


ImplDeDtor(FileOperationFailure)


DirectorySession::DirectorySession(const char* path)
#if YCL_Win32
	: dir(new DirectoryFindData(path ? path : ""))
#else
	: sDirPath(path && *path != char() ? path : "."),
	dir(::opendir(sDirPath.c_str()))
#endif
{
	if(!dir)
		throw FileOperationFailure(errno, std::generic_category(),
			"Opening directory failed.");
#if !YCL_Win32
	ystdex::rtrim(sDirPath, YCL_PATH_DELIMITER);
	YAssert(sDirPath.empty() || sDirPath.back() != YCL_PATH_DELIMITER,
		"Invalid directory name state found.");
	sDirPath += YCL_PATH_DELIMITER;
#endif
}
DirectorySession::~DirectorySession()
{
#if !YCL_Win32
	const auto res(::closedir(dir));

	YAssert(res == 0, "No valid directory found.");
	yunused(res);
#else
	delete static_cast<DirectoryFindData*>(dir);
#endif
}

void
DirectorySession::Rewind() ynothrow
{
	YAssert(dir, "Invalid native handle found.");
#if YCL_Win32
	static_cast<DirectoryFindData*>(dir)->Rewind();
#else
	::rewinddir(dir);
#endif
}


HDirectory&
HDirectory::operator++()
{
	YAssert(!p_dirent || bool(GetNativeHandle()), "Invariant violation found.");
#if YCL_Win32
	p_dirent = static_cast<DirectoryFindData*>(GetNativeHandle())->Read();
#else
	p_dirent = ::readdir(GetNativeHandle());
#endif
	return *this;
}

NodeCategory
HDirectory::GetNodeCategory() const ynothrow
{
	if(p_dirent)
	{
		YAssert(bool(GetNativeHandle()), "Invariant violation found.");

		NodeCategory res(NodeCategory::Empty);
#if YCL_Win32
		const auto&
			dir_data(Deref(static_cast<DirectoryFindData*>(GetNativeHandle())));
		const auto& find_data(dir_data.GetFindData());

		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			res |= NodeCategory::Directory;
		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			switch(find_data.dwReserved0)
			{
			case IO_REPARSE_TAG_SYMLINK:
				res |= NodeCategory::SymbolicLink;
				break;
			case IO_REPARSE_TAG_MOUNT_POINT:
				res |= NodeCategory::MountPoint;
			default:
				// TODO: Implement for other tags?
				;
			}
		}

		struct ::_stat st;
		auto name(dir_data.GetDirName());

		YAssert(!name.empty() && name.back() == L'*', "Invalid state found.");
		name.pop_back();
		YAssert(!name.empty() && name.back() == L'\\', "Invalid state found.");
		if(::_wstat((name
			+ Deref(static_cast<wstring*>(p_dirent))).c_str(), &st) == 0)
			res |= FetchNodeCategoryFromStat(st);
#else
		auto name(sDirPath + Deref(p_dirent).d_name);
		struct ::stat st;

#	if !YCL_DS
		// TODO: Set error properly.
		if(::lstat(name.c_str(), &st) == 0)
			res |= FetchNodeCategoryFromStat(st);
#	endif
		if(bool(res & NodeCategory::Link) && ::stat(name.c_str(), &st) == 0)
			res |= FetchNodeCategoryFromStat(st);
#endif
		return res != NodeCategory::Empty ? res : NodeCategory::Invalid;
	}
	return NodeCategory::Empty;
}

const char*
HDirectory::GetName() const ynothrow
{
	if(p_dirent)
	{
#if !YCL_Win32
		return p_dirent->d_name;
#else
		YCL_Impl_RetTryCatchAll(utf8_name = platform_ex::WCSToUTF8(
			Deref(static_cast<wstring*>(p_dirent))), utf8_name.c_str())
#endif
	}
	return ".";
}

#undef YCL_Impl_RetTryCatchAll


bool
IsAbsolute(const char* path)
{
#if YCL_Win32
	return Deref(path) == YCL_PATH_DELIMITER
		|| (*path != char() && path[1] == ':');
#else
	if(Deref(path) == '/')
		return true;

	const auto p(std::strstr(path, ":/"));

	return p && p != path && !std::strstr(p, ":/");
#endif
}
bool
IsAbsolute(const char16_t* path)
{
#if YCL_Win32
	return Deref(path) == u'\\' || (*path != char16_t() && path[1] == u':');
#else
	if(Deref(path) == u'/')
		return true;

	const std::u16string upath(path);
	const auto n(upath.find(u":/"));

	// TODO: Optimize for performance.
	return n != std::u16string::npos && n != 0
		&& upath.substr(n).find(u":/") == std::u16string::npos;
#endif
}

size_t
GetRootNameLength(const char* path)
{
	const char* p(std::strchr(Nonnull(path), ':'));

	return !p ? 0 : size_t(p - path + 1);
}


namespace FAT
{

namespace LFN
{

tuple<string, string, bool>
ConvertToAlias(const u16string& long_name)
{
	YAssert(ystdex::ntctslen(long_name.c_str()) == long_name.length(),
		"Invalid long file name found.");

	string alias;
	// NOTE: Strip leading periods.
	size_t offset(long_name.find_first_not_of(u'.'));
	// NOTE: Set when the alias had to be modified to be valid.
	bool lossy(offset != 0);
	const auto check_char([&](string& str, char16_t uc){
		int bc(std::wctob(std::towupper(wint_t(uc))));

		if(!lossy && std::wctob(wint_t(uc)) != bc)
			lossy = true;
		if(bc == ' ')
		{
			// NOTE: Skip spaces in filename.
			lossy = true;
			return;
		}
		// TODO: Optimize.
		if(bc == EOF || string(IllegalAliasCharacters).find(char(bc))
			!= string::npos)
			// NOTE: Replace unconvertible or illegal characters with
			//	underscores. See Microsoft FAT specification Section 7.4.
			yunseq(bc = '_', lossy = true);
		str += char(bc);
	});
	const auto len(long_name.length());

	for(; alias.length() < MaxAliasMainPartLength && long_name[offset] != u'.'
		&& offset != len; ++offset)
		check_char(alias, long_name[offset]);
	if(!lossy && long_name[offset] != u'.' && long_name[offset] != char16_t())
		// NOTE: More than 8 characters in name.
		lossy = true;

	auto ext_pos(long_name.rfind(u'.'));
	string ext;

	if(!lossy && ext_pos != u16string::npos && long_name.rfind(u'.', ext_pos)
		!= u16string::npos)
		// NOTE: More than one period in name.
		lossy = true;
	if(ext_pos != u16string::npos && ext_pos + 1 != len)
	{
		++ext_pos;
		for(size_t ext_len(0); ext_len < LFN::MaxAliasExtensionLength
			&& ext_pos != len; yunseq(++ext_len, ++ext_pos))
			check_char(ext, long_name[ext_pos]);
		if(ext_pos != len)
			// NOTE: More than 3 characters in extension.
			lossy = true;
	}
	return make_tuple(std::move(alias), std::move(ext), lossy);
}

EntryDataUnit
GenerateAliasChecksum(const EntryDataUnit* p) ynothrowv
{
	static_assert(std::is_same<EntryDataUnit, unsigned char>::value,
		"Only unsigned char as byte is supported by checksum generation.");

	YAssertNonnull(p);
	// NOTE: The operation is an unsigned char rotate right.
	return std::accumulate(p, p + AliasEntryLength, 0,
		[](EntryDataUnit v, EntryDataUnit b){
			return ((v & 1) != 0 ? 0x80 : 0) + (v >> 1) + b;
		});
}

YF_API void
WriteNumericTail(string& alias, size_t k) ynothrowv
{
	YAssert(!(MaxAliasMainPartLength < alias.length()), "Invalid alias found.");

	auto p(&alias[MaxAliasMainPartLength - 1]);

	while(k > 0)
	{
		*p-- = '0' + k % 10;
		k /= 10;
	}
	*p = '~';
}

} // namespace LFN;

//! \since build 607
namespace
{

yconstexpr bool
is_time_no_leap_valid(const std::tm& t)
{
	return !(t.tm_hour < 0 || 23 < t.tm_hour || t.tm_hour < 0 || 59 < t.tm_min
		|| t.tm_sec < 0 || 59 < t.tm_min);
}
yconstexpr bool
is_date_range_valid(const std::tm& t)
{
	return !(t.tm_mon < 0 || 12 < t.tm_mon || t.tm_mday < 1 || 31 < t.tm_mday);
}

} // unnamed namespace;

std::time_t
ConvertFileTime(Timestamp d, Timestamp t) ynothrow
{
	struct std::tm time_parts;

	yunseq(
		time_parts.tm_hour = t >> 11,
		time_parts.tm_min = (t >> 5) & 0x3F,
		time_parts.tm_sec = (t & 0x1F) << 1,
		time_parts.tm_mday = d & 0x1F,
		time_parts.tm_mon = ((d >> 5) & 0x0F) - 1,
		time_parts.tm_year = (d >> 9) + 80,
		time_parts.tm_isdst = 0
	);
	return std::mktime(&time_parts);
}

pair<Timestamp, Timestamp>
FetchDateTime() ynothrow
{
	struct std::tm tmp;
	std::time_t epoch;

	if(std::time(&epoch) != std::time_t(-1))
	{
#if YCL_Win32
		// NOTE: The return type and parameter order differs than ISO C11
		//	library extension.
		::localtime_s(&tmp, &epoch);
#else
		::localtime_r(&epoch, &tmp);
		// FIXME: For platforms without %::(localtime_r, localtime_s).
#endif
		// NOTE: Microsoft FAT base year is 1980.
		return {is_date_range_valid(tmp) ? ((tmp.tm_year - 80) & 0x7F) << 9
			| ((tmp.tm_mon + 1) & 0xF) << 5 | (tmp.tm_mday & 0x1F) : 0,
			is_time_no_leap_valid(tmp) ? (tmp.tm_hour & 0x1F) << 11
			| (tmp.tm_min & 0x3F) << 5 | ((tmp.tm_sec >> 1) & 0x1F) : 0};
	}
	return {0, 0};
}


void
EntryData::CopyLFN(char16_t* str) const ynothrowv
{
	const auto pos(LFN::FetchLongNameOffset((*this)[LFN::Ordinal]));

	YAssertNonnull(str);
	for(size_t i(0); i < LFN::EntryLength; ++i)
		if(pos + i < LFN::MaxLength - 1)
			str[pos + i]
				= ystdex::read_uint_le<16>(data() + LFN::OffsetTable[i]);
}

bool
EntryData::FindAlias(const char* name, size_t len) const
{
	const auto alias(GenerateAlias());

	return ystdex::ntctsnicmp(name, alias.c_str(),
		std::min<size_t>(alias.length(), len)) == 0;
}

string
EntryData::GenerateAlias() const
{
	if((*this)[0] != Free)
	{
		if((*this)[0] == '.')
			return (*this)[1] == '.' ? ".." : ".";

		// NOTE: Copy the base filename.
		bool case_info(((*this)[CaseInfo] & LFN::CaseLowerBasename) != 0);
		const auto conv([&](size_t i){
			const auto c((*this)[i]);

			return char(case_info ? std::tolower(int(c)) : c);
		});
		string res;

		res.reserve(LFN::MaxAliasLength - 1);
		for(size_t i(0); i < LFN::MaxAliasMainPartLength
			&& (*this)[Name + i] != ' '; ++i)
			res += conv(Name + i);
		if((*this)[Extension] != ' ')
		{
			res += '.';
			case_info = ((*this)[CaseInfo] & LFN::CaseLowerExtension) != 0;
			for(size_t i(0); i < LFN::MaxAliasExtensionLength
				&& (*this)[Extension + i] != ' '; ++i)
				res += conv(Extension + i);
		}
		return res;
	}
	return {};
}

void
EntryData::SetupRoot(ClusterIndex root_cluster) ynothrow
{
	Clear();
	ClearAlias(),
	SetDot(Name),
	SetDirectoryAttribute();
	WriteCluster(root_cluster);
}

void
EntryData::WriteCluster(ClusterIndex c) ynothrow
{
	using ystdex::write_uint_le;

	write_uint_le<16>(data() + Cluster, c),
	write_uint_le<16>(data() + ClusterHigh, c >> 16);
}

void
EntryData::WriteAlias(const string& alias) ynothrow
{
	size_t i(0), j(0);

	for(; j < LFN::MaxAliasMainPartLength && alias[i] != '.'
		&& alias[i] != char(); yunseq(++i, ++j))
		(*this)[j] = EntryDataUnit(alias[i]);
	while(j < LFN::MaxAliasMainPartLength)
	{
		(*this)[j] = ' ';
		++j;
	}
	if(alias[i] == '.')
		for(++i; alias[i] != char() && j < LFN::AliasEntryLength;
			yunseq(++i, ++j))
			(*this)[j] = EntryDataUnit(alias[i]);
	for(; j < LFN::AliasEntryLength; ++j)
		(*this)[j] = ' ';
}

void
EntryData::WriteCDateTime() ynothrow
{
	using ystdex::write_uint_le;
	const auto& date_time(FetchDateTime());

	write_uint_le<16>(data() + CTime, date_time.second),
	write_uint_le<16>(data() + CDate, date_time.first);
}

void
EntryData::WriteDateTime() ynothrow
{
	using ystdex::write_uint_le;
	using ystdex::unseq_apply;
	const auto date_time(FetchDateTime());
	const auto dst(data());

	unseq_apply([&](size_t offset){
		write_uint_le<16>(dst + offset, date_time.first);
	}, CDate, MDate, ADate),
	unseq_apply([&](size_t offset){
		write_uint_le<16>(dst + offset, date_time.second);
	}, CTime, MTime);
}

} // namespace FAT;

} // namespace platform;

namespace platform_ex
{

#if !YCL_Win32
char16_t
FS_IsRoot(const char16_t* str)
{
	const std::u16string ustr(str);

	return ustr == u"/"
#	if YCL_DS
		|| ustr == u"fat:/" || ustr == u"sd:/"
#	endif
	;
}
#endif

} // namespace platform_ex;

