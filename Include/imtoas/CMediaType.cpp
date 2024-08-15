#include <imtoas/CMediaType.h>


namespace imtoas
{


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


bool CMediaType::ReadFromJsonObject(CMediaType& object, const QJsonObject& jsonObject)
{
	if (jsonObject.contains("schema")){
		CSchema schema;
		const bool isSchemaReaded = CSchema::ReadFromJsonObject(schema, jsonObject["schema"].toObject());
		if (!isSchemaReaded){
			return false;
		}
		object.SetSchema(schema);
	}

	if (jsonObject.contains("encoding")){
		const QJsonArray encodingArray = jsonObject["encoding"].toArray();
		qsizetype encodingCount = encodingArray.size();
		QList<CEncoding> encodingList;
		for (int encodingIndex = 0; encodingIndex < encodingCount; ++encodingIndex){
			CEncoding encoding;
			if (!CEncoding::ReadFromJsonObject(encoding, encodingArray[encodingIndex].toObject())){
				return false;
			}
			encodingList << encoding;
		}
		object.SetEncodings(encodingList);
	}

	return true;
}


bool CMediaType::operator==(const CMediaType& other) const
{
	bool retVal = true;
	retVal = retVal && (m_schema == other.m_schema);
	retVal = retVal && (m_encodings == other.m_encodings);

	return retVal;
}


} // namespace imtoas

