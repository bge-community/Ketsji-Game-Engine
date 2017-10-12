#include "EXP_Attribute.h"
#include "EXP_PyObjectPlus.h"

#include "BLI_math_vector.h"

EXP_Attribute::EXP_Attribute()
	:m_getter(nullptr),
	m_setter(nullptr),
	m_offset(0)
{
}

EXP_Attribute::EXP_Attribute(const std::string& className, const std::string& name, intptr_t offset, float range[2],
		GetterFunction getter, SetterFunction setter)
	:m_className(className),
	m_name(name),
	m_getter(getter),
	m_setter(setter),
	m_offset(offset)
{
	copy_v2_v2(m_range, range);
}

bool EXP_Attribute::IsValid(PyObjectPlus *self)
{
	if (!self || !self->py_is_valid()) {
		PyErr_SetString(PyExc_SystemError, BGE_PROXY_ERROR_MSG);
		return false;
	}

	return true;
}

void EXP_Attribute::PrintError(const std::string& msg) const
{
	PyErr_SetString(PyExc_AttributeError, (m_className + "." + m_name + msg).c_str());
}
