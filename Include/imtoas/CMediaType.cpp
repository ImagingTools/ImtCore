// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CMediaType.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


QString CMediaType::GetId() const
{
	return m_id;
}


void CMediaType::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


CSchema CMediaType::GetSchema() const
{
	return m_schema;
}


void CMediaType::SetSchema(const CSchema& schema)
{
	if (schema != m_schema){
		m_schema = schema;
	}
}


QList<CEncoding> CMediaType::GetEncodings() const
{
	return m_encodings;
}


void CMediaType::SetEncodings(const QList<CEncoding>& encoding)
{
	if (encoding != m_encodings){
		m_encodings = encoding;
	}
}


bool CMediaType::ReadFromJsonObject(CMediaType& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	if (jsonObject.contains("schema")){
		CSchema schema;
		QJsonObject schemaObject = jsonObject["schema"].toObject();
		if (!COasTools::DeReferenceObject(schemaObject, globalObject, schemaObject)){
			return false;
		}

		const bool isSchemaReaded = CSchema::ReadFromJsonObject(schema, schemaObject, globalObject);
		if (!isSchemaReaded){
			return false;
		}
		object.SetSchema(schema);
	}

	if (jsonObject.contains("encoding")){
		const QJsonObject encodingArray = jsonObject["encoding"].toObject();
		QList<CEncoding> encodingList;
		if (!COasTools::ExtractItemsFromObject(encodingList, encodingArray, globalObject)){
			return false;
		}
		object.SetEncodings(encodingList);
	}

	return true;
}


bool CMediaType::operator==(const CMediaType& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_schema == other.m_schema);
	retVal = retVal && (m_encodings == other.m_encodings);

	return retVal;
}


} // namespace imtoas

