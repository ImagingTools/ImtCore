#pragma once


// ACF includes
#include <imath/imath.h>
#include <i3d/CVector3d.h>


/**
	Contains the 3D primitives.
	This package is system independent.
	This package use following ACF packages:
	\li istd
	\li iser
	\li imath
	\ingroup 3D
*/
namespace imt3d
{


class IDepthBitmap;
class IPointCloud3d;

bool ConvertDepthImageToCloud(const imt3d::IDepthBitmap& bitmap, IPointCloud3d& pointCloud);
bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, imt3d::IDepthBitmap& bitmap);
template <typename PointType> bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, imt3d::IDepthBitmap& bitmap);

} // namespace imt3d


