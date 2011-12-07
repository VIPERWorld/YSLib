﻿/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Adaptor Adaptor
\ingroup YSLib
\brief YSLib 适配器模块。
*/

/*!	\file yadaptor.h
\ingroup Adaptor
\brief 外部库关联。
\version r2268;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-02-22 20:16:21 +0800;
\par 修改时间:
	2011-12-01 08:51 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YAdaptor;
*/


#ifndef YSL_INC_ADAPTOR_YADAPTOR_H_
#define YSL_INC_ADAPTOR_YADAPTOR_H_

//引入平台设置和存储调试设施。
#include "ynew.h"

//包含 YStandardEx 公用部分。
#include <ystdex/utility.hpp>

//包含 YCLib 公用部分。
#include <YCLib/ycommon.h>

//包含 CHRLib 。
#include <CHRLib/chrproc.h>

// 确保包含 C 标准库必要部分。
#include <cstddef>
#include <cstdint>
#include <climits>
#include <cmath>

// 确保包含 C++ 标准库必要部分。
#include <algorithm> // for std::min, std::max;

/*
!\brief YSLib 命名空间。
\since 早于 build 132 。
*/
YSL_BEGIN

	//目标平台中立接口。

	//通用数据类型。
	typedef std::uint8_t	u8;
	typedef std::uint16_t	u16;
	typedef std::uint32_t	u32;
	typedef std::uint64_t	u64;
	typedef std::int8_t		s8;
	typedef std::int16_t	s16;
	typedef std::int32_t	s32;
	typedef std::int64_t	s64;
	typedef volatile u8		vu8;
	typedef volatile u16	vu16;
	typedef volatile u32	vu32;
	typedef volatile u64	vu64;
	typedef volatile s8		vs8;
	typedef volatile s16	vs16;
	typedef volatile s32	vs32;
	typedef volatile s64	vs64;

	//实用类型。
	using ystdex::noncopyable;
	using ystdex::nullptr_t;

	//算法。
	using std::min;
	using std::max;


	//非目标平台中立接口。

	//特殊数据类型。
	using ystdex::errno_t;
	using ystdex::ptrdiff_t;
	using ystdex::size_t;
	using ystdex::ssize_t;
	using ystdex::wint_t;

	//基本实用例程。
	using platform::mmbset;
	using platform::mmbcpy;
	using platform::direxists;
	using platform::chdir;
	using platform::getcwd_n;

	//系统处理函数。
	using platform::terminate;

	//基本图形定义。
	using platform::SPos;
	using platform::SDst;

	//文件系统抽象。
	using platform::const_path_t;
	using platform::path_t;

	using platform::HDirectory;
	using platform::IsAbsolute;
	using platform::GetRootNameLength;

	//文件系统常量。
	using platform::DEF_PATH_DELIMITER;
	using platform::DEF_PATH_SEPERATOR;
	using platform::PATHSTR;
	using platform::FILENAMESTR;

	//基本输入输出接口。
	using platform::InitVideo;
	using platform::ScreenSynchronize;
	using platform::WaitForInput;

	//时钟。
	using platform::GetRTC;
	using platform::ResetRTC;

	//输入类型。
	namespace KeySpace = platform::KeySpace;
	using platform::KeyCode;
	using platform::KeysInfo;
	using platform::CursorInfo;

//! \brief 设计模式。
YSL_BEGIN_NAMESPACE(Pattern)
YSL_END_NAMESPACE(Pattern)

//! \brief 图形处理。
YSL_BEGIN_NAMESPACE(Drawing)

	using platform::PixelType;
	using platform::BitmapPtr;
	using platform::ConstBitmapPtr;
	using platform::ScreenBufferType;
	using platform::Color;
	namespace ColorSpace = platform::ColorSpace;

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

	using namespace CHRLib;

YSL_END_NAMESPACE(Text)

	using Text::ucs4_t;
	using Text::ucs2_t;
	using Text::ucsint_t;

	//! \brief 运行时平台。
	namespace DS
	{
		using namespace platform_ex;
	}

YSL_END

#endif
