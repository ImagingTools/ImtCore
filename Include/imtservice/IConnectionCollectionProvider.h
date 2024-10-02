#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtservice/IConnectionCollection.h>


namespace imtservice
{


/**
	Interface for describing an service connection collection.
	\ingroup Service
*/
class IConnectionCollectionProvider: virtual public istd::IPolymorphic
{
public:
	virtual std::shared_ptr<IConnectionCollection> GetConnectionCollection(const QByteArray& serviceId) const = 0;
};


} // namespace imtservice


