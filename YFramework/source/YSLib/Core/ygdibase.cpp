﻿/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdibase.cpp
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r607
\author FrankHB <frankhb1989@gmail.com>
\since build 206
\par 创建时间:
	2011-05-03 07:23:44 +0800
\par 修改时间:
	2013-10-17 17:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YGDIBase
*/


#include "YSLib/Core/ygdibase.h"
#include "YSLib/Core/ycutil.h"
#include <ystdex/algorithm.hpp>

namespace YSLib
{

namespace Drawing
{

const Size Size::Invalid(std::numeric_limits<SDst>::lowest(),
		std::numeric_limits<SDst>::lowest());


const Rect Rect::Invalid(Size::Invalid);

bool
Rect::Contains(int px, int py) const ynothrow
{
	return IsInInterval<int>(px - X, Width)
		&& IsInInterval<int>(py - Y, Height);
}
bool
Rect::Contains(const Rect& r) const ynothrow
{
	return r.GetSize() && Contains(r.GetPoint())
		&& Contains(r.GetPoint() + r.GetSize() - Vec(1, 1));
}

bool
Rect::ContainsStrict(int px, int py) const ynothrow
{
	return Width > 1 && Height > 1 && IsInOpenInterval<int>(px - X,
		Width - 1) && IsInOpenInterval<int>(py - Y, Height - 1);
}
bool
Rect::ContainsStrict(const Rect& r) const ynothrow
{
	return r.GetSize() && ContainsStrict(r.GetPoint())
		&& ContainsStrict(r.GetPoint() + r.GetSize() - Vec(1, 1));
}

Rect&
Rect::operator&=(const Rect& r) ynothrow
{
	const SPos x1(max(X, r.X)), x2(min(X + Width, r.X + r.Width)),
		y1(max(Y, r.Y)), y2(min(Y + Height, r.Y + r.Height));

	return *this = x2 < x1 || y2 < y1 ? Rect() : Rect(x1, y1, x2 - x1, y2 - y1);
}

Rect&
Rect::operator|=(const Rect& r) ynothrow
{
	if(!*this)
		return *this = r;
	if(!r)
		return *this;

	const SPos mx(min(X, r.X)), my(min(Y, r.Y));

	return *this = Rect(mx, my, max(X + Width, r.X + r.Width) - mx,
		max(Y + Height, r.Y + r.Height) - my);
}

Rect
operator&(const Rect& a, const Rect& b) ynothrow
{
	return Rect(a) &= b;
}

Rect
operator|(const Rect& a, const Rect& b) ynothrow
{
	return Rect(a) |= b;
}


const Graphics Graphics::Invalid;

BitmapPtr
Graphics::operator[](size_t r) const ynothrow
{
	YAssert(pBuffer, "Null pointer found.");
	YAssert(r < sGraphics.Height, "Access out of range.");

	return pBuffer + r * sGraphics.Width;
}

BitmapPtr
Graphics::at(size_t r) const ythrow(GeneralEvent, std::out_of_range)
{
	if(YB_UNLIKELY(!pBuffer))
		throw GeneralEvent("Null pointer found.");
	if(YB_UNLIKELY(r >= sGraphics.Height))
		throw std::out_of_range("Access out of range.");

	return pBuffer + r * sGraphics.Width;
}

} // namespace Drawing;

} // namespace YSLib;

