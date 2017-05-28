#include "EXP_Value.h"

class TestCValue : public CValue
{
	Py_Header

	bool m_val;

public:
	std::string GetName()
	{
		return "CValue";
	}

	static PyObject *pyattr_get(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);

	KX_PYMETHOD_DOC(TestCValue, func);
// 	KX_PYMETHOD_DOC(TestCValue, funcK);
};

class TestPyBind
{
public:
	bool m_val;

	bool get() const
	{
		return m_val;
	}

	void set(bool val)
	{
		m_val = val;
	}

	void func(int a1, const std::string& a2, float a3)
	{
	}
};

PyObject *pybindinit();
