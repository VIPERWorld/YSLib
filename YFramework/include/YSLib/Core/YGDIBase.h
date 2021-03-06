﻿/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGDIBase.h
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r2256
\author FrankHB <frankhb1989@gmail.com>
\since build 563
\par 创建时间:
	2011-05-03 07:20:51 +0800
\par 修改时间:
	2016-04-27 23:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YGDIBase
*/


#ifndef YSL_INC_Core_YGDIBase_h_
#define YSL_INC_Core_YGDIBase_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities // for YSLib::GeneralEvent,
//	YSLib::HalfDifference;
#include <limits>
#include <ystdex/operators.hpp> // for ystdex::equality_comparable;
#include <ystdex/string.hpp> // for ystdex::quote;

namespace YSLib
{

namespace Drawing
{

class Size;
class Rect;


/*!
\brief 屏幕二元组。
\warning 非虚析构。
\since build 242
*/
template<typename _type>
class GBinaryGroup : private ystdex::equality_comparable<GBinaryGroup<_type>>
{
	//! \since build 630
	static_assert(ystdex::is_nothrow_copyable<_type>(),
		"Invalid type found.");

public:
	static const GBinaryGroup Invalid; //!< 无效（不在屏幕坐标系中）对象。

	_type X = 0, Y = 0; //!< 分量。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	\since build 319
	*/
	yconstfn DefDeCtor(GBinaryGroup)
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn DefDeCopyCtor(GBinaryGroup)
	/*!
	\brief 构造：使用 Size 对象。
	\since build 319
	*/
	explicit yconstfn
	GBinaryGroup(const Size&) ynothrow;
	/*!
	\brief 构造：使用 Rect 对象。
	\since build 319
	*/
	explicit yconstfn
	GBinaryGroup(const Rect&) ynothrow;
	/*!
	\brief 构造：使用两个纯量。
	\tparam _tScalar1 第一分量纯量类型。
	\tparam _tScalar2 第二分量纯量类型。
	\warning 模板参数和 _type 符号不同时隐式转换可能改变符号，不保证唯一结果。
	\since build 319
	*/
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	GBinaryGroup(_tScalar1 x, _tScalar2 y) ynothrow
		: X(_type(x)), Y(_type(y))
	{}
	/*!
	\brief 构造：使用纯量对。
	\note 使用 std::get 取分量。仅取前两个分量。
	\since build 481
	*/
	template<typename _tPair>
	yconstfn
	GBinaryGroup(const _tPair& pr) ynothrow
		: X(std::get<0>(pr)), Y(std::get<1>(pr))
	{}

	//! \since build 554
	DefDeCopyAssignment(GBinaryGroup)

	/*!
	\brief 负运算：取加法逆元。
	\since build 319
	*/
	yconstfn PDefHOp(GBinaryGroup, -, ) const ynothrow
		ImplRet(GBinaryGroup(-X, -Y))

	/*!
	\brief 加法赋值。
	*/
	PDefHOp(GBinaryGroup&, +=, const GBinaryGroup& val) ynothrow
		ImplRet(yunseq(X += val.X, Y += val.Y), *this)
	/*!
	\brief 减法赋值。
	*/
	PDefHOp(GBinaryGroup&, -=, const GBinaryGroup& val) ynothrow
		ImplRet(yunseq(X -= val.X, Y -= val.Y), *this)

	yconstfn DefGetter(const ynothrow, _type, X, X)
	yconstfn DefGetter(const ynothrow, _type, Y, Y)

	DefSetter(, _type, X, X)
	DefSetter(, _type, Y, Y)

	/*!
	\brief 判断是否是零元素。
	*/
	yconstfn DefPred(const ynothrow, Zero, X == 0 && Y == 0)

