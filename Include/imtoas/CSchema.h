// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	enum CombineType
	{
		CT_SIMPLE,	///< NONE: Data MUST be valid for single schema (no alternatives).
		CT_ALL_OF,	///< AND: Data MUST be valid against ALL subschemas (allOf).
		CT_ONE_OF,	///< XOR: Data MUST be valid against ONLY one subschema (oneOf).
		CT_ANY_OF	///< OR: Data MUST be valid against any subschema (anyOf).
	};

	CSchema();

	[[nodiscard]] QString GetId() const;
	void SetId(const QString& id);

	[[nodiscard]] QString GetType() const;
	void SetType(const QString& type);

	[[nodiscard]] QList<QString> GetRequiredProperties() const;
	void SetRequiredProperties(const QList<QString>& requiredPropertyList);

	[[nodiscard]] QList<CProperty> GetProperties() const;
	void SetProperties(const QList<CProperty>& properties);

	[[nodiscard]] QList<QString> GetEnumValues() const;
	void SetEnumValues(const QList<QString>& enumValues);

	[[nodiscard]] CombineType GetCombineType() const;
	void SetCombineType(CombineType combineType);

	[[nodiscard]] QList<CProperty> GetCombineProperties() const;
	void SetCombineProperties(const QList<CProperty>& combineProperties);

	[[nodiscard]] static bool ReadFromJsonObject(CSchema& object, const QJsonObject& jsonObject, const QJsonObject& globalObject);

	bool operator==(const CSchema& other) const;
	bool operator!=(const CSchema& other) const { return !(operator==(other)); }

private:
	QString m_id;
	QString m_type;
	QList<QString> m_requiredProperties;
	QList<CProperty> m_properties;
	QList<QString> m_enumValues;
	CombineType m_combineType;
	QList<CProperty> m_combineProperties;
};


} // namespace imtoas


Q_DECLARE_METATYPE(imtoas::CSchema);


