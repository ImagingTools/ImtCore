// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnection.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CTenantConnection::CTenantConnection()
{
	m_info.status = CS_ACTIVE;
}


TenantConnectionInfo CTenantConnection::GetConnectionInfo() const
{
	return m_info;
}


void CTenantConnection::SetConnectionInfo(const TenantConnectionInfo& connectionInfo)
{
	if (m_info != connectionInfo){
		istd::CChangeNotifier notifier(this);
		m_info = connectionInfo;
	}
}


QByteArray CTenantConnection::GetConnectionId() const
{
	return m_info.connectionId;
}


bool CTenantConnection::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.connectionId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag tenantATag("TenantAId", "Tenant A ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantATag);
	retVal = retVal && archive.Process(m_info.tenantAId);
	retVal = retVal && archive.EndTag(tenantATag);

	iser::CArchiveTag tenantBTag("TenantBId", "Tenant B ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantBTag);
	retVal = retVal && archive.Process(m_info.tenantBId);
	retVal = retVal && archive.EndTag(tenantBTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_info.status);
	retVal = retVal && archive.Process(status);
	m_info.status = static_cast<ConnectionStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_info.updatedAt);
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
	m_info = sourcePtr->m_info;

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
	m_info = TenantConnectionInfo();

	return true;
}


} // namespace imtauth
