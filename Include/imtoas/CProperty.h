#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>


namespace imtoas
{


class CProperty
{
public:
	[[nodiscard]] QString GetType() const;
	void SetType(const QString& type);

	[[nodiscard]] QString GetFormat() const;
	void SetFormat(const QString& format);

	[[nodiscard]] static bool ReadFromJsonObject(CProperty& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CProperty& other) const;
	bool operator!=(const CProperty& other) const { return !(operator==(other)); }

private:
	QString m_type;
	QString m_format;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CProperty);


