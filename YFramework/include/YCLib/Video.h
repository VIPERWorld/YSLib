﻿/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.h
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r701
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2011-05-26 19:41:08 +0800
\par 修改时间:
	2013-08-05 20:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Video
*/


#ifndef YCL_INC_Video_h_
#define YCL_INC_Video_h_ 1

#include "ycommon.h"

namespace platform
{

using SPos = std::int16_t; //!< 屏幕坐标度量。
using SDst = std::uint16_t; //!< 屏幕坐标距离。

//! \since build 417
//@{
using MonoType = ystdex::octet;
using AlphaType = ystdex::octet;
//@}

#if YCL_DS
	/*!
	\brief 标识 XYZ1555 像素格式。
	\note 值表示按整数表示的顺序从高位到低位为 ABGR 。
	\since build 297
	*/
#	define YCL_PIXEL_FORMAT_XYZ555 0xAABBCCDD

/*!
\brief LibNDS 兼容像素。
\note ABGR1555 。
*/
using PixelType = std::uint16_t;
/*!
\brief 取像素 Alpha 值。
\since build 417
*/
yconstfn PDefH(AlphaType, FetchAlpha, PixelType px) ynothrow
	ImplRet(px & 1 << 15 ? 0xFF : 0)

/*!
\brief 取不透明像素。
\since build 413
*/
yconstfn PDefH(PixelType, FetchOpaque, PixelType px) ynothrow
	ImplRet(px | 1 << 15)

/*
\brief 使用 8 位 RGB 构造本机类型像素。
\since build 417
*/
yconstfn PDefH(PixelType, FetchPixel,
	MonoType r, MonoType g, MonoType b) ynothrow
	ImplRet(r >> 3 | std::uint16_t(g >> 3) << 5 | std::uint16_t(b >> 3) << 10)

#	define DefColorH_(hex, name) name = \
	(FetchPixel(((hex) >> 16) & 0xFF, ((hex) >> 8) & 0xFF, (hex) & 0xFF) \
	| 1 << 15)
#elif YCL_MinGW32
	/*!
	\brief 标识 XYZ888 像素格式。
	\note 值表示按整数表示的顺序从高位到低位为 ARGB 。
	\since build 297
	*/
#	define YCL_PIXEL_FORMAT_XYZ888 0xAADDCCBB

/*!
\brief Windows DIB 格式兼容像素。
\note MSDN 注明此处第 4 字节保留为 0 ，但此处使用作为 8 位 Alpha 值使用。
	即 ARGB8888 （考虑字节序为BGRA8888）。
\note 转换 DIB 在设备上下文绘制时无需转换格式，比 ::COLORREF 更高效。
\warning 仅用于屏幕绘制，不保证无条件兼容于所有 DIB 。
\since build 434
\todo 断言对齐，保证类型兼容。
*/
struct PixelType
{
	//! \since build 417
	//@{
	MonoType rgbBlue;
	MonoType rgbGreen;
	MonoType rgbRed;
	AlphaType rgbReserved;
	//@}
};

/*!
\brief 取像素 Alpha 值。
\since build 417
*/
yconstfn PDefH(AlphaType, FetchAlpha, PixelType px) ynothrow
	ImplRet(px.rgbReserved)

/*!
\brief 取不透明像素。
\since build 413
*/
yconstfn PDefH(PixelType, FetchOpaque, PixelType px) ynothrow
	ImplRet({px.rgbBlue, px.rgbGreen, px.rgbRed, 0xFF})

/*
\brief 使用 8 位 RGB 构造 std::uint32_t 像素。
\since build 417
*/
yconstfn PDefH(std::uint32_t, FetchPixel,
	AlphaType r, AlphaType g, AlphaType b) ynothrow
	ImplRet(r | g << 8 | std::uint32_t(b) << 16)

/*!
\brief 定义 Windows DIB 格式兼容像素。
\note 得到的 32 位整数和 ::RGBQUAD 在布局上兼容。
\note Alpha 值为 0xFF 。
\since build 296
*/
#	define DefColorH_(hex, name) name = (FetchPixel((((hex) >> 16) & 0xFF), \
	(((hex) >> 8) & 0xFF), ((hex) & 0xFF)) << 8 | 0xFF)
#else
#	error "Unsupported platform found."
#endif

using BitmapPtr = PixelType*;
using ConstBitmapPtr = const PixelType*;


//! \brief 系统默认颜色空间。
namespace ColorSpace
{
//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
#define	HexAdd0x(hex) 0x##hex
#define DefColorH(hex_, name) DefColorH_(HexAdd0x(hex_), name)

/*!
\brief 默认颜色集。
\see http://www.w3schools.com/html/html_colornames.asp 。
\since build 416
*/
#if YCL_DS
enum ColorSet : PixelType
#else
enum ColorSet : std::uint32_t
#endif
{
	DefColorH(00FFFF, Aqua),
	DefColorH(000000, Black),
	DefColorH(0000FF, Blue),
	DefColorH(FF00FF, Fuchsia),
	DefColorH(808080, Gray),
	DefColorH(008000, Green),
	DefColorH(00FF00, Lime),
	DefColorH(800000, Maroon),
	DefColorH(000080, Navy),
	DefColorH(808000, Olive),
	DefColorH(800080, Purple),
	DefColorH(FF0000, Red),
	DefColorH(C0C0C0, Silver),
	DefColorH(008080, Teal),
	DefColorH(FFFFFF, White),
	DefColorH(FFFF00, Yellow)
};

#undef DefColorH
#undef DefColorH_
#undef HexAdd0x
} // namespace ColorSpace;


