// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

	[[nodiscard]] QList<QString> GetEnumValues() const;
	void SetEnumValues(const QList<QString>& enums);

	[[nodiscard]] QString GetDefaultValue() const;
	void SetDefault(const QString& defaultValue);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] static bool ReadFromJsonObject(CServerVariable& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CServerVariable& other) const;
	bool operator!=(const CServerVariable& other) const { return !(operator==(other)); }

private:
	QString m_id;
	QList<QString> m_enumValues;
	QString m_default;
	QString m_description;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CServerVariable);


