// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imt3d
{


class IObject3d;


/**
	Common interface for a 3D-object provider.
*/
class IObject3dProvider: virtual public istd::IChangeable
{
public:
	/**
		Get access to produced 3D-object.
		\return	pointer to 3D-object instance if it is accessible, or NULL.
	*/
	virtual const imt3d::IObject3d* GetObject3d() const = 0;
};


} // namespace imt3d


