#include "pybind11/pybind11.h"

#include "pybind.h"

#include "CM_Message.h"

namespace py = pybind11;

PyMethodDef TestCValue::Methods[] = {
	KX_PYMETHODTABLE(TestCValue, func),
// 	KX_PYMETHODTABLE(TestCValue, setSourceList),
	{nullptr, nullptr} //Sentinel
};

PyAttributeDef TestCValue::Attributes[] = {
	KX_PYATTRIBUTE_RW_FUNCTION("value", TestCValue, pyattr_get, pyattr_set),
	KX_PYATTRIBUTE_NULL //Sentinel
};

PyTypeObject TestCValue::Type = {
	PyVarObject_HEAD_INIT(nullptr, 0)
	"TestCValue",
	sizeof(PyObjectPlus_Proxy),
	0,
	py_base_dealloc,
	0,
	0,
	0,
	0,
	py_base_repr,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	0, 0, 0, 0, 0, 0, 0,
	Methods,
	0,
	0,
	&CValue::Type,
	0, 0, 0, 0, 0, 0,
	py_base_new
};

PyObject *TestCValue::pyattr_get(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	TestCValue* self = static_cast<TestCValue*>(self_v);
	return PyBool_FromLong(self->m_val);
}

int TestCValue::pyattr_set(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
	TestCValue* self = static_cast<TestCValue*>(self_v);
	int param = PyObject_IsTrue(value);
	if (param == -1) {
		PyErr_SetString(PyExc_AttributeError, "shader.enabled = bool: TestCValue, expected True or False");
		return PY_SET_ATTR_FAIL;
	}

	self->m_val = param;

	return PY_SET_ATTR_SUCCESS;
}

KX_PYMETHODDEF_DOC(TestCValue, func, " setSource(vertexProgram, fragmentProgram, apply)")
{
	int a1;
	char *a2;
	float a3;
	if (!PyArg_ParseTuple(args, "isf:setSource", &a1, &a2, &a3)) {
		return nullptr;
	}
	Py_RETURN_NONE;
}

static py::object newTestCValue()
{
	return py::object((new TestCValue())->NewProxy(true), true);
}

PYBIND11_PLUGIN(bgetest) {
	py::module m("bgetest", "py example plugin");
	CM_FunctionDebug("create module");

	m.def("cvalue", &newTestCValue);

	py::class_<TestPyBind> c1(m, "TestPyBind");
	c1.def(py::init<>());
// 	c1.def_property("value", &TestPyBind::get, &TestPyBind::set);
	c1.def_readwrite("value", &TestPyBind::m_val);
	c1.def("func", &TestPyBind::func);

	return m.ptr();
}

PyObject *pybindinit()
{
	CM_FunctionDebug("");
	return pybind11_init();
}
