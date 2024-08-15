#include <imtoas/CRequestBody.h>


namespace imtoas
{


CRequestBody::CRequestBody()
	:	m_isRequired(false)
{
}


QString CRequestBody::GetDescription() const
{
	return m_description;
}


void CRequestBody::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


QList<CMediaType> CRequestBody::GetContent() const
{
	return m_content;
}


void CRequestBody::SetContent(const QList<CMediaType>& content)
{
	if (content != m_content){
		m_content = content;
	}
}


bool CRequestBody::IsRequired() const
{
	return m_isRequired;
}


void CRequestBody::SetRequired(bool required)
{
	if (required != m_isRequired){
		m_isRequired = required;
	}
}


bool CRequestBody::ReadFromJsonObject(CRequestBody& object, const QJsonObject& jsonObject)
{
	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	if (!jsonObject.contains("content")){
		return false;
	}
	const QJsonArray contentArray = jsonObject["content"].toArray();
	qsizetype contentCount = contentArray.size();
	if (contentCount <= 0){
		return false;
	}
	QList<CMediaType> contentList;
	for (int contentIndex = 0; contentIndex < contentCount; ++contentIndex){
		CMediaType content;
		if (!CMediaType::ReadFromJsonObject(content, contentArray[contentIndex].toObject())){
			return false;
		}
		contentList << content;
	}
	object.SetContent(contentList);

	QVariant requiredData = jsonObject.value("required").toVariant();
	if (!requiredData.isNull()){
		object.SetRequired(requiredData.toBool());
	}

	return true;
}


bool CRequestBody::operator==(const CRequestBody& other) const
{
	bool retVal = true;
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_content == other.m_content);
	retVal = retVal && (m_isRequired == other.m_isRequired);

	return retVal;
}


} // namespace imtoas

