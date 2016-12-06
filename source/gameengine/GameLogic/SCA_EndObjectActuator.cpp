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

/** \file gameengine/GameLogic/SCA_EndObjectActuator.cpp
 *  \ingroup gamelogic
 */

// Remove the actuator's parent when triggered


#include "SCA_IActuator.h"
#include "SCA_EndObjectActuator.h"
#include "SCA_IScene.h"

SCA_EndObjectActuator::SCA_EndObjectActuator(SCA_IObject *gameobj,
                                             SCA_IScene *scene)
	: SCA_IActuator(gameobj, SCA_ACT_END_OBJECT),
      m_scene(scene)
{
	// intentionally empty
}

SCA_EndObjectActuator::~SCA_EndObjectActuator()
{ 
	// there's nothing to be done here, really....
}

bool SCA_EndObjectActuator::Update()
{
	bool bNegativeEvent = IsNegativeEvent();
	RemoveAllEvents();

	if (bNegativeEvent)
		return false; // do nothing on negative events
	m_scene->DelayedRemoveObject(GetParent());

	return false;
}

CValue *SCA_EndObjectActuator::GetReplica()
{
	SCA_EndObjectActuator *replica = new SCA_EndObjectActuator(*this);
	if (replica == NULL)
		return NULL;

	replica->ProcessReplica();
	return replica;
}

#ifdef WITH_PYTHON

/* ------------------------------------------------------------------------- */
/* Python functions : integration hooks                                      */
/* ------------------------------------------------------------------------- */

PyTypeObject SCA_EndObjectActuator::Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"SCA_EndObjectActuator",
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

PyMethodDef SCA_EndObjectActuator::Methods[] = {
	{NULL,NULL} //Sentinel
};

PyAttributeDef SCA_EndObjectActuator::Attributes[] = {
	KX_PYATTRIBUTE_NULL	//Sentinel
};

#endif // WITH_PYTHON


