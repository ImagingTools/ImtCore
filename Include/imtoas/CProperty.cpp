#include <imtoas/CProperty.h>


namespace imtoas
{


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


bool CProperty::ReadFromJsonObject(CProperty& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant typeData = jsonObject.value("type").toVariant();
	if (!typeData.isNull()){
		object.SetType(typeData.toString());
	}

	QVariant optionData = jsonObject.value("oneOf").toVariant();
	if (!optionData.isNull()){
		/// \todo fnish it
		/// \link https://datatracker.ietf.org/doc/html/draft-bhutton-json-schema-00#section-4
	}

	QVariant formatData = jsonObject.value("format").toVariant();
	if (!formatData.isNull()){
		object.SetFormat(formatData.toString());
	}

	return true;
}


bool CProperty::operator==(const CProperty& other) const
{
	bool retVal = true;
	retVal = retVal && (m_type == other.m_type);
	retVal = retVal && (m_format == other.m_format);

	return retVal;
}


} // namespace imtoas