	/*!
	\brief 选择分量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 319
	*/
	//@{
	PDefH(_type&, GetRef, bool b = true) ynothrow
		ImplRet(b ? X : Y)
	PDefH(const _type&, GetRef, bool b = true) const ynothrow
		ImplRet(b ? X : Y)
	//@}
};

//! \relates GBinaryGroup
//@{
template<typename _type>
const GBinaryGroup<_type> GBinaryGroup<_type>::Invalid{
	std::numeric_limits<_type>::lowest(), std::numeric_limits<_type>::lowest()};

/*!
\brief 比较：屏幕二元组相等关系。
\since build 319
*/
template<typename _type>
yconstfn bool
operator==(const GBinaryGroup<_type>& x, const GBinaryGroup<_type>& y) ynothrow
{
	return x.X == y.X && x.Y == y.Y;
}

/*!
\brief 加法：屏幕二元组。
\since build 319
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(const GBinaryGroup<_type>& x, const GBinaryGroup<_type>& y) ynothrow
{
	return GBinaryGroup<_type>(x.X + y.X, x.Y + y.Y);
}

/*!
\brief 减法：屏幕二元组。
\since build 319
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator-(const GBinaryGroup<_type>& x, const GBinaryGroup<_type>& y) ynothrow
{
	return GBinaryGroup<_type>(x.X - y.X, x.Y - y.Y);
}

/*!
\brief 数乘：屏幕二元组。
\since build 319
*/
template<typename _type, typename _tScalar>
yconstfn GBinaryGroup<_type>
operator*(const GBinaryGroup<_type>& val, _tScalar l) ynothrow
{
	return GBinaryGroup<_type>(val.X * l, val.Y * l);
}

/*!
\brief 转置。
\since build 575
*/
template<class _tBinary>
yconstfn _tBinary
Transpose(const _tBinary& obj) ynothrow
{
	return _tBinary(obj.Y, obj.X);
}

//! \since build 577
//@{
//! \brief 转置变换：逆时针旋转直角。
template<typename _type>
yconstfn GBinaryGroup<_type>
TransposeCCW(const GBinaryGroup<_type>& val) ynothrow
{
	return GBinaryGroup<_type>(val.Y, -val.X);
}

//! \brief 转置变换：顺时针旋转直角。
template<typename _type>
yconstfn GBinaryGroup<_type>
TransposeCW(const GBinaryGroup<_type>& val) ynothrow
{
	return GBinaryGroup<_type>(-val.Y, val.X);
}
//@}

/*!
\brief 取分量。
\since build 554
*/
//@{
template<size_t _vIdx, typename _type>
yconstfn _type&
get(GBinaryGroup<_type>& val)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? val.X : val.Y;
}
template<size_t _vIdx, typename _type>
yconstfn const _type&
get(const GBinaryGroup<_type>& val)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? val.X : val.Y;
}
//@}

/*!
\brief 转换为字符串。
\note 使用 ADL 。
\since build 308
*/
template<typename _type>
string
to_string(const GBinaryGroup<_type>& val)
{
	using YSLib::to_string;

	return ystdex::quote(to_string(val.X) + ", " + to_string(val.Y), '(', ')');
}
//@}


/*!
\brief 屏幕二维点（直角坐标表示）。
\since build 242
*/
using Point = GBinaryGroup<SPos>;


/*!
\brief 屏幕二维向量（直角坐标表示）。
\since build 242
*/
using Vec = GBinaryGroup<SPos>;


/*!
\brief 屏幕区域大小。
\warning 非虚析构。
\since build 161
*/
class YF_API Size : private ystdex::equality_comparable<Size>
{
public:
	/*!
	\brief 无效对象。
	\since build 296
	*/
	static const Size Invalid;

	SDst Width, Height; //!< 宽和高。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	\since build 319
	*/
	yconstfn
	Size() ynothrow
		: Width(0), Height(0)
	{}
	/*!
	\brief 复制构造。
	\since build 319
	*/
	yconstfn
	Size(const Size& s) ynothrow
		: Width(s.Width), Height(s.Height)
	{}
	/*!
	\brief 构造：使用 Rect 对象。
	\since build 319
	*/
	explicit yconstfn
	Size(const Rect&) ynothrow;
	/*!
	\brief 构造：使用屏幕二元组。
	\since build 319
	*/
	template<typename _type>
	explicit yconstfn
	Size(const GBinaryGroup<_type>& val) ynothrow
		: Width(static_cast<SDst>(val.X)), Height(static_cast<SDst>(val.Y))
	{}
	/*!
	\brief 构造：使用两个纯量。
	\since build 319
	*/
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	Size(_tScalar1 w, _tScalar2 h) ynothrow
		: Width(static_cast<SDst>(w)), Height(static_cast<SDst>(h))
	{}

	//! \since build 554
	DefDeCopyAssignment(Size)

