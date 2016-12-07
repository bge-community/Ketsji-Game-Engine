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

/** \file gameengine/GameLogic/SCA_ReplaceMeshActuator.cpp
 *  \ingroup gamelogic
 *
 * Replace the mesh for this actuator's parent
 */

#include <stddef.h>

#include "SCA_ReplaceMeshActuator.h"
#include "KX_MeshProxy.h"

#include "EXP_PyObjectPlus.h" 


SCA_ReplaceMeshActuator::SCA_ReplaceMeshActuator(SCA_IObject *gameobj,
                                                 class RAS_MeshObject *mesh,
                                                 SCA_IScene *scene,
                                                 bool use_gfx,
                                                 bool use_phys)
	: SCA_IActuator(gameobj, SCA_ACT_REPLACE_MESH),
	m_mesh(mesh),
	m_scene(scene),
	m_use_gfx(use_gfx),
	m_use_phys(use_phys)
{

}

SCA_ReplaceMeshActuator::~SCA_ReplaceMeshActuator()
{ 
	// there's nothing to be done here, really....
}

bool SCA_ReplaceMeshActuator::Update()
{
	bool bNegativeEvent = IsNegativeEvent();
	RemoveAllEvents();

	if (bNegativeEvent)
		return false; // do nothing on negative events

	if (m_mesh || m_use_phys) /* NULL mesh is ok if were updating physics */
		m_scene->ReplaceMesh(GetParent(), m_mesh, m_use_gfx, m_use_phys);

	return false;
}

CValue *SCA_ReplaceMeshActuator::GetReplica()
{
	SCA_ReplaceMeshActuator *replica = new SCA_ReplaceMeshActuator(*this);

	if (replica == NULL)
		return NULL;

	replica->ProcessReplica();

	return replica;
}

void SCA_ReplaceMeshActuator::InstantReplaceMesh()
{
	if (m_mesh)
		m_scene->ReplaceMesh(GetParent(), m_mesh, m_use_gfx, m_use_phys);
}


#ifdef WITH_PYTHON

/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */

/* Integration hooks ------------------------------------------------------- */

PyTypeObject SCA_ReplaceMeshActuator::Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"SCA_ReplaceMeshActuator",
	sizeof(PyObjectPlus_Proxy),
	0,
	py_base_dealloc,
	0,
	0,
	0,
	0,
	py_base_repr,
	0,0,0,0,0,0,0,0,0,
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	0,0,0,0,0,0,0,
	Methods,
	0,
	0,
	&SCA_IActuator::Type,
	0,0,0,0,0,0,
	py_base_new
};

PyMethodDef SCA_ReplaceMeshActuator::Methods[] = {
	KX_PYMETHODTABLE(SCA_ReplaceMeshActuator, instantReplaceMesh),
	{NULL,NULL} //Sentinel
};

PyAttributeDef SCA_ReplaceMeshActuator::Attributes[] = {
	KX_PYATTRIBUTE_RW_FUNCTION("mesh", SCA_ReplaceMeshActuator, pyattr_get_mesh, pyattr_set_mesh),
	KX_PYATTRIBUTE_BOOL_RW    ("useDisplayMesh", SCA_ReplaceMeshActuator, m_use_gfx),
	KX_PYATTRIBUTE_BOOL_RW    ("usePhysicsMesh", SCA_ReplaceMeshActuator, m_use_phys),
	{ NULL }	//Sentinel
};

PyObject *SCA_ReplaceMeshActuator::pyattr_get_mesh(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_ReplaceMeshActuator *actuator = static_cast<SCA_ReplaceMeshActuator *>(self);
	if (!actuator->m_mesh)
		Py_RETURN_NONE;
	KX_MeshProxy *meshproxy = new KX_MeshProxy(actuator->m_mesh);
	return meshproxy->NewProxy(true);
}

int SCA_ReplaceMeshActuator::pyattr_set_mesh(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
	SCA_ReplaceMeshActuator *actuator = static_cast<SCA_ReplaceMeshActuator *>(self);
	RAS_MeshObject *new_mesh;

	if (!ConvertPythonToMesh(actuator->GetLogicManager(), value, &new_mesh, true, "actuator.mesh = value: SCA_ReplaceMeshActuator"))
		return PY_SET_ATTR_FAIL;

	actuator->m_mesh = new_mesh;
	return PY_SET_ATTR_SUCCESS;
}

KX_PYMETHODDEF_DOC(SCA_ReplaceMeshActuator, instantReplaceMesh, "instantReplaceMesh() : immediately replace mesh without delay\n")
{
	InstantReplaceMesh();
	Py_RETURN_NONE;
}

#endif // WITH_PYTHON

/* eof */
