#pragma once

// ACF includes
#include <istd/IChangeable.h>

// imt3d includes
#include <imt3d/IObject3d.h>

namespace imt3d
{


	/**
		3d object supplier 
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