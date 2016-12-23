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

/** \file gameengine/GameLogic/SCA_ActionActuator.cpp
 *  \ingroup gamelogic
 */

#include <string>
#include "SCA_LogicManager.h"
#include "SCA_ActionActuator.h"
#include "BL_ArmatureObject.h"
#include "BL_SkinDeformer.h"
#include "BL_Action.h"
#include "BL_ActionManager.h"
#include "KX_GameObject.h"
#include "KX_PyMath.h"
#include "MEM_guardedalloc.h"
#include "DNA_nla_types.h"
#include "DNA_action_types.h"
#include "DNA_armature_types.h"
#include "DNA_scene_types.h"
#include "BLI_blenlib.h"
#include "BLI_math.h"
#include "BLI_utildefines.h"
#include "MT_Matrix4x4.h"
#include "EXP_FloatValue.h"
#include "EXP_PyObjectPlus.h"

extern "C" {
#include "BKE_animsys.h"
#include "BKE_action.h"
#include "RNA_access.h"
#include "RNA_define.h"
}

SCA_ActionActuator::SCA_ActionActuator(SCA_IObject *gameobj,
                                       const std::string &propname,
                                       const std::string &framepropname,
                                       float starttime,
                                       float endtime,
                                       struct bAction *action,
                                       short playtype,
                                       short blend_mode,
                                       short blendin,
                                       short priority,
                                       short layer,
                                       float layer_weight,
                                       short ipo_flags,
                                       short end_reset,
                                       float stride)
	: SCA_IActuator(gameobj, SCA_ACT_ACTION),
      m_lastpos(0, 0, 0),
      m_blendframe(0),
      m_flag(0),
      m_startframe (starttime),
      m_endframe(endtime) ,
      m_starttime(0),
      m_localtime(starttime),
      m_lastUpdate(-1),
      m_blendin(blendin),
      m_blendstart(0),
      m_stridelength(stride),
      m_layer_weight(layer_weight),
      m_playtype(playtype),
      m_blendmode(blend_mode),
      m_priority(priority),
      m_layer(layer),
      m_ipo_flags(ipo_flags),
      m_action(action),
      m_propname(propname),
      m_framepropname(framepropname)
{
	if (!end_reset)
		m_flag |= ACT_FLAG_CONTINUE;
}

SCA_ActionActuator::~SCA_ActionActuator()
{
}

void SCA_ActionActuator::ProcessReplica()
{
	SCA_IActuator::ProcessReplica();

	m_localtime = m_startframe;
	m_lastUpdate = -1;
}

void SCA_ActionActuator::SetBlendTime(float newtime)
{
	m_blendframe = newtime;
}

void SCA_ActionActuator::SetLocalTime(float curtime)
{
	float dt = (curtime - m_starttime) * KX_KetsjiEngine::GetAnimFrameRate();

	if (m_endframe < m_startframe)
		dt = -dt;

	m_localtime = m_startframe + dt;

	// Handle wrap around
	if (m_localtime < std::min(m_startframe, m_endframe) || m_localtime > std::max(m_startframe, m_endframe)) {
		switch (m_playtype) {
			case ACT_ACTION_PLAY:
			{
				// Clamp
				m_localtime = m_endframe;
				break;
			}
			case ACT_ACTION_LOOP_END:
			{
				// Put the time back to the beginning
				m_localtime = m_startframe;
				m_starttime = curtime;
				break;
			}
			case ACT_ACTION_PINGPONG:
			{
				// Swap the start and end frames
				float temp = m_startframe;
				m_startframe = m_endframe;
				m_endframe = temp;
				m_starttime = curtime;
				m_flag ^= ACT_FLAG_REVERSE;
				break;
			}
		}
	}
}

void SCA_ActionActuator::ResetStartTime(float curtime)
{
	float dt = m_localtime - m_startframe;

	m_starttime = curtime - dt / (KX_KetsjiEngine::GetAnimFrameRate());
	//SetLocalTime(curtime);
}

CValue *SCA_ActionActuator::GetReplica()
{
	SCA_ActionActuator *replica = new SCA_ActionActuator(*this);
	replica->ProcessReplica();
	return replica;
}

