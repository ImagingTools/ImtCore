#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


class IProductInstanceInfo;


/**
	Interface for a collection of installed product instances.
	\ingroup LicenseManagement
*/
class IProductInstanceInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of all available products.
	*/
	virtual const imtbase::ICollectionInfo& GetProductInstanceList() const = 0;

	/**
		Get license information for a given product. All available licenses are returned.
	*/
	virtual const imtlic::IProductInstanceInfo* GetProductInstance(const QByteArray& instanceId) const = 0;
};


} // namespace imtlic


