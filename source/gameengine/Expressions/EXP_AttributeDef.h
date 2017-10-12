#ifndef __EXP_ATTRIBUTE_DEF_H__
#define __EXP_ATTRIBUTE_DEF_H__

#include <string>

#include "EXP_Attribute.h"
#include "EXP_PythonUtils.h"

class EXP_AttributeDef
{
public:
	enum Type
	{
		TYPE_BOOL = 0,
		TYPE_SHORT,
		TYPE_USHORT,
		TYPE_INT,
		TYPE_UINT,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_VECTOR2,
		TYPE_VECTOR3,
		TYPE_CUSTOM
	};

	enum GetSetFlags
	{
		GETSET_CHECK,
		GETSET_CLAMP,
		GETSET_CUSTOM,
		GETSET_READONLY
	};

private:
	EXP_Attribute m_attribute;

	template <typename Type, GetSetFlags Flags>
	static PyObject *Getter(PyObject *self_py, const EXP_Attribute *attrdef)
	{
		PyObjectPlus *self = BGE_PROXY_REF(self_py);
		if (!EXP_Attribute::IsValid(self)) {
			return nullptr;
		}

		if (Flags & GETSET_CUSTOM) {
			Type temp = (Type)attrdef->m_customGetter(self, attrdef);
			return EXP_ConvertToPython<Type>(&temp);
		}
		return EXP_ConvertToPython<Type>(attrdef->PtrFromOffset<Type>(self));
	}

	template <typename Type, GetSetFlags Flags>
	static int Setter(PyObject *self_py, PyObject *value, const EXP_Attribute *attrdef)
	{
		PyObjectPlus *self = BGE_PROXY_REF(self_py);
		if (!EXP_Attribute::IsValid(self_py)) {
			return PY_SET_ATTR_FAIL;
		}

		bool success = false;
		Type temp;
		if (EXP_ConvertFromPython<Type>(value, &temp)) {
			if (Flags & GETSET_CLAMP) {
				CLAMP(temp, attrdef->m_borders[0], attrdef->m_borders[1]);
			}

			if (Flags & GETSET_CUSTOM) {
				success = attrdef->m_customSetter(self, &temp, attrdef);
			}
			else {
				Type *ptr = attrdef->PtrFromOffset<Type>(self);
				*ptr = temp;
				success = true;
			}

			if (Flags & GETSET_CHECK && success) {
				if (attrdef->m_customCheck(self, attrdef)) {
					success = false;
				}
			}
		}

		if (!success) {
			attrdef->PrintSetterError<Type>();
			return PY_SET_ATTR_FAIL;
		}

		return PY_SET_ATTR_SUCCESS;
	}

public:
	template <typename Type, GetSetFlags Flags>
	EXP_AttributeDef(const std::string& className, const std::string& name, intptr_t offset,
			EXP_Attribute::CustomGetterFunction customGetter, EXP_Attribute::CustomSetterFunction customSetter,
			EXP_Attribute::EXP_Attribute::CustomCheckFunction customCheck)
	{
		EXP_Attribute::GetterFunction getter = Getter<Type, Flags>;
		EXP_Attribute::SetterFunction setter = (Flags & GETSET_READONLY) ? nullptr : Setter<Type, Flags>;
		m_attribute = EXP_Attribute(className, name, getter, setter, offset, {0.0f, 0.0f}, customGetter, customSetter, customCheck);
	}

	template <typename Type, GetSetFlags Flags>
	EXP_AttributeDef(const std::string& className, const std::string& name, intptr_t offset)
		:EXP_AttributeDef(className, name, offset, nullptr, nullptr, nullptr)
	{
	}

	template <typename Type, GetSetFlags Flags>
	EXP_AttributeDef(const std::string& className, const std::string& name, EXP_Attribute::CustomGetterFunction customGetter,
			EXP_Attribute::CustomSetterFunction customSetter, EXP_Attribute::CustomCheckFunction customCheck)
		:EXP_AttributeDef(className, name, 0, customGetter, customSetter, customCheck)
	{
	}

	EXP_Attribute GetAttribute() const;
};

#define EXP_ATTRIBUTE_RW(type, class, name, member) \
	EXP_AttributeDef<type, EXP_Attribute::GETSET_NONE>(#class, name, offsetof(class, member)).GetAttribute()

#define EXP_ATTRIBUTE_RO(type, class, name, member) \
	EXP_AttributeDef<type, EXP_Attribute::GETSET_READONLY>(#class, name, offsetof(class, member)).GetAttribute()

#define EXP_ATTRIBUTE_RW_CUSTOM(type, class, name, getter, setter) \
	EXP_AttributeDef<type, EXP_Attribute::GETSET_NONE>(#class, name, getter, setter, nullptr).GetAttribute()

#define EXP_ATTRIBUTE_RO_CUSTOM(type, class, name, getter) \
	EXP_AttributeDef<type, EXP_Attribute::GETSET_NONE>(#class, name, getter, nullptr, nullptr).GetAttribute()

#define EXP_ATTRIBUTE_RW_CUSTOM_CHECK(type, class, name, getter, setter, check) \
	EXP_AttributeDef<type, EXP_Attribute::GETSET_NONE>(#class, name, getter, setter, check).GetAttribute()

#define EXP_ATTRIBUTE_NULL \
	EXP_Attribute()

#endif  // __EXP_ATTRIBUTE_DEF_H__
