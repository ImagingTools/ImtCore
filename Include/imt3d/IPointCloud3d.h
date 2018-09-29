#pragma once


// Qt includes
#include <QtCore/QLinkedList>

// ACF includes
#include <i3d/CVector3d.h>

// ImtCore includes
#include <imt3d/IObject3d.h>


namespace imt3d
{


/**
	Common interface for describing the 3D-objects.
*/
class IPointCloud3d: virtual public IObject3d
{
public:
	typedef QLinkedList<i3d::CVector3d> CloudPoints;

	/**
		Get the list of points.
	*/
	virtual const CloudPoints& GetPoints() const = 0;
};


} // namespace imt3d


