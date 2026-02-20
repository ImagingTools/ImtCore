// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CServer.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


QString CServer::GetUrl() const
{
	return m_url;
}


void CServer::SetUrl(const QString& url)
{
	if (url != m_url){
		m_url = url;

	}
}


QString CServer::GetDescription() const
{
	return m_description;
}


void CServer::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


QList<CServerVariable> CServer::GetVariables() const
{
	return m_variables;
}


void CServer::SetVariables(const QList<CServerVariable>& variables)
{
	if (variables != m_variables){
		m_variables = variables;
	}
}


bool CServer::ReadFromJsonObject(CServer& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant urlData = jsonObject.value("url").toVariant();
	if (urlData.isNull()){
		return false;
	}
	object.SetUrl(urlData.toString());

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	if (jsonObject.contains("variables")){
		const QJsonObject variablesArray = jsonObject["variables"].toObject();
		QList<CServerVariable> variablesList;
		if (!COasTools::ExtractItemsFromObject(variablesList, variablesArray, globalObject)){
			return false;
		}
		object.SetVariables(variablesList);
	}

	return true;
}


bool CServer::operator==(const CServer& other) const
{
	bool retVal = true;
	retVal = retVal && (m_url == other.m_url);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_variables == other.m_variables);

	return retVal;
}


} // namespace imtoas

