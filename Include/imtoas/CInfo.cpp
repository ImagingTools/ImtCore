// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CInfo.h>


namespace imtoas
{


QString CInfo::GetTitle() const
{
	return m_title;
}


void CInfo::SetTitle(const QString& title)
{
	if (title != m_title){
		m_title = title;
	}
}


QString CInfo::GetSummary() const
{
	return m_summary;
}


void CInfo::SetSummary(const QString& summary)
{
	if (summary != m_summary){
		m_summary = summary;
	}
}


QString CInfo::GetDescription() const
{
	return m_description;
}


void CInfo::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


QString CInfo::GetTermsOfService() const
{
	return m_termsOfService;
}


void CInfo::SetTermsOfService(const QString& termsOfService)
{
	if (termsOfService != m_termsOfService){
		m_termsOfService = termsOfService;
	}
}


CContactInfo CInfo::GetContact() const
{
	return m_contact;
}


void CInfo::SetContact(const CContactInfo& contact)
{
	if (contact != m_contact){
		m_contact = contact;
	}
}


CLicense CInfo::GetLicense() const
{
	return m_license;
}


void CInfo::SetLicense(const CLicense& license)
{
	if (license != m_license){
		m_license = license;

	}
}


QString CInfo::GetVersion() const
{
	return m_version;
}


void CInfo::SetVersion(const QString& version)
{
	if (version != m_version){
		m_version = version;
	}
}


bool CInfo::ReadFromJsonObject(CInfo& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant titleData = jsonObject.value("title").toVariant();
	if (titleData.isNull()){
		return false;
	}
	object.SetTitle(titleData.toString());

	QVariant summaryData = jsonObject.value("summary").toVariant();
	if (!summaryData.isNull()){
		object.SetSummary(summaryData.toString());
	}

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	QVariant termsOfServiceData = jsonObject.value("termsOfService").toVariant();
	if (!termsOfServiceData.isNull()){
		object.SetTermsOfService(termsOfServiceData.toString());
	}

	if (jsonObject.contains("contact")){
		CContactInfo contact;
		const bool isContactReaded = CContactInfo::ReadFromJsonObject(contact, jsonObject["contact"].toObject(), globalObject);
		if (!isContactReaded){
			return false;
		}
		object.SetContact(contact);
	}

	if (jsonObject.contains("license")){
		CLicense license;
		const bool isLicenseReaded = CLicense::ReadFromJsonObject(license, jsonObject["license"].toObject(), globalObject);
		if (!isLicenseReaded){
			return false;
		}
		object.SetLicense(license);
	}

	QVariant versionData = jsonObject.value("version").toVariant();
	if (versionData.isNull()){
		return false;
	}
	object.SetVersion(versionData.toString());

	return true;
}


bool CInfo::operator==(const CInfo& other) const
{
	bool retVal = true;
	retVal = retVal && (m_title == other.m_title);
	retVal = retVal && (m_summary == other.m_summary);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_termsOfService == other.m_termsOfService);
	retVal = retVal && (m_contact == other.m_contact);
	retVal = retVal && (m_license == other.m_license);
	retVal = retVal && (m_version == other.m_version);

	return retVal;
}


} // namespace imtoas

