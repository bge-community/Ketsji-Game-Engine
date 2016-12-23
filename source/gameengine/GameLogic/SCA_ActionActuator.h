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

/** \file SCA_ActionActuator.h
 *  \ingroup gamelogic
 */

#ifndef __SCA_ACTIONACTUATOR_H__
#define __SCA_ACTIONACTUATOR_H__

#include "SCA_IActuator.h"
#include "DNA_actuator_types.h"
#include "MT_Vector3.h"

class SCA_ActionActuator : public SCA_IActuator
{
	Py_Header

protected:
	MT_Vector3 m_lastpos;
	float m_blendframe;
	int m_flag;
	float m_startframe;  // The frame this action starts
	float m_endframe;  // The frame this action ends
	float m_starttime;  // The time this action started
	float m_localtime;  // The current time of the action
	float m_lastUpdate;
	float m_blendin;
	float m_blendstart;
	float m_stridelength;
	float m_layer_weight;
	short m_playtype;
	short m_blendmode;
	short m_priority;
	short m_layer;
	short m_ipo_flags;
	struct bAction *m_action;
	std::string m_propname;
	std::string m_framepropname;

	enum {
		ACT_FLAG_REVERSE		= 1<<0,
		ACT_FLAG_ACTIVE			= 1<<1,
		ACT_FLAG_CONTINUE		= 1<<2,
		ACT_FLAG_PLAY_END		= 1<<3,
		ACT_FLAG_ATTEMPT_PLAY	= 1<<4,
	};

public:
	SCA_ActionActuator(SCA_IObject *gameobj,
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
	                   float stride);

	virtual ~SCA_ActionActuator();

	virtual bool Update(double curtime, bool frame);
	virtual CValue *GetReplica();
	virtual void ProcessReplica();

	void SetBlendTime(float newtime);
	void SetLocalTime(float curtime);
	void ResetStartTime(float curtime);

	bAction *GetAction();
	void SetAction(bAction *act);

	virtual void DecLink();

#ifdef WITH_PYTHON
	static PyObject *pyattr_get_action(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_action(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);
	static PyObject *pyattr_get_use_continue(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_use_continue(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);
	static PyObject *pyattr_get_frame(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static int pyattr_set_frame(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);

	static int CheckBlendTime(void *self, const PyAttributeDef *);
	static int CheckType(void *self, const PyAttributeDef *);
#endif  // WITH_PYTHON
};

#endif  //__SCA_ACTIONACTUATOR_H__

