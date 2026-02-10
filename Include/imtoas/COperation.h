// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CSecurityScheme.h>
#include <imtoas/CParameter.h>
#include <imtoas/CRequestBody.h>
#include <imtoas/CResponse.h>
#include <imtoas/CServer.h>


namespace imtoas
{


class COperation
{
public:
	COperation();

	[[nodiscard]] QList<QString> GetTags() const;
	void SetTags(const QList<QString>& tags);

	[[nodiscard]] QString GetSummary() const;
	void SetSummary(const QString& summary);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] QString GetOperationId() const;
	void SetOperationId(const QString& operationId);

	[[nodiscard]] QList<CParameter> GetParameters() const;
	void SetParameters(const QList<CParameter>& parameters);

	[[nodiscard]] CRequestBody GetRequestBody() const;
	void SetRequestBody(const CRequestBody& requestBody);

	[[nodiscard]] QList<CResponse> GetResponses() const;
	void SetResponses(const QList<CResponse>& responses);

	[[nodiscard]] bool IsDeprecated() const;
	void SetDeprecated(bool deprecated = true);

	[[nodiscard]] QList<CSecurityScheme> GetSecurity() const;
	void SetSecurity(const QList<CSecurityScheme>& security);

	[[nodiscard]] QList<CServer> GetServers() const;
	void SetServers(const QList<CServer>& servers);

	[[nodiscard]] static bool ReadFromJsonObject(COperation& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const COperation& other) const;
	bool operator!=(const COperation& other) const { return !(operator==(other)); }

private:
	QList<QString> m_tags;
	QString m_summary;
	QString m_description;
	QString m_operationId;
	QList<CParameter> m_parameters;
	CRequestBody m_requestBody;
	QList<CResponse> m_responses;
	bool m_isDeprecated;
	QList<CSecurityScheme> m_security;
	QList<CServer> m_servers;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::COperation);


