#include <imtoas/CComponents.h>


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


bool CComponents::ReadFromJsonObject(CComponents& object, const QJsonObject& jsonObject)
{
	if (jsonObject.contains("schemas")){
		const QJsonArray schemasArray = jsonObject["schemas"].toArray();
		qsizetype schemasCount = schemasArray.size();
		QList<CSchema> schemasList;
		for (int schemasIndex = 0; schemasIndex < schemasCount; ++schemasIndex){
			CSchema schemas;
			if (!CSchema::ReadFromJsonObject(schemas, schemasArray[schemasIndex].toObject())){
				return false;
			}
			schemasList << schemas;
		}
		object.SetSchemas(schemasList);
	}

	if (jsonObject.contains("responses")){
		const QJsonArray responsesArray = jsonObject["responses"].toArray();
		qsizetype responsesCount = responsesArray.size();
		QList<CResponse> responsesList;
		for (int responsesIndex = 0; responsesIndex < responsesCount; ++responsesIndex){
			CResponse responses;
			if (!CResponse::ReadFromJsonObject(responses, responsesArray[responsesIndex].toObject())){
				return false;
			}
			responsesList << responses;
		}
		object.SetResponses(responsesList);
	}

	if (jsonObject.contains("parameters")){
		const QJsonArray parametersArray = jsonObject["parameters"].toArray();
		qsizetype parametersCount = parametersArray.size();
		QList<CParameter> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			CParameter parameters;
			if (!CParameter::ReadFromJsonObject(parameters, parametersArray[parametersIndex].toObject())){
				return false;
			}
			parametersList << parameters;
		}
		object.SetParameters(parametersList);
	}

	if (jsonObject.contains("requestBodies")){
		const QJsonArray requestBodiesArray = jsonObject["requestBodies"].toArray();
		qsizetype requestBodiesCount = requestBodiesArray.size();
		QList<CRequestBody> requestBodiesList;
		for (int requestBodiesIndex = 0; requestBodiesIndex < requestBodiesCount; ++requestBodiesIndex){
			CRequestBody requestBodies;
			if (!CRequestBody::ReadFromJsonObject(requestBodies, requestBodiesArray[requestBodiesIndex].toObject())){
				return false;
			}
			requestBodiesList << requestBodies;
		}
		object.SetRequestBodies(requestBodiesList);
	}

	if (jsonObject.contains("headers")){
		const QJsonArray headersArray = jsonObject["headers"].toArray();
		qsizetype headersCount = headersArray.size();
		QList<CHeader> headersList;
		for (int headersIndex = 0; headersIndex < headersCount; ++headersIndex){
			CHeader headers;
			if (!CHeader::ReadFromJsonObject(headers, headersArray[headersIndex].toObject())){
				return false;
			}
			headersList << headers;
		}
		object.SetHeaders(headersList);
	}

	if (jsonObject.contains("securitySchemes")){
		const QJsonArray securitySchemesArray = jsonObject["securitySchemes"].toArray();
		qsizetype securitySchemesCount = securitySchemesArray.size();
		QList<CSecurityScheme> securitySchemesList;
		for (int securitySchemesIndex = 0; securitySchemesIndex < securitySchemesCount; ++securitySchemesIndex){
			CSecurityScheme securitySchemes;
			if (!CSecurityScheme::ReadFromJsonObject(securitySchemes, securitySchemesArray[securitySchemesIndex].toObject())){
				return false;
			}
			securitySchemesList << securitySchemes;
		}
		object.SetSecuritySchemes(securitySchemesList);
	}

	if (jsonObject.contains("callbacks")){
		const QJsonArray callbacksArray = jsonObject["callbacks"].toArray();
		qsizetype callbacksCount = callbacksArray.size();
		QList<CPath> callbacksList;
		for (int callbacksIndex = 0; callbacksIndex < callbacksCount; ++callbacksIndex){
			CPath callbacks;
			if (!CPath::ReadFromJsonObject(callbacks, callbacksArray[callbacksIndex].toObject())){
				return false;
			}
			callbacksList << callbacks;
		}
		object.SetCallbacks(callbacksList);
	}

	if (jsonObject.contains("pathItems")){
		const QJsonArray pathItemsArray = jsonObject["pathItems"].toArray();
		qsizetype pathItemsCount = pathItemsArray.size();
		QList<CPath> pathItemsList;
		for (int pathItemsIndex = 0; pathItemsIndex < pathItemsCount; ++pathItemsIndex){
			CPath pathItems;
			if (!CPath::ReadFromJsonObject(pathItems, pathItemsArray[pathItemsIndex].toObject())){
				return false;
			}
			pathItemsList << pathItems;
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

