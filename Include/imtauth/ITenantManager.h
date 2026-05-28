// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


class ITenantManager: virtual public istd::IChangeable
{
public:
	virtual QByteArrayList GetTenantIds() const = 0;
	virtual ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId) const = 0;
	virtual QByteArray CreateTenant(const QString& tenantName, const QString& description = QString(), const QByteArray& ownerId = QByteArray()) = 0;
	virtual bool RemoveTenant(const QByteArray& tenantId) = 0;
	virtual bool UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description, const QByteArray& ownerId = QByteArray(), bool updateOwner = false) = 0;
	virtual bool SetTenantActive(const QByteArray& tenantId, bool isActive) = 0;

	/**
		Set hierarchy fields on an existing tenant.
		Calculates depth and materialized path based on the parent tenant.
		\param tenantId The tenant to update.
		\param parentTenantId The parent tenant ID (empty to make top-level).
		\return true if hierarchy was set successfully.
	*/
	virtual bool SetTenantHierarchy(const QByteArray& tenantId, const QByteArray& parentTenantId) { Q_UNUSED(tenantId); Q_UNUSED(parentTenantId); return false; }

	/**
		Get the System-Tenant ID.
		Returns the well-known System-Tenant ID constant.
	*/
	virtual QByteArray GetSystemTenantId() const { return imtauth::GetSystemTenantId(); }

	/**
		Ensure the System-Tenant exists.
		Creates the System-Tenant if it does not already exist.
		Should be called during server bootstrap/initialization.
		\return true if the System-Tenant exists or was successfully created.
	*/
	virtual bool EnsureSystemTenant() { return false; }
};


} // namespace imtauth
