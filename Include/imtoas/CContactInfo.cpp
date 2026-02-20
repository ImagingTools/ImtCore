// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CContactInfo.h>


namespace imtoas
{


QString CContactInfo::GetName() const
{
	return m_name;
}


void CContactInfo::SetName(const QString& name)
{
	if (name != m_name){
		m_name = name;
	}
}


QString CContactInfo::GetUrl() const
{
	return m_url;
}


void CContactInfo::SetUrl(const QString& url)
{
	if (url != m_url){
		m_url = url;

	}
}


QString CContactInfo::GetEmail() const
{
	return m_email;
}


void CContactInfo::SetEmail(const QString& email)
{
	if (email != m_email){
		m_email = email;
	}
}


bool CContactInfo::ReadFromJsonObject(CContactInfo& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant nameData = jsonObject.value("name").toVariant();
	if (!nameData.isNull()){
		object.SetName(nameData.toString());
	}

	QVariant urlData = jsonObject.value("url").toVariant();
	if (!urlData.isNull()){
		object.SetUrl(urlData.toString());
	}

	QVariant emailData = jsonObject.value("email").toVariant();
	if (!emailData.isNull()){
		object.SetEmail(emailData.toString());
	}

	return true;
}


bool CContactInfo::operator==(const CContactInfo& other) const
{
	bool retVal = true;
	retVal = retVal && (m_name == other.m_name);
	retVal = retVal && (m_url == other.m_url);
	retVal = retVal && (m_email == other.m_email);

	return retVal;
}


} // namespace imtoas

