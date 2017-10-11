/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file EXP_PyObjectPlus.h
 *  \ingroup expressions
 */

#ifndef __EXP_PYOBJECTPLUS_H__
#define __EXP_PYOBJECTPLUS_H__

// For now keep weakrefs optional.
#define USE_WEAKREFS

#include "EXP_Python.h"
#include "EXP_Attribute.h"
#include <string>
#include "MT_Vector3.h"
#include <stddef.h>

#ifdef WITH_PYTHON
#ifdef USE_MATHUTILS
extern "C" {
#  include "../../blender/python/mathutils/mathutils.h" // So we can have mathutils callbacks.
}
#endif

#define MAX_PROP_NAME 64

// Use with ShowDeprecationWarning macro.
typedef struct {
	bool warn_done;
	void *link;
} WarnLink;

#define ShowDeprecationWarning(old_way, new_way) \
	{ \
		static WarnLink wlink = {false, nullptr}; \
		if ((PyObjectPlus::m_ignore_deprecation_warnings || wlink.warn_done) == 0) \
		{ \
			PyObjectPlus::ShowDeprecationWarning_func(old_way, new_way); \
\
			WarnLink *wlink_last = PyObjectPlus::GetDeprecationWarningLinkLast(); \
			wlink.warn_done = true; \
			wlink.link = nullptr; \
\
			if (wlink_last) { \
				wlink_last->link = (void *)&(wlink); \
				PyObjectPlus::SetDeprecationWarningLinkLast(&(wlink)); \
			} \
			else { \
				PyObjectPlus::SetDeprecationWarningFirst(&(wlink)); \
				PyObjectPlus::SetDeprecationWarningLinkLast(&(wlink)); \
			} \
		} \
	}

typedef struct PyObjectPlus_Proxy {
	/// Required python macro.
	PyObject_HEAD
	/// Pointer to GE object, it holds a reference to this proxy.
	class PyObjectPlus *ref;
	/// Optional pointer to generic structure, the structure holds no reference to this proxy.
	void *ptr;
	/// True if the object pointed by ref should be deleted when the proxy is deleted.
	bool py_owns;
	/// True if proxy is connected to a GE object (ref is used).
	bool py_ref;
#ifdef USE_WEAKREFS
	/// Weak reference enabler.
	PyObject *in_weakreflist;
#endif
} PyObjectPlus_Proxy;

#define BGE_PROXY_ERROR_MSG "Blender Game Engine data has been freed, cannot use this python variable"
#define BGE_PROXY_REF(_self) (((PyObjectPlus_Proxy *)_self)->ref)
#define BGE_PROXY_PTR(_self) (((PyObjectPlus_Proxy *)_self)->ptr)
#define BGE_PROXY_PYOWNS(_self) (((PyObjectPlus_Proxy *)_self)->py_owns)
#define BGE_PROXY_PYREF(_self) (((PyObjectPlus_Proxy *)_self)->py_ref)
#ifdef USE_WEAKREFS
#  define BGE_PROXY_WKREF(_self) (((PyObjectPlus_Proxy *)_self)->in_weakreflist)
#endif

/// Note, sometimes we don't care what BGE type this is as long as its a proxy.
#define BGE_PROXY_CHECK_TYPE(_type) ((_type)->tp_dealloc == PyObjectPlus::py_base_dealloc)

/// Opposite of BGE_PROXY_REF.
#define BGE_PROXY_FROM_REF(_self) (((PyObjectPlus *)_self)->GetProxy())
/// Same as 'BGE_PROXY_REF' but doesn't incref.
#define BGE_PROXY_FROM_REF_BORROW(_self) _bge_proxy_from_ref_borrow(_self)


/** This must be the first line of each
 * PyC++ class
 * each PyC++ class must be registered in KX_PythonInitTypes.cpp
 */
#define Py_Header \
public: \
	static PyTypeObject Type; \
	static PyMethodDef Methods[]; \
	static EXP_Attribute Attributes[]; \
	virtual PyTypeObject *GetType(void) \
	{ \
		return &Type; \
	} \
	virtual PyObject *GetProxy() \
	{ \
		return GetProxyPlus_Ext(this, &Type, nullptr); \
	} \
	virtual PyObject *NewProxy(bool py_owns) \
	{ \
		return NewProxyPlus_Ext(this, &Type, nullptr, py_owns); \
	}

