// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CHeader.h>


namespace imtoas
{



QString CHeader::GetId() const
{
	return m_id;
}


void CHeader::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


CSchema CHeader::GetSchema() const
{
	return m_schema;
}


void CHeader::SetSchema(const CSchema& schema)
{
	if (schema != m_schema){
		m_schema = schema;
	}
}


QString CHeader::GetDescription() const
{
	return m_description;
}


void CHeader::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


bool CHeader::ReadFromJsonObject(CHeader& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	if (jsonObject.contains("schema")){
		CSchema schema;
		const bool isSchemaReaded = CSchema::ReadFromJsonObject(schema, jsonObject["schema"].toObject(), globalObject);
		if (!isSchemaReaded){
			return false;
		}
		object.SetSchema(schema);
	}

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	return true;
}


bool CHeader::operator==(const CHeader& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_schema == other.m_schema);
	retVal = retVal && (m_description == other.m_description);

	return retVal;
}


} // namespace imtoas

