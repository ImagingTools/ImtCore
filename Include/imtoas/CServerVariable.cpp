// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CServerVariable.h>


namespace imtoas
{


QString CServerVariable::GetId() const
{
	return m_id;
}


void CServerVariable::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


QList<QString> CServerVariable::GetEnumValues() const
{
	return m_enumValues;
}


void CServerVariable::SetEnumValues(const QList<QString>& enums)
{
	if (enums != m_enumValues){
		m_enumValues = enums;
	}
}


QString CServerVariable::GetDefaultValue() const
{
	return m_default;
}


void CServerVariable::SetDefault(const QString& defaultValue)
{
	if (defaultValue != m_default){
		m_default = defaultValue;
	}
}


QString CServerVariable::GetDescription() const
{
	return m_description;
}


void CServerVariable::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


bool CServerVariable::ReadFromJsonObject(CServerVariable& object, const QJsonObject& jsonObject, const QJsonObject& /*globalObject*/)
{
	if (jsonObject.contains("enum")){
		const QJsonArray enumArray = jsonObject["enum"].toArray();
		qsizetype enumCount = enumArray.size();
		if (enumCount <= 0){
			return false;
		}
		QList<QString> enumList;
		for (qsizetype enumIndex = 0; enumIndex < enumCount; ++enumIndex){
			enumList << enumArray[enumIndex].toString();
		}
		object.SetEnumValues(enumList);
	}

	QVariant defaultData = jsonObject.value("default").toVariant();
	if (defaultData.isNull()){
		return false;
	}
	object.SetDefault(defaultData.toString());

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	return true;
}


bool CServerVariable::operator==(const CServerVariable& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_enumValues == other.m_enumValues);
	retVal = retVal && (m_default == other.m_default);
	retVal = retVal && (m_description == other.m_description);

	return retVal;
}



} // namespace imtoas