/** Nonzero values are an error for setattr
 * however because of the nested lookups we need to know if the errors
 * was because the attribute didnt exits of if there was some problem setting the value.
 */
#define PY_SET_ATTR_FAIL 1
#define PY_SET_ATTR_SUCCESS 0

/** These macros are helpful when embedding Python routines. The second
 * macro is one that also requires a documentation string
 */
#define KX_PYMETHOD(class_name, method_name) \
	PyObject * Py##method_name(PyObject * args, PyObject * kwds); \
	static PyObject * \
	sPy##method_name(PyObject * self, PyObject * args, PyObject * kwds) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "() - " \
							BGE_PROXY_ERROR_MSG); \
			return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(args, kwds); \
	}

#define KX_PYMETHOD_VARARGS(class_name, method_name) \
	PyObject * Py##method_name(PyObject * args); \
	static PyObject * \
	sPy##method_name(PyObject * self, PyObject * args) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "() - " \
							BGE_PROXY_ERROR_MSG); return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(args); \
	}

#define KX_PYMETHOD_NOARGS(class_name, method_name) \
	PyObject * Py##method_name(); \
	static PyObject * \
	sPy##method_name(PyObject * self) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "() - " \
							BGE_PROXY_ERROR_MSG); return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(); \
	}

#define KX_PYMETHOD_O(class_name, method_name) \
	PyObject * Py##method_name(PyObject * value); \
	static PyObject * \
	sPy##method_name(PyObject * self, PyObject * value) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "(value) - " \
							BGE_PROXY_ERROR_MSG); return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(value); \
	}

#define KX_PYMETHOD_DOC(class_name, method_name) \
	PyObject * Py##method_name(PyObject * args, PyObject * kwds); \
	static PyObject * \
	sPy##method_name(PyObject * self, PyObject * args, PyObject * kwds) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "(...) - " \
							BGE_PROXY_ERROR_MSG); return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(args, kwds); \
	} \
	static const char method_name##_doc[];

#define KX_PYMETHOD_DOC_VARARGS(class_name, method_name) \
	PyObject * Py##method_name(PyObject * args); \
	static PyObject * \
	sPy##method_name(PyObject * self, PyObject * args) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "(...) - " \
							BGE_PROXY_ERROR_MSG); \
			return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(args); \
	} \
	static const char method_name##_doc[];

#define KX_PYMETHOD_DOC_O(class_name, method_name) \
	PyObject * Py##method_name(PyObject * value); \
	static PyObject * \
	sPy##method_name(PyObject * self, PyObject * value) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "(value) - " \
							BGE_PROXY_ERROR_MSG); \
			return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(value); \
	} \
	static const char method_name##_doc[];

#define KX_PYMETHOD_DOC_NOARGS(class_name, method_name) \
	PyObject * Py##method_name(); \
	static PyObject * \
	sPy##method_name(PyObject * self) \
	{ \
		if (BGE_PROXY_REF(self) == nullptr) { \
			PyErr_SetString(PyExc_RuntimeError, \
							#class_name "." #method_name "() - " \
							BGE_PROXY_ERROR_MSG); \
			return nullptr; \
		} \
		return ((class_name *)BGE_PROXY_REF(self))->Py##method_name(); \
	} \
	static const char method_name##_doc[];