	/*!
	\brief 判断是否为空或非空。
	\since build 320
	*/
	yconstfn DefBoolNeg(explicit yconstfn, Width != 0 || Height != 0)

	/*!
	\brief 求与另一个屏幕区域大小的交。
	\note 结果由分量最小值构造。
	\since build 555
	*/
	PDefHOp(Size&, &=, const Size& s) ynothrow
		ImplRet(yunseq(Width = min(Width, s.Width),
			Height = min(Height, s.Height)), *this)

	/*!
	\brief 求与另一个屏幕标准矩形的并。
	\note 结果由分量最大值构造。
	\since build 555
	*/
	PDefHOp(Size&, |=, const Size& s) ynothrow
		ImplRet(yunseq(Width = max(Width, s.Width),
			Height = max(Height, s.Height)), *this)

	/*!
	\brief 转换：屏幕二维向量。
	\note 以Width 和 Height 分量作为结果的 X 和 Y分量。
	*/
	yconstfn DefCvt(const ynothrow, Vec, Vec(Width, Height))

	/*!
	\brief 判断是否为线段：长或宽中有且一个数值等于 0 。
	\since build 264
	*/
	yconstfn DefPred(const ynothrow, LineSegment,
		!((Width == 0) ^ (Height == 0)))
	/*!
	\brief 判断是否为不严格的空矩形区域：包括空矩形和线段。
	\since build 264
	*/
	yconstfn DefPred(const ynothrow, UnstrictlyEmpty, Width == 0 || Height == 0)

	/*!
	\brief 选择分量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 319
	*/
	//@{
	PDefH(SDst&, GetRef, bool b = true) ynothrow
		ImplRet(b ? Width : Height)
	PDefH(const SDst&, GetRef, bool b = true) const ynothrow
		ImplRet(b ? Width : Height)
	//@}
};

/*!
\relates Size
\since build 319
*/
//@{
//! \brief 比较：屏幕区域大小相等关系。
yconstfn PDefHOp(bool, ==, const Size& x, const Size& y) ynothrow
	ImplRet(x.Width == y.Width && x.Height == y.Height)

/*!
\brief 求两个屏幕区域大小的交。
\sa Size::operator&=
\since build 555
*/
yconstfn PDefHOp(Size, &, const Size& x, const Size& y) ynothrow
	ImplRet({min(x.Width, y.Width), min(x.Height, y.Height)})

/*!
\brief 求两个屏幕区域大小的并。
\sa Size::operator|=
\since build 555
*/
yconstfn PDefHOp(Size, |, const Size& x, const Size& y) ynothrow
	ImplRet({max(x.Width, y.Width), max(x.Height, y.Height)})

/*!
\brief 取面积。
\since build 601
*/
yconstfn PDefH(size_t, GetAreaOf, const Size& s) ynothrow
	ImplRet(size_t(s.Width * s.Height))

/*!
\brief 计算第一参数和第二参数为大小的矩形中心重合时左上角相对第一个矩形的位置。
\since build 583
*/
yconstfn PDefH(Point, LocateCenter, const Size& x, const Size& y)
	// XXX: Conversion to 'SPos' might be implementation-defined.
	ImplRet({HalfDifference(SPos(x.Width), SPos(y.Width)),
		HalfDifference(SPos(x.Height), SPos(y.Height))})

/*!
\brief 计算按指定大小的矩形绕中心旋转一个直角后左上角的相对位置。
\since build 578
*/
yconstfn PDefH(Point, RotateCenter, const Size& s)
	// XXX: Conversion to 'SPos' might be implementation-defined.
	ImplRet({HalfDifference(SPos(s.Width), SPos(s.Height)),
		HalfDifference(SPos(s.Height), SPos(s.Width))})

/*!
\brief 取分量。
\since build 554
*/
//@{
template<size_t _vIdx>
yconstfn SDst&
get(Size& s)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? s.Width : s.Height;
}
template<size_t _vIdx>
yconstfn const SDst&
get(const Size& s)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? s.Width : s.Height;
}
//@}

/*!
\brief 转换为字符串。
\note 使用 ADL 。
\since build 308
*/
YF_API string
to_string(const Size&);
//@}


