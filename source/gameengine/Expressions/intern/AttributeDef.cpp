#include "EXP_AttributeDef.h"

bool EXP_AttributeDef::IsValid(PyObjectPlus *self)
{
	if (!self || !self->py_is_valid()) {
		PyErr_SetString(PyExc_SystemError, BGE_PROXY_ERROR_MSG);
		return false;
	}

	return true;
}

void EXP_AttributeDef::PrintError(const std::string& msg)
{
	PyErr_SetString(PyExc_AttributeError, (m_className + "." + m_name + msg).c_str());
}

EXP_Attribute EXP_AttributeDef::GetAttribute() const
{
	return m_attribute;
}
