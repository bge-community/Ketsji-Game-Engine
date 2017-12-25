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
 * Contributor(s): Tristan Porteries.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file RAS_DisplayArray.h
 *  \ingroup bgerast
 */

#ifndef __RAS_DISPLAY_ARRAY_H__
#define __RAS_DISPLAY_ARRAY_H__

#include "RAS_IDisplayArray.h"
#include "RAS_DisplayArrayStorage.h"
#include "RAS_VertexData.h"

template <class FormatType>
class RAS_BatchDisplayArray;

/// An array with data used for OpenGL drawing.
template <class FormatType>
class RAS_DisplayArray : public virtual RAS_IDisplayArray
{
friend class RAS_BatchDisplayArray<FormatType>;

protected:
	using VertexData = RAS_VertexData<FormatType>;

	std::vector<VertexData> m_vertexes;

	RAS_DisplayArray(const RAS_DisplayArray& other)
		:RAS_IDisplayArray(other),
		m_vertexes(other.m_vertexes)
	{
	}

public:
	RAS_DisplayArray(PrimitiveType type, const RAS_VertexFormat& format)
		:RAS_IDisplayArray(type, format, VertexData::GetMemoryFormat())
	{
	}

	RAS_DisplayArray(PrimitiveType type, const RAS_VertexFormat& format,
		const IVertexDataList& vertices, const VertexInfoList& vertexInfos,
		const IndexList& primitiveIndices, const IndexList& triangleIndices)
		:RAS_IDisplayArray(type, format, VertexData::GetMemoryFormat(), vertexInfos, primitiveIndices, triangleIndices)
	{
		const unsigned int size = vertices.size();
		m_vertexes.resize(size);
		for (unsigned int i = 0; i < size; ++i) {
			VertexData *data = static_cast<VertexData *>(vertices[i]);
			m_vertexes[i] = *data;
		}
	}

	virtual ~RAS_DisplayArray()
	{
	}

	virtual RAS_IDisplayArray *GetReplica()
	{
		RAS_DisplayArray<FormatType> *replica = new RAS_DisplayArray<FormatType>(*this);
		replica->UpdateCache();

		return replica;
	}

	virtual RAS_Vertex GetVertexNoCache(const unsigned int index)
	{
		return RAS_Vertex(&m_vertexes[index], m_format);
	}

	virtual RAS_IVertexData *GetVertexData(const unsigned int index)
	{
		return &m_vertexes[index];
	}

	virtual const RAS_IVertexData *GetVertexPointer() const
	{
		return m_vertexes.data();
	}

	virtual unsigned int AddVertexData(RAS_IVertexData *data)
	{
		m_vertexes.push_back(*static_cast<VertexData *>(data));
		return m_vertexes.size() - 1;
	}

	virtual void Clear()
	{
		m_vertexes.clear();
		m_vertexDataPtrs.clear();
		m_vertexInfos.clear();
		m_primitiveIndices.clear();
		m_triangleIndices.clear();
		m_maxOrigIndex = 0;
	}

	virtual unsigned int GetVertexCount() const
	{
		return m_vertexes.size();
	}

	virtual void UpdateCache()
	{
		const unsigned int size = GetVertexCount();
		m_vertexDataPtrs.resize(size);
		for (unsigned int i = 0; i < size; ++i) {
			m_vertexDataPtrs[i] = &m_vertexes[i];
		}
	}
};

#endif  // __RAS_DISPLAY_ARRAY_H__
