#include <imtoas/CServer.h>


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


bool CServer::ReadFromJsonObject(CServer& object, const QJsonObject& jsonObject)
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
		const QJsonArray variablesArray = jsonObject["variables"].toArray();
		qsizetype variablesCount = variablesArray.size();
		QList<CServerVariable> variablesList;
		for (int variablesIndex = 0; variablesIndex < variablesCount; ++variablesIndex){
			CServerVariable variables;
			if (!CServerVariable::ReadFromJsonObject(variables, variablesArray[variablesIndex].toObject())){
				return false;
			}
			variablesList << variables;
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

