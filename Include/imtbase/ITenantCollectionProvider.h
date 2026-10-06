// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/ITenantObjectCollection.h>
#include <imtbase/IOperationContext.h>


namespace imtbase
{


/**
	Provider of the views of a collection bound to the data storage of a single tenant.
	The providing collection itself addresses the data without organization (shared storage).
	\ingroup Collection
*/
class ITenantCollectionProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Check if the data of the collection is stored separately per tenant.
		If not, all tenants share the data of the providing collection.
	*/
	virtual bool IsTenantSeparated() const = 0;

	/**
		Get the collection bound to the data storage of the given tenant.
		An empty tenant ID addresses the data without organization.
		The returned collection is owned by the provider and lives as long as the provider.
		\return \c nullptr if the tenant has no accessible data storage.
	*/
	virtual ITenantObjectCollection* GetTenantCollection(const QByteArray& tenantId) const = 0;
};


/**
	Collection of the data of a tenant: for a collection stored separately per tenant the collection
	bound to the tenant, otherwise (or for an empty tenant ID) the collection itself.
*/
inline IObjectCollection* GetTenantDataCollection(IObjectCollection* collectionPtr, const QByteArray& tenantId)
{
	const ITenantCollectionProvider* providerPtr = dynamic_cast<const ITenantCollectionProvider*>(collectionPtr);
	if ((providerPtr == nullptr) || !providerPtr->IsTenantSeparated() || tenantId.isEmpty()){
		return collectionPtr;
	}

	return providerPtr->GetTenantCollection(tenantId);
}


/**
	Collection of the data of the tenant of an operation (see GetTenantDataCollection()).
*/
inline IObjectCollection* GetOperationCollection(IObjectCollection* collectionPtr, const IOperationContext* operationContextPtr)
{
	return GetTenantDataCollection(collectionPtr, (operationContextPtr != nullptr) ? operationContextPtr->GetTenantId() : QByteArray());
}


} // namespace imtbase


