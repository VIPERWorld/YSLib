﻿/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file scroll.h
\ingroup Shell
\brief 样式相关的图形用户界面滚动控件实现。
\version 0.3048;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:10:35 +0800;
\par 修改时间:
	2011-04-16 21:06 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::Scroll;
*/


#ifndef INCLUDED_SCROLL_H_
#define INCLUDED_SCROLL_H_

#include "uicontx.h"
#include "button.h"
#include "yfocus.h"
//#include "../Core/yres.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 滚动事件类型空间。
namespace ScrollEventSpace
{
	//! \brief 滚动事件类型。
	typedef enum
	{
		SmallDecrement = 0, //!< 滚动框小距离减量移动。
		SmallIncrement = 1, //!< 滚动框小距离增量移动。
		LargeDecrement = 2, //!< 滚动框大距离减量移动。
		LargeIncrement = 3, //!< 滚动框大距离增量移动。
		ThumbPosition = 4, //!< 滚动框定位（通过直接设置位置）。
		ThumbTrack = 5, //!< 滚动框当前正在移动。
		First = 6, //!< 滚动框移动至最小位置。
		Last = 7, //!< 滚动框移动至最大位置。
		EndScroll = 8 //!< 滚动框移动停止。
	} ScrollEventType;
};


//! \brief 滚动事件参数类。
struct ScrollEventArgs : public EventArgs,
	public GMValueEventArgs<SDst>
{
public:
	typedef GMValueEventArgs<SDst> MEventArgs;
	typedef MEventArgs::ValueType ValueType;

	ScrollEventSpace::ScrollEventType Type; //滚动事件类型。

	/*!
	\brief 构造：使用指定滚动事件类型和值。
	\note 值等于旧值。
	*/
	ScrollEventArgs(ScrollEventSpace::ScrollEventType, ValueType);
	/*!
	\brief 构造：使用指定滚动事件类型、值和旧值。
	*/
	ScrollEventArgs(ScrollEventSpace::ScrollEventType, ValueType, ValueType);
};

inline
ScrollEventArgs::ScrollEventArgs(ScrollEventSpace::ScrollEventType t,
	ScrollEventArgs::ValueType v)
	: GMValueEventArgs<SDst>(v), Type(t)
{}
inline
ScrollEventArgs::ScrollEventArgs(ScrollEventSpace::ScrollEventType t,
	ScrollEventArgs::ValueType v, ScrollEventArgs::ValueType old_value)
	: GMValueEventArgs<SDst>(v, old_value), Type(t)
{}


DefDelegate(HScrollEvent, IControl, ScrollEventArgs)


//! \brief 轨道。
class ATrack : public AUIBoxControl, public GMRange<u16>
{
public:
	//! \brief 轨道区域。
	typedef enum
	{
		None = 0,
		OnThumb = 1,
		OnPrev = 2,
		OnNext = 3
	} Area;
	//注意值类型需要和继承的 GMRange 的 ValueType 一致。
	typedef u16 ValueType; //!< 值类型。

protected:
	YThumb Thumb; //!< 滑块：轨道区域上的滚动框。

private:
	SDst min_thumb_length; //!< 最小滑块长度。
	// MRange 实现滚动事件关联值操作。
	ValueType large_delta; \
		//!< 大距离滚动偏移量：滚动事件关联的滑块位置变化绝对值。
	// MRange::value 实际最大取值为 MRange::max_value - large_delta 。

	//! \brief 事件依赖项。
	class Dependencies
	{
	public:
		DeclDepEvent(HVisualEvent, ThumbDrag) //!< 滑块拖动事件。
		DeclDepEvent(HScrollEvent, Scroll) //!< 滚动事件。

		Dependencies();
	} Events;

public:
	/*!
	\brief 构造：使用指定边界和大小。
	*/
	explicit
	ATrack(const Rect& = Rect::Empty, SDst = 8);

	DefPredicate(Horizontal, GetOrientation() == Horizontal)
	DefPredicate(Vertical, GetOrientation() == Vertical)

	/*!
	\brief 取焦点指针。
	*/
	ImplI1(IUIBox) DefMutableGetterBase(IControl*, FocusingPtr,
		MSimpleFocusResponser)
	/*!
	\brief 取顶端控件指针。
	\note 仅滑块。
	*/
	ImplI1(AUIBoxControl) IControl*
	GetTopControlPtr(const Point&);
	DefMutableDepEventGetter(HVisualEvent, ThumbDrag, Events.ThumbDrag) \
		//!< 滑块拖动事件。
	DefMutableDepEventGetter(HScrollEvent, Scroll, Events.Scroll) \
		//!< 滚动事件。
	DefGetter(SDst, MinThumbLength, min_thumb_length)
	DeclIEntry(Orientation GetOrientation() const) //!< 取轨道方向。
	DefGetter(SDst, ThumbLength, SelectFrom(Thumb.GetSize(),
		IsHorizontal())) //!< 取轨道方向上的滑块长度。
	DefGetter(SDst, ThumbPosition,
		SelectFrom(Thumb.GetLocation(), IsHorizontal())) //!< 取滑块位置。
	virtual DefGetter(SDst, TrackLength,
		SelectFrom(GetSize(), IsHorizontal())) //!< 取轨道方向上的轨道长度。
	DefGetter(ValueType, LargeDelta, large_delta)

