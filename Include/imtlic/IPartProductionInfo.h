#pragma once


// Acf includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>

namespace imtlic
{


/**
	Common interface for a product instance part production. A product instance is the result of the product installation by the customer.
	\ingroup LicenseManagement
*/
class IPartProductionInfo : virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		/**
			Serial number of the product..
		*/
		MIT_STATUS = idoc::IDocumentMetaInfo::MIT_USER + 3000,

	};

	/**
		Get the status of this product instance.
	*/
	virtual QByteArray GetStatus() const = 0;

	/**
		Set the status of this product instance.
	*/
	virtual void SetStatus(const QByteArray& status) = 0;
};


} // namespace imtlic


