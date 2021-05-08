#pragma once


// ImtCore includes
#include <imt3d/IPointsBasedObject.h>


namespace imt3d
{


/**
	Common interface describing pixel to 3d-coordinate trnsformation.
*/
class IImage3dCalibration
{
public:
	/**
		Transform image position to 3d coordinate.
	*/
	virtual bool Transform(IPointsBasedObject::PointXyz32& result, const i2d::CVector2d& pixelPosition) const = 0;
};

} // namespace imt3d


