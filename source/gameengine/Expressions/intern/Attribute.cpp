#include "EXP_Attribute.h"

#include "BLI_math_vector.h"

EXP_Attribute::EXP_Attribute()
	:m_getter(nullptr),
	m_setter(nullptr),
	m_offset(0),
	m_customGetter(nullptr),
	m_customSetter(nullptr),
	m_customCheck(nullptr)
{
}

EXP_Attribute::EXP_Attribute(const std::string& className, const std::string& name, intptr_t offset, float borders[2],
		GetterFunction getter, SetterFunction setter,
		CustomGetterFunction customGetter, CustomSetterFunction customSetter, CustomCheckFunction customCheck)
	:m_className(className),
	m_name(name),
	m_getter(getter),
	m_setter(setter),
	m_offset(offset),
	m_customGetter(customGetter),
	m_customSetter(customSetter),
	m_customCheck(customCheck)
{
	copy_v2_v2(m_borders, borders);
}
