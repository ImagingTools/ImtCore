// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CParameter.h>


namespace imtoas
{


CParameter::CParameter()
	:m_isRequired(false),
	m_isDeprecated(false),
	m_isAllowEmptyValue(true),
	m_isExplode(false),
	m_isAllowReserved(false)
{
}


QString CParameter::GetId() const
{
	return m_id;
}


void CParameter::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


QString CParameter::GetName() const
{
	return m_name;
}


void CParameter::SetName(const QString& name)
{
	if (name != m_name){
		m_name = name;
	}
}


QString CParameter::GetIn() const
{
	return m_in;
}


void CParameter::SetIn(const QString& in)
{
	if (in != m_in){
		m_in = in;
	}
}


QString CParameter::GetDescription() const
{
	return m_description;
}


void CParameter::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


bool CParameter::IsRequired() const
{
	return m_isRequired;
}


void CParameter::SetRequired(bool required)
{
	if (required != m_isRequired){
		m_isRequired = required;
	}
}


bool CParameter::IsDeprecated() const
{
	return m_isDeprecated;
}


void CParameter::SetDeprecated(bool deprecated)
{
	if (deprecated != m_isDeprecated){
		m_isDeprecated = deprecated;
	}
}


bool CParameter::IsAllowEmptyValue() const
{
	return m_isAllowEmptyValue;
}


void CParameter::SetAllowEmptyValue(bool allowEmptyValue)
{
	if (allowEmptyValue != m_isAllowEmptyValue){
		m_isAllowEmptyValue = allowEmptyValue;
	}
}


QString CParameter::GetStyle() const
{
	return m_style;
}


void CParameter::SetStyle(const QString& style)
{
	if (style != m_style){
		m_style = style;
	}
}


bool CParameter::IsExplode() const
{
	return m_isExplode;
}


void CParameter::SetExplode(bool explode)
{
	if (explode != m_isExplode){
		m_isExplode = explode;
	}
}


bool CParameter::IsAllowReserved() const
{
	return m_isAllowReserved;
}


void CParameter::SetAllowReserved(bool allowReserved)
{
	if (allowReserved != m_isAllowReserved){
		m_isAllowReserved = allowReserved;
	}
}


CSchema CParameter::GetSchema() const
{
	return m_schema;
}


void CParameter::SetSchema(const CSchema& schema)
{
	if (schema != m_schema){
		m_schema = schema;
	}
}


CMediaType CParameter::GetContent() const
{
	return m_content;
}


void CParameter::SetContent(const CMediaType& content)
{
	if (content != m_content){
		m_content = content;
	}
}


bool CParameter::ReadFromJsonObject(CParameter& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant nameData = jsonObject.value("name").toVariant();
	if (nameData.isNull()){
		return false;
	}
	object.SetName(nameData.toString());

	QVariant inData = jsonObject.value("in").toVariant();
	if (inData.isNull()){
		return false;
	}
	object.SetIn(inData.toString());

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	QVariant requiredData = jsonObject.value("required").toVariant();
	if (!requiredData.isNull()){
		object.SetRequired(requiredData.toBool());
	}

	QVariant deprecatedData = jsonObject.value("deprecated").toVariant();
	if (!deprecatedData.isNull()){
		object.SetDeprecated(deprecatedData.toBool());
	}

	QVariant allowEmptyValueData = jsonObject.value("allowEmptyValue").toVariant();
	if (!allowEmptyValueData.isNull()){
		object.SetAllowEmptyValue(allowEmptyValueData.toBool());
	}

	QVariant styleData = jsonObject.value("style").toVariant();
	if (!styleData.isNull()){
		object.SetStyle(styleData.toString());
	}

	QVariant explodeData = jsonObject.value("explode").toVariant();
	if (!explodeData.isNull()){
		object.SetExplode(explodeData.toBool());
	}

	QVariant allowReservedData = jsonObject.value("allowReserved").toVariant();
	if (!allowReservedData.isNull()){
		object.SetAllowReserved(allowReservedData.toBool());
	}

	if (jsonObject.contains("schema")){
		CSchema schema;
		const bool isSchemaReaded = CSchema::ReadFromJsonObject(schema, jsonObject["schema"].toObject(), globalObject);
		if (!isSchemaReaded){
			return false;
		}
		object.SetSchema(schema);
	}

	if (jsonObject.contains("content")){
		CMediaType content;
		const bool isContentReaded = CMediaType::ReadFromJsonObject(content, jsonObject["content"].toObject(), globalObject);
		if (!isContentReaded){
			return false;
		}
		object.SetContent(content);
	}

	return true;
}


bool CParameter::operator==(const CParameter& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_name == other.m_name);
	retVal = retVal && (m_in == other.m_in);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_isRequired == other.m_isRequired);
	retVal = retVal && (m_isDeprecated == other.m_isDeprecated);
	retVal = retVal && (m_isAllowEmptyValue == other.m_isAllowEmptyValue);
	retVal = retVal && (m_style == other.m_style);
	retVal = retVal && (m_isExplode == other.m_isExplode);
	retVal = retVal && (m_isAllowReserved == other.m_isAllowReserved);
	retVal = retVal && (m_schema == other.m_schema);
	retVal = retVal && (m_content == other.m_content);

	return retVal;
}


} // namespace imtoas

