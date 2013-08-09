﻿/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file About.cpp
\ingroup YReader
\brief 关于界面。
\version r139
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:06:35 +0800
\par 修改时间:
	2013-08-05 22:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::About
*/


#include "About.h"

namespace YReader
{

//! \since build 434
namespace
{

const char TU_About[]{u8R"NPL(
root
($type "Panel")
($bounds "0 0 208 144")
(
	lblTitle
	($type "Label")
	($bounds "8 4 192 28")
)
(
	lblVersion
	($type "Label")
	($bounds "8 36 192 40")
)
(
	lblCopyright
	($type "Label")
	($bounds "8 80 192 20")
)
(
	btnClose
	($type "Button")
	($bounds "12 106 60 22")
)
(
	btnExit
	($type "Button")
	($bounds "84 106 60 22")
)
)NPL"};

} // unnamed namespace;

FrmAbout::FrmAbout()
	: Form({8, 24, 208, 144}),
	dynWgts(FetchWidgetLoader(), TU_About)
{
	auto& node(dynWgts.WidgetNode);
	auto& root(AccessWidget<Panel>(node));
	auto& lblTitle(AccessWidget<Label>(node, "lblTitle"));
	auto& lblVersion(AccessWidget<Label>(node, "lblVersion"));
	auto& lblCopyright(AccessWidget<Label>(node, "lblCopyright"));
	auto& btnClose(AccessWidget<Button>(node, "btnClose"));
	auto& btnExit(AccessWidget<Button>(node, "btnExit"));

	AddWidgets(*this, AccessWidget<Panel>(node)),
	lblTitle.Font.SetSize(20),
	yunseq(
		lblTitle.Background = nullptr,
		lblTitle.Text = G_APP_NAME,
		lblTitle.HorizontalAlignment = TextAlignment::Left,
		lblTitle.VerticalAlignment = TextAlignment::Down,
		lblTitle.ForeColor = ColorSpace::Blue,
		lblVersion.Background = nullptr,
		lblVersion.AutoWrapLine = true,
		lblVersion.Text = G_APP_VER + String(" @ " __DATE__ ", " __TIME__),
		lblVersion.ForeColor = ColorSpace::Green,
		lblCopyright.Background = nullptr,
		lblCopyright.Text = String("(C)2009-2013 by ") + G_COMP_NAME,
		lblCopyright.ForeColor = ColorSpace::Maroon,
		btnClose.Text = u"关闭",
		btnExit.Text = u"退出",
		root.Background = SolidBrush(Color(248, 120, 120)),
		btnClose.Background = SolidBrush(Color(176, 184, 192)),
		FetchEvent<TouchDown>(root) += [&](CursorEventArgs&& e){
			root.Background = SolidBrush(GenerateRandomColor());
			SetInvalidationOf(root);
			if(e.Strategy == RoutedEventArgs::Direct)
				e.Handled = true;
		},
		FetchEvent<TouchHeld>(root) += std::bind(OnTouchHeld_DraggingRaw,
			std::placeholders::_1, std::ref(*this)),
		FetchEvent<Click>(btnClose) += [this](CursorEventArgs&&){
			Hide(*this);
		},
		FetchEvent<Click>(btnExit) += [](CursorEventArgs&&){
			YSLib::PostQuitMessage(0);
		}
	);
	SetInvalidationOf(*this);
}

} // namespace YReader;

