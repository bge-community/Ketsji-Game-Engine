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

/** \file KX_CollisionSensor.h
 *  \ingroup ketsji
 *  \brief Senses touch and collision events
 */

#ifndef __KX_TOUCHSENSOR_H__
#define __KX_TOUCHSENSOR_H__

#include "SCA_ISensor.h"
#include "EXP_ListValue.h"

class PHY_CollData;

#include "KX_ClientObjectInfo.h"

#if defined(_WIN64)
typedef unsigned __int64 uint_ptr;
#else
typedef unsigned long uint_ptr;
#endif

class KX_CollisionEventManager;

class KX_CollisionSensor : public SCA_ISensor
{
protected:
	Py_Header

	/**
	 * The sensor should only look for objects with this property.
	 */
	std::string m_touchedpropname;
	bool m_bFindMaterial;
	bool m_bCollisionPulse;                         /* changes in the colliding objects trigger pulses */

	class PHY_IPhysicsController *   m_physCtrl;

	bool m_bCollision;
	bool m_bTriggered;
	bool m_bLastTriggered;

	// Use with m_bCollisionPulse to detect changes
	int m_bLastCount;                           /* size of m_colliders last tick */
	uint_ptr m_bColliderHash;                   /* hash collision objects pointers to trigger in case one object collides and another takes its place */
	uint_ptr m_bLastColliderHash;

	SCA_IObject *m_hitObject;
	class CListValue *       m_colliders;
	std::string m_hitMaterial;

public:
	KX_CollisionSensor(class SCA_EventManager *eventmgr,
	                   class KX_GameObject *gameobj,
	                   bool bFindMaterial,
	                   bool bCollisionPulse,
	                   const std::string& touchedpropname);
	virtual ~KX_CollisionSensor();

	virtual CValue *GetReplica();
	virtual void ProcessReplica();
	virtual void SynchronizeTransform();
	virtual bool Evaluate();
	virtual void Init();
	virtual void ReParent(SCA_IObject *parent);

	virtual void RegisterSumo(KX_CollisionEventManager *collisionman);
	virtual void UnregisterSumo(KX_CollisionEventManager *collisionman);
	virtual void UnregisterToManager();

	virtual bool NewHandleCollision(void *obj1, void *obj2, const PHY_CollData *colldata);

	// Allows to do pre-filtering and save computation time
	// obj1 = sensor physical controller, obj2 = physical controller of second object
	// return value = true if collision should be checked on pair of object
	virtual bool BroadPhaseFilterCollision(void *obj1, void *obj2)
	{
		return true;
	}
	virtual bool BroadPhaseSensorFilterCollision(void *obj1, void *obj2);
	virtual sensortype GetSensorType()
	{
		return ST_TOUCH;
	}


	virtual bool IsPositiveTrigger()
	{
		bool result = m_bTriggered;
		if (m_invert) {
			result = !result;
		}
		return result;
	}

	virtual void EndFrame();

	class PHY_IPhysicsController *GetPhysicsController()
	{
		return m_physCtrl;
	}


#ifdef WITH_PYTHON

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	static PyObject *pyattr_get_object_hit(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static PyObject *pyattr_get_object_hit_list(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);

#endif

};

#endif  /* __KX_TOUCHSENSOR_H__ */
