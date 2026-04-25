// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imt3d/IPointsBasedObject.h>
#include <imt3dview/SVertexLayout.h>


namespace imt3dview
{


/**
	Centralized mapping from imt3d::IPointsBasedObject::PointFormat to a backend-neutral
	SVertexLayout. Replaces the duplicated switch-by-PointFormat blocks that used to live
	in CShape3dBase::DrawGl.
*/
class CPointFormatTraits
{
public:
	/**
		Build a vertex layout describing the interleaved point format.
	*/
	static SVertexLayout BuildLayout(imt3d::IPointsBasedObject::PointFormat format);

	/**
		True if the given point format carries per-vertex normal data.
	*/
	static bool HasNormals(imt3d::IPointsBasedObject::PointFormat format);

	/**
		True if the given point format carries per-vertex color data.
	*/
	static bool HasColor(imt3d::IPointsBasedObject::PointFormat format);
};


} // namespace imt3dview
