// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtdb/ITenantStorageResolver.h>


namespace imtdb
{


/**
	Thread-safe in-memory registry mapping tenants to their physical storage.
	Resolution is fail-closed: unknown tenants are rejected unless the
	fallback to the shared storage is explicitly enabled.
*/
class CTenantStorageRegistry
{
public:
	CTenantStorageRegistry();

	/**
		Enable or disable the fallback to the shared schema for unregistered tenants.
		Disabled by default (fail-closed).
	*/
	void SetSharedFallbackEnabled(bool isEnabled);
	bool IsSharedFallbackEnabled() const;

	/**
		Set the name of the shared schema used for the fallback and for
		\c TSK_SHARED_SCHEMA assignments with an empty schema name.
	*/
	void SetSharedSchemaName(const QByteArray& schemaName);
	QByteArray GetSharedSchemaName() const;

	/**
		Set the prefix used to derive schema names for dedicated tenant schemas.
	*/
	void SetSchemaNamePrefix(const QByteArray& prefix);
	QByteArray GetSchemaNamePrefix() const;

	bool ResolveTenantStorage(const QByteArray& tenantId, TenantStorageInfo& result) const;
	bool RegisterTenantStorage(const QByteArray& tenantId, const TenantStorageInfo& info);
	bool UnregisterTenantStorage(const QByteArray& tenantId);
	bool IsTenantStorageRegistered(const QByteArray& tenantId) const;
	QByteArrayList GetRegisteredTenantIds() const;

	/**
		Derive a safe schema name for the given tenant from the configured prefix.
		The tenant ID is sanitized to lowercase [a-z0-9_] characters.
	*/
	QByteArray CreateSchemaNameForTenant(const QByteArray& tenantId) const;

	/**
		Derive a safe schema name from the given prefix and tenant ID.
		The tenant ID is sanitized to lowercase [a-z0-9_] characters.
	*/
	static QByteArray CreateSchemaName(const QByteArray& prefix, const QByteArray& tenantId);

private:
	mutable QMutex m_mutex;

	bool m_isSharedFallbackEnabled;
	QByteArray m_sharedSchemaName;
	QByteArray m_schemaNamePrefix;

	QMap<QByteArray, TenantStorageInfo> m_assignments;
};


} // namespace imtdb
