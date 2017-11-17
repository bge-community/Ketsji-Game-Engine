#ifndef __RAS_VERTEX_FACTORY_H__
#define __RAS_VERTEX_FACTORY_H__

#include "RAS_VertexData.h"

#include "boost/pool/object_pool.hpp"

class RAS_IVertexFactory
{
public:
	RAS_IVertexFactory() = default;
	virtual ~RAS_IVertexFactory() = default;

	static RAS_IVertexFactory *Construct(const RAS_VertexFormat& format);

	virtual RAS_IVertexData *CreateVertex(
				const mt::vec3& xyz,
				const mt::vec2 * const uvs,
				const mt::vec4& tangent,
				const unsigned int *rgba,
				const mt::vec3& normal) = 0;

	virtual RAS_IVertexData *CreateVertex(
				const float xyz[3],
				const float (*uvs)[2],
				const float tangent[4],
				const unsigned int *rgba,
				const float normal[3]) = 0;

	virtual void DeleteVertex(RAS_IVertexData *data) = 0;
};

template <class FormatType>
class RAS_VertexFactory : public RAS_IVertexFactory
{
	using VertexData = RAS_VertexData<FormatType>;
	// Temporary vertex data storage.
	boost::object_pool<VertexData> m_vertexPool;

public:
	RAS_VertexFactory() = default;
	virtual ~RAS_VertexFactory() = default;

	virtual RAS_IVertexData *CreateVertex(
				const mt::vec3& xyz,
				const mt::vec2 * const uvs,
				const mt::vec4& tangent,
				const unsigned int *rgba,
				const mt::vec3& normal)
	{
		VertexData *data = new (m_vertexPool.malloc()) VertexData(xyz, uvs, tangent, rgba, normal);
		return data;
	}

	virtual RAS_IVertexData *CreateVertex(
				const float xyz[3],
				const float (*uvs)[2],
				const float tangent[4],
				const unsigned int *rgba,
				const float normal[3])
	{
		VertexData *data = new (m_vertexPool.malloc()) VertexData(xyz, uvs, tangent, rgba, normal);
		return data;
	}

	virtual void DeleteVertex(RAS_IVertexData *data)
	{
		m_vertexPool.destroy(static_cast<VertexData *>(data));
	}
};

#endif  // __RAS_VERTEX_FACTORY_H__