/*!
\brief 加法：使用屏幕二元组和屏幕区域大小分量对应相加构造屏幕二元组。
\since build 319
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(GBinaryGroup<_type> val, const Size& s) ynothrow
{
	// XXX: Conversion to '_type' might be implementation-defined.
	return {val.X + _type(s.Width), val.Y + _type(s.Height)};
}

/*!
\brief 减法：使用屏幕二元组和屏幕区域大小分量对应相加构造屏幕二元组。
\since build 577
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator-(GBinaryGroup<_type> val, const Size& s) ynothrow
{
	// XXX: Conversion to '_type' might be implementation-defined.
	return {val.X - _type(s.Width), val.Y - _type(s.Height)};
}


/*!
\brief 转置。
\since build 575
*/
yconstfn PDefH(Size, Transpose, const Size& s) ynothrow
	ImplRet({s.Height, s.Width})


/*!
\brief 计算两个大小和阈值限定的最小缩放值。
\since build 555
*/
template<typename _tScalar = float>
yconstfn _tScalar
ScaleMin(const Size& x, const Size& y, _tScalar threshold = 1.F)
{
	return YSLib::min({threshold,
		_tScalar(_tScalar(x.Width) / _tScalar(y.Width)),
		_tScalar(_tScalar(x.Height) / _tScalar(y.Height))});
}


/*!
\brief 屏幕标准矩形：表示屏幕矩形区域。
\note 边平行于水平直线；使用左上点屏幕坐标、宽和高表示。
\warning 非虚析构。
\since build 161
*/
class YF_API Rect : private Point, private Size,
	private ystdex::equality_comparable<Rect>
{
public:
	/*!
	\brief 无效对象。
	\since build 296
	*/
	static const Rect Invalid;

	/*!
	\brief 左上角横坐标。
	\sa Point::X
	\since build 296
	*/
	using Point::X;
	/*!
	\brief 左上角纵坐标。
	\sa Point::Y
	\since build 296
	*/
	using Point::Y;
	/*!
	\brief 长。
	\sa Size::Width
	\since build 296
	*/
	using Size::Width;
	/*!
	\brief 宽。
	\sa Size::Height
	\since build 296
	*/
	using Size::Height;


	/*!
	\brief 无参数构造：默认实现。
	\since build 453
	*/
	DefDeCtor(Rect)
	/*!
	\brief 构造：使用屏幕二维点。
	\since build 319
	*/
	explicit yconstfn
	Rect(const Point& pt) ynothrow
		: Point(pt), Size()
	{}
	/*!
	\brief 构造：使用 Size 对象。
	\since build 399
	*/
	yconstfn
	Rect(const Size& s) ynothrow
		: Point(), Size(s)
	{}
	/*!
	\brief 构造：使用屏幕二维点和 Size 对象。
	\since build 319
	*/
	yconstfn
	Rect(const Point& pt, const Size& s) ynothrow
		: Point(pt), Size(s)
	{}
	/*!
	\brief 构造：使用屏幕二维点和表示长宽的两个 SDst 值。
	\since build 319
	*/
	yconstfn
	Rect(const Point& pt, SDst w, SDst h) ynothrow
		: Point(pt.X, pt.Y), Size(w, h)
	{}
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和 Size 对象。
	\since build 319
	*/
	yconstfn
	Rect(SPos x, SPos y, const Size& s) ynothrow
		: Point(x, y), Size(s.Width, s.Height)
	{}
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和表示大小的两个 SDst 值。
	\since build 319
	*/
	yconstfn
	Rect(SPos x, SPos y, SDst w, SDst h) ynothrow
		: Point(x, y), Size(w, h)
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn DefDeCopyCtor(Rect)

	DefDeCopyAssignment(Rect)
	//! \since build 319
	//@{
	Rect&
	operator=(const Point& pt) ynothrow
	{
		yunseq(X = pt.X, Y = pt.Y);
		return *this;
	}
	Rect&
	operator=(const Size& s) ynothrow
	{
		yunseq(Width = s.Width, Height = s.Height);
		return *this;
	}
	//@}

	/*!
	\brief 求与另一个屏幕标准矩形的交。
	\note 若相离结果为 Rect() ，否则为包含于两个参数中的最大矩形。
	\since build 320
	*/
	Rect&
	operator&=(const Rect&) ynothrow;

	/*!
	\brief 求与另一个屏幕标准矩形的并。
	\note 结果为包含两个参数中的最小矩形。
	\since build 320
	*/
	Rect&
	operator|=(const Rect&) ynothrow;

	/*!
	\brief 判断是否为空。
	\sa Size::operator!
	\since build 320
	*/
	using Size::operator!;

	/*!
	\brief 判断是否非空。
	\sa Size::bool
	\since build 320
	*/
	using Size::operator bool;

	/*!
	\brief 判断点 (px, py) 是否在矩形内或边上。
	\since build 528
	*/
	bool
	Contains(SPos px, SPos py) const ynothrow;
	/*!
	\brief 判断点 pt 是否在矩形内或边上。
	\since build 319
	*/
	PDefH(bool, Contains, const Point& pt) const ynothrow
		ImplRet(Contains(pt.X, pt.Y))
	/*!
	\brief 判断矩形是否在矩形内或边上。
	\note 空矩形总是不被包含。
	\since build 319
	*/
	bool
	Contains(const Rect&) const ynothrow;
	/*!
	\brief 判断点 (px, py) 是否在矩形内。
	\since build 528
	*/
	bool
	ContainsStrict(SPos px, SPos py) const ynothrow;
	/*!
	\brief 判断点 pt 是否在矩形内。
	\since build 319
	*/
	PDefH(bool, ContainsStrict, const Point& pt) const ynothrow
		ImplRet(ContainsStrict(pt.X, pt.Y))
	/*!
	\brief 判断矩形是否在矩形内或边上。
	\note 空矩形总是不被包含。
	\since build 319
	*/
	bool
	ContainsStrict(const Rect&) const ynothrow;
	/*!
	\brief 判断矩形是否为线段：长和宽中有且一个数值等于 0 。
	\sa Size::IsLineSegment
	\since build 296
	*/
	using Size::IsLineSegment;
	/*!
	\brief 判断矩形是否为不严格的空矩形区域：包括空矩形和线段。
	\sa Size::IsUnstrictlyEmpty
	\since build 296
	*/
	using Size::IsUnstrictlyEmpty;

	//! \since build 588
	// XXX: Conversion to 'SPos' might be implementation-defined.
	yconstfn DefGetter(const ynothrow, SPos, Bottom, Y + SPos(Height))
	/*!
	\brief 取左上角位置。
	*/
	yconstfn DefGetter(const ynothrow, const Point&, Point,
		static_cast<const Point&>(*this))
	/*!
	\brief 取左上角位置引用。
	\since build 296
	*/
	DefGetter(ynothrow, Point&, PointRef, static_cast<Point&>(*this))
	//! \since build 588
	// XXX: Conversion to 'SPos' might be implementation-defined.
	yconstfn DefGetter(const ynothrow, SPos, Right, X + SPos(Width))
	/*!
	\brief 取大小。
	*/
	yconstfn DefGetter(const ynothrow, const Size&, Size,
		static_cast<const Size&>(*this))
	/*!
	\brief 取大小引用。
	\since build 296
	*/
	DefGetter(ynothrow, Size&, SizeRef, static_cast<Size&>(*this))
	//! \since build 588
	//@{
	using Point::GetX;
	using Point::GetY;

	using Point::SetX;
	using Point::SetY;
	//@}
};

