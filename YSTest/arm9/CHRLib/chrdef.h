﻿/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrdef.h
\ingroup CHRLib CHRLib 库
\brief CHRLib 类型定义。
\version 0.1562;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-17 17:52:35 + 08:00;
\par 修改时间:
	2010-12-02 20:07 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::CHRDefinition;
*/


#ifndef INCLUDED_CHRDEF_H_
#define INCLUDED_CHRDEF_H_

#include <platform.h>

#define CHRLIB_BEGIN	namespace CHRLib {
#define CHRLIB_END		}
#define CHRLIB_			::CHRLib::
#define CHRLIB			::CHRLib

#define CHRLIB_BEGIN_NAMESPACE(s)	namespace s {
#define CHRLIB_END_NAMESPACE(s)	}

CHRLIB_BEGIN

typedef unsigned char ubyte_t;
typedef unsigned long usize_t;

// 字符类型定义。
typedef std::char32_t fchar_t; //!< UCS-4 字符类型。
typedef std::char16_t uchar_t; //!< UCS-2 字符类型。
typedef s32 uint_t;

/*!
\brief 宽字符串转换宏。
*/
#define FS(str) reinterpret_cast<const CHRLib::fchar_t*>(L##str)

CHRLIB_END

#endif

