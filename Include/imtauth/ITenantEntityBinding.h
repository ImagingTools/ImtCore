// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace imtauth
{


/**
	Interface describing a tenant-entity binding.
	Associates an entity (e.g. user) with a tenant for visibility/filtering purposes.

	The TenantEntityBindings table uses this structure to track which entities
	belong to which tenants, enabling tenant-scoped filtering (include/exclude).

	\ingroup TenantMembership
*/
class ITenantEntityBinding: virtual public iser::IObject
{
public:
	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	virtual QByteArray GetEntityType() const = 0;
	virtual void SetEntityType(const QByteArray& entityType) = 0;

	virtual QByteArray GetEntityId() const = 0;
	virtual void SetEntityId(const QByteArray& entityId) = 0;

	virtual QString GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	virtual QByteArray GetCreatedByUserId() const = 0;
	virtual void SetCreatedByUserId(const QByteArray& userId) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantEntityBinding> ITenantEntityBindingUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantEntityBinding> ITenantEntityBindingSharedPtr;


} // namespace imtauth
