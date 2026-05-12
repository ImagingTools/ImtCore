// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imt3dview
{


/**
	Opaque, ref-counted handle for a backend-owned render resource (geometry, material,
	program, framebuffer, ...). Lifetime is tied to the backend that produced it; once
	the backend is shut down all outstanding handles must be considered invalid.
*/
class IRenderResource: virtual public istd::IPolymorphic
{
public:
	virtual ~IRenderResource() = default;
};


typedef std::shared_ptr<IRenderResource> IRenderResourcePtr;


} // namespace imt3dview
