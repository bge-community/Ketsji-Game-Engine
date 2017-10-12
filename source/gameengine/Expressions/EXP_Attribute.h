#ifndef __EXP_ATTRIBUTE_H__
#define __EXP_ATTRIBUTE_H__

#include <string>
#include "MT_Vector2.h"
#include "MT_Vector3.h"
#include "EXP_Python.h"

class PyObjectPlus;

class EXP_Attribute
{
public:
	using GetterFunction = PyObject *(*)(PyObject *, void *);
	using SetterFunction = int (*)(PyObject *, PyObject *, void *);

	/// Name of the python class.
	std::string m_className;
	/// Name of the python attribute.
	std::string m_name;

	GetterFunction m_getter;
	SetterFunction m_setter;

/*	/// Type of value.
	Type m_type;
	/// Read/write access or read-only.
	Access m_access;
	/// Getter/setter flags.
	GetSetFlags m_flags;*/
	/// Use float for integer based types too.
	float m_range[2];

	/// Attribute offset.
	intptr_t m_offset;

	EXP_Attribute();

	EXP_Attribute(const std::string& className, const std::string& name, intptr_t offset, float range[2],
			GetterFunction getter, SetterFunction setter);

	template <typename Type>
	Type *PtrFromOffset(PyObjectPlus *self) const
	{
		return (Type *)((intptr_t)(self) + m_offset);
	}

	static bool IsValid(PyObjectPlus *self);

	template <typename Type>
	void PrintSetterError() const;

	void PrintError(const std::string& msg) const;
};

template <>
void EXP_Attribute::PrintSetterError<bool>() const
{
	PrintError(" = bool: Excepted a boolean.");
}

template <>
void EXP_Attribute::PrintSetterError<int>() const
{
	PrintError(" = int: Excepted a int.");
}

template <>
void EXP_Attribute::PrintSetterError<unsigned int>() const
{
	PrintError(" = int: Excepted a int.");
}

template <>
void EXP_Attribute::PrintSetterError<short>() const
{
	PrintError(" = int: Excepted a int.");
}

template <>
void EXP_Attribute::PrintSetterError<unsigned short>() const
{
	PrintError(" = int: Excepted a int.");
}

template <>
void EXP_Attribute::PrintSetterError<float>() const
{
	PrintError(" = float: Excepted a float.");
}

template <>
void EXP_Attribute::PrintSetterError<std::string>() const
{
	PrintError(" = str: Excepted a string.");
}

template <>
void EXP_Attribute::PrintSetterError<MT_Vector2>() const
{
	PrintError(" = Vector: Excepted a 2d vector.");
}

template <>
void EXP_Attribute::PrintSetterError<MT_Vector3>() const
{
	PrintError(" = Vector: Excepted a 3d vector.");
}

#endif  // __EXP_ATTRIBUTE_H__
