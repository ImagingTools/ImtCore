// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>


namespace imtauth
{


/**
	Interface for managing tenant-entity bindings.
	Provides add/remove/query operations for the TenantEntityBindings table
	without exposing internal collection mechanics.

	\ingroup TenantMembership
*/
class ITenantEntityBindingManager: virtual public istd::IChangeable
{
public:
	/**
		Add a binding between a tenant and an entity.
		Idempotent — duplicate bindings are ignored.
		\return true if binding was added (or already existed).
	*/
	virtual bool AddBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) = 0;

	/**
		Remove all bindings for a given tenant + entity.
		\return true if removal succeeded (or no binding existed).
	*/
	virtual bool RemoveBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) = 0;

	/**
		Remove all bindings for a given entity across all tenants.
		\return true if removal succeeded.
	*/
	virtual bool RemoveAllBindingsForEntity(const QByteArray& entityType, const QByteArray& entityId) = 0;

	/**
		Remove all bindings for a given tenant + entity type.
		\return true if removal succeeded.
	*/
	virtual bool RemoveAllBindingsForTenant(const QByteArray& tenantId, const QByteArray& entityType) = 0;

	/**
		Check if a binding exists.
	*/
	virtual bool HasBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) const = 0;

	/**
		Get all entity IDs of a given type bound to a tenant.
	*/
	virtual QByteArrayList GetEntityIds(const QByteArray& tenantId, const QByteArray& entityType) const = 0;
};


} // namespace imtauth
