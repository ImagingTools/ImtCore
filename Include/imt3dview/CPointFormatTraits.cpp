// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dview/CPointFormatTraits.h>


namespace imt3dview
{


// public methods

VertexLayout CPointFormatTraits::BuildLayout(imt3d::IPointsBasedObject::PointFormat format)
{
	VertexLayout layout;

	switch (format){
	case imt3d::IPointsBasedObject::PF_XYZ_32:
		{
			VertexAttribute pos;
			pos.attribute = VA_POSITION;
			pos.type = AT_FLOAT_32;
			pos.componentCount = 3;
			pos.offset = 0;
			layout.attributes.push_back(pos);
			layout.stride = sizeof(imt3d::IPointsBasedObject::PointXyz32);
		}
		break;

	case imt3d::IPointsBasedObject::PF_XYZ_64:
		{
			VertexAttribute pos;
			pos.attribute = VA_POSITION;
			pos.type = AT_FLOAT_64;
			pos.componentCount = 3;
			pos.offset = 0;
			layout.attributes.push_back(pos);
			layout.stride = sizeof(imt3d::IPointsBasedObject::PointXyz64);
		}
		break;

	case imt3d::IPointsBasedObject::PF_XYZ_ABC_32:
		{
			VertexAttribute pos;
			pos.attribute = VA_POSITION;
			pos.type = AT_FLOAT_32;
			pos.componentCount = 3;
			pos.offset = 0;
			layout.attributes.push_back(pos);
			layout.stride = sizeof(imt3d::IPointsBasedObject::PointXyzAbc32);
		}
		break;

	case imt3d::IPointsBasedObject::PF_XYZW_32:
		{
			VertexAttribute pos;
			pos.attribute = VA_POSITION;
			pos.type = AT_FLOAT_32;
			pos.componentCount = 3;
			pos.offset = 0;
			layout.attributes.push_back(pos);
			layout.stride = sizeof(imt3d::IPointsBasedObject::PointXyzw32);
		}
		break;

	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
		{
			VertexAttribute pos;
			pos.attribute = VA_POSITION;
			pos.type = AT_FLOAT_32;
			pos.componentCount = 3;
			pos.offset = 0;
			layout.attributes.push_back(pos);

			VertexAttribute normal;
			normal.attribute = VA_NORMAL;
			normal.type = AT_FLOAT_32;
			normal.componentCount = 3;
			normal.offset = 4 * sizeof(float);
			layout.attributes.push_back(normal);

			layout.stride = sizeof(imt3d::IPointsBasedObject::PointXyzwNormal32);
		}
		break;

	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
		{
			VertexAttribute pos;
			pos.attribute = VA_POSITION;
			pos.type = AT_FLOAT_32;
			pos.componentCount = 3;
			pos.offset = 0;
			layout.attributes.push_back(pos);

			VertexAttribute normal;
			normal.attribute = VA_NORMAL;
			normal.type = AT_FLOAT_32;
			normal.componentCount = 3;
			normal.offset = 4 * sizeof(float);
			layout.attributes.push_back(normal);

			VertexAttribute color;
			color.attribute = VA_COLOR;
			color.type = AT_FLOAT_32;
			color.componentCount = 3;
			color.offset = 8 * sizeof(float);
			layout.attributes.push_back(color);

			layout.stride = sizeof(imt3d::IPointsBasedObject::PointXyzwNormalRgba32);
		}
		break;

	case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
		{
			VertexAttribute pos;
			pos.attribute = VA_POSITION;
			pos.type = AT_FLOAT_32;
			pos.componentCount = 3;
			pos.offset = 0;
			layout.attributes.push_back(pos);

			VertexAttribute color;
			color.attribute = VA_COLOR;
			color.type = AT_FLOAT_32;
			color.componentCount = 3;
			color.offset = 4 * sizeof(float);
			layout.attributes.push_back(color);

			layout.stride = sizeof(imt3d::IPointsBasedObject::PointXyzwRgba32);
		}
		break;

	case imt3d::IPointsBasedObject::PF_UNDEFINED:
	default:
		break;
	}

	return layout;
}


bool CPointFormatTraits::HasNormals(imt3d::IPointsBasedObject::PointFormat format)
{
	switch (format){
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
		return true;
	default:
		return false;
	}
}


bool CPointFormatTraits::HasColor(imt3d::IPointsBasedObject::PointFormat format)
{
	switch (format){
	case imt3d::IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
	case imt3d::IPointsBasedObject::PF_XYZW_RGBA_32:
		return true;
	default:
		return false;
	}
}


} // namespace imt3dview
