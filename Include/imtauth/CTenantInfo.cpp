// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


// public methods

CTenantInfo::CTenantInfo():
	m_isActive(true)
{
}


// reimplemented (ITenantInfo)

QByteArray CTenantInfo::GetTenantId() const
{
	return m_tenantId;
}


void CTenantInfo::SetTenantId(const QByteArray& tenantId)
{
	if (m_tenantId != tenantId){
		istd::CChangeNotifier changeNotifier(this);

		m_tenantId = tenantId;
	}
}


QString CTenantInfo::GetTenantName() const
{
	return m_name;
}


void CTenantInfo::SetTenantName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QString CTenantInfo::GetTenantDescription() const
{
	return m_description;
}


void CTenantInfo::SetTenantDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


bool CTenantInfo::IsActive() const
{
	return m_isActive;
}


void CTenantInfo::SetActive(bool isActive)
{
	if (m_isActive != isActive){
		istd::CChangeNotifier changeNotifier(this);

		m_isActive = isActive;
	}
}


// reimplemented (iser::ISerializable)

bool CTenantInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag tenantTag("Tenant");
	bool retVal = archive.BeginTag(tenantTag);

	retVal = retVal && archive.Process(m_tenantId, "TenantId");
	retVal = retVal && archive.Process(m_name, "Name");
	retVal = retVal && archive.Process(m_description, "Description");
	retVal = retVal && archive.Process(m_isActive, "IsActive");

	retVal = archive.EndTag(tenantTag) && retVal;

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTenantInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantInfo* sourcePtr = dynamic_cast<const CTenantInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_tenantId = sourcePtr->m_tenantId;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_isActive = sourcePtr->m_isActive;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CTenantInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_tenantId.clear();
	m_name.clear();
	m_description.clear();
	m_isActive = true;

	return true;
}


} // namespace imtauth


