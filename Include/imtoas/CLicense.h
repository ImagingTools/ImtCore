#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>


namespace imtoas
{


class CLicense
{
public:
	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] QString GetIdentifier() const;
	void SetIdentifier(const QString& identifier);

	[[nodiscard]] QString GetUrl() const;
	void SetUrl(const QString& url);

	static bool ReadFromJsonObject(CLicense& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CLicense& other) const;
	bool operator!=(const CLicense& other) const { return !(operator==(other)); }

private:
	QString m_name;
	QString m_identifier;
	QString m_url;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CLicense);


