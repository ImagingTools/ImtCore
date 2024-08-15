#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>


namespace imtoas
{


class CSecurityScheme
{
public:
	[[nodiscard]] QString GetType() const;
	void SetType(const QString& type);

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

private:
	QString m_type;
	QString m_description;
	QString m_name;
	QString m_in;
	QString m_scheme;
	QString m_openIdConnectUrl;
	
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CSecurityScheme);


