// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CSchema.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


CSchema::CSchema()
	:m_combineType(CT_SIMPLE)
{
}


QString CSchema::GetId() const
{
	return m_id;
}


void CSchema::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


QString CSchema::GetType() const
{
	return m_type;
}


void CSchema::SetType(const QString& type)
{
	if (type != m_type){
		m_type = type;
	}
}


QList<QString> CSchema::GetRequiredProperties() const
{
	return m_requiredProperties;
}


void CSchema::SetRequiredProperties(const QList<QString>& requiredPropertyList)
{
	if (requiredPropertyList != m_requiredProperties){
		m_requiredProperties = requiredPropertyList;
	}
}


QList<CProperty> CSchema::GetProperties() const
{
	return m_properties;
}


void CSchema::SetProperties(const QList<CProperty>& properties)
{
	if (properties != m_properties){
		m_properties = properties;
	}
}


QList<QString> CSchema::GetEnumValues() const
{
	return m_enumValues;
}


void CSchema::SetEnumValues(const QList<QString>& enumValues)
{
	if (enumValues != m_enumValues){
		m_enumValues = enumValues;
	}
}


CSchema::CombineType CSchema::GetCombineType() const
{
	return m_combineType;
}


void CSchema::SetCombineType(CombineType combineType)
{
	if (combineType != m_combineType){
		m_combineType = combineType;
	}
}


QList<CProperty> CSchema::GetCombineProperties() const
{
	return m_combineProperties;
}


void CSchema::SetCombineProperties(const QList<CProperty>& combineProperties)
{
	if (combineProperties != m_combineProperties){
		m_combineProperties = combineProperties;
	}
}


bool CSchema::ReadFromJsonObject(CSchema& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant typeData = jsonObject.value("type").toVariant();
	if (!typeData.isNull()){
		object.SetType(typeData.toString());
	}

	if (jsonObject.contains("required")){
		const QJsonArray requiredArray = jsonObject["required"].toArray();
		qsizetype requiredCount = requiredArray.size();
		QList<QString> requiredList;
		for (qsizetype requiredIndex = 0; requiredIndex < requiredCount; ++requiredIndex){
			requiredList << requiredArray[requiredIndex].toString();
		}
		object.SetRequiredProperties(requiredList);
	}

	if (jsonObject.contains("properties")){
		const QJsonObject propertiesArray = jsonObject["properties"].toObject();
		QList<CProperty> propertiesList;
		if (!COasTools::ExtractItemsFromObject(propertiesList, propertiesArray, globalObject)){
			return false;
		}
		object.SetProperties(propertiesList);
	}

	if (jsonObject.contains("enum")){
		const QJsonArray enumArray = jsonObject["enum"].toArray();
		qsizetype enumCount = enumArray.size();
		QList<QString> enumList;
		for (qsizetype enumIndex = 0; enumIndex < enumCount; ++enumIndex){
			enumList << enumArray[enumIndex].toString();
		}
		object.SetEnumValues(enumList);
	}

	// maybe the property is a combine
	QString combineKey;
	if (jsonObject.contains("oneOf")){
		combineKey = "oneOf";
		object.SetCombineType(CT_ONE_OF);
	}
	else if (jsonObject.contains("allOf")){
		combineKey = "allOf";
		object.SetCombineType(CT_ALL_OF);
	}
	else if (jsonObject.contains("anyOf")){
		combineKey = "anyOf";
		object.SetCombineType(CT_ANY_OF);
	}
	QJsonArray combineData = jsonObject.value(combineKey).toArray();
	QList<CProperty> combineProperties;
	for (QJsonArray::const_iterator combineDataIter = combineData.cbegin(); combineDataIter != combineData.cend(); ++combineDataIter){
		QJsonObject combineObject = combineDataIter->toObject();
		if (!COasTools::DeReferenceObject(combineObject, globalObject, combineObject)){
			return false;
		}
		CProperty combineProperty;
		if (!CProperty::ReadFromJsonObject(combineProperty, combineObject, globalObject)){
			return false;
		}
		combineProperties << combineProperty;
	}
	object.SetCombineProperties(combineProperties);

	return true;
}


bool CSchema::operator==(const CSchema& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_type == other.m_type);
	retVal = retVal && (m_requiredProperties == other.m_requiredProperties);
	retVal = retVal && (m_properties == other.m_properties);
	retVal = retVal && (m_enumValues == other.m_enumValues);
	retVal = retVal && (m_combineType == other.m_combineType);
	retVal = retVal && (m_combineProperties == other.m_combineProperties);

	return retVal;
}


} // namespace imtoas

