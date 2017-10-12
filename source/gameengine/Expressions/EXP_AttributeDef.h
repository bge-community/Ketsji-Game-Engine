#ifndef __EXP_ATTRIBUTE_DEF_H__
#define __EXP_ATTRIBUTE_DEF_H__

#include <string>

#include "EXP_PyObjectPlus.h"
#include "EXP_Attribute.h"
#include "EXP_PythonUtils.h"

enum EXP_GetSetFlags
{
	EXP_GETSET_NONE = 0,
	EXP_GETSET_CHECK = (1 << 0),
	EXP_GETSET_CLAMP = (1 << 1),
	EXP_GETSET_LIMIT = (1 << 2),
	EXP_GETSET_CUSTOM = (1 << 3),
	EXP_GETSET_READONLY = (1 << 4)
};

template <typename Type, class ParentClass>
using CustomGetterFunctionType = bool (ParentClass::*)(Type&, const EXP_Attribute *);

template <typename Type, class ParentClass>
using CustomSetterFunctionType = bool (ParentClass::*)(const Type&, const EXP_Attribute *);

template <class ParentClass>
using CustomCheckFunctionType = bool (ParentClass::*)(const EXP_Attribute *);

template <typename Type, class ParentClass, EXP_GetSetFlags Flags,
		CustomGetterFunctionType<Type, ParentClass> CustomGetterFunction = nullptr,
		CustomSetterFunctionType<Type, ParentClass> CustomSetterFunction = nullptr,
		CustomCheckFunctionType<ParentClass> CustomCheckFunction = nullptr>
class EXP_AttributeDef
{
private:
	EXP_Attribute m_attribute;

	static PyObject *Getter(PyObject *self_py, void *closure)
	{
		ParentClass *self = static_cast<ParentClass *>(BGE_PROXY_REF(self_py));
		if (!EXP_Attribute::IsValid(self)) {
			return nullptr;
		}

		const EXP_Attribute *attrdef = static_cast<EXP_Attribute *>(closure);
		if (Flags & EXP_GETSET_CUSTOM) {
			Type temp;
			if ((self->*CustomGetterFunction)(temp, attrdef)) {
				return EXP_ConvertToPython<Type>(&temp);
			}
			return nullptr;
		}
		return EXP_ConvertToPython<Type>(attrdef->PtrFromOffset<Type>(self));
	}

	static int Setter(PyObject *self_py, PyObject *value, void *closure)
	{
		ParentClass *self = static_cast<ParentClass *>(BGE_PROXY_REF(self_py));
		if (!EXP_Attribute::IsValid(self)) {
			return PY_SET_ATTR_FAIL;
		}

		const EXP_Attribute *attrdef = static_cast<EXP_Attribute *>(closure);
		bool success = false;
		Type temp;
		if (EXP_ConvertFromPython<Type>(value, &temp)) {
			/*if (Flags & EXP_GETSET_CLAMP) {
				CLAMP(temp, attrdef->m_range[0], attrdef->m_range[1]);
			}*/ // TODO EXP_GETSET_LIMIT

			if (Flags & EXP_GETSET_CUSTOM) {
				success = (self->*CustomSetterFunction)(temp, attrdef);
			}
			else {
				Type *ptr = attrdef->PtrFromOffset<Type>(self);
				*ptr = temp;
				success = true;
			}

			if (Flags & EXP_GETSET_CHECK && success) {
				success = (self->*CustomCheckFunction)(attrdef);
			}
		}

		if (!success) {
			attrdef->PrintSetterError<Type>();
			return PY_SET_ATTR_FAIL;
		}

		return PY_SET_ATTR_SUCCESS;
	}

public:
	EXP_AttributeDef(const std::string& className, const std::string& name, intptr_t offset)
	{
		EXP_Attribute::GetterFunction getter = Getter;
		EXP_Attribute::SetterFunction setter = (Flags & EXP_GETSET_READONLY) ? nullptr : Setter;
		float range[2] = {0.0f, 0.0f};
		m_attribute = EXP_Attribute(className, name, offset, range, getter, setter);
	}

	EXP_Attribute GetAttribute() const
	{
		return m_attribute;
	}
};

#define EXP_ATTRIBUTE_RW(type, class, name, member) \
	EXP_AttributeDef<type, class, EXP_GETSET_NONE>(#class, name, offsetof(class, member)).GetAttribute()

#define EXP_ATTRIBUTE_RO(type, class, name, member) \
	EXP_AttributeDef<type, class, EXP_GETSET_READONLY>(#class, name, offsetof(class, member)).GetAttribute()

#define EXP_ATTRIBUTE_RW_LIMIT(type, class, name, member) \
	EXP_AttributeDef<type, class, EXP_GETSET_LIMIT>(#class, name, offsetof(class, member)).GetAttribute()

#define EXP_ATTRIBUTE_RW_CLAMP(type, class, name, member) \
	EXP_AttributeDef<type, class, EXP_GETSET_CLAMP>(#class, name, offsetof(class, member)).GetAttribute()

#define EXP_ATTRIBUTE_RW_CHECK(type, class, name, member, check) \
	EXP_AttributeDef<type, class, EXP_GETSET_CHECK, nullptr, nullptr, &class::check>(#class, name, offsetof(class, member)).GetAttribute()


#define EXP_ATTRIBUTE_RW_CUSTOM(type, class, name, getter, setter) \
	EXP_AttributeDef<type, class, EXP_GETSET_NONE, &class::getter, &class::setter>(#class, name, 0).GetAttribute()

#define EXP_ATTRIBUTE_RO_CUSTOM(type, class, name, getter) \
	EXP_AttributeDef<type, class, EXP_GETSET_NONE, getter, setter>(#class, name, getter, nullptr, nullptr).GetAttribute()

#define EXP_ATTRIBUTE_RW_CUSTOM_CHECK(type, class, name, getter, setter, check) \
	EXP_AttributeDef<type, class, EXP_GETSET_NONE, getter, setter>(#class, name, getter, setter, check).GetAttribute()

#define EXP_ATTRIBUTE_NULL \
	EXP_Attribute()

#endif  // __EXP_ATTRIBUTE_DEF_H__
