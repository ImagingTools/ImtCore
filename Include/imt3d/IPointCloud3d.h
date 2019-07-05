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
	virtual bool CreateCloud(PointFormat pointFormat, int pointsCount, const istd::CIndex2d* gridSizePtr = nullptr) = 0;

	/**
		Create point cloud with specified point format using external data buffer.
	*/
	virtual bool CreateCloud(PointFormat pointFormat, int pointsCount, void* dataPtr, bool releaseFlag, const istd::CIndex2d* gridSizePtr = nullptr) = 0;
};


} // namespace imt3d


