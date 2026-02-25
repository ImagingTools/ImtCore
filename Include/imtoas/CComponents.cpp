// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CComponents.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


QList<CSchema> CComponents::GetSchemas() const
{
	return m_schemas;
}


void CComponents::SetSchemas(const QList<CSchema>& schemas)
{
	if (schemas != m_schemas){
		m_schemas = schemas;
	}
}


QList<CResponse> CComponents::GetResponses() const
{
	return m_responses;
}


void CComponents::SetResponses(const QList<CResponse>& responses)
{
	if (responses != m_responses){
		m_responses = responses;
	}
}


QList<CParameter> CComponents::GetParameters() const
{
	return m_parameters;
}


void CComponents::SetParameters(const QList<CParameter>& parameters)
{
	if (parameters != m_parameters){
		m_parameters = parameters;
	}
}


QList<CRequestBody> CComponents::GetRequestBodies() const
{
	return m_requestBodies;
}


void CComponents::SetRequestBodies(const QList<CRequestBody>& requestBodies)
{
	if (requestBodies != m_requestBodies){
		m_requestBodies = requestBodies;
	}
}


QList<CHeader> CComponents::GetHeaders() const
{
	return m_headers;
}


void CComponents::SetHeaders(const QList<CHeader>& headers)
{
	if (headers != m_headers){
		m_headers = headers;
	}
}


QList<CSecurityScheme> CComponents::GetSecuritySchemes() const
{
	return m_securitySchemes;
}


void CComponents::SetSecuritySchemes(const QList<CSecurityScheme>& securitySchemes)
{
	if (securitySchemes != m_securitySchemes){
		m_securitySchemes = securitySchemes;
	}
}


QList<CPath> CComponents::GetCallbacks() const
{
	return m_callbacks;
}


void CComponents::SetCallbacks(const QList<CPath>& callbacks)
{
	if (callbacks != m_callbacks){
		m_callbacks = callbacks;
	}
}


QList<CPath> CComponents::GetPathItems() const
{
	return m_pathItems;
}


void CComponents::SetPathItems(const QList<CPath>& pathItems)
{
	if (pathItems != m_pathItems){
		m_pathItems = pathItems;
	}
}


bool CComponents::ReadFromJsonObject(CComponents& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	if (jsonObject.contains("schemas")){
		const QJsonObject schemasArray = jsonObject["schemas"].toObject();
		QList<CSchema> schemasList;
		if (!COasTools::ExtractItemsFromObject(schemasList, schemasArray, globalObject)){
			return false;
		}
		object.SetSchemas(schemasList);
	}

	if (jsonObject.contains("responses")){
		const QJsonObject responsesArray = jsonObject["responses"].toObject();
		QList<CResponse> responsesList;
		if (!COasTools::ExtractItemsFromObject(responsesList, responsesArray, globalObject)){
			return false;
		}
		object.SetResponses(responsesList);
	}

	if (jsonObject.contains("parameters")){
		const QJsonObject parametersArray = jsonObject["parameters"].toObject();
		QList<CParameter> parametersList;
		if (!COasTools::ExtractItemsFromObject(parametersList, parametersArray, globalObject)){
			return false;
		}
		object.SetParameters(parametersList);
	}

	if (jsonObject.contains("requestBodies")){
		const QJsonObject requestBodiesArray = jsonObject["requestBodies"].toObject();
		QList<CRequestBody> requestBodiesList;
		if (!COasTools::ExtractItemsFromObject(requestBodiesList, requestBodiesArray, globalObject)){
			return false;
		}
		object.SetRequestBodies(requestBodiesList);
	}

	if (jsonObject.contains("headers")){
		const QJsonObject headersArray = jsonObject["headers"].toObject();
		QList<CHeader> headersList;
		if (!COasTools::ExtractItemsFromObject(headersList, headersArray, globalObject)){
			return false;
		}
		object.SetHeaders(headersList);
	}

	if (jsonObject.contains("securitySchemes")){
		const QJsonObject securitySchemesArray = jsonObject["securitySchemes"].toObject();
		QList<CSecurityScheme> securitySchemesList;
		if (!COasTools::ExtractItemsFromObject(securitySchemesList, securitySchemesArray, globalObject)){
			return false;
		}
		object.SetSecuritySchemes(securitySchemesList);
	}

	if (jsonObject.contains("callbacks")){
		const QJsonObject callbacksArray = jsonObject["callbacks"].toObject();
		QList<CPath> callbacksList;
		if (!COasTools::ExtractItemsFromObject(callbacksList, callbacksArray, globalObject)){
			return false;
		}
		object.SetCallbacks(callbacksList);
	}

	if (jsonObject.contains("pathItems")){
		const QJsonObject pathItemsArray = jsonObject["pathItems"].toObject();
		QList<CPath> pathItemsList;
		if (!COasTools::ExtractItemsFromObject(pathItemsList, pathItemsArray, globalObject)){
			return false;
		}
		object.SetPathItems(pathItemsList);
	}

	return true;
}


bool CComponents::operator==(const CComponents& other) const
{
	bool retVal = true;
	retVal = retVal && (m_schemas == other.m_schemas);
	retVal = retVal && (m_responses == other.m_responses);
	retVal = retVal && (m_parameters == other.m_parameters);
	retVal = retVal && (m_requestBodies == other.m_requestBodies);
	retVal = retVal && (m_headers == other.m_headers);
	retVal = retVal && (m_securitySchemes == other.m_securitySchemes);
	retVal = retVal && (m_callbacks == other.m_callbacks);
	retVal = retVal && (m_pathItems == other.m_pathItems);

	return retVal;
}


} // namespace imtoas

