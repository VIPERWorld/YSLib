﻿/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yapp.h
\ingroup Core
\brief 系统资源和应用程序实例抽象。
\version r1578
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-27 17:12:27 +0800
\par 修改时间:
	2013-08-05 20:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YApplication
*/


#ifndef YSL_INC_Core_yapp_h_
#define YSL_INC_Core_yapp_h_ 1

#include "yshell.h"

namespace YSLib
{

using Messaging::MessageQueue;

/*!
\brief 程序实例。
\since build 243
*/
class YF_API Application : public Shell
{
public:
	MessageQueue Queue; //!< 主消息队列：在程序实例中实现以保证单线程。

protected:
	/*!
	\brief 当前 Shell 句柄：指示当前线程空间中运行的 Shell 。
	\note 全局单线程，生存期与进程相同。
	*/
	shared_ptr<Shell> hShell;

public:
	//标准程序实例事件。
	std::function<void()> ApplicationExit; //!< 资源释放函数。
//	std::function<void()> Idle;

	/*!
	\brief 无参数构造。
	*/
	Application();

	/*!
	\brief 析构：释放 Shell 所有权和其它资源。
	*/
	virtual
	~Application();

	/*!
	\brief 取得线程空间中当前运行的 Shell 的句柄。
	*/
	DefGetter(const ynothrow, shared_ptr<Shell>, ShellHandle, hShell)

	/*!
	\brief 处理消息：分发消息。
	\pre 断言：当前 Shell 句柄有效。
	\since build 317
	*/
	void
	OnGotMessage(const Message&) override;

	//启动线程消息循环。
//	void
//	Run(shared_ptr<Shell>);

	/*!
	\brief 线程切换：若参数非空，和线程空间中当前运行的 Shell 的句柄交换。
	\return 参数是否有效。
	\since build 295
	*/
	bool
	Switch(shared_ptr<Shell>&) ynothrow;
	/*!
	\brief 线程切换：若参数非空，和线程空间中当前运行的 Shell 的句柄交换。
	\return 参数是否有效。
	\warning 空句柄在此处是可接受的，但继续运行可能会导致断言失败。
	\since build 295
	*/
	bool
	Switch(shared_ptr<Shell>&& h) ynothrow
	{
		return Switch(h);
	}
};


/*!
\brief 取应用程序实例。
\note 保证在平台相关的全局资源初始化之后初始化此实例。
\warning 初始化前不保证线程安全性。
\since build 398
*/
extern YF_API Application&
FetchAppInstance() ynothrow;

/*!
\ingroup helper_functions
\brief 取当前应用程序线程空间中活动的 Shell 句柄。
\warning 应用程序实例初始化前不保证线程安全性。
*/
inline shared_ptr<Shell>
FetchShellHandle() ynothrow
{
	return FetchAppInstance().GetShellHandle();
}

/*!
\ingroup helper_functions
\brief 激活 Shell 对象：控制权转移给此对象以维持单线程运行。
\pre 断言： <tt>bool(hShl)</tt> 。
\warning 不保证线程安全性。
*/
inline bool
Activate(const shared_ptr<Shell>& hShl)
{
	YAssert(bool(hShl), "Null shell handle found.");

	auto h(hShl);

	return FetchAppInstance().Switch(h);
}


/*!
\brief 全局默认队列消息发送函数。
\warning 应用程序实例初始化前不保证线程安全性。
\since build 297
*/
//@{
//! \since build 317
YF_API void
PostMessage(const Message&, Messaging::Priority) ynothrow;
//! \since build 320
inline void
PostMessage(Messaging::ID id, Messaging::Priority prior,
	const ValueObject& vo = {}) ynothrow
{
	PostMessage(Message(id, vo), prior);
}
//! \since build 320
inline void
PostMessage(Messaging::ID id, Messaging::Priority prior, ValueObject&& c)
	ynothrow
{
	PostMessage(Message(id, std::move(c)), prior);
}
//! \since build 320
template<Messaging::MessageID _vID>
inline void
PostMessage(Messaging::Priority prior,
	const typename Messaging::SMessageMap<_vID>::TargetType& target) ynothrow
{
	PostMessage(_vID, prior, ValueObject(target));
}
//@}

/*!
\brief 以优先级 p 发起 Shell 终止请求，返回 nExitCode。
*/
YF_API void
PostQuitMessage(int nExitCode, Messaging::Priority p = 0xF0);

} // namespace YSLib;

#endif

