// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CLicense.h>


namespace imtoas
{


QString CLicense::GetName() const
{
	return m_name;
}


void CLicense::SetName(const QString& name)
{
	if (name != m_name){
		m_name = name;
	}
}


QString CLicense::GetIdentifier() const
{
	return m_identifier;
}


void CLicense::SetIdentifier(const QString& identifier)
{
	if (identifier != m_identifier){
		m_identifier = identifier;
	}
}


QString CLicense::GetUrl() const
{
	return m_url;
}


void CLicense::SetUrl(const QString& url)
{
	if (url != m_url){
		m_url = url;
	}
}


bool CLicense::ReadFromJsonObject(CLicense& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant nameData = jsonObject.value("name").toVariant();
	if (nameData.isNull()){
		return false;
	}
	object.SetName(nameData.toString());

	QVariant identifierData = jsonObject.value("identifier").toVariant();
	if (!identifierData.isNull()){
		object.SetIdentifier(identifierData.toString());
	}

	QVariant urlData = jsonObject.value("url").toVariant();
	if (!urlData.isNull()){
		object.SetUrl(urlData.toString());
	}

	return true;
}


bool CLicense::operator==(const CLicense& other) const
{
	bool retVal = true;
	retVal = retVal && (m_name == other.m_name);
	retVal = retVal && (m_identifier == other.m_identifier);
	retVal = retVal && (m_url == other.m_url);

	return retVal;
}


} // namespace imtoas

