﻿
/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yobject.h
\ingroup Core
\brief 平台无关的基础对象。
\version r3458;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2012-06-23 19:20 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YObject;
*/


#ifndef YSL_INC_CORE_YOBJECT_H_
#define YSL_INC_CORE_YOBJECT_H_

#include "ycutil.h"
#include "yexcept.h"
#include "../Adaptor/ycont.h"

YSL_BEGIN

//特征类策略：对象类型标签模版。

/*!
\brief 指定对于 _type 类型成员具有所有权的标签。
\since build 218 。
*/
PDefTmplH1(_type)
struct OwnershipTag
{};


/*!
\brief 标签类型元运算。
\since build 218 。
*/
template<class _tOwner, typename _type>
struct HasOwnershipOf : public std::integral_constant<bool,
	std::is_base_of<OwnershipTag<_type>, _tOwner>::value>
{};


//基本对象定义。

/*!
\brief 值类型对象类。
\pre 满足 CopyConstructible 。
\warning \c public 析构函数非虚实现。
\since build 217 。

具有值语义和深复制语义的对象。
*/
class ValueObject
{
public:
	typedef enum
	{
	//	Create = 0,
		Destroy = 1,
		Clone = 2,
		Equality = 3,
		TypeCheck = 4
	} OpType;
	typedef bool (*ManagerType)(void*&, void*&, OpType);

	//! \brief 指示指针构造的标记。
	struct PointerConstructTag
	{};

private:
	PDefTmplH1(_type)
	struct GManager
	{
		static_assert(std::is_object<_type>::value, "Non object type found.");

		static bool
		Do(void*& x, void*& y, OpType op)
		{
			switch(op)
			{
			case Destroy:
				delete static_cast<_type*>(x);
				break;
			case Clone:
				YAssert(y, "Null pointer found.");

				x = new _type(*static_cast<const _type*>(y));
				break;
			case Equality:
				YAssert(x && y, "Null pointer found.");

				return AreEqual(*static_cast<const _type*>(x),
					*static_cast<const _type*>(y));
			case TypeCheck:
#if YCL_FUNCTION_NO_EQUALITY_GUARANTEE
				return x ? *static_cast<const std::type_info*>(x)
					== typeid(GManager) : false;
#else
				break;
#endif
			default:
				YAssert(false, "Invalid operation found.");
			}
			return false;
		}

		/*!
		\brief 检查类型是否相等。
		\since build 300 。
		*/
		static bool
		CheckType(ManagerType m)
		{
			YAssert(m, "Null pointer found.");

#if YCL_FUNCTION_NO_EQUALITY_GUARANTEE
			const void* p(&typeid(GManager));
			void* q(nullptr);

			return m(const_cast<void*&>(p), q, TypeCheck);
#else
			return m == GManager::Do;
#endif
		}
	};

	ManagerType manager;
	mutable void* obj_ptr;

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	\since build 296 。
	*/
	yconstfn
	ValueObject() ynothrow
		: manager(nullptr), obj_ptr(nullptr)
	{}
	/*!
	\brief 构造：使用对象左值引用。
	\pre obj 可被复制构造。
	\note 得到包含指定对象副本的实例。
	*/
	PDefTmplH1(_type)
	ValueObject(const _type& obj)
		: manager(&GManager<_type>::Do), obj_ptr(new _type(obj))
	{}
	/*!
	\brief 构造：使用对象指针。
	\note 得到包含指针指向的指定对象的实例，并获得所有权。
	*/
	PDefTmplH1(_type)
	ValueObject(_type* p, PointerConstructTag)
		: manager(&GManager<_type>::Do), obj_ptr(p)
	{}
	ValueObject(const ValueObject&);
	/*!
	\brief 转移构造。
	\since build 296 。
	*/
	ValueObject(ValueObject&&) ynothrow;
	~ValueObject()
	{
		Clear();
	}

	ValueObject&
	operator=(const ValueObject& c)
	{
		ValueObject(c).Swap(*this);
		return *this;
	}
	/*!
	\brief 转移构造。
	\since build 296 。
	*/
	ValueObject&
	operator=(ValueObject&&) ynothrow;

	bool
	operator==(const ValueObject&) const;

	DefPred(const ynothrow, Empty, !obj_ptr)

