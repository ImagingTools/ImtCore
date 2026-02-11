// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CSchema.h>
#include <imtoas/CSecurityScheme.h>
#include <imtoas/CPath.h>
#include <imtoas/CParameter.h>
#include <imtoas/CRequestBody.h>
#include <imtoas/CResponse.h>
#include <imtoas/CHeader.h>


namespace imtoas
{


class CComponents
{
public:
	[[nodiscard]] QList<CSchema> GetSchemas() const;
	void SetSchemas(const QList<CSchema>& schemas);

	[[nodiscard]] QList<CResponse> GetResponses() const;
	void SetResponses(const QList<CResponse>& responses);

	[[nodiscard]] QList<CParameter> GetParameters() const;
	void SetParameters(const QList<CParameter>& parameters);

	[[nodiscard]] QList<CRequestBody> GetRequestBodies() const;
	void SetRequestBodies(const QList<CRequestBody>& requestBodies);

	[[nodiscard]] QList<CHeader> GetHeaders() const;
	void SetHeaders(const QList<CHeader>& headers);

	[[nodiscard]] QList<CSecurityScheme> GetSecuritySchemes() const;
	void SetSecuritySchemes(const QList<CSecurityScheme>& securitySchemes);

	[[nodiscard]] QList<CPath> GetCallbacks() const;
	void SetCallbacks(const QList<CPath>& callbacks);

	[[nodiscard]] QList<CPath> GetPathItems() const;
	void SetPathItems(const QList<CPath>& pathItems);

	static bool ReadFromJsonObject(CComponents& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CComponents& other) const;
	bool operator!=(const CComponents& other) const { return !(operator==(other)); }

private:
	QList<CSchema> m_schemas;
	QList<CResponse> m_responses;
	QList<CParameter> m_parameters;
	QList<CRequestBody> m_requestBodies;
	QList<CHeader> m_headers;
	QList<CSecurityScheme> m_securitySchemes;
	QList<CPath> m_callbacks;
	QList<CPath> m_pathItems;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CComponents);


