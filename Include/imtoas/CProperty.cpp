#include <imtoas/CProperty.h>


namespace imtoas
{


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


bool CProperty::ReadFromJsonObject(CProperty& object, const QJsonObject& jsonObject)
{
	QVariant typeData = jsonObject.value("type").toVariant();
	if (typeData.isNull()){
		return false;
	}
	object.SetType(typeData.toString());

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

