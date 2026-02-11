// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>

// ACF includes
#include <istd/istd.h>


namespace imtoas
{


class CSecurityScheme
{
public:
	enum Type
	{
		T_API_KEY,
		T_HTTP,
		T_MUTUAL_TLS,
		T_OAUTH2,
		T_OPEN_ID_CONNECT
	};
	I_DECLARE_ENUM(Type,
			T_API_KEY,
			T_HTTP,
			T_MUTUAL_TLS,
			T_OAUTH2,
			T_OPEN_ID_CONNECT)

	CSecurityScheme();

	[[nodiscard]] Type GetType() const;
	void SetType(Type type);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] QString GetIn() const;
	void SetIn(const QString& in);

	[[nodiscard]] QString GetScheme() const;
	void SetScheme(const QString& scheme);

	[[nodiscard]] QString GetOpenIdConnectUrl() const;
	void SetOpenIdConnectUrl(const QString& openIdConnectUrl);

	[[nodiscard]] static bool ReadFromJsonObject(CSecurityScheme& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CSecurityScheme& other) const;
	bool operator!=(const CSecurityScheme& other) const { return !(operator==(other)); }

	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

private:
	QString m_id;
	Type m_type;
	QString m_description;
	QString m_name;
	QString m_in;
	QString m_scheme;
	QString m_openIdConnectUrl;
	
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CSecurityScheme);


