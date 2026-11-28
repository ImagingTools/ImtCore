// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CParameterSchema.h>


// STL includes
#include <cmath>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QRegularExpression>


namespace imtassist
{


namespace
{


void SetError(QString* errorPtr, const QString& error)
{
	if (errorPtr != nullptr){
		*errorPtr = error;
	}
}


bool HasControlCharacters(const QString& text)
{
	for (const QChar& character: text){
		if (character.category() == QChar::Other_Control){
			return true;
		}
	}

	return false;
}


QString GetTypeName(CParameterSchema::FieldType type)
{
	switch (type){
	case CParameterSchema::FT_STRING:
		return "string";
	case CParameterSchema::FT_INTEGER:
		return "integer";
	case CParameterSchema::FT_BOOLEAN:
		return "boolean";
	case CParameterSchema::FT_ID:
		return "id";
	case CParameterSchema::FT_ENUM:
		return "enum";
	case CParameterSchema::FT_STRING_LIST:
		return "stringList";
	case CParameterSchema::FT_OBJECT_LIST:
		return "objectList";
	}

	return QString();
}


} // namespace


CParameterSchema::CParameterSchema(const QByteArray& schemaId, int version)
	:m_schemaId(schemaId),
	m_version(version)
{
}


QByteArray CParameterSchema::GetSchemaId() const
{
	return m_schemaId;
}


int CParameterSchema::GetVersion() const
{
	return m_version;
}


CParameterSchema::Fields CParameterSchema::GetFields() const
{
	return m_fields;
}


CParameterSchema& CParameterSchema::AddField(const Field& field)
{
	for (Field& existingField: m_fields){
		if (existingField.name == field.name){
			existingField = field;

			return *this;
		}
	}

	m_fields.append(field);

	return *this;
}


CParameterSchema& CParameterSchema::AddString(const QString& name, bool isRequired, int maxLength, const QString& pattern)
{
	Field field;
	field.name = name;
	field.type = FT_STRING;
	field.isRequired = isRequired;
	field.maxLength = maxLength;
	field.pattern = pattern;

	return AddField(field);
}


CParameterSchema& CParameterSchema::AddInteger(const QString& name, bool isRequired, qint64 minValue, qint64 maxValue)
{
	Field field;
	field.name = name;
	field.type = FT_INTEGER;
	field.isRequired = isRequired;
	field.minValue = minValue;
	field.maxValue = maxValue;

	return AddField(field);
}


CParameterSchema& CParameterSchema::AddBoolean(const QString& name, bool isRequired)
{
	Field field;
	field.name = name;
	field.type = FT_BOOLEAN;
	field.isRequired = isRequired;

	return AddField(field);
}


CParameterSchema& CParameterSchema::AddId(const QString& name, bool isRequired)
{
	Field field;
	field.name = name;
	field.type = FT_ID;
	field.isRequired = isRequired;
	field.maxLength = 256;

	return AddField(field);
}


CParameterSchema& CParameterSchema::AddEnum(const QString& name, bool isRequired, const QStringList& values)
{
	Field field;
	field.name = name;
	field.type = FT_ENUM;
	field.isRequired = isRequired;
	field.enumValues = values;

	return AddField(field);
}


CParameterSchema& CParameterSchema::AddStringList(const QString& name, bool isRequired, int maxItems)
{
	Field field;
	field.name = name;
	field.type = FT_STRING_LIST;
	field.isRequired = isRequired;
	field.maxItems = maxItems;

	return AddField(field);
}


CParameterSchema& CParameterSchema::AddObjectList(
			const QString& name,
			bool isRequired,
			const std::shared_ptr<const CParameterSchema>& itemSchemaPtr,
			int maxItems)
{
	Field field;
	field.name = name;
	field.type = FT_OBJECT_LIST;
	field.isRequired = isRequired;
	field.maxItems = maxItems;
	field.itemSchemaPtr = itemSchemaPtr;

	return AddField(field);
}


bool CParameterSchema::Validate(const QJsonObject& object, QString* errorPtr) const
{
	for (QJsonObject::const_iterator iter = object.constBegin(); iter != object.constEnd(); ++iter){
		bool isKnown = false;
		for (const Field& field: m_fields){
			if (field.name == iter.key()){
				isKnown = true;
				break;
			}
		}

		if (!isKnown){
			SetError(errorPtr, QString("Unknown field '%1'").arg(iter.key().left(64)));

			return false;
		}
	}

	for (const Field& field: m_fields){
		QJsonValue value = object.value(field.name);
		if (value.isUndefined() || value.isNull()){
			if (field.isRequired){
				SetError(errorPtr, QString("Missing required field '%1'").arg(field.name));

				return false;
			}

			continue;
		}

		if (!ValidateField(field, value, errorPtr)){
			return false;
		}
	}

	return true;
}


QJsonObject CParameterSchema::ToJson() const
{
	QJsonArray fields;
	for (const Field& field: m_fields){
		QJsonObject fieldObject;
		fieldObject["name"] = field.name;
		fieldObject["type"] = GetTypeName(field.type);
		fieldObject["required"] = field.isRequired;
		if (!field.description.isEmpty()){
			fieldObject["description"] = field.description;
		}
		if (!field.pattern.isEmpty()){
			fieldObject["pattern"] = field.pattern;
		}

		if (field.type == FT_ENUM){
			fieldObject["values"] = QJsonArray::fromStringList(field.enumValues);
		}

		if (field.type == FT_INTEGER){
			fieldObject["min"] = double(field.minValue);
			fieldObject["max"] = double(field.maxValue);
		}

		if ((field.type == FT_OBJECT_LIST) && field.itemSchemaPtr){
			fieldObject["items"] = field.itemSchemaPtr->ToJson();
		}

		fields.append(fieldObject);
	}

	QJsonObject retVal;
	retVal["schemaId"] = QString::fromUtf8(m_schemaId);
	retVal["version"] = m_version;
	retVal["fields"] = fields;

	return retVal;
}


QJsonObject CParameterSchema::ToJsonSchema(bool useAdditionalProperties) const
{
	QJsonObject properties;
	QJsonArray required;

	for (const Field& field: m_fields){
		QJsonObject property;
		if (!field.description.isEmpty()){
			property["description"] = field.description;
		}

		switch (field.type){
		case FT_STRING:
			property["type"] = QStringLiteral("string");
			property["maxLength"] = field.maxLength;
			if (!field.pattern.isEmpty()){
				property["pattern"] = field.pattern;
			}
			break;

		case FT_ID:
			property["type"] = QStringLiteral("string");
			property["minLength"] = 1;
			property["maxLength"] = field.maxLength;
			break;

		case FT_ENUM:
			property["type"] = QStringLiteral("string");
			property["enum"] = QJsonArray::fromStringList(field.enumValues);
			break;

		case FT_INTEGER:
			property["type"] = QStringLiteral("integer");
			property["minimum"] = double(field.minValue);
			property["maximum"] = double(field.maxValue);
			break;

		case FT_BOOLEAN:
			property["type"] = QStringLiteral("boolean");
			break;

		case FT_STRING_LIST:
			{
				QJsonObject items;
				items["type"] = QStringLiteral("string");
				property["type"] = QStringLiteral("array");
				property["items"] = items;
				property["maxItems"] = field.maxItems;
			}
			break;

		case FT_OBJECT_LIST:
			property["type"] = QStringLiteral("array");
			property["items"] = field.itemSchemaPtr ? field.itemSchemaPtr->ToJsonSchema(useAdditionalProperties) : QJsonObject();
			property["maxItems"] = field.maxItems;
			break;
		}

		properties[field.name] = property;

		if (field.isRequired){
			required.append(field.name);
		}
	}

	QJsonObject retVal;
	retVal["type"] = QStringLiteral("object");
	retVal["properties"] = properties;
	if (!required.isEmpty()){
		retVal["required"] = required;
	}

	if (useAdditionalProperties){
		retVal["additionalProperties"] = false;
	}

	return retVal;
}


// private methods

bool CParameterSchema::ValidateField(const Field& field, const QJsonValue& value, QString* errorPtr) const
{
	switch (field.type){
	case FT_STRING:
	case FT_ID:
	case FT_ENUM:
		{
			if (!value.isString()){
				SetError(errorPtr, QString("Field '%1' must be a string").arg(field.name));

				return false;
			}

			QString text = value.toString();
			if (text.size() > field.maxLength){
				SetError(errorPtr, QString("Field '%1' is too long").arg(field.name));

				return false;
			}

			if ((field.type == FT_STRING) && !field.pattern.isEmpty() && !QRegularExpression(field.pattern).match(text).hasMatch()){
				SetError(errorPtr, QString("Field '%1' has an invalid format").arg(field.name));

				return false;
			}

			if (field.type == FT_ID){
				if (text.trimmed().isEmpty() || HasControlCharacters(text)){
					SetError(errorPtr, QString("Field '%1' is not a valid identifier").arg(field.name));

					return false;
				}
			}

			if ((field.type == FT_ENUM) && !field.enumValues.contains(text)){
				SetError(errorPtr, QString("Field '%1' has an unsupported value").arg(field.name));

				return false;
			}
		}
		return true;

	case FT_INTEGER:
		{
			if (!value.isDouble()){
				SetError(errorPtr, QString("Field '%1' must be an integer").arg(field.name));

				return false;
			}

			double number = value.toDouble();
			if (!std::isfinite(number) || (std::floor(number) != number)){
				SetError(errorPtr, QString("Field '%1' must be an integer").arg(field.name));

				return false;
			}

			if ((number < double(field.minValue)) || (number > double(field.maxValue))){
				SetError(errorPtr, QString("Field '%1' is out of range").arg(field.name));

				return false;
			}
		}
		return true;

	case FT_BOOLEAN:
		if (!value.isBool()){
			SetError(errorPtr, QString("Field '%1' must be a boolean").arg(field.name));

			return false;
		}
		return true;

	case FT_STRING_LIST:
		{
			if (!value.isArray()){
				SetError(errorPtr, QString("Field '%1' must be a list").arg(field.name));

				return false;
			}

			QJsonArray array = value.toArray();
			if (array.size() > field.maxItems){
				SetError(errorPtr, QString("Field '%1' has too many items").arg(field.name));

				return false;
			}

			for (const QJsonValue& item: array){
				if (!item.isString() || (item.toString().size() > field.maxLength)){
					SetError(errorPtr, QString("Field '%1' contains an invalid item").arg(field.name));

					return false;
				}
			}
		}
		return true;

	case FT_OBJECT_LIST:
		{
			if (!value.isArray() || !field.itemSchemaPtr){
				SetError(errorPtr, QString("Field '%1' must be a list of objects").arg(field.name));

				return false;
			}

			QJsonArray array = value.toArray();
			if (array.size() > field.maxItems){
				SetError(errorPtr, QString("Field '%1' has too many items").arg(field.name));

				return false;
			}

			for (const QJsonValue& item: array){
				if (!item.isObject()){
					SetError(errorPtr, QString("Field '%1' contains an invalid item").arg(field.name));

					return false;
				}

				QString itemError;
				if (!field.itemSchemaPtr->Validate(item.toObject(), &itemError)){
					SetError(errorPtr, QString("Field '%1': %2").arg(field.name, itemError));

					return false;
				}
			}
		}
		return true;
	}

	return false;
}


} // namespace imtassist

