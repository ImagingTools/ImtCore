// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// STL includes
#include <limits>
#include <memory>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QStringList>


namespace imtassist
{


/**
	Versioned schema of a JSON object.

	Used for the parameters and results of assistant actions and for the data of
	UI representations. Validation is strict: unknown fields, missing required
	fields, wrong types and values outside the declared limits are rejected.
	Data coming from the model is untrusted and must pass the schema before use.

	\ingroup imtassist
*/
class CParameterSchema
{
public:
	enum FieldType
	{
		FT_STRING,
		FT_INTEGER,
		FT_BOOLEAN,
		/**
			Object identifier: non-empty string without control characters.
		*/
		FT_ID,
		/**
			One of the declared enumeration values.
		*/
		FT_ENUM,
		/**
			Array of strings.
		*/
		FT_STRING_LIST,
		/**
			Array of objects, each validated against the item schema of the field.
		*/
		FT_OBJECT_LIST
	};

	enum
	{
		DEFAULT_MAX_STRING_LENGTH = 4096,
		DEFAULT_MAX_ITEMS = 1000
	};

	struct Field
	{
		QString name;
		FieldType type = FT_STRING;
		bool isRequired = false;
		QString description;
		QString pattern;
		int maxLength = DEFAULT_MAX_STRING_LENGTH;
		int maxItems = DEFAULT_MAX_ITEMS;
		qint64 minValue = std::numeric_limits<qint32>::min();
		qint64 maxValue = std::numeric_limits<qint32>::max();
		QStringList enumValues;
		std::shared_ptr<const CParameterSchema> itemSchemaPtr;
	};

	typedef QList<Field> Fields;

	CParameterSchema() = default;
	CParameterSchema(const QByteArray& schemaId, int version);

	QByteArray GetSchemaId() const;
	int GetVersion() const;

	Fields GetFields() const;

	/**
		Add a field. A field with an existing name replaces the previous one.
	*/
	CParameterSchema& AddField(const Field& field);

	/**
		Convenience functions for the common field types.
	*/
	/**
		Add a string field with an optional regular-expression pattern.
	*/
	CParameterSchema& AddString(
				const QString& name,
				bool isRequired,
				int maxLength = DEFAULT_MAX_STRING_LENGTH,
				const QString& pattern = QString());
	CParameterSchema& AddInteger(const QString& name, bool isRequired, qint64 minValue, qint64 maxValue);
	CParameterSchema& AddBoolean(const QString& name, bool isRequired);
	CParameterSchema& AddId(const QString& name, bool isRequired);
	CParameterSchema& AddEnum(const QString& name, bool isRequired, const QStringList& values);
	CParameterSchema& AddStringList(const QString& name, bool isRequired, int maxItems = DEFAULT_MAX_ITEMS);
	CParameterSchema& AddObjectList(
				const QString& name,
				bool isRequired,
				const std::shared_ptr<const CParameterSchema>& itemSchemaPtr,
				int maxItems = DEFAULT_MAX_ITEMS);

	/**
		Validate a JSON object against this schema.
		\param object       Object to validate.
		\param errorPtr     Optional technical description of the first violation.
		                    It names the field but never echoes the value.
		\return True if the object is valid.
	*/
	bool Validate(const QJsonObject& object, QString* errorPtr = nullptr) const;

	/**
		Describe the schema as JSON, for example for the provider tool list.
	*/
	QJsonObject ToJson() const;

	/**
		Describe the schema as JSON Schema of an object, as used for the tool definitions of model providers.
		\param useAdditionalProperties  Forbid unknown fields with \c additionalProperties; some
		                                 providers (Gemini) do not accept this keyword.
	*/
	QJsonObject ToJsonSchema(bool useAdditionalProperties = true) const;

private:
	bool ValidateField(const Field& field, const QJsonValue& value, QString* errorPtr) const;

private:
	QByteArray m_schemaId;
	int m_version = 0;
	Fields m_fields;
};


} // namespace imtassist