bool SCA_ActionActuator::Update(double curtime, bool frame)
{
	bool bNegativeEvent = false;
	bool bPositiveEvent = false;
	bool bUseContinue = false;
	KX_GameObject *obj = (KX_GameObject *)GetParent();
	short playtype = BL_Action::ACT_MODE_PLAY;
	short blendmode = (m_blendmode == ACT_ACTION_ADD) ? BL_Action::ACT_BLEND_ADD : BL_Action::ACT_BLEND_BLEND;
	float start = m_startframe;
	float end = m_endframe;

	// If we don't have an action, we can't do anything
	if (!m_action)
		return false;

	// Convert our playtype to one that BL_Action likes
	switch (m_playtype) {
		case ACT_ACTION_LOOP_END:
		case ACT_ACTION_LOOP_STOP:
		{
			playtype = BL_Action::ACT_MODE_LOOP;
			break;
		}
		case ACT_ACTION_PINGPONG:
		{
			// We handle ping pong ourselves to increase compabitility
			// with files made prior to animation changes from GSoC 2011.
			playtype = BL_Action::ACT_MODE_PLAY;
		
			if (m_flag & ACT_FLAG_REVERSE) {
				start = m_endframe;
				end = m_startframe;
			}

			break;
		}
		case ACT_ACTION_FROM_PROP:
		{
			CValue *prop = GetParent()->GetProperty(m_propname);

			// If we don't have a property, we can't do anything, so just bail
			if (!prop)
				return false;

			playtype = BL_Action::ACT_MODE_PLAY;
			start = end = prop->GetNumber();

			break;
		}
	}

	if (m_flag & ACT_FLAG_CONTINUE)
		bUseContinue = true;
	
	
	// Handle events
	if (frame) {
		bNegativeEvent = m_negevent;
		bPositiveEvent = m_posevent;
		RemoveAllEvents();
	}

	// "Active" actions need to keep updating their current frame
	if (bUseContinue && (m_flag & ACT_FLAG_ACTIVE))
		m_localtime = obj->GetActionFrame(m_layer);

	if (m_flag & ACT_FLAG_ATTEMPT_PLAY)
		SetLocalTime(curtime);
	else
		ResetStartTime(curtime);

	// Handle a frame property if it's defined
	if ((m_flag & ACT_FLAG_ACTIVE) && m_framepropname[0] != 0) {
		CValue *oldprop = obj->GetProperty(m_framepropname);
		CValue *newval = new CFloatValue(obj->GetActionFrame(m_layer));
		if (oldprop)
			oldprop->SetValue(newval);
		else
			obj->SetProperty(m_framepropname, newval);

		newval->Release();
	}

	// Handle a finished animation
	if ((m_flag & ACT_FLAG_PLAY_END) && (m_flag & ACT_FLAG_ACTIVE) && obj->IsActionDone(m_layer)) {
		m_flag &= ~ACT_FLAG_ACTIVE;

		if (m_playtype == ACT_ACTION_PINGPONG) {
			m_flag ^= ACT_FLAG_REVERSE;
		}
		else {
			m_flag &= ~ACT_FLAG_ATTEMPT_PLAY;
			return false;
		}
	}
	
	// If a different action is playing, we've been overruled and are no longer active
	if (obj->GetCurrentAction(m_layer) != m_action && !obj->IsActionDone(m_layer))
		m_flag &= ~ACT_FLAG_ACTIVE;

	if (bPositiveEvent || (m_flag & ACT_FLAG_ATTEMPT_PLAY && !(m_flag & ACT_FLAG_ACTIVE))) {
		if (bPositiveEvent && m_playtype == ACT_ACTION_PLAY) {
			if (obj->IsActionDone(m_layer))
				m_localtime = start;
			ResetStartTime(curtime);
		}

		if (obj->PlayAction(m_action->id.name + 2, start, end, m_layer,
		                    m_priority, m_blendin, playtype, m_layer_weight,
		                    m_ipo_flags, 1.0f, blendmode))
		{
			m_flag |= ACT_FLAG_ACTIVE;
			if (bUseContinue)
				obj->SetActionFrame(m_layer, m_localtime);

			if (m_playtype == ACT_ACTION_PLAY || m_playtype == ACT_ACTION_PINGPONG)
				m_flag |= ACT_FLAG_PLAY_END;
			else
				m_flag &= ~ACT_FLAG_PLAY_END;
		}
		m_flag |= ACT_FLAG_ATTEMPT_PLAY;
	}
	else if ((m_flag & ACT_FLAG_ACTIVE) && bNegativeEvent) {
		m_flag &= ~ACT_FLAG_ATTEMPT_PLAY;
		m_localtime = obj->GetActionFrame(m_layer);
		bAction *curr_action = obj->GetCurrentAction(m_layer);
		if (curr_action && curr_action != m_action) {
			// Someone changed the action on us, so we wont mess with it
			// Hopefully there wont be too many problems with two actuators using
			// the same action...
			m_flag &= ~ACT_FLAG_ACTIVE;
			return false;
		}

		switch (m_playtype) {
			case ACT_ACTION_FROM_PROP:
			case ACT_ACTION_LOOP_STOP:
			{
				obj->StopAction(m_layer); // Stop the action after getting the frame

				// We're done
				m_flag &= ~ACT_FLAG_ACTIVE;
				return false;
			}
			case ACT_ACTION_LOOP_END:
			{
				// Convert into a play and let it finish
				obj->SetPlayMode(m_layer, BL_Action::ACT_MODE_PLAY);

				m_flag |= ACT_FLAG_PLAY_END;
				break;
			}
			case ACT_ACTION_FLIPPER:
			{
				// Convert into a play action and play back to the beginning
				float temp = end;
				end = start;
				start = curr_action ? obj->GetActionFrame(m_layer) : temp;
				obj->PlayAction(m_action->id.name + 2, start, end, m_layer,
				                m_priority, 0, BL_Action::ACT_MODE_PLAY,
				                m_layer_weight, m_ipo_flags, 1.0f, blendmode);

				m_flag |= ACT_FLAG_PLAY_END;
				break;
			}
		}
	}

	return m_flag & ACT_FLAG_ACTIVE;
}

