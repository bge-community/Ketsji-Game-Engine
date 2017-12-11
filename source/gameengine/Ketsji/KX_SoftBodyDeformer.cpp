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

/** \file gameengine/Converter/KX_SoftBodyDeformer.cpp
 *  \ingroup bgeconv
 */


#ifdef _MSC_VER
#  pragma warning (disable:4786)
#endif //WIN32

#include "BLI_utildefines.h"

#include "KX_SoftBodyDeformer.h"
#include "RAS_MeshObject.h"
#include "RAS_DisplayArray.h"
#include "RAS_BoundingBoxManager.h"

#ifdef WITH_BULLET

#include "CcdPhysicsEnvironment.h"
#include "CcdPhysicsController.h"
#include "BulletSoftBody/btSoftBody.h"

#include "btBulletDynamicsCommon.h"

KX_SoftBodyDeformer::KX_SoftBodyDeformer(RAS_MeshObject *pMeshObject, BL_DeformableGameObject *gameobj)
	:RAS_Deformer(pMeshObject),
	m_gameobj(gameobj)
{
	KX_Scene *scene = m_gameobj->GetScene();
	RAS_BoundingBoxManager *boundingBoxManager = scene->GetBoundingBoxManager();
	m_boundingBox = boundingBoxManager->CreateBoundingBox();
	// Set AABB default to mesh bounding box AABB.
	m_boundingBox->CopyAabb(m_mesh->GetBoundingBox());

	m_bDynamic = true;
}

KX_SoftBodyDeformer::~KX_SoftBodyDeformer()
{
}

void KX_SoftBodyDeformer::Relink(std::map<SCA_IObject *, SCA_IObject *>& map)
{
	BL_DeformableGameObject *obj = static_cast<BL_DeformableGameObject *>(map[m_gameobj]);

	if (obj) {
		m_gameobj = obj;
		m_mesh = m_gameobj->GetMeshList().front();
	}
	else {
		m_gameobj = nullptr;
		m_mesh = nullptr;
	}
}

void KX_SoftBodyDeformer::Update()
{
	CcdPhysicsController *ctrl = (CcdPhysicsController *)m_gameobj->GetPhysicsController();
	if (!ctrl)
		return;

	btSoftBody *softBody = ctrl->GetSoftBody();
	if (!softBody)
		return;

	btSoftBody::tNodeArray&   nodes(softBody->m_nodes);
	const std::vector<unsigned int>& indices = ctrl->GetSoftBodyIndices();

	// AABB Box : min/max.
	mt::vec3 aabbMin(FLT_MAX);
	mt::vec3 aabbMax(-FLT_MAX);

	const mt::mat3x4 invtrans = m_gameobj->NodeGetWorldTransform().Inverse();
	const bool autoUpdate = m_gameobj->GetAutoUpdateBounds();

	for (DisplayArraySlot& slot : m_slots) {
		RAS_IDisplayArray *array = slot.m_displayArray;
		for (unsigned int i = 0, size = array->GetVertexCount(); i < size; ++i) {
			RAS_Vertex v = array->GetVertex(i);
			const RAS_VertexInfo& vinfo = array->GetVertexInfo(i);

			const unsigned int index = indices[vinfo.GetOrigIndex()];
			const mt::vec3 pt(ToMt(nodes[index].m_x));
			v.SetXYZ(pt);

			v.SetNormal(nodes[index].m_n.m_floats);

			if (!autoUpdate) {
				continue;
			}

			// Extract object transform from the vertex position.
			const mt::vec3 ptWorld = invtrans * pt;
			// if the AABB need an update.
			aabbMin = mt::vec3::Min(aabbMin, ptWorld);
			aabbMax = mt::vec3::Max(aabbMax, ptWorld);
		}

		const short modifiedFlag = slot.m_arrayUpdateClient.GetInvalidAndClear();
		if (modifiedFlag != RAS_IDisplayArray::NONE_MODIFIED) {
			/// Update vertex data from the original mesh.
			array->UpdateFrom(slot.m_origDisplayArray, modifiedFlag);
		}

		array->NotifyUpdate(RAS_IDisplayArray::POSITION_MODIFIED | RAS_IDisplayArray::NORMAL_MODIFIED);
	}

	if (autoUpdate) {
		m_boundingBox->SetAabb(aabbMin, aabbMax);
	}
}

bool KX_SoftBodyDeformer::NeedUpdate() const
{
	return true;
}

#endif
