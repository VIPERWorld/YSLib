﻿/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydevice.h
\ingroup Core
\brief 平台无关的设备抽象层。
\version r2954;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-28 16:39:39 +0800;
\par 修改时间:
	2012-06-23 02:08 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YDevice;
*/


#ifndef YSL_INC_CORE_YDEVICE_H_
#define YSL_INC_CORE_YDEVICE_H_

#include "yobject.h"
#include "ygdibase.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Devices)

//图形设备。
class GraphicDevice : private noncopyable, protected Drawing::Graphics
{
public:
	/*!
	\brief 构造：指定宽度和高度，从指定缓冲区指针。
	\since build 319 。
	*/
	inline
	GraphicDevice(SDst w, SDst h, Drawing::BitmapPtr p = nullptr) ynothrow
		: Graphics(p, Drawing::Size(w, h))
	{}
	/*!
	\brief 析构：默认实现。
	\since build 297 。
	*/
	virtual DefEmptyDtor(GraphicDevice)

	//! \since build 319 。
	//@{
	using Graphics::operator!;

	using Graphics::operator bool;
	//@}

	//! \since build 296 。
	//@{
	using Graphics::GetBufferPtr;
	using Graphics::GetHeight;
	using Graphics::GetWidth;
	using Graphics::GetSize;
	using Graphics::GetSizeOfBuffer;
	DefGetter(const ynothrow, const Graphics&, Context, *this)
	//@}
};


//屏幕。
class Screen : public GraphicDevice
{
public:
	/*!
	\brief 构造：指定宽度和高度，从指定缓冲区指针。
	\since build 319 。
	*/
	Screen(SDst w, SDst h, Drawing::BitmapPtr p = nullptr) ynothrow
		: GraphicDevice(w, h, p)
	{}

	/*!
	\brief 取指针。
	\note 进行状态检查。
	*/
	virtual Drawing::BitmapPtr
	GetCheckedBufferPtr() const ynothrow;

	/*!
	\brief 更新。
	\note 复制到屏幕。
	\since build 319 。
	*/
	virtual void
	Update(Drawing::BitmapPtr) ynothrow;
};

YSL_END_NAMESPACE(Devices)

YSL_END

#endif

