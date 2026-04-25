// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QVector3D>


namespace imt3dview
{


/**
	Backend-neutral primitive type used by SDrawCommand.
*/
enum EPrimitiveType
{
	PT_TRIANGLES = 0,
	PT_LINES,
	PT_LINE_STRIP,
	PT_POINTS
};


/**
	Material/uniform configuration for a single draw call.
	Aggregates the per-shape state previously set inline in CShape3dBase::DrawGl
	and the subclass-specific DrawShapeGl methods.
*/
struct SMaterial
{
	enum EColorMode
	{
		CM_PER_VERTEX = 0,	// pointColor attribute
		CM_SOLID = 1		// itemColor uniform
	};

	EColorMode colorMode = CM_SOLID;
	QVector3D solidColor;
	float pointSize = 1.0f;
	float lineWidth = 1.0f;
	bool useNormals = false;
	bool useLighting = false;
	bool usePointSize = false;
};


} // namespace imt3dview