//! \relates Rect
//@{
/*!
\brief 比较：屏幕标准矩形相等关系。
\since build 319
*/
yconstfn PDefHOp(bool, ==, const Rect& x, const Rect& y) ynothrow
	ImplRet(x.GetPoint() == y.GetPoint() && x.GetSize() == y.GetSize())

/*!
\brief 加法：使用标准矩形 r 和偏移向量 v 构造屏幕标准矩形。
\since build 319
*/
yconstfn PDefHOp(Rect, +, const Rect& r, const Vec& v) ynothrow
	ImplRet({r.GetPoint() + v, r.GetSize()})

/*!
\brief 减法：使用标准矩形 r 和偏移向量的加法逆元 v 构造屏幕标准矩形。
\since build 319
*/
yconstfn PDefHOp(Rect, -, const Rect& r, const Vec& v) ynothrow
	ImplRet({r.GetPoint() - v, r.GetSize()})

/*!
\brief 求两个屏幕标准矩形的交。
\sa Rect::operator&=
\since build 555
*/
inline PDefHOp(Rect, &, const Rect& x, const Rect& y) ynothrow
	ImplRet(Rect(x) &= y)

/*!
\brief 求两个屏幕标准矩形的并。
\sa Rect::operator|=
\since build 555
*/
inline PDefHOp(Rect, |, const Rect& x, const Rect& y) ynothrow
	ImplRet(Rect(x) |= y)

