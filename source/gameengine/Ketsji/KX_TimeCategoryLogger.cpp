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

/** \file gameengine/Ketsji/KX_TimeCategoryLogger.cpp
 *  \ingroup ketsji
 */

#include "KX_TimeCategoryLogger.h"
#include "RAS_DebugDraw.h"

#include "boost/format.hpp"

const std::string profileLabels[KX_TimeLogger::NUM_CATEGORY] = {
	"Physics", // PHYSICS
	"Logic", // LOGIC
	"Animation", // ANIMATIONS
	"Network", // NETWORK
	"Scenegraph", // SCENEGRAPH
	"Rasterizer", // RASTERIZER
	"Services", // SERVICES
	"Overhead", // OVERHEAD
	"Outside", // OUTSIDE
	"GPU Latency" // LATENCY
};

KX_TimeCategoryLogger::KX_TimeCategoryLogger()
	:m_lastCategory(KX_TimeLogger::NONE)
{
}

KX_TimeCategoryLogger::~KX_TimeCategoryLogger()
{
}

void KX_TimeCategoryLogger::StartLog(KX_TimeLogger::Category tc)
{
	if (m_lastCategory != KX_TimeLogger::NONE) {
		m_loggers[m_lastCategory].EndLog(KX_TimeLogger::Clock::now());
	}
	m_loggers[tc].StartLog(KX_TimeLogger::Clock::now());
	m_lastCategory = tc;
}

void KX_TimeCategoryLogger::EndLog(KX_TimeLogger::Category tc)
{
	m_loggers[tc].EndLog(KX_TimeLogger::Clock::now());
}

void KX_TimeCategoryLogger::EndLog()
{
	m_loggers[m_lastCategory].EndLog(KX_TimeLogger::Clock::now());
	m_lastCategory = KX_TimeLogger::NONE;
}

void KX_TimeCategoryLogger::NextMeasurement()
{
	m_lastTotalAverage = {0.0, 0.0, 0.0};
	for (unsigned short tc = 0; tc < KX_TimeLogger::NUM_CATEGORY; ++tc) {
		KX_TimeLogger& logger = m_loggers[tc];
		logger.NextMeasurement(KX_TimeLogger::Clock::now());

		const std::array<double, 3> averages = logger.GetAverages();
		m_lastAverages[tc] = averages;
		for (unsigned short i = 0; i < 3; ++i) {
			m_lastTotalAverage[i] += averages[i];
		}
	}
}

double KX_TimeCategoryLogger::GetAverageFrameRate() const
{
	if (m_lastTotalAverage[0] < 1e-6) {
		// Equivalent to 1.0 / 1e-6.
		return 1e6f;
	}
	return 1.0 / m_lastTotalAverage[0];
}

std::map<std::string, double> KX_TimeCategoryLogger::GetProfileDict()
{
	std::map<std::string, double> dict;

	for (unsigned short tc = 0; tc < KX_TimeLogger::NUM_CATEGORY; ++tc) {
		dict[profileLabels[tc]] = m_lastAverages[tc][0];
	}

	return dict;
}

static const MT_Vector4 white(1.0f, 1.0f, 1.0f, 1.0f);

static std::string getTimeString(double time)
{
	static const std::string units[] = {"s", "ms", "us"};
	static const float values[] = {1.0f, 1.0e-3f, 1.0e-6f};

	for (unsigned short i = 0; i < 3; ++i) {
		if (time > values[i]) {
			return (boost::format("%6.2f") % (time / values[i])).str() + units[i];
		}
	}

	return "  0.00us";
}

static const MT_Vector4& getTimeColor(double ratio)
{
	static const MT_Vector4 colors[] = {
		MT_Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		MT_Vector4(1.0f, 1.0f, 0.0f, 1.0f),
		MT_Vector4(1.0f, 0.0f, 0.0f, 1.0f),
	};
	static const float values[] = {0.15f, 0.50f, 1.0f};

	for (unsigned short i = 0; i < 3; ++i) {
		if (ratio < values[i]) {
			return colors[i];
		}
	}

	return colors[2];
}

void KX_TimeCategoryLogger::RenderFrameRate(RAS_DebugDraw& debugDraw, int xindent, int ysize,
											 int& xcoord, int& ycoord, int profileIndent)
{
	debugDraw.RenderText2D("Frametime :", MT_Vector2(xcoord + xindent, ycoord), white);

	std::string debugtxt;
	for (unsigned short i = 0; i < 3; ++i) {
		debugtxt += getTimeString(m_lastTotalAverage[i]) + " | ";
	}
	debugtxt += (boost::format("(%.1ffps)") % GetAverageFrameRate()).str();

	debugDraw.RenderText2D(debugtxt, MT_Vector2(xcoord + xindent + profileIndent, ycoord), white);
	// Increase the indent by default increase
	ycoord += ysize;
}

void KX_TimeCategoryLogger::RenderCategories(RAS_DebugDraw& debugDraw, int xindent, int ysize,
											 int& xcoord, int& ycoord, int profileIndent)
{
	static unsigned short boxIndent = 227;
	for (unsigned short tc = 0; tc < KX_TimeLogger::NUM_CATEGORY; ++tc) {
		debugDraw.RenderText2D(profileLabels[tc] + ":", MT_Vector2(xcoord + xindent, ycoord), white);

		double tottime = m_lastTotalAverage[2];
		if (tottime < 1e-6) {
			tottime = 1e-6;
		}
		const float ratio = (m_lastAverages[tc][2] / tottime);
		const MT_Vector4& color = getTimeColor(ratio);

		std::string debugtxt;
		for (unsigned short i = 0; i < 3; ++i) {
			const double time = m_lastAverages[tc][i];
			debugtxt += getTimeString(time) + " | ";
		}
		debugtxt += (boost::format("%2.0f") % (ratio * 100.0)).str() + "%";

		debugDraw.RenderText2D(debugtxt, MT_Vector2(xcoord + xindent + profileIndent, ycoord), color);

		const MT_Vector2 boxSize(50 * ratio, 10);
		debugDraw.RenderBox2D(MT_Vector2(xcoord + profileIndent + boxIndent, ycoord), boxSize, color);

		ycoord += ysize;
	}
}
