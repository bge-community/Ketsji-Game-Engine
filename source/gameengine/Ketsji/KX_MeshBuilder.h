#ifndef __KX_MESH_BUILDER_H__
#define __KX_MESH_BUILDER_H__

#include "KX_Mesh.h"
#include "RAS_VertexFactory.h"
#include "RAS_IDisplayArray.h"

#include "EXP_ListValue.h"

class KX_BlenderMaterial;

class KX_MeshBuilderSlot : public EXP_Value
{
	Py_Header

private:
	KX_BlenderMaterial *m_material;
	RAS_IDisplayArray::PrimitiveType m_primitive;
	RAS_VertexFormat m_format;
	std::unique_ptr<RAS_IVertexFactory> m_factory;

	RAS_IDisplayArray::IVertexDataList m_vertices;
	RAS_IDisplayArray::IndexList m_primitiveIndices;
	RAS_IDisplayArray::IndexList m_triangleIndices;

public:
	KX_MeshBuilderSlot(KX_BlenderMaterial *material, RAS_IDisplayArray::PrimitiveType primitiveType, const RAS_VertexFormat& format);
	~KX_MeshBuilderSlot();

	virtual std::string GetName();

	KX_BlenderMaterial *GetMaterial() const;
	void SetMaterial(KX_BlenderMaterial *material);

	/// Return true if the number of vertices or indices doesn't match the primitive type used.
	bool Invalid() const;

	/// Create a display array with data contained in this slot.
	RAS_IDisplayArray *GetDisplayArray() const;

#ifdef WITH_PYTHON

	template <class ListType, ListType KX_MeshBuilderSlot::*List>
	static int get_size_cb(void *self_v);
	static PyObject *get_item_vertices_cb(void *self_v, int index);
	template <RAS_IDisplayArray::IndexList KX_MeshBuilderSlot::*List>
	static PyObject *get_item_indices_cb(void *self_v, int index);

	static PyObject *pyattr_get_vertices(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static PyObject *pyattr_get_indices(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static PyObject *pyattr_get_triangleIndices(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static PyObject *pyattr_get_material(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_material(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef, PyObject *value);
	static PyObject *pyattr_get_uvCount(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static PyObject *pyattr_get_colorCount(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);
	static PyObject *pyattr_get_primitive(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);

	EXP_PYMETHOD(KX_MeshBuilderSlot, AddVertex);
	EXP_PYMETHOD_VARARGS(KX_MeshBuilderSlot, RemoveVertex);
	EXP_PYMETHOD_O(KX_MeshBuilderSlot, AddIndex);
	EXP_PYMETHOD_O(KX_MeshBuilderSlot, AddPrimitiveIndex);
	EXP_PYMETHOD_VARARGS(KX_MeshBuilderSlot, RemovePrimitiveIndex);
	EXP_PYMETHOD_O(KX_MeshBuilderSlot, AddTriangleIndex);
	EXP_PYMETHOD_VARARGS(KX_MeshBuilderSlot, RemoveTriangleIndex);

#endif  // WITH_PYTHON
};

class KX_MeshBuilder : public EXP_Value
{
	Py_Header

private:
	std::string m_name;

	EXP_ListValue<KX_MeshBuilderSlot> m_slots;
	RAS_Mesh::LayersInfo m_layersInfo;
	RAS_VertexFormat m_format;

	KX_Scene *m_scene;

public:
	KX_MeshBuilder(const std::string& name, KX_Scene *scene, const RAS_Mesh::LayersInfo& layersInfo,
			const RAS_VertexFormat& format);
	~KX_MeshBuilder();

	virtual std::string GetName();

	EXP_ListValue<KX_MeshBuilderSlot>& GetSlots();

#ifdef WITH_PYTHON

	static PyObject *pyattr_get_slots(EXP_PyObjectPlus *self_v, const EXP_PYATTRIBUTE_DEF *attrdef);

	EXP_PYMETHOD(KX_MeshBuilder, AddMaterial);
	EXP_PYMETHOD_NOARGS(KX_MeshBuilder, Finish);

#endif  // WITH_PYTHON
};

#endif  // __KX_MESH_BUILDER_H__
