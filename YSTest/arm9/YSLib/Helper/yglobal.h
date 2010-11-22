﻿/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Helper Helper
\ingroup YSLib
\brief YSLib 助手模块。
*/

/*!	\file yglobal.h
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version 0.1844;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:14:57 + 08:00;
\par 修改时间:
	2010-11-15 19:06 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#ifndef INCLUDED_YGLOBAL_H_
#define INCLUDED_YGLOBAL_H_

#include "../Core/ysmsg.h"
#include "../Core/yexcept.h"

YSL_BEGIN

//平台相关的全局变量。
extern YScreen *pScreenUp, *pScreenDown;
extern YDesktop *pDesktopUp, *pDesktopDown;


/*!
\brief 默认消息发生函数。
*/
void
Idle();

/*!
\brief 以指定前景色和背景色初始化指定屏幕的控制台。
*/
bool
InitConsole(YScreen&, Drawing::PixelType, Drawing::PixelType);

/*!
\brief 全局资源释放函数。
*/
void
Destroy(YObject&, EventArgs&);

/*!
\brief 公共消息处理函数。
*/
LRES
ShlProc(HSHL, const Message&);


//全局函数。

/*!
\brief 全局 Shell 消息处理函数。
*/
LRES
MainShlProc(const Message& msg);

YSL_END

#endif

