#include <imtoas/CHeader.h>


namespace imtoas
{


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
	retVal = retVal && (m_schema == other.m_schema);
	retVal = retVal && (m_description == other.m_description);

	return retVal;
}


} // namespace imtoas

