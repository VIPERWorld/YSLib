﻿/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReadingList.h
\ingroup YReader
\brief 阅读列表。
\version r222
\author FrankHB <frankhb1989@gmail.com>
\since build 328
\par 创建时间:
	2012-07-24 22:13:46 +0800
\par 修改时间:
	2013-08-05 21:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReadingList
*/


#ifndef INC_YReader_ReadingList_h_
#define INC_YReader_ReadingList_h_ 1

#include "Shells.h"
#include <YSLib/Service/AccessHistory.hpp>

namespace YReader
{

/*!
\brief 书签。
\warning 非虚析构。
\since build 391
*/
class Bookmark
{
public:
	/*!
	\brief 文件位置类型。
	\since build 391
	*/
	using PositionType = size_t;

	IO::Path Path;
	//! \since build 391
	PositionType Position;

	Bookmark(const IO::Path& path, size_t pos)
		: Path(path), Position(pos)
	{}
	DefDeCopyCtor(Bookmark)
	DefDeMoveCtor(Bookmark)

	//! \since build 399	
	explicit
	operator ValueNode::Container() const;
};


/*!
\brief 判断书签是否相等。
\since build 286
*/
inline bool
operator==(const Bookmark& x, const Bookmark& y)
{
	return x.Path == y.Path && x.Position == y.Position;
}


/*!
\brief 书签列表类型。
\since build 391
*/
using BookmarkList = vector<Bookmark::PositionType>;


/*!
\brief 阅读记录。
\warning 非虚析构。
\since build 286
*/
class ReadingList : private GAccessList<Bookmark>
{
public:
	/*!
	\brief 阅读记录列表类型。
	\since build 404
	*/
	using GAccessList<Bookmark>::ListType;

public:
	/*!
	\brief 默认构造：阅读迭代器指向阅读记录末尾。
	\since build 404
	*/
	ReadingList() = default;
	DefDelCopyCtor(ReadingList)
	DefDelMoveCtor(ReadingList)

	//! \since build 399
	explicit
	operator ValueNode::Container() const;

	// !\since build 404
	using GAccessList<Bookmark>::IsAtBegin;
	using GAccessList<Bookmark>::IsAtEnd;

	/*!
	\brief 清除所有记录。
	\since build 404
	*/
	using GAccessList<Bookmark>::Clear;

	/*!
	\brief 清除当前迭代器之后的所有记录并使迭代器指向末尾。
	\since build 404
	*/
	using GAccessList<Bookmark>::DropSubsequent;

	/*!
	\brief 插入阅读记录。
	*/
	void
	Insert(const IO::Path&, size_t);

	/*!
	\brief 切换阅读记录。
	\param 是否后退。
	\return 被移除的记录。
	\warning 不检查是否越界。
	\since build 404

	后退或前进，同时移除该记录。
	*/
	using GAccessList<Bookmark>::Switch;
};

} // namespace YReader;

#endif

