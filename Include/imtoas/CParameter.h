// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>

// ImtCore includes
#include <imtoas/CSchema.h>
#include <imtoas/CMediaType.h>


namespace imtoas
{


class CParameter
{
public:
	CParameter();

	[[nodiscard]] QString GetName() const;
	void SetName(const QString& name);

	[[nodiscard]] QString GetIn() const;
	void SetIn(const QString& in);

	[[nodiscard]] QString GetDescription() const;
	void SetDescription(const QString& description);

	[[nodiscard]] bool IsRequired() const;
	void SetRequired(bool required = true);

	[[nodiscard]] bool IsDeprecated() const;
	void SetDeprecated(bool deprecated = true);

	[[nodiscard]] bool IsAllowEmptyValue() const;
	void SetAllowEmptyValue(bool allowEmptyValue = true);

	[[nodiscard]] QString GetStyle() const;
	void SetStyle(const QString& style);

	[[nodiscard]] bool IsExplode() const;
	void SetExplode(bool explode = true);

	[[nodiscard]] bool IsAllowReserved() const;
	void SetAllowReserved(bool allowReserved = true);

	[[nodiscard]] CSchema GetSchema() const;
	void SetSchema(const CSchema& schema);

	[[nodiscard]] CMediaType GetContent() const;
	void SetContent(const CMediaType& content);

	[[nodiscard]] static bool ReadFromJsonObject(CParameter& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CParameter& other) const;
	bool operator!=(const CParameter& other) const { return !(operator==(other)); }

	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

private:
	QString m_id;
	QString m_name;
	QString m_in;
	QString m_description;
	bool m_isRequired;
	bool m_isDeprecated;
	bool m_isAllowEmptyValue;
	QString m_style;
	bool m_isExplode;
	bool m_isAllowReserved;
	CSchema m_schema;
	CMediaType m_content;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CParameter);


