// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnectionCode.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CTenantConnectionCode::CTenantConnectionCode()
{
	m_info.allowConnectionsByCode = true;
}


TenantConnectionCodeInfo CTenantConnectionCode::GetCodeInfo() const
{
	return m_info;
}


void CTenantConnectionCode::SetCodeInfo(const TenantConnectionCodeInfo& codeInfo)
{
	if (m_info != codeInfo){
		istd::CChangeNotifier notifier(this);
		m_info = codeInfo;
	}
}


QByteArray CTenantConnectionCode::GetTenantId() const
{
	return m_info.tenantId;
}


bool CTenantConnectionCode::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.tenantId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag codeTag("ConnectionCode", "Connection code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(codeTag);
	retVal = retVal && archive.Process(m_info.connectionCode);
	retVal = retVal && archive.EndTag(codeTag);

	iser::CArchiveTag allowTag("AllowConnectionsByCode", "Allow connections by code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(allowTag);
	int allowInt = m_info.allowConnectionsByCode ? 1 : 0;
	retVal = retVal && archive.Process(allowInt);
	m_info.allowConnectionsByCode = (allowInt != 0);
	retVal = retVal && archive.EndTag(allowTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
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
	m_info = sourcePtr->m_info;

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
	m_info = TenantConnectionCodeInfo();

	return true;
}


} // namespace imtauth