	/*!
	\brief 设置轨道方向上的滑块长度。
	*/
	virtual void
	SetThumbLength(SDst);
	/*!
	\brief 设置滑块位置。
	*/
	void
	SetThumbPosition(SPos);
	/*!
	\brief 设置滚动事件关联值最大取值。
	\note 当指定值不大于 1 时无效。
	\note 约束 large_delta 小于指定值。
	*/
	void
	SetMaxValue(ValueType);
	/*!
	\brief 设置滚动事件关联值。
	\note 同步按滑块长度和最大取值的比例设置滑块位置。
	*/
	void
	SetValue(ValueType);
	/*!
	\brief 设置大距离滚动偏移量。
	\note 同步按滑块长度和最大取值的比例设置滑块位置。
	*/
	void
	SetLargeDelta(ValueType);

	/*!
	\brief 绘制界面。
	*/
	virtual void
	Paint();

protected:
	/*!
	\brief 检查轨道方向指定位置所在的区域。
	\note 断言检查。
	*/
	Area
	CheckArea(SDst) const;

public:
	/*!
	\brief 设置和调用滚动事件处理器。
	\param 滚动事件和旧的滚动事件值。
	*/
	void
	CheckScroll(ScrollEventSpace::ScrollEventType, ValueType);

	/*!
	\brief 定位滑块。
	\note 按指定滚动事件关联值设置滑块位置并触发滚动定位事件。
	*/
	virtual void
	LocateThumb(ValueType);

private:
	/*!
	\brief 定位滑块。
	\note 指定滚动事件关联值设置滑块位置并触发对应事件。
	*/
	void
	LocateThumb(ScrollEventSpace::ScrollEventType, ValueType);

public:
	/*!
	\brief 定位滑块至最小位置。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbToFirst();

	/*!
	\brief 定位滑块至最大位置。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbToLast();

private:
	/*!
	\brief 定位滑块（增量移动）。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbForIncrement(ScrollEventSpace::ScrollEventType, ValueType);

	/*!
	\brief 定位滑块（减量移动）。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbForDecrement(ScrollEventSpace::ScrollEventType, ValueType);

public:
	/*!
	\brief 定位滑块（大距离增量移动）。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbForLargeIncrement();

	/*!
	\brief 定位滑块（大距离减量移动）。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbForLargeDecrement();

	/*!
	\brief 定位滑块（指定小距离增量移动）。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbForSmallIncrement(ValueType);

	/*!
	\brief 定位滑块（指定小距离减量移动）。
	\note 设置滑块位置并触发滚动定位事件。
	*/
	void
	LocateThumbForSmallDecrement(ValueType);

	/*!
	\brief 更新滚动事件关联值。
	\note 由滑块位置按比例设置。
	*/
	void
	UpdateValue();

private:
	/*!
	\brief 处理屏幕接触开始事件。
	*/
	void
	OnTouchDown(TouchEventArgs&);

	/*!
	\brief 处理滑块移动事件。
	*/
	void
	OnThumbDrag(EventArgs&);
};

inline void
ATrack::LocateThumb(ATrack::ValueType v)
{
	LocateThumb(ScrollEventSpace::ThumbPosition, v);
}

inline void
ATrack::LocateThumbToFirst()
{
	LocateThumb(ScrollEventSpace::First, 0);
}

inline void
ATrack::LocateThumbToLast()
{
	LocateThumb(ScrollEventSpace::Last, 0);
}

inline void
ATrack::LocateThumbForLargeIncrement()
{
	LocateThumbForIncrement(ScrollEventSpace::LargeIncrement, GetLargeDelta());
}

inline void
ATrack::LocateThumbForLargeDecrement()
{
	LocateThumbForDecrement(ScrollEventSpace::LargeDecrement, GetLargeDelta());
}

inline void
ATrack::LocateThumbForSmallIncrement(ValueType abs_delta)
{
	LocateThumbForIncrement(ScrollEventSpace::SmallIncrement, abs_delta);
}

inline void
ATrack::LocateThumbForSmallDecrement(ValueType abs_delta)
{
	LocateThumbForDecrement(ScrollEventSpace::SmallDecrement, abs_delta);
}


