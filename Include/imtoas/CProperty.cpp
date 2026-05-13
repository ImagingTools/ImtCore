// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CProperty.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


CProperty::CProperty()
	:m_combineType(CT_SIMPLE)
{
}


QString CProperty::GetId() const
{
	return m_id;
}


void CProperty::SetId(const QString& id)
{
	if (m_id != id){
		m_id = id;
	}
}


QString CProperty::GetType() const
{
	return m_type;
}


void CProperty::SetType(const QString& type)
{
	if (type != m_type){
		m_type = type;
	}
}


QString CProperty::GetFormat() const
{
	return m_format;
}


void CProperty::SetFormat(const QString& format)
{
	if (format != m_format){
		m_format = format;
	}
}


CProperty::CombineType CProperty::GetCombineType() const
{
	return m_combineType;
}


void CProperty::SetCombineType(CombineType combineType)
{
	if (combineType != m_combineType){
		m_combineType = combineType;
	}
}


QList<CProperty> CProperty::GetCombineProperties() const
{
	return m_combineProperties;
}


void CProperty::SetCombineProperties(const QList<CProperty>& combineProperties)
{
	if (combineProperties != m_combineProperties){
		m_combineProperties = combineProperties;
	}
}


bool CProperty::ReadFromJsonObject(CProperty& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant typeData = jsonObject.value("type").toVariant();
	if (!typeData.isNull()){
		object.SetType(typeData.toString());
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

	QVariant formatData = jsonObject.value("format").toVariant();
	if (!formatData.isNull()){
		object.SetFormat(formatData.toString());
	}

	return true;
}


bool CProperty::operator==(const CProperty& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_type == other.m_type);
	retVal = retVal && (m_format == other.m_format);
	retVal = retVal && (m_combineType == other.m_combineType);
	retVal = retVal && (m_combineProperties == other.m_combineProperties);

	return retVal;
}


} // namespace imtoas

