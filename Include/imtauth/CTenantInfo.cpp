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
		istd::CChangeNotifier notifier(this);

		m_isActive = isActive;
	}
}


QString CTenantInfo::GetCreatedAt() const
{
	return m_createdAt;
}


void CTenantInfo::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CTenantInfo::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CTenantInfo::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);

		m_updatedAt = updatedAt;
	}
}


// reimplemented (iser::ISerializable)

bool CTenantInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag isActiveTag("IsActive", "Is active", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isActiveTag);
	retVal = retVal && archive.Process(m_isActive);
	retVal = retVal && archive.EndTag(isActiveTag);

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


// reimplemented (istd::IChangeable)

bool CTenantInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantInfo* sourcePtr = dynamic_cast<const CTenantInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_tenantId = sourcePtr->m_tenantId;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_isActive = sourcePtr->m_isActive;
		m_createdAt = sourcePtr->m_createdAt;
		m_updatedAt = sourcePtr->m_updatedAt;

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
	istd::CChangeNotifier notifier(this);

	m_tenantId.clear();
	m_name.clear();
	m_description.clear();
	m_isActive = true;
	m_createdAt.clear();
	m_updatedAt.clear();

	return true;
}


} // namespace imtauth