//! \brief 水平轨道。
class YHorizontalTrack : public GMCounter<YHorizontalTrack>, public YComponent,
	public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\note 断言检查：宽大于长的 2 倍。
	*/
	explicit
	YHorizontalTrack(const Rect& = Rect::Empty, SDst = 8);

	ImplI1(ATrack)
	DefGetter(Orientation, Orientation, Horizontal)

private:
	/*!
	\brief 处理水平滑块移动事件。
	*/
	void
	OnTouchMove_Thumb_Horizontal(TouchEventArgs&);
};


//! \brief 垂直轨道。
class YVerticalTrack : public GMCounter<YVerticalTrack>, public YComponent,
	public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\note 断言检查：长大于宽的 2 倍。
	*/
	explicit
	YVerticalTrack(const Rect& = Rect::Empty, SDst = 8);

	ImplI1(ATrack)
		DefGetter(Orientation, Orientation, Vertical)

private:
	/*!
	\brief 处理垂直滑块移动事件。
	*/
	void
	OnTouchMove_Thumb_Vertical(TouchEventArgs&);
};


//! \brief 滚动条。
class AScrollBar : public AUIBoxControl
{
public:
	typedef ATrack::ValueType ValueType; //!< 值类型。

private:
	std::auto_ptr<ATrack> pTrack; //轨道。

protected:
	YThumb PrevButton, NextButton; //!< 滚动条按钮。

private:
	ValueType small_delta; \
		//!< 小距离滚动偏移量：滚动事件关联的滑块位置变化绝对值。

public:
	/*!
	\brief 构造：使用指定边界、大小和方向。
	*/
	explicit
	AScrollBar(const Rect& = Rect::Empty, SDst = 8, Orientation = Horizontal);

	/*!
	\brief 取顶端控件指针。
	\note 仅滑块和滚动条按钮。
	*/
	ImplI1(AUIBoxControl) IControl*
	GetTopControlPtr(const Point&);
	/*!
	\brief 取轨道引用。
	\pre 断言检查： pTrack.get() 。
	*/
	ATrack&
	GetTrack() const ythrow();
	DefGetterMember(ValueType, MaxValue, GetTrack())
	DefGetterMember(ValueType, Value, GetTrack())
	DefGetterMember(ValueType, LargeDelta, GetTrack())
	DefGetter(ValueType, SmallDelta, small_delta)

	DefSetterMember(ValueType, MaxValue, GetTrack())
	DefSetterMember(ValueType, Value, GetTrack())
	DefSetterMember(ValueType, LargeDelta, GetTrack())
	DefSetter(ValueType, SmallDelta, small_delta)

	/*!
	\brief 绘制界面。
	*/
	virtual void
	Paint();

private:
	/*!
	\brief 处理减量按钮屏幕接触开始事件。
	*/
	void
	OnTouchDown_PrevButton(TouchEventArgs&);

	/*!
	\brief 处理增量按钮屏幕接触开始事件。
	*/
	void
	OnTouchDown_NextButton(TouchEventArgs&);
};

inline ATrack&
AScrollBar::GetTrack() const ythrow()
{
	YAssert(pTrack.get(),
		"Null widget pointer found @ AScrollBar::GetTrack;");

	return *pTrack;
}


//! \brief 水平滚动条。
class YHorizontalScrollBar : public GMCounter<YHorizontalScrollBar>,
	public YComponent,
	public AScrollBar
{
public:
	typedef YComponent ParentType;

	explicit
	YHorizontalScrollBar(const Rect& = Rect::Empty, SDst = 8);

public:
	ImplI1(ATrack)
	DefGetter(Orientation, Orientation, Horizontal)
};


//! \brief 垂直滚动条。
class YVerticalScrollBar : public GMCounter<YVerticalScrollBar>,
	public YComponent,
	public AScrollBar
{
public:
	typedef YComponent ParentType;

	explicit
	YVerticalScrollBar(const Rect& = Rect::Empty, SDst = 8);

public:
	ImplI1(ATrack)
	DefGetter(Orientation, Orientation, Vertical)
};


//! \brief 带滚动条的容器基实现类。
class ScrollableContainer : public AUIBoxControl
{
protected:
	YHorizontalScrollBar HorizontalScrollBar;
	YVerticalScrollBar VerticalScrollBar;

public:
	explicit
	ScrollableContainer(const Rect& = Rect::Empty);

	/*!
	\brief 取顶端控件指针。
	*/
	ImplI1(AUIBoxControl) IControl*
	GetTopControlPtr(const Point&);

	/*!
	\brief 绘制界面。
	*/
	virtual void
	Paint();

protected:
	/*!
	\brief 固定布局。
	\return 调整后的视图大小。
	*/
	Size
	FixLayout(const Size&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif
