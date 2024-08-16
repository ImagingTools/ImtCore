#include <imtoas/CSchema.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


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

	return true;
}


bool CSchema::operator==(const CSchema& other) const
{
	bool retVal = true;
	retVal = retVal && (m_type == other.m_type);
	retVal = retVal && (m_requiredProperties == other.m_requiredProperties);
	retVal = retVal && (m_properties == other.m_properties);
	retVal = retVal && (m_enumValues == other.m_enumValues);

	return retVal;
}


} // namespace imtoas

