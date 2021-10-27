#pragma once


// ImtCore includes
#include <imt3d/IPointsBasedObject.h>


namespace imt3d
{


/**
	Common interface describing a cloud of 3D-points.
*/
class IPointCloud3d: virtual public IPointsBasedObject
{
public:
	/**
		Create point cloud with specified point format.
	*/
	virtual bool CreateCloud(PointFormat pointFormat) = 0;

	/**
		Create point cloud with specified point format using external data buffer.
	*/
	virtual bool CreateCloud(
				PointFormat pointFormat,
				size_t pointsCount,
				const void* dataPtr,
				const istd::CIndex2d* gridSizePtr = nullptr) = 0;

	/**
		Append points to existing data in the point cloud.
		The points should be in the format specified by \c CreateCloud.
	*/
	virtual bool InsertPoints(
				size_t pointsCount,
				const void* dataPtr) = 0;
};


} // namespace imt3d


