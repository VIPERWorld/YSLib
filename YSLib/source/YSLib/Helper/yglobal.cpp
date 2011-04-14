﻿/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yglobal.cpp
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version 0.2956;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:28:52 +0800;
\par 修改时间:
	2011-04-13 11:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#include "yglobal.h"
#include "../Core/yfilesys.h"
#include "../Core/yapp.h"
#include "../Core/yshell.h"
#include "../Core/ydevice.h"
#include "../Adaptor/yfont.h"
#include "../Adaptor/ysinit.h"
#include "../Shell/ydesktop.h"
//#include <clocale>

YSL_BEGIN

using namespace Runtime;

//全局常量。
extern const char* DEF_DIRECTORY; //<! 默认目录。
extern const char* G_COMP_NAME; //<! 制作组织名称。
extern const char* G_APP_NAME; //!< 产品名称。
extern const char* G_APP_VER; //!< 产品版本。
const IO::Path YApplication::CommonAppDataPath(DEF_DIRECTORY);
const String YApplication::CompanyName(G_COMP_NAME);
const String YApplication::ProductName(G_APP_NAME);
const String YApplication::ProductVersion(G_APP_VER);

//全局变量。
#ifdef YSL_USE_MEMORY_DEBUG
MemoryList DebugMemory(NULL);
#endif

/*!
\ingroup PublicObject
\brief 全局变量映射。
\note 需要保证 YApplication::DefaultShellHandle 在 theApp 初始化之后初始化，
	因为 YMainShell 的基类 YShell 的构造函数调用了 theApp 的非静态成员函数。
*/
//@{
YApplication& theApp(YApplication::GetInstance());
const GHandle<YShell> YApplication::DefaultShellHandle(new YMainShell());
//@}


const SDst Global::MainScreenWidth(SCREEN_WIDTH);
const SDst Global::MainScreenHeight(SCREEN_HEIGHT);

Global::Global()
	: hScreenUp(NULL), hScreenDown(NULL), hDesktopUp(NULL), hDesktopDown(NULL)
{}
Global::~Global()
{}

YScreen&
Global::GetScreenUp() const ythrow()
{
	YAssert(hScreenUp, "Fatal error @ Global::GetScreenUp:"
		" the up screen handle is null.");

	return *hScreenUp;
}
YScreen&
Global::GetScreenDown() const ythrow()
{
	YAssert(hScreenDown, "Fatal error @ Global::GetScreenDown:"
		" the down screen handle is null.");

	return *hScreenDown;
}
YDesktop&
Global::GetDesktopUp() const ythrow()
{
	YAssert(hDesktopUp, "Fatal error @ Global::GetDesktopUp:"
		" the up desktop handle is null.");

	return *hDesktopUp;
}
YDesktop&
Global::GetDesktopDown() const ythrow()
{
	YAssert(hDesktopDown, "Fatal error @ Global::GetDesktopDown:"
		" the down desktop handle is null.");

	return *hDesktopDown;
}

void
Global::InitializeDevices() ythrow()
{
	//初始化显示设备。
	try
	{
		hScreenUp = new YScreen(MainScreenWidth, MainScreenHeight);
		hScreenDown = new YScreen(MainScreenWidth, MainScreenHeight);
	}
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}
	try
	{
		hDesktopUp = new YDesktop(*hScreenUp);
		hDesktopDown = new YDesktop(*hScreenDown);
	}
	catch(...)
	{
		throw LoggedEvent("Desktop initialization failed.");
	}
}

void
Global::ReleaseDevices() ythrow()
{
	YReset(hDesktopUp);
	YReset(hScreenUp);
	YReset(hDesktopDown);
	YReset(hScreenDown);
}


namespace
{
	bool
	operator==(const KeysInfo& a, const KeysInfo& b)
	{
		return a.Up == b.Up && a.Down == b.Down && a.Held == b.Held;
	}

	inline bool
	operator!=(const KeysInfo& a, const KeysInfo& b)
	{
		return !(a == b);
	}
}

YSL_BEGIN_NAMESPACE(Messaging)

bool
InputContext::operator==(const IContext& rhs) const
{
	const InputContext* p(dynamic_cast<const InputContext*>(&rhs));

	return Key == p->Key && CursorLocation == p->CursorLocation;
}

YSL_END_NAMESPACE(Messaging)

namespace
{
	/*!
	\note 转换指针设备光标位置为屏幕点。
	*/
	inline Drawing::Point
	ToSPoint(const Runtime::CursorInfo& c)
	{
		return Drawing::Point(c.GetX(), c.GetY());
	}

	//图形用户界面输入等待函数。
	void
	WaitForGUIInput()
	{
		using namespace Messaging;

		static KeysInfo Key;
		static CursorInfo TouchPos_Old, TouchPos;
		static SmartPtr<InputContext> pContext;

		if(Key.Held & KeySpace::Touch)
			TouchPos_Old = TouchPos;
		scanKeys();
		WriteKeysInfo(Key, TouchPos);

		const Point pt(ToSPoint(Key.Held & KeySpace::Touch
			? TouchPos : TouchPos_Old));

		if(!pContext || ((theApp.GetDefaultMessageQueue().IsEmpty()
			|| Key != pContext->Key || pt != pContext->CursorLocation)
			&& pt != Point::FullScreen))
		{
			pContext = new InputContext(Key, pt);
			SendMessage(Message(Shells::GetCurrentShellHandle(), SM_INPUT, 0x40,
				pContext));
		}
	}
}


void
Idle()
{
	WaitForGUIInput();
}

