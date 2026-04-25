// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <vector>


namespace imt3dview
{


/**
	Logical vertex attribute slots that the standard ImtCore shading pipeline understands.
*/
enum VertexAttributeSlot
{
	VA_POSITION = 0,
	VA_NORMAL,
	VA_COLOR
};


/**
	Component data type within a vertex attribute.
*/
enum AttrType
{
	AT_FLOAT_32 = 0,
	AT_FLOAT_64,
	AT_UINT_8
};


/**
	Single attribute within a vertex layout.
*/
struct VertexAttribute
{
	VertexAttributeSlot attribute = VA_POSITION;
	AttrType type = AT_FLOAT_32;
	int componentCount = 0;
	int offset = 0;
};


/**
	Backend-neutral description of an interleaved vertex format.
*/
struct VertexLayout
{
	std::vector<VertexAttribute> attributes;
	int stride = 0;
};


} // namespace imt3dview
