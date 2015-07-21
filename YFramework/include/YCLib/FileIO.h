﻿/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileIO.h
\ingroup YCLib
\brief 平台相关的文件访问和输入/输出接口。
\version r557
\author FrankHB <frankhb1989@gmail.com>
\since build 615
\par 创建时间:
	2015-07-14 18:50:35 +0800
\par 修改时间:
	2015-07-21 09:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#ifndef YCL_INC_FileIO_h_
#define YCL_INC_FileIO_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform
#include YFM_YBaseMacro
#include <ystdex/string.hpp> // for ynothrow, ystdex::enable_for_string_class_t,
//	std::uint64_t;
#include <cstdio> // for std::FILE;
#include <ios> // for std::ios_base::sync_with_stdio;
#include <fstream> // for std::filebuf;
#if __GLIBCXX__
#	include <ext/stdio_filebuf.h> // for __gnu_cxx::stdio_filebuf;
#	include <ystdex/cstdio.h> // for ystdex::unique_file_ptr;
#endif
#include <system_error> // for std::system_error;
#include <chrono> // for std::chrono::nanoseconds;

namespace platform
{

/*!
\brief 文件描述符包装类。
\note 满足 NullablePointer 要求。
\see ISO WG21/N4140 17.6.3.3[nullablepointer.requirements] 。
\since build 565
*/
class YF_API FileDescriptor
{
private:
	//! \since build 554
	int desc;

public:
	FileDescriptor() ynothrow
		: desc(-1)
	{}
	FileDescriptor(int fd) ynothrow
		: desc(fd)
	{}
	/*!
	\brief 构造：使用标准流。
	\note 对非空参数可能设置 \c errno 。

	当参数为空时得到无效文件空描述符，否则调用 POSIX \c fileno 函数。
	*/
	FileDescriptor(std::FILE*) ynothrow;
	FileDescriptor(std::nullptr_t) ynothrow
		: desc(-1)
	{}

	PDefHOp(int, *, )
		ImplRet(desc)

	explicit DefCvt(const ynothrow, bool, desc != -1)

	/*!
	\brief 设置模式。
	\note 参数和返回值意义和语义同 \c setmode 函数，在 Windows 以外的平台无作用。
	\since build 565
	*/
	int
	SetMode(int) const ynothrow;

	friend PDefHOp(bool, ==, const FileDescriptor& x, const FileDescriptor& y)
		ImplRet(x.desc == y.desc)
	friend PDefHOp(bool, !=, const FileDescriptor& x, const FileDescriptor& y)
		ImplRet(x.desc != y.desc)
};


/*!
\brief 文件描述符删除器。
\since build 565
*/
struct YF_API FileDescriptorDeleter
{
	using pointer = FileDescriptor;

	void
	operator()(pointer) ynothrow;
};


/*!
\brief 设置标准库流二进制输入/输出模式。
\pre 间接断言：参数非空。
\since build 599
*/
//@{
YF_API void
SetBinaryIO(std::FILE*) ynothrow;

inline PDefH(void, SetupBinaryStdIO, std::FILE* in = stdin,
	std::FILE* out = stdout, bool sync = {}) ynothrow
	ImplExpr(SetBinaryIO(in), SetBinaryIO(out),
		std::ios_base::sync_with_stdio(sync))
//@}

/*!
\brief 尝试关闭流：设置 \c error 后关闭参数指定的流，必要时重试。
\return 非 \c EINTR 的错误。
\note 使用 \c std::fclose 关闭流。
\since build 616
*/
YB_NONNULL(1) int
TryClose(std::FILE*) ynothrow;


/*!
\brief 测试路径可访问性。
\param path 路径，意义同 POSIX <tt>::open</tt> 。
\param amode 模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。 。
\pre 断言：\c filename 。
\note \c errno 在出错时会被设置，具体值由实现定义。
\since build 549
*/
//@{
YF_API int
uaccess(const char* path, int amode) ynothrow;
YF_API int
uaccess(const char16_t* path, int amode) ynothrow;
//@}

//! \since build 324
//@{
/*!
\param filename 文件名，意义同 POSIX <tt>::open</tt> 。
\param oflag 打开标识，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\pre 断言：\c filename 。
*/
//@{
//! \brief 以 UTF-8 文件名无缓冲打开文件。
//@{
YF_API int
uopen(const char* filename, int oflag) ynothrow;
YF_API int
uopen(const char* filename, int oflag, int pmode) ynothrow;
//@}
//! \brief 以 UCS-2 文件名无缓冲打开文件。
//@{
YF_API int
uopen(const char16_t* filename, int oflag) ynothrow;
//! \param pmode 打开模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
YF_API int
uopen(const char16_t* filename, int oflag, int pmode) ynothrow;
//@}

/*!
\param mode 打开模式，基本语义同 ISO C99 ，具体行为取决于实现。
\pre 断言：<tt>filename && mode && *mode != 0</tt> 。
*/
//@{
/*!
\brief 以 UTF-8 文件名打开文件。
\since build 299
*/
YF_API std::FILE*
ufopen(const char* filename, const char* mode) ynothrow;
//! \brief 以 UCS-2 文件名打开文件。
YF_API std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow;
//@}

/*!
\param mode 打开模式，基本语义与 ISO C++11 对应，具体行为取决于实现。
\pre 断言：<tt>filename</tt> 。
\since build 616
*/
//@{
//! \brief 以 UTF-8 文件名打开文件。
YF_API std::FILE*
ufopen(const char* filename, std::ios_base::openmode mode) ynothrow;
//! \brief 以 UCS-2 文件名打开文件。
YF_API std::FILE*
ufopen(const char16_t* filename, std::ios_base::openmode mode) ynothrow;
//@}
//@}

/*!
\note 使用 ufopen 二进制只读模式打开测试实现。
\pre 间接断言：参数非空。
*/
//@{
//! \brief 判断指定 UTF-8 文件名的文件是否存在。
YF_API YB_NONNULL(1) bool
ufexists(const char*) ynothrow;
//! \brief 判断指定 UCS-2 文件名的文件是否存在。
YF_API YB_NONNULL(1) bool
ufexists(const char16_t*) ynothrow;
//@}
/*!
\brief 判断指定字符串为文件名的文件是否存在。
\note 使用 NTCTS 参数 ufexists 实现。
*/
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline bool
ufexists(const _tString& str) ynothrow
{
	return platform::ufexists(str.c_str());
}

/*!
\brief 关闭管道流。
\param 基本语义同 POSIX.1 2004 的 <tt>::pclose</tt> ，具体行为取决于实现。
\pre 参数非空，表示通过和 upopen 或使用相同实现打开的管道流。
\since build 566
*/
YF_API YB_NONNULL(1) int
upclose(std::FILE*) ynothrow;

/*!
\param filename 文件名，意义同 POSIX <tt>::popen</tt> 。
\param mode 打开模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\pre 断言：\c filename 。
\pre 间接断言： \c mode 。
\warning 应使用 upclose 而不是 \c std::close 关闭管道流，否则行为可能未定义。
\since build 566
*/
//@{
//! \brief 以 UTF-8 文件名无缓冲打开管道流。
YF_API YB_NONNULL(1, 2) std::FILE*
upopen(const char* filename, const char* mode) ynothrow;
//! \brief 以 UCS-2 文件名无缓冲打开管道流。
YF_API YB_NONNULL(1, 2) std::FILE*
upopen(const char16_t* filename, const char16_t* mode) ynothrow;
//@}

/*!
\brief 取当前工作目录（ UCS-2 编码）复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\note 当 <tt>!buf || size == 0</tt> 时失败，设置 \c errno 为 \c EINVAL 。
\note 指定的 size 不能容纳结果时失败，设置 \c errno 为 \c ERANGE 。
\note 若分配存储失败，设置 \c errno 为 \c ENOMEM 。
\since build 324
*/
YF_API YB_NONNULL(1) char16_t*
u16getcwd_n(char16_t* buf, size_t size) ynothrow;

/*
\pre 断言：参数非空。
\return 操作是否成功。
\note \c errno 在出错时会被设置，具体值由实现定义。
\note DS 使用 newlib 实现。 MinGW32 使用 MSVCRT 实现。 Android 使用 bionic 实现。
	其它 Linux 使用 GLibC 实现。
*/
//@{
/*!
\brief 切换当前工作路径至指定的 UTF-8 字符串。
\since build 476
*/
YF_API YB_NONNULL(1) bool
uchdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径以默认权限新建一个目录。
\note 权限由实现定义： DS 使用最大权限； MinGW32 使用 _wmkdir 指定的默认权限。
\since build 475
*/
YF_API YB_NONNULL(1) bool
umkdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个空目录。
\since build 475
*/
YF_API YB_NONNULL(1) bool
urmdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个非目录文件。
\since build 476
*/
YF_API YB_NONNULL(1) bool
uunlink(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个文件。
\since build 476
*/
YF_API YB_NONNULL(1) bool
uremove(const char*) ynothrow;

/*!
\brief 截断文件至指定长度。
\pre 指定文件需已经打开并可写。
\note 不改变文件读写位置。
\since build 341

若文件不足指定长度，扩展并使用空字节填充；否则保留起始指定长度的字节。
*/
YF_API YB_NONNULL(1) bool
truncate(std::FILE*, size_t) ynothrow;
//@}


//! \since build 616
//@{
#if __GLIBCXX__
template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_filebuf : public yimpl(__gnu_cxx::stdio_filebuf<_tChar, _tTraits>)
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	using traits = _tTraits;

private:
	//! \invairant <tt>bool(uptr) == this->is_open()</tt> 。
	ystdex::unique_file_ptr uptr{{}, TryClose};

public:
	using yimpl(__gnu_cxx::stdio_filebuf<_tChar, _tTraits>::stdio_filebuf);

	template<typename _tPathChar>
	std::basic_filebuf<_tChar, _tTraits>*
	open(const _tPathChar* s, std::ios_base::openmode mode)
	{
		if(!this->is_open())
		{
			yassume(!uptr);
			uptr.reset(ufopen(s, mode));
			this->_M_file.sys_open(uptr.get(), mode);
			if(this->is_open())
			{
				this->_M_allocate_internal_buffer();
				this->_M_mode = mode;
				yunseq(this->_M_reading = {}, this->_M_writing = {});
				this->_M_set_buffer(-1);
				yunseq(this->_M_state_cur = this->_M_state_beg,
					this->_M_state_last = this->_M_state_beg);
				if((mode & std::ios_base::ate) && this->seekoff(0,
					std::ios_base::end) == pos_type(off_type(-1)))
					close();
				else
					return this;
			}
		}
		return {};
	}

	std::basic_filebuf<_tChar, _tTraits>*
	close()
	{
		if(std::basic_filebuf<_tChar, _tTraits>::close())
		{
			uptr.reset();
			return this;
		}
		return {};
	}
};


//extern template class YF_API basic_filebuf<char>;
//extern template class YF_API basic_filebuf<wchar_t>;

using filebuf = basic_filebuf<char>;
using wfilebuf = basic_filebuf<wchar_t>;


template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_fstream : public std::basic_iostream<_tChar, _tTraits>
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	using traits_type = _tTraits;

private:
	using base_type = std::basic_iostream<char_type, traits_type>;

	basic_filebuf<_tChar, _tTraits> fbuf{};

public:
	basic_fstream()
		: base_type({})
	{
		this->init(&fbuf);
	}
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_ctor_t<basic_fstream, _tParam>)>
	explicit
	basic_fstream(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		: base_type({}),
		fbuf(platform::ufopen(s, mode), mode)
	{
		this->init(&fbuf);
		this->open(yforward(s), mode);
	}
	DefDelCopyCtor(basic_fstream)
#	if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 50000 && __GLIBCXX__ > 20140922
	basic_fstream(basic_fstream&& rhs)
		: base_type(std::move(rhs)),
		fbuf(std::move(rhs.fbuf))
	{
		base_type::set_rdbuf(&fbuf);
	}
#	endif
	DefDeDtor(basic_fstream)

	DefDelCopyAssignment(basic_fstream)
#	if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 50000 && __GLIBCXX__ > 20140922
	basic_fstream&
	operator=(basic_fstream&& rhs)
	{
		base_type::operator=(std::move(rhs));
		fbuf = std::move(rhs.fbuf);
		return *this;
	}

	void
	swap(basic_fstream& rhs)
	{
		base_type::swap(rhs),
		fbuf.swap(rhs.fbuf);
	}
#	endif

	std::basic_filebuf<_tChar, _tTraits>*
	rdbuf() const
	{
		return const_cast<std::basic_filebuf<_tChar, _tTraits>*>(static_cast<
			const std::basic_filebuf<_tChar, _tTraits>*>(&fbuf));
	}

	bool
	is_open() const
	{
		return fbuf.is_open();
	}

	void
	open(const char* s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
	{
		if (!fbuf.open(s, mode))
			this->setstate(std::ios_base::failbit);
		else
			this->clear();
	}
	void
	open(const std::string& s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
	{
		if (!fbuf.open(s, mode))
			this->setstate(std::ios_base::failbit);
		else
			this->clear();
	}

	void
	close()
	{
		if (!fbuf.close())
			this->setstate(std::ios_base::failbit);
	}
};

#	if __GLIBCXX__ > 20140922
template<typename _tChar, class _tTraits>
inline DefSwap(, basic_fstream<_tChar YPP_Comma _tTraits>)
#	endif


//extern template class YF_API basic_fstream<char>;
//extern template class YF_API basic_fstream<wchar_t>;

using fstream = basic_fstream<char>;
using wfstream = basic_fstream<wchar_t>;
#else
// TODO: Use VC++ extensions to support %char16_t path initialization.
using std::basic_filebuf;
using std::filebuf;
using std::wfilebuf;
using std::basic_fstream;
using std::fstream;
using std::wfstream;
#endif
//@}


/*!
\brief 表示文件操作失败的异常。
\since build 411
*/
class YF_API FileOperationFailure : public std::system_error
{
public:
	//! \since build 538
	using system_error::system_error;

	//! \since build 586
	DefDeCopyCtor(FileOperationFailure)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~FileOperationFailure() override;
};


/*!
\brief 取文件的修改时间。
\return 以 POSIX 时间相同历元的时间间隔。
\throw FileOperationFailure 参数无效或文件修改时间查询失败。
\note 当前 Windows 使用 \c ::GetFileTime 实现，其它只保证最高精确到秒。
\since build 544
*/
//@{
YF_API std::chrono::nanoseconds
GetFileModificationTimeOf(int);
//! \pre 断言：参数非空。
//@{
YF_API YB_NONNULL(1) std::chrono::nanoseconds
GetFileModificationTimeOf(std::FILE*);
YF_API YB_NONNULL(1) std::chrono::nanoseconds
GetFileModificationTimeOf(const char*);
YF_API YB_NONNULL(1) std::chrono::nanoseconds
GetFileModificationTimeOf(const char16_t*);
//@}
//! \note 使用 NTCTS 参数 GetFileModificationTimeOf 实现。
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline std::chrono::nanoseconds
GetFileModificationTimeOf(const _tString& str)
{
	return platform::GetFileModificationTimeOf(str.c_str());
}
//@}

/*!
\brief 取文件的大小。
\return 以字节计算的文件大小。
\throw FileOperationFailure 参数无效或文件大小查询失败。
\since build 475
*/
//@{
YF_API std::uint64_t
GetFileSizeOf(int);
//! \pre 断言：参数非空。
YF_API YB_NONNULL(1) std::uint64_t
GetFileSizeOf(std::FILE*);
//@}

} // namespace platform;

#endif

