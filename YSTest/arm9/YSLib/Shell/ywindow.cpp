﻿/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.cpp
\ingroup Shell
\brief 平台无关的图形用户界面窗口实现。
\version 0.3274;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 + 08:00;
\par 修改时间:
	2010-11-27 23:38 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWindow;
*/


#include "ydesktop.h"
#include "ycontrol.h"

YSL_BEGIN

using namespace Components::Controls;
using namespace Components::Widgets;
using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

MWindow::MWindow(const GHStrong<YImage> i, HWND hWnd, HSHL hShl)
	: MWindowObject(hWnd),
	Buffer(), hShell(hShl), prBackImage(i), bRefresh(false), bUpdate(false)
{}


AWindow::AWindow(const Rect& r, const GHStrong<YImage> i, HWND hWnd,
	HSHL hShl)
	: AVisualControl(r, GetPointer(hWnd)), MWindow(i, hWnd, hShl)
{}

BitmapPtr
AWindow::GetBackgroundPtr() const
{
	return prBackImage ? prBackImage->GetImagePtr() : NULL;
}

void
AWindow::SetSize(const Size& s)
{
	Buffer.SetSize(s.Width, s.Height);
	Widget::SetSize(s);
}

void
AWindow::RequestToTop()
{
	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		pDsk->MoveToTop(*this);
}

bool
AWindow::DrawBackgroundImage()
{
	YWindowAssert(this, Forms::AWindow, DrawBackgroundImage);

	if(prBackImage && GetBufferPtr())
	{
		ConstBitmapPtr imgBg(prBackImage->GetImagePtr());

		if(imgBg)
		{
			std::memcpy(GetBufferPtr(), imgBg, Buffer.GetSizeOfBuffer());
			return true;
		}
	}
	return false;
}

void
AWindow::DrawBackground()
{
	YWindowAssert(this, Forms::AWindow, DrawBackground);

	if(!DrawBackgroundImage())
		BeFilledWith(BackColor);
}

void
AWindow::Draw()
{
	DrawBackground();
	DrawWidgets();
	bUpdate = true;
}

void
AWindow::Refresh()
{
	if(bRefresh)
	{
		bRefresh = false;
		Draw();
	}
	Widget::Refresh();
}

void
AWindow::Update()
{
	if(bUpdate)
	{
		bUpdate = false;

		HWND hWnd(GetWindowHandle());

		if(hWnd)
		{
			UpdateToWindow();
			hWnd->SetUpdate(true);
		}
	}
}

void
AWindow::UpdateToScreen(YDesktop& d) const
{
	if(IsVisible())
		Buffer.CopyToBuffer(d.GetBackgroundPtr(), RDeg0, d.GetSize(),
		Point::Zero, GetLocation(), Buffer);
}

void
AWindow::UpdateToWindow(IWindow& w) const
{
	if(IsVisible())
	{
		const Graphics g(w);

		Buffer.CopyToBuffer(g.GetBufferPtr(), RDeg0, g.GetSize(),
			Point::Zero, GetLocation(), Buffer);
	}
}

void
AWindow::UpdateToScreen() const
{
	YDesktop* const pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		UpdateToScreen(*pDsk);
}

void
AWindow::UpdateToWindow() const
{
	const HWND hWnd(GetWindowHandle());

	if(hWnd)
		UpdateToWindow(*hWnd);
}

void
AWindow::Show()
{
	SetVisible(true);
	Draw();
	UpdateToScreen();
}


YFrameWindow::YFrameWindow(const Rect& r, const GHStrong<YImage> i,
	HWND hWnd, HSHL hShl)
	: YComponent(),
	AWindow(r, i, hWnd, hShl), MUIContainer()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p += static_cast<GMFocusResponser<IVisualControl>&>(*this);
	SetSize(GetSize());
	DrawBackground();
	InsertMessage(hShell, SM_WNDCREATE, 0xF0,
		handle_cast<WPARAM>(GetWindowHandle()), reinterpret_cast<LPARAM>(this));
	*hShl += *this;

	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		*pDsk += static_cast<IVisualControl&>(*this);
}
YFrameWindow::~YFrameWindow() ythrow()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p -= static_cast<GMFocusResponser<IVisualControl>&>(*this);
	hShell->RemoveAll(*this);

	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		pDsk->RemoveAll(*this);
	InsertMessage(hShell, SM_WNDDESTROY, 0xF0,
		handle_cast<WPARAM>(GetWindowHandle()), reinterpret_cast<LPARAM>(this));
}

bool
YFrameWindow::DrawWidgets()
{
	YWindowAssert(this, Forms::YFrameWindow, DrawWidgets);

	bool bBgChanged(!IsBgRedrawed());
	WidgetSet::iterator i;

	for(i = sWgtSet.begin(); !bBgChanged && i != sWgtSet.end(); ++i)
	{
		IWidget& w(**i);

		bBgChanged |= !w.IsTransparent() && w.IsVisible() && !w.IsBgRedrawed();
	}
	if(bBgChanged)
	{
		DrawBackground();
		for(i = sWgtSet.begin(); i != sWgtSet.end(); ++i)
		{
			IWidget& w(**i);

			if(w.IsVisible() && !(IsBgRedrawed()
				&& w.IsBgRedrawed()) && !w.IsTransparent())
			{
				w.DrawBackground();
				w.SetBgRedrawed(true);
			}
		}
	}
	for (i = sWgtSet.begin(); i != sWgtSet.end(); ++i)
	{
		IWidget& w(**i);

		if(w.IsVisible())
			w.DrawForeground();
	}
	DrawForeground();

	bool result(bBgChanged || !IsBgRedrawed());

	SetBgRedrawed(true);
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