bAction *SCA_ActionActuator::GetAction()
{
	return m_action;
}

void SCA_ActionActuator::SetAction(bAction *act)
{
	m_action = act;
}

void SCA_ActionActuator::DecLink()
{
	SCA_IActuator::DecLink();
	/* In this case no controllers use this action actuator,
	   and it should stop its action. */
	if (m_links == 0) {
		KX_GameObject *obj = (KX_GameObject *)GetParent();
		obj->StopAction(m_layer);
	}
}

#ifdef WITH_PYTHON

/* ------------------------------------------------------------------------- */
/* Python Integration Hooks                                                  */
/* ------------------------------------------------------------------------- */

PyTypeObject SCA_ActionActuator::Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"SCA_ActionActuator",
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

PyMethodDef SCA_ActionActuator::Methods[] = {
	{NULL,NULL} //Sentinel
};

PyAttributeDef SCA_ActionActuator::Attributes[] = {
	KX_PYATTRIBUTE_FLOAT_RW("frameStart", 0, MAXFRAMEF, SCA_ActionActuator, m_startframe),
	KX_PYATTRIBUTE_FLOAT_RW("frameEnd", 0, MAXFRAMEF, SCA_ActionActuator, m_endframe),
	KX_PYATTRIBUTE_FLOAT_RW("blendIn", 0, MAXFRAMEF, SCA_ActionActuator, m_blendin),
	KX_PYATTRIBUTE_RW_FUNCTION("action", SCA_ActionActuator, pyattr_get_action, pyattr_set_action),
	KX_PYATTRIBUTE_SHORT_RW("priority", 0, 100, false, SCA_ActionActuator, m_priority),
	KX_PYATTRIBUTE_SHORT_RW("layer", 0, MAX_ACTION_LAYERS-1, true, SCA_ActionActuator, m_layer),
	KX_PYATTRIBUTE_FLOAT_RW("layerWeight", 0, 1.0, SCA_ActionActuator, m_layer_weight),
	KX_PYATTRIBUTE_RW_FUNCTION("frame", SCA_ActionActuator, pyattr_get_frame, pyattr_set_frame),
	KX_PYATTRIBUTE_STRING_RW("propName", 0, MAX_PROP_NAME, false, SCA_ActionActuator, m_propname),
	KX_PYATTRIBUTE_STRING_RW("framePropName", 0, MAX_PROP_NAME, false, SCA_ActionActuator, m_framepropname),
	KX_PYATTRIBUTE_RW_FUNCTION("useContinue", SCA_ActionActuator, pyattr_get_use_continue, pyattr_set_use_continue),
	KX_PYATTRIBUTE_FLOAT_RW_CHECK("blendTime", 0, MAXFRAMEF, SCA_ActionActuator, m_blendframe, CheckBlendTime),
	KX_PYATTRIBUTE_SHORT_RW_CHECK("mode", 0, 100, false, SCA_ActionActuator, m_playtype, CheckType),
	KX_PYATTRIBUTE_NULL	//Sentinel
};