	PDefTmplH1(_type)
	inline _type&
	GetObject()
	{
		YAssert(obj_ptr, "Null pointer found.");
		YAssert(GManager<_type>::CheckType(manager), "Invalid type found.");

		return *static_cast<_type*>(obj_ptr);
	}
	PDefTmplH1(_type)
	inline const _type&
	GetObject() const
	{
		YAssert(obj_ptr, "Null pointer found.");
		YAssert(GManager<_type>::CheckType(manager), "Invalid type found.");

		return *static_cast<const _type*>(obj_ptr);
	}

	/*!
	\brief 访问指定类型 const 对象。
	\throw std::bad_cast 类型检查失败 。
	\since build 306 。
	*/
	PDefTmplH1(_type)
	inline const _type&
	Access()
	{
		if(!GManager<_type>::CheckType(manager))
			throw std::bad_cast();
		return *static_cast<_type*>(obj_ptr);
	}
	/*!
	\brief 访问指定类型 const 对象。
	\throw std::bad_cast 类型检查失败 。
	\since build 306 。
	*/
	PDefTmplH1(_type)
	inline const _type&
	Access() const
	{
		if(!GManager<_type>::CheckType(manager))
			throw std::bad_cast();
		return *static_cast<const _type*>(obj_ptr);
	}

private:
	PDefTmplH1(_type)
	static inline bool
	AreEqual(_type& x, _type& y, decltype(x == y) = false)
	{
		return x == y;
	}
	PDefTmplH2(_type, _tUnused)
	static inline bool
	AreEqual(_type&, _tUnused&)
	{
		return true;
	}

public:
	/*
	\brief 清除。
	\post <tt>*this == ValueObject()</tt> 。
	\since build 296 。
	*/
	void
	Clear() ynothrow;

	/*!
	\brief 交换。
	\since build 296 。
	*/
	void
	Swap(ValueObject&) ynothrow;
};


/*!
\ingroup HelperFunctions
\brief 使用指针构造 ValueObject 实例。
\since build 233 。
*/
PDefTmplH1(_type)
inline ValueObject
MakeValueObjectByPtr(_type* p)
{
	return ValueObject(p, ValueObject::PointerConstructTag());
}


/*!
\brief 依赖项类模板。

基于被依赖的默认对象，可通过写时复制策略创建新对象；可能为空。
\tparam _type 被依赖的对象类型，需能被无参数构造。
\tparam _tOwnerPointer 依赖所有者指针类型。
\warning 依赖所有者指针需要实现所有权语义，
	否则出现无法释放资源导致内存泄漏或其它非预期行为。
\since build 195 。
\todo 线程模型及安全性。
*/
template<typename _type, class _tOwnerPointer = shared_ptr<_type>>
class GDependency
{
public:
	typedef _type DependentType;
	typedef _tOwnerPointer PointerType;
	typedef decltype(*PointerType()) ConstReferenceType;
	typedef typename std::remove_const<typename std::remove_reference<
		ConstReferenceType>::type>::type ReferentType;
	typedef ReferentType& ReferenceType;

private:
	PointerType ptr;

public:
	inline
	GDependency(PointerType p = PointerType())
		: ptr(p)
	{
		GetCopyOnWritePtr();
	}

	DefDeCopyAssignment(GDependency)
	DefDeMoveAssignment(GDependency)

	DefCvt(const ynothrow, ConstReferenceType, *ptr)
	DefCvt(ynothrow, ReferenceType, *ptr)
	DefCvt(const ynothrow, bool, bool(ptr))

	DefGetter(const ynothrow, ConstReferenceType, Ref, operator ConstReferenceType())
	DefGetter(ynothrow, ReferenceType, Ref, operator ReferenceType())
	DefGetter(ynothrow, ReferenceType, NewRef, *GetCopyOnWritePtr())

	PointerType
	GetCopyOnWritePtr()
	{
		if(!ptr)
			ptr = PointerType(new DependentType());
		else if(!ptr.unique())
			ptr = PointerType(CloneNonpolymorphic(ptr));

		YAssert(bool(ptr), "Null pointer found.");

		return ptr;
	}

	inline
	void Reset()
	{
		reset(ptr);
	}
};

/*!
\brief 范围模块类。
\warning 非虚析构。
\since build 193 。
*/
PDefTmplH1(_type)
class GMRange
{
public:
	typedef _type ValueType;

protected:
	ValueType max_value; //!< 最大取值。
	ValueType value; //!< 值。

	/*!
	\brief 构造：使用指定最大取值和值。
	*/
	GMRange(ValueType m, ValueType v)
		: max_value(m), value(v)
	{}

public:
	DefGetter(const ynothrow, ValueType, MaxValue, max_value)
	DefGetter(const ynothrow, ValueType, Value, value)
};

YSL_END

#endif

