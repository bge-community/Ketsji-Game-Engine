/*
 *
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

/** \file gameengine/GameLogic/SCA_ANDController.cpp
 *  \ingroup gamelogic
 */


#include "SCA_ANDController.h"
#include "SCA_ISensor.h"
#include "SCA_LogicManager.h"
#include "EXP_BoolValue.h"


SCA_ANDController::SCA_ANDController(SCA_IObject *gameobj)
	: SCA_IController(gameobj)
{
}

SCA_ANDController::~SCA_ANDController()
{
}

void SCA_ANDController::Trigger(SCA_LogicManager *logicmgr)
{
	bool sensorresult = true;

	for (std::vector<SCA_ISensor *>::const_iterator is = m_linkedsensors.begin();
	     !(is == m_linkedsensors.end());
	     is++)
	{
		SCA_ISensor *sensor = *is;
		if (!sensor->GetState()) {
			sensorresult = false;
			break;
		}
	}
	
	for (std::vector<SCA_IActuator *>::const_iterator i = m_linkedactuators.begin();
	     !(i == m_linkedactuators.end());
	     i++)
	{
		SCA_IActuator *actua = *i;
		logicmgr->AddActiveActuator(actua, sensorresult);
	}
}

CValue *SCA_ANDController::GetReplica()
{
	CValue *replica = new SCA_ANDController(*this);
	// this will copy properties and so on...
	replica->ProcessReplica();

	return replica;
}

#ifdef WITH_PYTHON

/* ------------------------------------------------------------------------- */
/* Integration hooks ------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

PyTypeObject SCA_ANDController::Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"SCA_ANDController",
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
	&SCA_IController::Type,
	0,0,0,0,0,0,
	py_base_new
};

PyMethodDef SCA_ANDController::Methods[] = {
	{NULL,NULL} //Sentinel
};

PyAttributeDef SCA_ANDController::Attributes[] = {
	KX_PYATTRIBUTE_NULL	//Sentinel
};
#endif // WITH_PYTHON
