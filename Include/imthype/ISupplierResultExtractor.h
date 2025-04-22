#pragma once


// ACF includes
#include <iinsp/ISupplier.h>


namespace imthype
{


/**
	Common interface for a provider of the supplier's result (work product).
*/
class ISupplierResultExtractor: virtual public istd::IPolymorphic
{
public:
	/**
		Get access to the supplier product.
	*/
	virtual const istd::IChangeable* GetSupplierProduct(const iinsp::ISupplier* supplierPtr) const = 0;
};


} // namespace imthype


