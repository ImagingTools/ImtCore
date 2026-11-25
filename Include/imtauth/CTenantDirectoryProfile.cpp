// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantDirectoryProfile.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ArchiveFields.h>


namespace imtauth
{


namespace
{


namespace af = ArchiveFields;


template <typename Value>
void AssignValue(istd::IChangeable& object, Value& member, const Value& value)
{
	if (member != value){
		istd::CChangeNotifier notifier(&object);

		member = value;
	}
}


} // anonymous namespace


// public methods

CTenantDirectoryProfile::CTenantDirectoryProfile():
	m_tenantActive(true),
	m_listed(false),
	m_acceptingInvitationsFromUnknown(true)
{
}


// reimplemented (imtauth::ITenantDirectoryProfile)

QByteArray CTenantDirectoryProfile::GetTenantId() const
{
	return m_tenantId;
}


void CTenantDirectoryProfile::SetTenantId(const QByteArray& value)
{
	AssignValue(*this, m_tenantId, value);
}


QString CTenantDirectoryProfile::GetTenantName() const
{
	return m_tenantName;
}


void CTenantDirectoryProfile::SetTenantName(const QString& value)
{
	AssignValue(*this, m_tenantName, value);
}


bool CTenantDirectoryProfile::IsTenantActive() const
{
	return m_tenantActive;
}


void CTenantDirectoryProfile::SetTenantActive(bool value)
{
	AssignValue(*this, m_tenantActive, value);
}


bool CTenantDirectoryProfile::IsListed() const
{
	return m_listed;
}


void CTenantDirectoryProfile::SetListed(bool value)
{
	AssignValue(*this, m_listed, value);
}


bool CTenantDirectoryProfile::IsAcceptingInvitationsFromUnknown() const
{
	return m_acceptingInvitationsFromUnknown;
}


void CTenantDirectoryProfile::SetAcceptingInvitationsFromUnknown(bool value)
{
	AssignValue(*this, m_acceptingInvitationsFromUnknown, value);
}


QString CTenantDirectoryProfile::GetDescription() const
{
	return m_description;
}


void CTenantDirectoryProfile::SetDescription(const QString& value)
{
	AssignValue(*this, m_description, value);
}


QByteArrayList CTenantDirectoryProfile::GetOfferedRoleIds() const
{
	return m_offeredRoleIds;
}


void CTenantDirectoryProfile::SetOfferedRoleIds(const QByteArrayList& value)
{
	AssignValue(*this, m_offeredRoleIds, value);
}


QString CTenantDirectoryProfile::GetIndustry() const
{
	return m_industry;
}


void CTenantDirectoryProfile::SetIndustry(const QString& value)
{
	AssignValue(*this, m_industry, value);
}


QString CTenantDirectoryProfile::GetCountry() const
{
	return m_country;
}


void CTenantDirectoryProfile::SetCountry(const QString& value)
{
	AssignValue(*this, m_country, value);
}


QString CTenantDirectoryProfile::GetWebsite() const
{
	return m_website;
}


void CTenantDirectoryProfile::SetWebsite(const QString& value)
{
	AssignValue(*this, m_website, value);
}


QDateTime CTenantDirectoryProfile::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CTenantDirectoryProfile::SetUpdatedAt(const QDateTime& value)
{
	AssignValue(*this, m_updatedAt, value);
}


// reimplemented (iser::ISerializable)

bool CTenantDirectoryProfile::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = af::Leaf(archive, "TenantId", m_tenantId);
	retVal = retVal && af::Leaf(archive, "TenantName", m_tenantName);
	retVal = retVal && af::Leaf(archive, "IsTenantActive", m_tenantActive);
	retVal = retVal && af::Leaf(archive, "IsListed", m_listed);
	retVal = retVal && af::Leaf(archive, "AcceptsInvitationsFromUnknown", m_acceptingInvitationsFromUnknown);
	retVal = retVal && af::Leaf(archive, "Description", m_description);
	retVal = retVal && af::Leaf(archive, "OfferedRoleIds", m_offeredRoleIds);
	retVal = retVal && af::Leaf(archive, "Industry", m_industry);
	retVal = retVal && af::Leaf(archive, "Country", m_country);
	retVal = retVal && af::Leaf(archive, "Website", m_website);
	retVal = retVal && af::Leaf(archive, "UpdatedAt", m_updatedAt);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTenantDirectoryProfile::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ITenantDirectoryProfile* sourcePtr = dynamic_cast<const ITenantDirectoryProfile*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_tenantId = sourcePtr->GetTenantId();
	m_tenantName = sourcePtr->GetTenantName();
	m_tenantActive = sourcePtr->IsTenantActive();
	m_listed = sourcePtr->IsListed();
	m_acceptingInvitationsFromUnknown = sourcePtr->IsAcceptingInvitationsFromUnknown();
	m_description = sourcePtr->GetDescription();
	m_offeredRoleIds = sourcePtr->GetOfferedRoleIds();
	m_industry = sourcePtr->GetIndustry();
	m_country = sourcePtr->GetCountry();
	m_website = sourcePtr->GetWebsite();
	m_updatedAt = sourcePtr->GetUpdatedAt();

	return true;
}


bool CTenantDirectoryProfile::IsEqual(const IChangeable& object) const
{
	const ITenantDirectoryProfile* otherPtr = dynamic_cast<const ITenantDirectoryProfile*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_tenantId == otherPtr->GetTenantId()
				&& m_tenantName == otherPtr->GetTenantName()
				&& m_tenantActive == otherPtr->IsTenantActive()
				&& m_listed == otherPtr->IsListed()
				&& m_acceptingInvitationsFromUnknown == otherPtr->IsAcceptingInvitationsFromUnknown()
				&& m_description == otherPtr->GetDescription()
				&& m_offeredRoleIds == otherPtr->GetOfferedRoleIds()
				&& m_industry == otherPtr->GetIndustry()
				&& m_country == otherPtr->GetCountry()
				&& m_website == otherPtr->GetWebsite()
				&& m_updatedAt == otherPtr->GetUpdatedAt();
}


istd::IChangeableUniquePtr CTenantDirectoryProfile::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantDirectoryProfile);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantDirectoryProfile::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_tenantId = QByteArray();
	m_tenantName = QString();
	m_tenantActive = true;
	m_listed = false;
	m_acceptingInvitationsFromUnknown = true;
	m_description = QString();
	m_offeredRoleIds = QByteArrayList();
	m_industry = QString();
	m_country = QString();
	m_website = QString();
	m_updatedAt = QDateTime();

	return true;
}


} // namespace imtauth


