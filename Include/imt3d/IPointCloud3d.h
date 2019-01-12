#pragma once


// Acf includes
#include <i3d/CVector3d.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IObject3d.h>


namespace imt3d
{


/**
	Common interface for describing the cloud (list) of 3D-points.
*/
class IPointCloud3d: virtual public IObject3d
{
public:
	typedef QVector<i3d::CVector3d> CloudPoints;
	typedef istd::TSmartPtr<IPointCloud3d> PointCloudPtr;

	/**
		Create cloud object from the list of 3D-points.
	*/
	virtual void CreateCloud(const CloudPoints& points) = 0;

	/**
		Get the list of pointsin 3D-space.
	*/
	virtual const CloudPoints& GetPoints() const = 0;
};


} // namespace imt3d


