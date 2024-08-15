#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>

// ImtCore includes
#include <imtoas/CProperty.h>


namespace imtoas
{


class CSchema
{
public:
	[[nodiscard]] QString GetType() const;
	void SetType(const QString& type);

	[[nodiscard]] QList<QString> GetRequiredProperties() const;
	void SetRequiredProperties(const QList<QString>& requiredPropertyList);

	[[nodiscard]] QList<CProperty> GetProperties() const;
	void SetProperties(const QList<CProperty>& properties);

	[[nodiscard]] QList<QString> GetEnumValues() const;
	void SetEnumValues(const QList<QString>& enumValues);

	[[nodiscard]] static bool ReadFromJsonObject(CSchema& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CSchema& other) const;
	bool operator!=(const CSchema& other) const { return !(operator==(other)); }

private:
	QString m_type;
	QList<QString> m_requiredProperties;
	QList<CProperty> m_properties;
	QList<QString> m_enumValues;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CSchema);


