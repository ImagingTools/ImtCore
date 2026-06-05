// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnectionCode.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


CTenantConnectionCode::CTenantConnectionCode():
	m_allowConnectionsByCode(true)
{
}


QByteArray CTenantConnectionCode::GetTenantId() const
{
	return m_tenantId;
}


void CTenantConnectionCode::SetTenantId(const QByteArray& tenantId)
{
	if (m_tenantId != tenantId){
		istd::CChangeNotifier notifier(this);
		m_tenantId = tenantId;
	}
}


QString CTenantConnectionCode::GetConnectionCode() const
{
	return m_connectionCode;
}


void CTenantConnectionCode::SetConnectionCode(const QString& connectionCode)
{
	if (m_connectionCode != connectionCode){
		istd::CChangeNotifier notifier(this);
		m_connectionCode = connectionCode;
	}
}


bool CTenantConnectionCode::GetAllowConnectionsByCode() const
{
	return m_allowConnectionsByCode;
}


void CTenantConnectionCode::SetAllowConnectionsByCode(bool allow)
{
	if (m_allowConnectionsByCode != allow){
		istd::CChangeNotifier notifier(this);
		m_allowConnectionsByCode = allow;
	}
}


QString CTenantConnectionCode::GetCreatedAt() const
{
	return m_createdAt;
}


void CTenantConnectionCode::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);
		m_createdAt = createdAt;
	}
}


bool CTenantConnectionCode::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag codeTag("ConnectionCode", "Connection code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(codeTag);
	retVal = retVal && archive.Process(m_connectionCode);
	retVal = retVal && archive.EndTag(codeTag);

	iser::CArchiveTag allowTag("AllowConnectionsByCode", "Allow connections by code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(allowTag);
	int allowInt = m_allowConnectionsByCode ? 1 : 0;
	retVal = retVal && archive.Process(allowInt);
	m_allowConnectionsByCode = (allowInt != 0);
	retVal = retVal && archive.EndTag(allowTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	return retVal;
}


bool CTenantConnectionCode::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantConnectionCode* sourcePtr = dynamic_cast<const CTenantConnectionCode*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_tenantId = sourcePtr->m_tenantId;
	m_connectionCode = sourcePtr->m_connectionCode;
	m_allowConnectionsByCode = sourcePtr->m_allowConnectionsByCode;
	m_createdAt = sourcePtr->m_createdAt;

	return true;
}


istd::IChangeableUniquePtr CTenantConnectionCode::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantConnectionCode);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantConnectionCode::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_tenantId.clear();
	m_connectionCode.clear();
	m_allowConnectionsByCode = true;
	m_createdAt.clear();

	return true;
}


} // namespace imtauth
