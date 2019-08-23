#pragma once


// ACF includes
#include <imath/imath.h>
#include <i3d/CVector3d.h>


/**
	Basic implementations for 3D-calculations
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

/**
	Convert a depth image to a point cloud.
*/
bool ConvertDepthImageToCloud(const imt3d::IDepthBitmap& bitmap, IPointCloud3d& pointCloud);

/**
	Convert a point cloud to a depth image.
*/
bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, imt3d::IDepthBitmap& bitmap);

/**
	Generic conversion of a point cloud to a depth image.
*/
template <typename PointType>
bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, imt3d::IDepthBitmap& bitmap);


} // namespace imt3d


