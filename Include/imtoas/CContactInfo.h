#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>


namespace imtoas
{


class CContactInfo
{
public:
	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] QString GetUrl() const;
	void SetUrl(const QString& url);

	[[nodiscard]] QString GetEmail() const;
	void SetEmail(const QString& email);

	static bool ReadFromJsonObject(CContactInfo& object, const QJsonObject& jsonObject);

	bool operator==(const CContactInfo& other) const;
	bool operator!=(const CContactInfo& other) const { return !(operator==(other)); }

private:
	QString m_name;
	QString m_url;
	QString m_email;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CContactInfo);


