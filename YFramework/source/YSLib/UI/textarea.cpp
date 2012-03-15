﻿/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textarea.cpp
\ingroup UI
\brief 样式无关的用户界面文本区域部件。
\version r1396;
\author FrankHB<frankhb1989@gmail.com>
\since build 222 。
\par 创建时间:
	2011-06-30 20:10:27 +0800;
\par 修改时间:
	2012-03-13 13:13 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::TextArea;
*/


#include "YSLib/UI/textarea.h"
#include "YSLib/UI/ywgtevt.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

TextArea::TextArea(const Rect& r, FontCache& fc)
	: Widget(r), TextState(fc), TextRenderer(*this),
	Rotation(Drawing::RDeg0)
{}


BufferedTextArea::BufferedTextArea(const Rect& r, FontCache& fc)
	: Widget(r), TextRegion(fc),
	Rotation(Drawing::RDeg0)
{
	//初始化视图。
	TextRegion::SetSize(GetWidth(), GetHeight());
}

Rect
BufferedTextArea::Refresh(const PaintContext& pc)
{
	Widget::Refresh(pc);

	const auto& g(pc.Target);
	const auto& pt(pc.Location);
	const auto& r(pc.ClipArea);

	BlitTo(g.GetBufferPtr(), *this, g.GetSize(),
		r, static_cast<const Point&>(r) - pt, r, Rotation);
	return Rect(pc.Location, GetSizeOf(*this));
}

YSL_END_NAMESPACE(Components)

YSL_END

