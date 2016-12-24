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
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file SCA_CameraActuator.h
 *  \ingroup gamelogic
 */

#ifndef __SCA_CAMERAACTUATOR_H__
#define __SCA_CAMERAACTUATOR_H__

#include "SCA_IActuator.h"
#include "SCA_LogicManager.h"


 // The camera actuator does a Robbie Muller perspective for you. This is a
 // weird set of rules that positions the camera sort of behind the object,
 // tracking, while avoiding any objects between the 'ideal' position and the
 // actor being tracked.

class SCA_CameraActuator : public SCA_IActuator
{
	Py_Header

private :
	// Object that will be tracked.
	SCA_IObject *m_ob;
	// height
	float m_height;
	// min height
	float m_minHeight;
	// max height
	float m_maxHeight;
	// axis the camera tries to get behind: +x/+y/-x/-y
	short m_axis;
	// damping
	float m_damping;

public:
	 //Set the bool toggle to true to use x lock, false for y lock
	SCA_CameraActuator(SCA_IObject *gameobj,
	                   SCA_IObject *ob,
	                   float height,
	                   float minheight,
	                   float maxheight,
	                   short axis,
	                   float damping);

	~SCA_CameraActuator();

	CValue *GetReplica();
	virtual void ProcessReplica();
	virtual bool Update(double curtime, bool frame);
	virtual bool UnlinkObject(SCA_IObject *clientobj);
	virtual void Relink(std::map<void *, void *> &obj_map);

#ifdef WITH_PYTHON
	// set&get object to look at */
	static PyObject *pyattr_get_object(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_object(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);
#endif  /* WITH_PYTHON */
};

#endif  /* __SCA_CAMERAACTUATOR_H__ */