PyObject *SCA_ActionActuator::pyattr_get_action(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_ActionActuator *self = static_cast<SCA_ActionActuator *>(self_v);
	return PyUnicode_FromString(self->GetAction() ? self->GetAction()->id.name + 2 : "");
}

int SCA_ActionActuator::pyattr_set_action(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
	SCA_ActionActuator *self = static_cast<SCA_ActionActuator *>(self_v);
	
	if (!PyUnicode_Check(value)) {
		CM_PythonAttributError("SCA_ActionActuator", "action", "The value passed (" << val
		                       << ") is not the expected string name of the action");
		return PY_SET_ATTR_FAIL;
	}

	bAction *action= NULL;
	std::string val = _PyUnicode_AsString(value);
	
	if (val != "") {
		action= (bAction *)self->GetLogicManager()->GetActionByName(val);
		if (!action) {
			CM_PythonAttributError("SCA_ActionActuator", "action", "Action supplied (" << val << ") not found!");
			return PY_SET_ATTR_FAIL;
		}
	}
	
	self->SetAction(action);
	return PY_SET_ATTR_SUCCESS;
}

PyObject *SCA_ActionActuator::pyattr_get_use_continue(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_ActionActuator *self = static_cast<SCA_ActionActuator *>(self_v);
	return PyBool_FromLong(self->m_flag & ACT_FLAG_CONTINUE);
}

int SCA_ActionActuator::pyattr_set_use_continue(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
	SCA_ActionActuator *self = static_cast<SCA_ActionActuator *>(self_v);
	
	if (PyObject_IsTrue(value))
		self->m_flag |= ACT_FLAG_CONTINUE;
	else
		self->m_flag &= ~ACT_FLAG_CONTINUE;
	
	return PY_SET_ATTR_SUCCESS;
}

PyObject *SCA_ActionActuator::pyattr_get_frame(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_ActionActuator *self = static_cast<SCA_ActionActuator *>(self_v);
	return PyFloat_FromDouble(((KX_GameObject *)self->m_gameobj)->GetActionFrame(self->m_layer));
}

int SCA_ActionActuator::pyattr_set_frame(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
	SCA_ActionActuator *self = static_cast<SCA_ActionActuator *>(self_v);
	
	((KX_GameObject *)self->m_gameobj)->SetActionFrame(self->m_layer, PyFloat_AsDouble(value));
	
	return PY_SET_ATTR_SUCCESS;
}

int CheckBlendTime(void *self, const PyAttributeDef *)
{
	SCA_ActionActuator *act = reinterpret_cast<SCA_ActionActuator *>(self);

	if (act->m_blendframe > act->m_blendin)
		act->m_blendframe = act->m_blendin;

	return 0;
}

int CheckType(void *self, const PyAttributeDef *)
{
	SCA_ActionActuator *act = reinterpret_cast<SCA_ActionActuator *>(self);

	switch (act->m_playtype) {
		case ACT_ACTION_PLAY:
		case ACT_ACTION_PINGPONG:
		case ACT_ACTION_FLIPPER:
		case ACT_ACTION_LOOP_STOP:
		case ACT_ACTION_LOOP_END:
		case ACT_ACTION_FROM_PROP:
			return 0;
		default:
			CM_PythonAttributError("SCA_ActionActuator", "mode", "Invalid play mode (" << act->m_playtype << ") supplied");
			return 1;
	}
}

#endif // WITH_PYTHON