/// Method table macro (with doc).
#define KX_PYMETHODTABLE(class_name, method_name) \
	{#method_name, (PyCFunction) class_name::sPy##method_name, METH_VARARGS, (const char *)class_name::method_name##_doc}

#define KX_PYMETHODTABLE_O(class_name, method_name) \
	{#method_name, (PyCFunction) class_name::sPy##method_name, METH_O, (const char *)class_name::method_name##_doc}

#define KX_PYMETHODTABLE_NOARGS(class_name, method_name) \
	{#method_name, (PyCFunction) class_name::sPy##method_name, METH_NOARGS, (const char *)class_name::method_name##_doc}

#define KX_PYMETHODTABLE_KEYWORDS(class_name, method_name) \
	{#method_name, (PyCFunction) class_name::sPy##method_name, METH_VARARGS | METH_KEYWORDS, (const char *)class_name::method_name##_doc}

/// Function implementation macro.
#define KX_PYMETHODDEF_DOC(class_name, method_name, doc_string) \
	const char class_name::method_name##_doc[] = doc_string; \
	PyObject *class_name::Py##method_name(PyObject * args, PyObject * kwds)

#define KX_PYMETHODDEF_DOC_VARARGS(class_name, method_name, doc_string) \
	const char class_name::method_name##_doc[] = doc_string; \
	PyObject *class_name::Py##method_name(PyObject * args)

#define KX_PYMETHODDEF_DOC_O(class_name, method_name, doc_string) \
	const char class_name::method_name##_doc[] = doc_string; \
	PyObject *class_name::Py##method_name(PyObject * value)

#define KX_PYMETHODDEF_DOC_NOARGS(class_name, method_name, doc_string) \
	const char class_name::method_name##_doc[] = doc_string; \
	PyObject *class_name::Py##method_name()



/*------------------------------
 * PyObjectPlus
 *------------------------------ */
#else  // WITH_PYTHON

#define Py_Header \
public: \

#define Py_HeaderPtr \
public: \

#endif

/// The PyObjectPlus abstract class.
class PyObjectPlus
{
	Py_Header // Always start with Py_Header

protected:
	/** Called when the object is freed from a python owner proxy.
	 * It has effect to use reference count for deletion and to not
	 * be every time deleted in CValue.
	 */
	virtual void DestructFromPython();

public:
	PyObjectPlus();
	virtual ~PyObjectPlus();

#ifdef WITH_PYTHON
	PyObject *m_proxy; // Actually a PyObjectPlus_Proxy.

	/* These static functions are referenced by ALL PyObjectPlus_Proxy types
	 * they take the C++ reference from the PyObjectPlus_Proxy and call
	 * its own virtual py_repr, py_base_dealloc, etc. functions.
	 */
	static PyObject *py_base_new(PyTypeObject *type, PyObject *args, PyObject *kwds); // Allows subclassing.
	static void py_base_dealloc(PyObject *self);
	static PyObject *py_base_repr(PyObject *self);

	/* These are all virtual python methods that are defined in each class
	 * Our own fake subclassing calls these on each class, then calls the parent */
	virtual PyObject *py_repr();
	/// Subclass may overwrite this function to implement more sophisticated method of validating a proxy.
	virtual bool py_is_valid();

	static PyObject *GetProxyPlus_Ext(PyObjectPlus *self, PyTypeObject *tp, void *ptr);
	/** self=nullptr => proxy to generic pointer detached from GE object
	 * if py_owns is true, the memory pointed by ptr will be deleted automatically with MEM_freeN
	 * self!=nullptr=> proxy attached to GE object, ptr is optional and point to a struct from which attributes can be defined
	 * if py_owns is true, the object will be deleted automatically, ptr will NOT be deleted
	 * (assume object destructor takes care of it) */
	static PyObject *NewProxyPlus_Ext(PyObjectPlus *self, PyTypeObject *tp, void *ptr, bool py_owns);

	static WarnLink *GetDeprecationWarningLinkFirst();
	static WarnLink *GetDeprecationWarningLinkLast();
	static void SetDeprecationWarningFirst(WarnLink *wlink);
	static void SetDeprecationWarningLinkLast(WarnLink *wlink);
	static void NullDeprecationWarning();

	/// Enable/disable display of deprecation warnings.
	static void SetDeprecationWarnings(bool ignoreDeprecationWarnings);
	/// Shows a deprecation warning.
	static void ShowDeprecationWarning_func(const std::string& old_way, const std::string& new_way);
	static void ClearDeprecationWarning();

#endif

	void InvalidateProxy();

	/// Makes sure any internal data owned by this class is deep copied.
	virtual void ProcessReplica();

	static bool m_ignore_deprecation_warnings;
};

#ifdef WITH_PYTHON
inline PyObject *_bge_proxy_from_ref_borrow(PyObjectPlus *self_v)
{
	PyObject *self_proxy = BGE_PROXY_FROM_REF(self_v);
	/* this is typically _very_ bad practice,
	 * however we know the proxy is owned by 'self_v' */
	self_proxy->ob_refcnt--;
	return self_proxy;
}

#endif

#endif  // __EXP_PYOBJECTPLUS_H__
