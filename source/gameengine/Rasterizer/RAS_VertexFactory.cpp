#include "RAS_VertexFactory.h"

#include "CM_Template.h"

RAS_IVertexFactory *RAS_IVertexFactory::Construct(const RAS_VertexFormat& format)
{
	return CM_InstantiateTemplateSwitch<RAS_IVertexFactory, RAS_VertexFactory, RAS_VertexFormatTuple>(format);
}