/*!
\brief 按指定大小缩小矩形。
\pre 断言：矩形的当前大小足够缩小。
\since build 479
*/
inline PDefH(void, Diminish, Rect& r, SDst off1 = 1, SDst off2 = 2)
	// XXX: Conversion to 'SPos' might be implementation-defined.
	ImplExpr(YAssert(r.Width > off2 && r.Height > off2,
		"Boundary is too small."), yunseq(r.X += SPos(off1), r.Y += SPos(off1),
		r.Width -= off2, r.Height -= off2))

//! \since build 577
//@{
/*!
\brief 根据对角线上的两个顶点坐标出啊关键矩形。
\note 可以是可以是主对角线或副对角线之一上的顶点。
*/
/*!
\todo 使用 ISO C++14 的带有 \c constexpr 的 std::min 和 std::max 。
\todo 提取 abs 实现。
*/
yconstfn PDefH(Rect, MakeRect, const Point& pt1, const Point& pt2) ynothrow
	ImplRet(Rect(min(pt1.X, pt2.X), min(pt1.Y, pt2.Y),
		SDst(pt1.X < pt2.X ? pt2.X - pt1.X : pt1.X - pt2.X),
		SDst(pt1.Y < pt2.Y ? pt2.Y - pt1.Y : pt1.Y - pt2.Y)))

//! \brief 转置变换：逆时针旋转直角。
yconstfn PDefH(Rect, TransposeCCW, const Rect& r) ynothrow
	ImplRet(MakeRect(TransposeCCW(Point(r.GetRight(), r.Y)),
		TransposeCCW(Point(r.X, r.GetBottom()))))

//! \brief 转置变换：顺时针旋转直角。
yconstfn PDefH(Rect, TransposeCW, const Rect& r) ynothrow
	ImplRet(MakeRect(TransposeCW(Point(r.GetRight(), r.Y)),
		TransposeCW(Point(r.X, r.GetBottom()))))
//@}

/*!
\brief 取分量。
\since build 554
*/
//@{
template<size_t _vIdx>
yconstfn ystdex::conditional_t<_vIdx < 2, SPos, SDst>&
get(Rect& r)
{
	return get<_vIdx>(
		tuple<SPos&, SPos&, SDst&, SDst&>(r.X, r.Y, r.Width, r.Height));
}
template<size_t _vIdx>
yconstfn const ystdex::conditional_t<_vIdx < 2, SPos, SDst>&
get(const Rect& r)
{
	return get<_vIdx>(tuple<const SPos&, const SPos&, const SDst&, const SDst&>(
		r.X, r.Y, r.Width, r.Height));
}
//@}

/*!
\brief 转换为字符串。
\note 使用 ADL 。
\since build 308
*/
YF_API string
to_string(const Rect&);
//@}


template<typename _type>
yconstfn
GBinaryGroup<_type>::GBinaryGroup(const Rect& r) ynothrow
	: X(r.X), Y(r.Y)
{}

yconstfn
Size::Size(const Rect& r) ynothrow
	: Width(r.Width), Height(r.Height)
{}





/*!
\brief 逆时针旋转角度指示输出指向。
\note 保证底层为无符号整数类型。
\since build 416
*/
enum Rotation : yimpl(size_t)
{
	RDeg0 = 0,
	RDeg90 = 1,
	RDeg180 = 2,
	RDeg270 = 3
};

//! \relates Roation
//@{
//! \since build 577
yconstfn PDefH(Rotation, Flip, Rotation rot)
	ImplRet(Rotation((size_t(rot) + 2) % 4))

//! \since build 575
yconstfn PDefH(Rotation, RotateCCW, Rotation rot)
	ImplRet(Rotation((size_t(rot) + 1) % 4))

//! \since build 575
yconstfn PDefH(Rotation, RotateCW, Rotation rot)
	ImplRet(Rotation((size_t(rot) - 1) % 4))
//@}


/*!
\brief 二元方向。
\since build 416
*/
enum Orientation
{
	Horizontal = 0,
	Vertical = 1
};

} // namespace Drawing;

} // namespace YSLib;

#endif