bool
InitConsole(YScreen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	using namespace platform;

	if(&scr == theApp.GetPlatformResource().GetScreenUpHandle())
		YConsoleInit(true, fc, bc);
	else if(&scr == theApp.GetPlatformResource().GetScreenDownHandle())
		YConsoleInit(false, fc, bc);
	else
		return false;
	return true;
}

void
Destroy_Static(YObject&, EventArgs&)
{
}

int
ShlProc(GHandle<YShell> hShl, const Message& msg)
{
	return hShl->ShlProc(msg);
}

//非 yglobal.h 声明的平台相关函数。

bool
InitAllScreens()
{
	using namespace Runtime;

	InitVideo();

	YScreen& up_scr(theApp.GetPlatformResource().GetScreenUp());

	up_scr.pBuffer = DS::InitScrUp(up_scr.bg);

	YScreen& down_scr(theApp.GetPlatformResource().GetScreenDown());

	down_scr.pBuffer = DS::InitScrDown(down_scr.bg);
	return true;
}


namespace
{
	//! \brief 全局非静态资源释放函数。
	void
	YDestroy()
	{
		//释放默认字体资源。
		theApp.DestroyFontCache();

		//释放设备。
		theApp.GetPlatformResource().ReleaseDevices();		
	}

	//! \brief 全局初始化函数。
	void
	YInit()
	{
		//设置默认异常终止函数。
		std::set_terminate(terminate);

		//启用设备。
		powerOn(POWER_ALL);

		//启用 LibNDS 默认异常处理。
		defaultExceptionHandler();

		//初始化主控制台。
		InitYSConsole();

	/*	if(!setlocale(LC_ALL, "zh_CN.GBK"))
		{
			EpicFail();
			platform::yprintf("setlocale() with %s failed.\n", "zh_CN.GBK");
			terminate();
		}*/

		//初始化文件系统。
		//初始化 EFSLib 和 LibFAT 。
		//当 .nds 文件大于32MB时， EFS 行为异常。
	#ifdef USE_EFS
		if(!EFS_Init(EFS_AND_FAT | EFS_DEFAULT_DEVICE, NULL))
		{
			//如果初始化 EFS 失败则初始化 FAT 。
	#endif
			if(!fatInitDefault())
				LibfatFail();
			IO::ChangeDirectory(Text::StringToMBCS(
				YApplication::CommonAppDataPath));
	#ifdef USE_EFS
		}
	#endif

		//检查程序是否被正确安装。
		CheckInstall();

		//初始化系统字体资源。
		InitializeSystemFontCache();

		//初始化系统设备。
		theApp.GetPlatformResource().InitializeDevices();

		//注册全局应用程序对象。
		theApp.ResetShellHandle();
		//theApp.SetOutputPtr(hDesktopUp);
		//DefaultShellHandle->SetShlProc(ShlProc);
	}
}


/*!
\brief Shell 对象释放函数。
*/
extern void
ReleaseShells();


void
ShowFatalError(const char* s)
{
	using namespace platform;

	YDebugSetStatus();
	YDebugBegin();
	std::printf("Fatal Error:\n%s\n", s);
	terminate();
}

#ifdef YSL_USE_MEMORY_DEBUG

namespace
{
	void
	OnExit_DebugMemory_continue()
	{
		std::fflush(stderr);
		std::puts("Input to continue...");
		WaitForInput();
	}
}

void
OnExit_DebugMemory()
{
	using namespace platform;

	YDebugSetStatus();
	YDebugBegin();
	std::puts("Normal exit;");

//	std::FILE* fp(std::freopen("memdbg.log", "w", stderr));
	const typename MemoryList::MapType& Map(DebugMemory.Blocks);
//	MemoryList::MapType::size_type s(DebugMemory.GetSize());

	if(!Map.empty())
	{
		std::fprintf(stderr, "%i memory leak(s) detected:\n", Map.size());

		MemoryList::MapType::size_type n(0);

		for(typename MemoryList::MapType::const_iterator i(Map.begin());
			i != Map.end(); ++i)
		{
			if(n++ < 4)
				DebugMemory.Print(i, stderr);
			else
			{
				n = 0;
				OnExit_DebugMemory_continue();
			}
		}
	//	DebugMemory.PrintAll(stderr);
	//	DebugMemory.PrintAll(fp);
		OnExit_DebugMemory_continue();
	}

	const typename MemoryList::ListType&
		List(DebugMemory.DuplicateDeletedBlocks);

	if(!List.empty())
	{
		std::fprintf(stderr, "%i duplicate memory deleting(s) detected:\n",
			List.size());

		MemoryList::ListType::size_type n(0);

		for(typename MemoryList::ListType::const_iterator i(List.begin());
			i != List.end(); ++i)
		{
			if(n++ < 4)
				DebugMemory.Print(i, stderr);
			else
			{
				n = 0;
				OnExit_DebugMemory_continue();
			}
		}
	//	DebugMemory.PrintAllDuplicate(stderr);
	//	DebugMemory.PrintAllDuplicate(fp);
	}
//	std::fclose(fp);
	std::puts("Input to terminate...");
	WaitForInput();
}

#endif

YSL_END

/*!
\brief 程序默认入口函数。
*/
extern int
YMain(int argc, char* argv[]);

int
main(int argc, char* argv[])
{
	try
	{
		//全局初始化。
		YSL_ YInit();

		int r(YMain(argc, argv));

		YSL_ ReleaseShells();
		YSL_ YDestroy();
	#ifdef YSL_USE_MEMORY_DEBUG
		YSL_ OnExit_DebugMemory();
	#endif
		return r;
	}
	catch(std::exception& e)
	{
		YSL_ theApp.Log.FatalError(e.what());
	}
	catch(...)
	{
		YSL_ theApp.Log.FatalError("Unhandled exception"
			" @ int main(int, char*[]);");
	}
}

