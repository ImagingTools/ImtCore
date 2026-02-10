// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CInfo.h>
#include <imtoas/CSecurity.h>
#include <imtoas/CPath.h>
#include <imtoas/CComponents.h>
#include <imtoas/CServer.h>


namespace imtoas
{


class COpenApiSchema
{
public:
	[[nodiscard]] QString GetOpenapi() const;
	void SetOpenapi(const QString& openapi);

	[[nodiscard]] CInfo GetInfo() const;
	void SetInfo(const CInfo& info);

	[[nodiscard]] QString GetJsonSchemaDialect() const;
	void SetJsonSchemaDialect(const QString& jsonSchemaDialect);

	[[nodiscard]] QList<CServer> GetServers() const;
	void SetServers(const QList<CServer>& servers);

	[[nodiscard]] QList<CPath> GetPaths() const;
	void SetPaths(const QList<CPath>& paths);

	[[nodiscard]] CComponents GetComponents() const;
	void SetComponents(const CComponents& components);

	[[nodiscard]] CSecurity GetSecurity() const;
	void SetSecurity(const CSecurity& security);

	[[nodiscard]] static bool ReadFromJsonObject(COpenApiSchema& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const COpenApiSchema& other) const;
	bool operator!=(const COpenApiSchema& other) const { return !(operator==(other)); }

private:
	QString m_openapi;
	CInfo m_info;
	QString m_jsonSchemaDialect;
	QList<CServer> m_servers;
	QList<CPath> m_paths;
	CComponents m_components;
	CSecurity m_security;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::COpenApiSchema);


