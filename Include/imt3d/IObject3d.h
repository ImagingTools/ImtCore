#pragma once


// ACF includes
#include <iser/IObject.h>
#include <i3d/CVector3d.h>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/CCuboid.h>


namespace imt3d
{


/**
	Common interface for describing the 3D-objects.
*/
class IObject3d: virtual public iser::IObject
{
public:
	/**
		Return \c true if the volume of the 3D-object is equals 0.
	*/
	virtual bool IsEmpty() const = 0;

	/**
		Get the center of the 3D-object.
	*/
	virtual Point3d GetCenter() const = 0;

	/**
		Move object to position \c position.
	*/
	virtual void MoveCenterTo(const Point3d& position) = 0;

	/**
		Get bounding cuboid of this object.
	*/
	virtual CCuboid GetBoundingCuboid() const = 0;
};


} // namespace imt3d


