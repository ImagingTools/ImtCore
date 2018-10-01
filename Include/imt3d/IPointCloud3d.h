#pragma once


// Qt includes
#include <QtCore/QLinkedList>

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
	typedef istd::TSmartPtr<IPointCloud3d> PointCloudPtr;

	/**
		Get the list of points.
	*/
	virtual const CloudPoints& GetPoints() const = 0;
};


} // namespace imt3d


