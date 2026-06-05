// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnection.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


CTenantConnection::CTenantConnection():
	m_status(CS_ACTIVE)
{
}


QByteArray CTenantConnection::GetConnectionId() const
{
	return m_connectionId;
}


void CTenantConnection::SetConnectionId(const QByteArray& connectionId)
{
	if (m_connectionId != connectionId){
		istd::CChangeNotifier notifier(this);
		m_connectionId = connectionId;
	}
}


QByteArray CTenantConnection::GetTenantAId() const
{
	return m_tenantAId;
}


void CTenantConnection::SetTenantAId(const QByteArray& tenantAId)
{
	if (m_tenantAId != tenantAId){
		istd::CChangeNotifier notifier(this);
		m_tenantAId = tenantAId;
	}
}


QByteArray CTenantConnection::GetTenantBId() const
{
	return m_tenantBId;
}


void CTenantConnection::SetTenantBId(const QByteArray& tenantBId)
{
	if (m_tenantBId != tenantBId){
		istd::CChangeNotifier notifier(this);
		m_tenantBId = tenantBId;
	}
}


ITenantConnectionInfo::ConnectionStatus CTenantConnection::GetStatus() const
{
	return m_status;
}


void CTenantConnection::SetStatus(ConnectionStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);
		m_status = status;
	}
}


QString CTenantConnection::GetCreatedAt() const
{
	return m_createdAt;
}


void CTenantConnection::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);
		m_createdAt = createdAt;
	}
}


QString CTenantConnection::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CTenantConnection::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);
		m_updatedAt = updatedAt;
	}
}


bool CTenantConnection::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_connectionId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag tenantATag("TenantAId", "Tenant A ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantATag);
	retVal = retVal && archive.Process(m_tenantAId);
	retVal = retVal && archive.EndTag(tenantATag);

	iser::CArchiveTag tenantBTag("TenantBId", "Tenant B ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantBTag);
	retVal = retVal && archive.Process(m_tenantBId);
	retVal = retVal && archive.EndTag(tenantBTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_status);
	retVal = retVal && archive.Process(status);
	m_status = static_cast<ConnectionStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


bool CTenantConnection::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantConnection* sourcePtr = dynamic_cast<const CTenantConnection*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_connectionId = sourcePtr->m_connectionId;
	m_tenantAId = sourcePtr->m_tenantAId;
	m_tenantBId = sourcePtr->m_tenantBId;
	m_status = sourcePtr->m_status;
	m_createdAt = sourcePtr->m_createdAt;
	m_updatedAt = sourcePtr->m_updatedAt;

	return true;
}


istd::IChangeableUniquePtr CTenantConnection::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantConnection);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantConnection::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_connectionId.clear();
	m_tenantAId.clear();
	m_tenantBId.clear();
	m_status = CS_ACTIVE;
	m_createdAt.clear();
	m_updatedAt.clear();

	return true;
}


} // namespace imtauth
