#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QList>


namespace imtoas
{


class CServerVariable
{
public:
	[[nodiscard]] QList<QString> GetEnumValues() const;
	void SetEnumValues(const QList<QString>& enums);

	[[nodiscard]] QString GetDefaultValue() const;
	void SetDefault(const QString& defaultValue);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] static bool ReadFromJsonObject(CServerVariable& object, const QJsonObject& jsonObject);

	bool operator==(const CServerVariable& other) const;
	bool operator!=(const CServerVariable& other) const { return !(operator==(other)); }

private:
	QList<QString> m_enumValues;
	QString m_default;
	QString m_description;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CServerVariable);


