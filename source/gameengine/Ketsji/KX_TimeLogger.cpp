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

/** \file gameengine/Ketsji/KX_TimeLogger.cpp
 *  \ingroup ketsji
 */

#include "KX_TimeLogger.h"

#include <algorithm>

#include "CM_Message.h"

KX_TimeLogger::KX_TimeLogger()
	:m_logStart(0.0),
	m_logging(false)
{
}

KX_TimeLogger::~KX_TimeLogger()
{
}

void KX_TimeLogger::StartLog(double now)
{
	if (!m_logging) {
		m_logging = true;
		m_logStart = now;
	}
}

void KX_TimeLogger::EndLog(double now)
{
	if (m_logging) {
		m_logging = false;
		double time = now - m_logStart;
		if (m_measurements.size() > 0) {
			m_measurements[0] += time;
		}
	}
}

void KX_TimeLogger::NextMeasurement(double now)
{
	// End logging to current measurement
	EndLog(now);

	// Add a new measurement at the front
	double m = 0.0;
	m_measurements.push_front(m);

	// Remove measurement if we grow beyond the maximum size
	if ((m_measurements.size()) > MAX_MEASUREMENTS) {
		while (m_measurements.size() > MAX_MEASUREMENTS) {
			m_measurements.pop_back();
		}
	}
}

std::array<double, 3> KX_TimeLogger::GetAverages() const
{
	std::array<double, 3> avg = {0.0, 0.0, 0.0};
	const unsigned short numMeasurements = m_measurements.size();
	const unsigned short samples[3] = {2, std::min(numMeasurements, (unsigned short)26),
		std::min(numMeasurements, (unsigned short)101)};
	static const unsigned short first[3] = {1, 2, 26};

	for (unsigned short i = 0; i < 3; ++i) {
		for (unsigned short j = first[i], size = samples[i]; j < size; ++j) {
			const double time = m_measurements[j];
			for (unsigned short k = i; k < 3; ++k) {
				avg[k] += time;
			}
		}
		avg[i] /= samples[i] - 1;
	}

	/*for (unsigned short i = 25, size = samples[2]; i < size; ++i) {
		avg[2] += m_measurements[i];
	}
	for (unsigned short i = 1, size = samples[1]; i < size; ++i) {
		const double time = m_measurements[i];
		avg[1] += time;
		avg[2] += time;
	}*/
	

	/*if (numMeasurements > 1) {
		for (unsigned int i = 1; i < numMeasurements; i++) {
			avg += m_measurements[i];
		}
		avg /= (double)numMeasurements - 1.0;
	}*/

	return avg;
}
