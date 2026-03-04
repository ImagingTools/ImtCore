// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/COpenApiSchema.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


QString COpenApiSchema::GetOpenapi() const
{
	return m_openapi;
}


void COpenApiSchema::SetOpenapi(const QString& openapi)
{
	if (openapi != m_openapi){
		m_openapi = openapi;
	}
}


CInfo COpenApiSchema::GetInfo() const
{
	return m_info;
}


void COpenApiSchema::SetInfo(const CInfo& info)
{
	if (info != m_info){
		m_info = info;
	}
}



QString COpenApiSchema::GetJsonSchemaDialect() const
{
	return m_jsonSchemaDialect;
}


void COpenApiSchema::SetJsonSchemaDialect(const QString& jsonSchemaDialect)
{
	if (jsonSchemaDialect != m_jsonSchemaDialect){
		m_jsonSchemaDialect = jsonSchemaDialect;
	}
}


QList<CServer> COpenApiSchema::GetServers() const
{
	return m_servers;
}


void COpenApiSchema::SetServers(const QList<CServer>& servers)
{
	if (servers != m_servers){
		m_servers = servers;
	}
}


QList<CPath> COpenApiSchema::GetPaths() const
{
	return m_paths;
}


void COpenApiSchema::SetPaths(const QList<CPath>& paths)
{
	if (paths != m_paths){
		m_paths = paths;
	}
}


CComponents COpenApiSchema::GetComponents() const
{
	return m_components;
}


void COpenApiSchema::SetComponents(const CComponents& components)
{
	if (components != m_components){
		m_components = components;
	}
}


CSecurity COpenApiSchema::GetSecurity() const
{
	return m_security;
}


void COpenApiSchema::SetSecurity(const CSecurity& security)
{
	if (security != m_security){
		m_security = security;
	}
}


bool COpenApiSchema::ReadFromJsonObject(COpenApiSchema& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant openapiData = jsonObject.value("openapi").toVariant();
	if (openapiData.isNull()){
		return false;
	}
	object.SetOpenapi(openapiData.toString());

	if (!jsonObject.contains("info")){
		return false;
	}
	CInfo info;
	const bool isInfoReaded = CInfo::ReadFromJsonObject(info, jsonObject["info"].toObject(), globalObject);
	if (!isInfoReaded){
		return false;
	}
	object.SetInfo(info);

	QVariant jsonSchemaDialectData = jsonObject.value("jsonSchemaDialect").toVariant();
	if (!jsonSchemaDialectData.isNull()){
		object.SetJsonSchemaDialect(jsonSchemaDialectData.toString());
	}

	if (jsonObject.contains("servers")){
		const QJsonArray serversArray = jsonObject["servers"].toArray();
		qsizetype serversCount = serversArray.size();
		if (serversCount <= 0){
			return false;
		}
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

	if (jsonObject.contains("paths")){
		const QJsonObject pathsArray = jsonObject["paths"].toObject();
		QList<CPath> pathsList;
		if (!COasTools::ExtractItemsFromObject(pathsList, pathsArray, globalObject)){
			return false;
		}
		object.SetPaths(pathsList);
	}

	if (jsonObject.contains("components")){
		CComponents components;
		const bool isComponentsReaded = CComponents::ReadFromJsonObject(components, jsonObject["components"].toObject(), globalObject);
		if (!isComponentsReaded){
			return false;
		}
		object.SetComponents(components);
	}

	if (jsonObject.contains("security")){
		CSecurity security;
		const bool isSecurityReaded = CSecurity::ReadFromJsonObject(security, jsonObject["security"].toObject(), globalObject);
		if (!isSecurityReaded){
			return false;
		}
		object.SetSecurity(security);
	}

	return true;
}


bool COpenApiSchema::operator==(const COpenApiSchema& other) const
{
	bool retVal = true;
	retVal = retVal && (m_openapi == other.m_openapi);
	retVal = retVal && (m_info == other.m_info);
	retVal = retVal && (m_jsonSchemaDialect == other.m_jsonSchemaDialect);
	retVal = retVal && (m_servers == other.m_servers);
	retVal = retVal && (m_paths == other.m_paths);
	retVal = retVal && (m_components == other.m_components);
	retVal = retVal && (m_security == other.m_security);

	return retVal;
}


} // namespace imtoas

