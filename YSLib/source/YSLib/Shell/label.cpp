﻿/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file label.cpp
\ingroup Shell
\brief 样式无关的标签模块实现。
\version 0.1909;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 08:32:34 +0800;
\par 修改时间:
	2011-04-16 21:04 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::Label;
*/


#include "label.h"
#include "yuicont.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

MLabel::MLabel(const Drawing::Font& f, MLabel::TextAlignmentStyle a)
	: Font(f), Margin(2, 2, 2, 2),
	Alignment(a), /*AutoSize(false), AutoEllipsis(false),*/ Text()
{}

void
MLabel::PaintText(Widget& w, const Graphics& g, const Point& pt)
{
	Drawing::TextState ts;
	const Rect& r(GetBoundsOf(w));

	ts.Font.SetFont(Font);
	ts.ResetForBounds(r, g.GetSize(), Margin);
	ts.Color = w.ForeColor;

	switch(Alignment)
	{
	case Center:
	case Right:
		{
			const SDst string_width(FetchStringWidth(ts.Font, Text)),
				area_width(w.GetWidth() - GetHorizontalFrom(Margin));
			if(area_width > string_width)
			{
				const SDst horizontal_offset(area_width - string_width);

				ts.ResetForBounds(r, g.GetSize(), Margin);
				ts.PenX += Alignment == Center ? horizontal_offset / 2
					: horizontal_offset;
			}
		}
	case Left:
	default:
		break;
	}
	DrawText(g, ts, Text);
}


YLabel::YLabel(const Rect& r, const Drawing::Font& f)
	: YWidget(r), MLabel(f)
{}

void
YLabel::Paint()
{
	YWidgetAssert(this, Widgets::YLabel, Draw);

	ParentType::Paint();
	PaintText(*this, FetchContext(*this), LocateForWindow(*this));
}


MTextList::MTextList(const Drawing::Font& f)
	: MLabel(f)
{}

/*void
MTextList::PaintTextList(Widget& w, const Point& pt)
{
	IWindow* pWnd(FetchDirectWindowPtr(
		ystdex::polymorphic_crosscast<IWidget&>(w)));

	if(pWnd && wpTextRegion)
	{
		wpTextRegion->Font = Font;
		wpTextRegion->Font.Update();
		wpTextRegion->ResetPen();
		wpTextRegion->Color = w.ForeColor;
		wpTextRegion->SetSize(w.GetWidth(), w.GetHeight());
		SetMarginsTo(*wpTextRegion, 2, 2, 2, 2);
		DrawText(pWnd->GetContext(), pt);
		wpTextRegion->SetSize(0, 0);
	}
}*/

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END
