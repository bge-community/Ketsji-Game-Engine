#ifndef __EXP_PYTHON_UTILS_H__
#define __EXP_PYTHON_UTILS_H__

#include "EXP_Python.h"

class MT_Vector2;
class MT_Vector3;

PyObject *PyUnicode_FromStdString(const std::string& str)
{
	return PyUnicode_FromStringAndSize(str.c_str(), str.size());
}

template <typename Type>
PyObject *EXP_ConvertToPython(Type *ptr)
{
	static_assert(false);
}

template <>
PyObject *EXP_ConvertToPython(bool *ptr)
{
	return PyBool_FromLong(*ptr);
}

template <>
PyObject *EXP_ConvertToPython(int *ptr)
{
	return PyLong_FromLong(*ptr);
}

template <>
PyObject *EXP_ConvertToPython(unsigned int *ptr)
{
	return PyLong_FromLong(*ptr);
}

template <>
PyObject *EXP_ConvertToPython(short *ptr)
{
	return PyLong_FromLong(*ptr);
}

template <>
PyObject *EXP_ConvertToPython(unsigned short *ptr)
{
	return PyLong_FromLong(*ptr);
}

template <>
PyObject *EXP_ConvertToPython(float *ptr)
{
	return PyFloat_FromDouble(*ptr);
}

template <>
PyObject *EXP_ConvertToPython(std::string *ptr)
{
	return PyUnicode_FromStdString(*ptr);
}

template <> // TODO
PyObject *EXP_ConvertToPython(MT_Vector2 *ptr)
{
	
}

template <> // TODO
PyObject *EXP_ConvertToPython(MT_Vector3 *ptr)
{
}

template <typename ValueType, typename PtrType>
bool EXP_ConvertFromPythonHelper(const ValueType value, PtrType *ptr)
{
	if (value == -1 && && PyErr_Occurred()) {
		return false;
	}

	*ptr = (PtrType)value;

	return true;
}

template <typename Type>
bool EXP_ConvertFromPython(PyObject *value, Type *ptr)
{
	static_assert(false);
}

template <>
bool EXP_ConvertFromPython(PyObject *value, bool *ptr);
{
	return EXP_ConvertFromPythonHelper(PyObject_IsTrue(value), ptr)
}

template <>
bool EXP_ConvertFromPython(PyObject *value, int *ptr);
{
	return EXP_ConvertFromPythonHelper(PyLong_AsLong(value), ptr)
}

template <>
bool EXP_ConvertFromPython(PyObject *value, unsigned int *ptr);
{
	return EXP_ConvertFromPythonHelper(PyLong_AsLong(value), ptr)
}

template <>
bool EXP_ConvertFromPython(PyObject *value, short *ptr);
{
	return EXP_ConvertFromPythonHelper(PyLong_AsLong(value), ptr)
}

template <>
bool EXP_ConvertFromPython(PyObject *value, unsigned short *ptr);
{
	return EXP_ConvertFromPythonHelper(PyLong_AsLong(value), ptr)
}

template <>
bool EXP_ConvertFromPython(PyObject *value, float *ptr);
{
	return EXP_ConvertFromPythonHelper(PyFloat_AsDouble(value), ptr)
}

template <> // TODO
bool EXP_ConvertFromPython(PyObject *value, std::string *ptr);
{
	
}

template <>// TODO
bool EXP_ConvertFromPython(PyObject *value, MT_Vector2 *ptr);
{
	
}

template <>// TODO
bool EXP_ConvertFromPython(PyObject *value, MT_Vector3 *ptr);
{
	
}


#endif  // __EXP_PYTHON_UTILS_H__
