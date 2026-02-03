#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtservice/IConnectionCollection.h>
#include <imtservice/IConnectionCollectionPlugin.h>


namespace imtservice
{


/**
	Interface for describing a service connection collection.
	\ingroup Service
*/
class IConnectionCollectionProvider: virtual public istd::IPolymorphic
{
public:
	// TODO: Raw pointer ???
	virtual imtservice::IConnectionCollection* GetConnectionCollectionByServicePath(const QString& servicePath) const = 0;
	virtual imtservice::IConnectionCollection* GetConnectionCollectionByServiceId(const QByteArray& serviceId) const = 0;
};


} // namespace imtservice


