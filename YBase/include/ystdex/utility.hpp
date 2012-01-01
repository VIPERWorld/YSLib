﻿/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file utility.hpp
\ingroup YStandardEx
\brief 函数对象和实用程序。
\version r1730;
\author FrankHB<frankhb1989@gmail.com>
\since build 189 。
\par 创建时间:
	2010-05-23 06:10:59 +0800;
\par 修改时间:
	2011-12-31 08:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YStandardEx::Utilities;
*/


#ifndef YCL_INC_YSTDEX_UTILITY_HPP_
#define YCL_INC_YSTDEX_UTILITY_HPP_

#include "../ydef.h"
#include <cstring>
#include <utility>
#include <functional>

namespace ystdex
{
	/*!	\defgroup helper_functions Helper Functions
	\brief 助手功能。
	*/

	/*
	\brief 不可复制对象：禁止继承此类的对象调用复制构造函数和复制赋值操作符。
	*/
	struct noncopyable
	{
	protected:
		/*!
		\brief \c protected 构造：空实现。
		\note 保护非多态类。
		*/
		yconstfn
		noncopyable()
		{}
		/*!
		\brief \c protected 析构：空实现。
		*/
		~noncopyable()
		{}

	public:
		/*!
		\brief 禁止复制构造。
		*/
		yconstfn
		noncopyable(const noncopyable&) = delete;

		/*!
		\brief 禁止赋值复制。
		*/
		noncopyable&
		operator=(const noncopyable&) = delete;
	};


	/*!	\defgroup functors General Functors
	\brief 算法。
	*/

	/*!
	\ingroup functors
	\brief 引用相等关系仿函数。
	*/
	template<typename _type>
	struct ref_eq : public std::binary_function<_type, _type, bool>
	{
		yconstfn bool
		operator()(const _type& _x, const _type& _y) const
		{
			return &_x == &_y;
		}
	};

	/*!
	\ingroup functors
	\brief 编译期选择自增/自减运算仿函数。
	*/
	//@{
	template<bool, typename _tScalar>
	struct xcrease_t
	{
		inline _tScalar&
		operator()(_tScalar& _x)
		{
			return ++_x;
		}
	};
	template<typename _tScalar>
	struct xcrease_t<false, _tScalar>
	{
		inline _tScalar&
		operator()(_tScalar& _x)
		{
			return --_x;
		}
	};
	//@}

	/*!
	\ingroup functors
	\brief 编译期选择加法/减法复合赋值运算仿函数。
	*/
	//@{
	template<bool, typename _tScalar1, typename _tScalar2>
	struct delta_assignment_t
	{
		yconstfn _tScalar1&
		operator()(_tScalar1& x, _tScalar2 y)
		{
			return x += y;
		}
	};
	template<typename _tScalar1, typename _tScalar2>
	struct delta_assignment_t<false, _tScalar1, _tScalar2>
	{
		yconstfn _tScalar1&
		operator()(_tScalar1& x, _tScalar2 y)
		{
			return x -= y;
		}
	};
	//@}

	/*!
	\ingroup helper_function
	\brief 编译期选择自增/自减运算。
	*/
	template<bool _bIsPositive, typename _tScalar>
	yconstfn _tScalar&
	xcrease(_tScalar& _x)
	{
		return xcrease_t<_bIsPositive, _tScalar>()(_x);
	}

	/*!
	\ingroup HelperFunctions
	\brief 编译期选择加法/减法复合赋值运算。
	*/
	template<bool _bIsPositive, typename _tScalar1, typename _tScalar2>
	yconstfn _tScalar1&
	delta_assignment(_tScalar1& _x, _tScalar2& _y)
	{
		return delta_assignment_t<_bIsPositive, _tScalar1, _tScalar2>()(_x, _y);
	}


	/*!
	\ingroup Functors
	\brief 引用仿函数。
	*/
	template<typename _type>
	struct deref_op : std::unary_function<_type, _type*>
	{
		/*!
		\brief 对指定对象使用 operator& 并返回结果。
		*/
		yconstfn _type*
		operator()(_type& _x) const
		{
			return &_x;
		}
	};


	/*!
	\ingroup Functors
	\brief 常量引用仿函数。
	*/
	template<typename _type>
	struct const_deref_op : std::unary_function<const _type, const _type*>
	{
		/*!
		\brief 对指定 const 对象使用 operator& 并返回结果。
		*/
		inline const _type*
		operator()(const _type& _x) const
		{
			return &_x;
		}
	};


	/*!
	\ingroup Functors
	\brief 间接访问比较仿函数。
	*/
	template<
		typename _type, typename _tPointer = _type*,
		template<typename _type> class _gfCompare = std::less
	>
	struct deref_comp : _gfCompare<_type>
	{
		/*!
		\brief 返回 _gfCompare<_type>::operator()(*_x, *_y) 。
		\note 如有空指针则不进行判断，直接返回 false 。
		*/
		bool
		operator()(const _tPointer& _x, const _tPointer& _y) const
		{
			return _x && _y && _gfCompare<_type>::operator()(*_x, *_y);
		}
	};


	/*!
	\ingroup Functors
	\brief 间接访问字符串比较仿函数。
	*/
	template<
		typename _tChar,
		int (*_lexi_cmp)(const _tChar*, const _tChar*) = std::strcmp,
		class _gfCompare = std::less<int>
	>
	struct deref_str_comp : _gfCompare
	{
		/*!
		\brief 返回 _gfCompare::operator()(_lexi_cmp(_x, _y), 0) 。
		\note 如有空指针则不进行判断，直接返回 false 。
		*/
		bool
		operator()(const _tChar* _x, const _tChar* _y) const
		{
			return _x && _y && _gfCompare::operator()(_lexi_cmp(_x, _y), 0);
		}
	};
}

#ifndef YCL_HAS_BUILTIN_NULLPTR
using ystdex::nullptr;
#endif

#endif

