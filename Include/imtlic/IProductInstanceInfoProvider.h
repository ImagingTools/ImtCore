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
	enum MetaInfoTypes
	{
		/**
			List of license instance infos given as QStringList.
		*/
		MIT_LICENSE_INSTANCE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1,
	};

	/**
		Get list of all available product instances.
	*/
	virtual const imtbase::ICollectionInfo& GetProductInstanceList() const = 0;

	/**
		Get information for a product installation instance.
	*/
	virtual const imtlic::IProductInstanceInfo* GetProductInstance(const QByteArray& instanceId) const = 0;
};


} // namespace imtlic


