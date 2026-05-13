// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CSecurityScheme.h>


namespace imtoas
{


CSecurityScheme::CSecurityScheme()
	:m_type(T_HTTP)
{
}

QString CSecurityScheme::GetId() const
{
	return m_id;
}


void CSecurityScheme::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}
CSecurityScheme::Type CSecurityScheme::GetType() const
{
	return m_type;
}


void CSecurityScheme::SetType(Type type)
{
	if (type != m_type){
		m_type = type;
	}
}


QString CSecurityScheme::GetDescription() const
{
	return m_description;
}


void CSecurityScheme::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


QString CSecurityScheme::GetName() const
{
	return m_name;
}


void CSecurityScheme::SetName(const QString& name)
{
	if (name != m_name){
		m_name = name;
	}
}


QString CSecurityScheme::GetIn() const
{
	return m_in;
}


void CSecurityScheme::SetIn(const QString& in)
{
	if (in != m_in){
		m_in = in;
	}
}


QString CSecurityScheme::GetScheme() const
{
	return m_scheme;
}


void CSecurityScheme::SetScheme(const QString& scheme)
{
	if (scheme != m_scheme){
		m_scheme = scheme;
	}
}


QString CSecurityScheme::GetOpenIdConnectUrl() const
{
	return m_openIdConnectUrl;
}


void CSecurityScheme::SetOpenIdConnectUrl(const QString& openIdConnectUrl)
{
	if (openIdConnectUrl != m_openIdConnectUrl){
		m_openIdConnectUrl = openIdConnectUrl;
	}
}


bool CSecurityScheme::ReadFromJsonObject(CSecurityScheme& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QByteArray typeId = jsonObject.value("type").toString().toUtf8();
	if (typeId.isEmpty()){
		return false;
	}

	Type type = T_API_KEY;
	bool isTypeIdValid = FromString(typeId, type);
	if (!isTypeIdValid){
		return false;
	}
	object.SetType(type);

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	if (type == T_API_KEY){
		QVariant nameData = jsonObject.value("name").toVariant();
		if (nameData.isNull()){
			return false;
		}
		object.SetName(nameData.toString());

		QVariant inData = jsonObject.value("in").toVariant();
		if (inData.isNull()){
			return false;
		}
		object.SetIn(inData.toString());
	}

	if (type == T_HTTP){
		QVariant schemeData = jsonObject.value("scheme").toVariant();
		if (schemeData.isNull()){
			return false;
		}
		object.SetScheme(schemeData.toString());
	}

	if (type == T_OPEN_ID_CONNECT){
		QVariant openIdConnectUrlData = jsonObject.value("openIdConnectUrl").toVariant();
		if (openIdConnectUrlData.isNull()){
			return false;
		}
		object.SetOpenIdConnectUrl(openIdConnectUrlData.toString());
	}

	return true;
}


bool CSecurityScheme::operator==(const CSecurityScheme& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_type == other.m_type);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_name == other.m_name);
	retVal = retVal && (m_in == other.m_in);
	retVal = retVal && (m_scheme == other.m_scheme);
	retVal = retVal && (m_openIdConnectUrl == other.m_openIdConnectUrl);

	return retVal;
}



} // namespace imtoas