//! \brief 颜色。
class YF_API Color
{
public:
	using ColorSet = ColorSpace::ColorSet;

private:
	/*!
	\brief RGB 分量。
	\since build 276
	*/
	MonoType r, g, b;
	/*!
	\brief Alpha 分量。
	\since build 276
	*/
	AlphaType a;

public:
	/*!
	\brief 无参数构造：所有分量为 0 的默认颜色。
	\since build 319
	*/
	yconstfn
	Color() ynothrow
		: r(0), g(0), b(0), a(0)
	{}
	/*!
	\brief 构造：使用本机颜色对象。
	\since build 319
	*/
	yconstfn
	Color(PixelType px) ynothrow
#if YCL_DS
		: r(px << 3 & 248), g(px >> 2 & 248), b(px >> 7 & 248),
		a(FetchAlpha(px) ? 0xFF : 0x00)
#elif YCL_MinGW32
		: r(px.rgbRed), g(px.rgbGreen), b(px.rgbBlue), a(px.rgbReserved)
#endif
	{}
#if YCL_MinGW32
	/*!
	\brief 构造：使用默认颜色。
	\since build 319
	*/
	yconstfn
	Color(ColorSet cs) ynothrow
		: r((cs & 0xFF00) >> 8), g((cs & 0xFF0000) >> 16),
		b((cs & 0xFF000000) >> 24), a(0xFF)
	{}
#endif
	/*!
	\brief 构造：使用 RGB 值和 alpha 位。
	\since build 319
	*/
	yconstfn
	Color(MonoType r_, MonoType g_, MonoType b_, AlphaType a_ = 0xFF) ynothrow
		: r(r_), g(g_), b(b_), a(a_)
	{}
	/*!
	\brief 构造：使用相同类型转换为单色的 RGB 值和 alpha位。
	\note 避免列表初始化时 narrowing 转换。
	\since build 360
	*/
	template<typename _tScalar>
	yconstfn
	Color(_tScalar r_, _tScalar g_, _tScalar b_, AlphaType a_ = 0xFF) ynothrow
		: Color(MonoType(r_), MonoType(g_), MonoType(b_), a_)
	{}

	/*!
	\brief 转换：本机颜色对象。
	\since build 319
	*/
	yconstfn
	operator PixelType() const ynothrow
	{
#if YCL_DS
		return int(a != 0) << 15 | FetchPixel(r, g, b);
#elif YCL_MinGW32
		return {b, g, r, a};
#endif
	}

	/*!
	\brief 取 alpha 分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, A, a)
	/*!
	\brief 取蓝色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, B, b)
	/*!
	\brief 取绿色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, G, g)
	/*!
	\brief 取红色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, R, r)
};


/*!
\brief 控制台接口。
\since build 328
*/
namespace Consoles
{

/*!
\brief 控制台颜色枚举。
\since build 416
*/
enum Color
{
	Black = 0,
	DarkBlue,
	DarkGreen,
	DarkCyan,
	DarkRed,
	DarkMagenta,
	DarkYellow,
	Gray,
	DarkGray,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Yellow,
	White
};

/*!
\brief 控制台颜色。
\note 顺序和 Consoles::Color 对应。
\since build 328
*/
yconstexpr platform::Color ConsoleColors[]{ColorSpace::Black, ColorSpace::Navy,
	ColorSpace::Green, ColorSpace::Teal, ColorSpace::Maroon, ColorSpace::Purple,
	ColorSpace::Olive, ColorSpace::Silver, ColorSpace::Gray, ColorSpace::Blue,
	ColorSpace::Lime, ColorSpace::Aqua, ColorSpace::Red, ColorSpace::Yellow,
	ColorSpace::Fuchsia, ColorSpace::White};

} // namespace Consoles;

/*!
\brief 启动控制台。
\note fc 为前景色，bc为背景色。
*/
YF_API void
YConsoleInit(std::uint8_t dspIndex,
	Color fc = ColorSpace::White, Color bc = ColorSpace::Black);


/*!
\brief 初始化视频输出。
\warning 不保证线程安全性。
*/
YF_API bool
InitVideo();

} // namespace platform;


namespace platform_ex
{

#if YCL_DS
/*!
\brief 复位屏幕显示模式：清除背景寄存器，使用 ::vramDefault 复位，设置双屏 Mode 5 。
*/
YF_API void
ResetVideo();


/*!
\brief 默认上屏初始化函数。
*/
YF_API platform::BitmapPtr
InitScrUp(int&);

/*!
\brief 默认下屏初始化函数。
*/
YF_API platform::BitmapPtr
InitScrDown(int&);

/*!
\brief 快速刷新缓存映像到显示屏缓冲区。
\note 第一参数为显示屏缓冲区，第二参数为源缓冲区。
\since build 319
*/
YF_API void
ScreenSynchronize(platform::PixelType*, const platform::PixelType*) ynothrow;
#endif

#if YCL_DS || YCL_HOSTED
/*!
\brief DS 显示状态。
\note 对于 DS 提供实际的状态设置；对于宿主实现，仅保存状态。
\since build 429
*/
class YF_API DSVideoState
{
#if YCL_HOSTED
private:
	bool LCD_main_on_top = true;
#endif

public:
	bool
	IsLCDMainOnTop() const;

	void
	SetLCDMainOnTop(bool);

	void
	SwapLCD();
};
#endif

} // namespace platform_ex;

#endif

