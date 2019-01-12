#pragma once


// ACF includes
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
class IGridInfo: virtual public istd::IChangeable
{
public:
	virtual const istd::CIndex2d GetGridSize() const = 0;
	virtual const istd::CIndex2d GetGridPosition(int index) const = 0;
	virtual int GetCloudPosition(const istd::CIndex2d& index) const = 0;
};


} // namespace imt3d


