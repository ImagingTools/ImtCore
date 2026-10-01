// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantStorageRegistry.h>


namespace imtdb
{


CTenantStorageRegistry::CTenantStorageRegistry()
:	m_isSharedFallbackEnabled(false),
	m_sharedSchemaName(QByteArrayLiteral("public")),
	m_schemaNamePrefix(QByteArrayLiteral("tenant_"))
{
}


void CTenantStorageRegistry::SetSharedFallbackEnabled(bool isEnabled)
{
	QMutexLocker locker(&m_mutex);

	m_isSharedFallbackEnabled = isEnabled;
}


bool CTenantStorageRegistry::IsSharedFallbackEnabled() const
{
	QMutexLocker locker(&m_mutex);

	return m_isSharedFallbackEnabled;
}


void CTenantStorageRegistry::SetSharedSchemaName(const QByteArray& schemaName)
{
	QMutexLocker locker(&m_mutex);

	m_sharedSchemaName = schemaName;
}


QByteArray CTenantStorageRegistry::GetSharedSchemaName() const
{
	QMutexLocker locker(&m_mutex);

	return m_sharedSchemaName;
}


void CTenantStorageRegistry::SetSchemaNamePrefix(const QByteArray& prefix)
{
	QMutexLocker locker(&m_mutex);

	m_schemaNamePrefix = prefix;
}


QByteArray CTenantStorageRegistry::GetSchemaNamePrefix() const
{
	QMutexLocker locker(&m_mutex);

	return m_schemaNamePrefix;
}


bool CTenantStorageRegistry::ResolveTenantStorage(const QByteArray& tenantId, TenantStorageInfo& result) const
{
	if (tenantId.isEmpty()){
		return false;
	}

	QMutexLocker locker(&m_mutex);

	auto foundIter = m_assignments.constFind(tenantId);
	if (foundIter != m_assignments.constEnd()){
		result = foundIter.value();

		if (result.schemaName.isEmpty() && (result.storageKind == TSK_SHARED_SCHEMA)){
			result.schemaName = m_sharedSchemaName;
		}

		return true;
	}

	if (!m_isSharedFallbackEnabled){
		// fail-closed: unknown tenants are rejected
		return false;
	}

	result = TenantStorageInfo();
	result.storageKind = TSK_SHARED_SCHEMA;
	result.status = TSS_ACTIVE;
	result.schemaName = m_sharedSchemaName;

	return true;
}


bool CTenantStorageRegistry::RegisterTenantStorage(const QByteArray& tenantId, const TenantStorageInfo& info)
{
	if (tenantId.isEmpty()){
		return false;
	}

	if ((info.storageKind != TSK_SHARED_SCHEMA) && info.schemaName.isEmpty() && info.connectionRef.isEmpty()){
		// a dedicated storage assignment must define at least a schema or a connection
		return false;
	}

	QMutexLocker locker(&m_mutex);

	m_assignments.insert(tenantId, info);

	return true;
}


bool CTenantStorageRegistry::UnregisterTenantStorage(const QByteArray& tenantId)
{
	QMutexLocker locker(&m_mutex);

	return m_assignments.remove(tenantId) > 0;
}


bool CTenantStorageRegistry::IsTenantStorageRegistered(const QByteArray& tenantId) const
{
	QMutexLocker locker(&m_mutex);

	return m_assignments.contains(tenantId);
}


QByteArrayList CTenantStorageRegistry::GetRegisteredTenantIds() const
{
	QMutexLocker locker(&m_mutex);

	QByteArrayList retVal;
	for (auto iter = m_assignments.constBegin(); iter != m_assignments.constEnd(); ++iter){
		retVal.append(iter.key());
	}

	return retVal;
}


QByteArray CTenantStorageRegistry::CreateSchemaNameForTenant(const QByteArray& tenantId) const
{
	QMutexLocker locker(&m_mutex);

	return CreateSchemaName(m_schemaNamePrefix, tenantId);
}


QByteArray CTenantStorageRegistry::CreateSchemaName(const QByteArray& prefix, const QByteArray& tenantId)
{
	QByteArray retVal = prefix;

	for (char character: tenantId){
		char lowerCharacter = char(QChar::fromLatin1(character).toLower().toLatin1());
		bool isAllowed =	((lowerCharacter >= 'a') && (lowerCharacter <= 'z')) ||
							((lowerCharacter >= '0') && (lowerCharacter <= '9'));
		retVal += isAllowed ? lowerCharacter : '_';
	}

	return retVal;
}


} // namespace imtdb
