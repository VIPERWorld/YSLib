﻿/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file container.hpp
\ingroup YStandardEx
\brief 通用容器操作。
\version r216
\author FrankHB<frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-09-12 01:36:20 +0800
\par 修改时间:
	2012-09-12 19:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Containers
*/


#ifndef YB_INC_YSTDEX_CONTAINER_HPP_
#define YB_INC_YSTDEX_CONTAINER_HPP_ 1

#include "functional.hpp"
#include <array> // for std::array;
#include <algorithm> // for std::copy_n;

namespace ystdex
{

/*!
\ingroup algorithms
\brief 插入参数指定的元素到容器。
\since build 274
*/
//@{
template<class _tContainer, typename... _tParams>
inline void
assign(_tContainer& con, _tParams&&... args)
{
	con.assign(yforward(args)...);
}
template<class _tContainer, typename _type, size_t _vN>
inline void
assign(_tContainer& con, const _type(&arr)[_vN])
{
	con.assign(arr, arr + _vN);
}
//@}


/*!
\brief 容器插入函数对象。
\note 成员命名参照 ISO C++11 24.5.2 中的类定义概要。
\since build 338
*/
template<typename _tContainer>
class container_inserter
{
public:
	typedef _tContainer container_type;

protected:
	_tContainer* container;

public:
	container_inserter(_tContainer& cont)
		: container(&cont)
	{}

	template<typename... _tParams>
	auto
	operator()(_tParams&&... args)
		-> decltype(container->insert(yforward(args)...))
	{
		yassume(container);

		return container->insert(yforward(args)...);
	}
};

/*!
\brief 顺序插入值至指定容器。
\since build 338
*/
template<typename _tContainer, typename... _tParams>
inline void
seq_insert(_tContainer& cont, _tParams&&... args)
{
	ystdex::seq_apply(container_inserter<_tContainer>(cont), yforward(args)...);
}


/*!
\ingroup algorithms
\brief 删除指定序列容器中和指定值的相等的元素。
\since build 289
*/
template<typename _tContainer>
void
erase_all(_tContainer& c, const typename _tContainer::value_type& val)
{
	c.erase(std::remove(c.begin(), c.end(), val), c.end());
}
/*!
\ingroup algorithms
\brief 删除指定序列容器中迭代器区间中和指定值的相等的元素。
\pre first 和 last 是 c 的有效的迭代器或 <tt>c.end()</tt> 。
\since build 289
*/
template<typename _tContainer, typename _tIn, typename _tValue>
void
erase_all(_tContainer& c, _tIn first, _tIn last, const _tValue& value)
{
	while(first != last)
		if(*first == value)
			c.erase(first++);
		else
			++first;
}

/*!
\ingroup algorithms
\brief 删除指定序列容器中满足条件的元素。
\since build 289
*/
template<typename _tContainer, typename _fPredicate>
void
erase_all_if(_tContainer& c, _fPredicate pred)
{
	c.erase(std::remove_if(c.begin(), c.end(), pred), c.end());
}
/*!
\ingroup algorithms
\brief 删除指定序列容器中迭代器区间中满足条件的元素。
\pre first 和 last 是 c 的有效的迭代器或 <tt>c.end()</tt> 。
\since build 289
*/
template<typename _tContainer, typename _tIn, typename _fPredicate>
void
erase_all_if(_tContainer& c, _tIn first, _tIn last, _fPredicate pred)
{
	while(first != last)
		if(pred(*first))
			c.erase(first++);
		else
			++first;
}


/*!
\ingroup algorithms
\brief 按指定键值搜索指定映射。
\return 一个用于表示结果的 std::pair 对象，其成员 first 为迭代器，
	second 表示是否不存在而需要插入。
\note 行为类似 std::map::operator[] 。
\since build 173
*/
template<class _tMap>
std::pair<typename _tMap::iterator, bool>
search_map(_tMap& m, const typename _tMap::key_type& k)
{
	const auto i(m.lower_bound(k));

	return std::make_pair(i, (i == m.end() || m.key_comp()(k, i->first)));
}


/*!
\brief 取指定参数初始化的 std::array 对象。
\since build 337
*/
template<typename _type, typename... _tParams>
inline std::array<_type, sizeof...(_tParams)>
make_array(_tParams&&... args)
{
	// TODO: Use one pair of braces.
	return {{decay_copy(yforward(args))...}};
}

/*!
\brief 取指定参数转换为 std::array 对象。
\since build 337
*/
//@{
template<typename _type, size_t _vN, typename _tSrc>
yconstfn std::array<_type, _vN>
to_array(const _tSrc& src)
{
	return std::array<_type, _vN>(src);
}
template<typename _type, size_t _vN>
yconstfn std::array<_type, _vN>
to_array(const std::array<_type, _vN>& src)
{
	return src;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(const _tSrcElement(&src)[_vN])
{
	std::array<_type, _vN> arr;

	std::copy_n(std::addressof(src[0]), _vN, std::addressof(arr[0]));
	return std::move(arr);
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(_tSrcElement(&&src)[_vN])
{
	std::array<_type, _vN> arr;

	std::copy_n(std::make_move_iterator(std::addressof(src[0])), _vN,
		std::addressof(arr[0]));
	return std::move(arr);
}
//@}

} // namespace ystdex;

#endif
