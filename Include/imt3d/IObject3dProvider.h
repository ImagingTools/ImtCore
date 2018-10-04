#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imt3d
{


class IObject3d;


/**
	Common interface for a 3D-object provider.
*/
class IObject3dProvider : virtual public istd::IChangeable
{
public:
	/**
		Get access to produced 3d object.
		\return	pointer to 3d object instance if it is accessible, or NULL.
	*/
	virtual const imt3d::IObject3d* GetObject3d() const = 0;
};


} // namespace imt3d


