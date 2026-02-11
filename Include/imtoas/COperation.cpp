// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/COperation.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


COperation::COperation()
	:	m_isDeprecated(false)
{
}


QList<QString> COperation::GetTags() const
{
	return m_tags;
}


void COperation::SetTags(const QList<QString>& tags)
{
	if (tags != m_tags){
		m_tags = tags;
	}
}


QString COperation::GetSummary() const
{
	return m_summary;
}


void COperation::SetSummary(const QString& summary)
{
	if (summary != m_summary){
		m_summary = summary;
	}
}


QString COperation::GetDescription() const
{
	return m_description;
}


void COperation::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


QString COperation::GetOperationId() const
{
	return m_operationId;
}


void COperation::SetOperationId(const QString& operationId)
{
	if (operationId != m_operationId){
		m_operationId = operationId;
	}
}


QList<CParameter> COperation::GetParameters() const
{
	return m_parameters;
}


void COperation::SetParameters(const QList<CParameter>& parameters)
{
	if (parameters != m_parameters){
		m_parameters = parameters;
	}
}


CRequestBody COperation::GetRequestBody() const
{
	return m_requestBody;
}


void COperation::SetRequestBody(const CRequestBody& requestBody)
{
	if (requestBody != m_requestBody){
		m_requestBody = requestBody;
	}
}


QList<CResponse> COperation::GetResponses() const
{
	return m_responses;
}


void COperation::SetResponses(const QList<CResponse>& responses)
{
	if (responses != m_responses){
		m_responses = responses;
	}
}


bool COperation::IsDeprecated() const
{
	return m_isDeprecated;
}


void COperation::SetDeprecated(bool deprecated)
{
	if (deprecated != m_isDeprecated){
		m_isDeprecated = deprecated;
	}
}


QList<CSecurityScheme> COperation::GetSecurity() const
{
	return m_security;
}


void COperation::SetSecurity(const QList<CSecurityScheme>& security)
{
	if (security != m_security){
		m_security = security;
	}
}


QList<CServer> COperation::GetServers() const
{
	return m_servers;
}


void COperation::SetServers(const QList<CServer>& servers)
{
	if (servers != m_servers){
		m_servers = servers;
	}
}


bool COperation::ReadFromJsonObject(COperation& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	if (jsonObject.contains("tags")){
		const QJsonArray tagsArray = jsonObject["tags"].toArray();
		qsizetype tagsCount = tagsArray.size();
		QList<QString> tagsList;
		for (qsizetype tagsIndex = 0; tagsIndex < tagsCount; ++tagsIndex){
			tagsList << tagsArray[tagsIndex].toString();
		}
		object.SetTags(tagsList);
	}

	QVariant summaryData = jsonObject.value("summary").toVariant();
	if (!summaryData.isNull()){
		object.SetSummary(summaryData.toString());
	}

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	QVariant operationIdData = jsonObject.value("operationId").toVariant();
	if (!operationIdData.isNull()){
		object.SetOperationId(operationIdData.toString());
	}

	if (jsonObject.contains("parameters")){
		const QJsonArray parametersArray = jsonObject["parameters"].toArray();
		qsizetype parametersCount = parametersArray.size();
		QList<CParameter> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			CParameter parameters;
			if (!CParameter::ReadFromJsonObject(parameters, parametersArray[parametersIndex].toObject(), globalObject)){
				return false;
			}
			parametersList << parameters;
		}
		object.SetParameters(parametersList);
	}

	if (jsonObject.contains("requestBody")){
		CRequestBody requestBody;
		const bool isRequestBodyReaded = CRequestBody::ReadFromJsonObject(requestBody, jsonObject["requestBody"].toObject(), globalObject);
		if (!isRequestBodyReaded){
			return false;
		}
		object.SetRequestBody(requestBody);
	}

	if (jsonObject.contains("responses")){
		const QJsonObject responsesArray = jsonObject["responses"].toObject();
		QList<CResponse> responsesList;
		if (!COasTools::ExtractItemsFromObject(responsesList, responsesArray, globalObject)){
			return false;
		}
		object.SetResponses(responsesList);
	}

	QVariant deprecatedData = jsonObject.value("deprecated").toVariant();
	if (!deprecatedData.isNull()){
		object.SetDeprecated(deprecatedData.toBool());
	}

	if (jsonObject.contains("security")){
		const QJsonArray securityArray = jsonObject["security"].toArray();
		qsizetype securityCount = securityArray.size();
		QList<CSecurityScheme> securityList;
		for (int securityIndex = 0; securityIndex < securityCount; ++securityIndex){
			QJsonObject securityObject = securityArray[securityIndex].toObject();
			if (securityObject.keys().size() > 1){
				return false;
			}
			else if (securityObject.keys().size() < 1){
				// empty scheme. skip
				continue;
			}

			securityObject = COasTools::FindSecuritySchemeByName(securityObject.keys().constFirst(), globalObject);
			CSecurityScheme security;
			if (!CSecurityScheme::ReadFromJsonObject(security, securityObject, globalObject)){
				return false;
			}
			securityList << security;
		}
		object.SetSecurity(securityList);
	}

	if (jsonObject.contains("servers")){
		const QJsonArray serversArray = jsonObject["servers"].toArray();
		qsizetype serversCount = serversArray.size();
		QList<CServer> serversList;
		for (int serversIndex = 0; serversIndex < serversCount; ++serversIndex){
			CServer servers;
			if (!CServer::ReadFromJsonObject(servers, serversArray[serversIndex].toObject(), globalObject)){
				return false;
			}
			serversList << servers;
		}
		object.SetServers(serversList);
	}

	return true;
}


bool COperation::operator==(const COperation& other) const
{
	bool retVal = true;
	retVal = retVal && (m_tags == other.m_tags);
	retVal = retVal && (m_summary == other.m_summary);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_operationId == other.m_operationId);
	retVal = retVal && (m_parameters == other.m_parameters);
	retVal = retVal && (m_requestBody == other.m_requestBody);
	retVal = retVal && (m_responses == other.m_responses);
	retVal = retVal && (m_isDeprecated == other.m_isDeprecated);
	retVal = retVal && (m_security == other.m_security);
	retVal = retVal && (m_servers == other.m_servers);

	return retVal;
}


} // namespace imtoas

