#include <imtoas/CResponse.h>


namespace imtoas
{


QString CResponse::GetDescription() const
{
	return m_description;
}


void CResponse::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


QList<CHeader> CResponse::GetHeaders() const
{
	return m_headers;
}


void CResponse::SetHeaders(const QList<CHeader>& headers)
{
	if (headers != m_headers){
		m_headers = headers;
	}
}


QList<CMediaType> CResponse::GetContentList() const
{
	return m_contentList;
}


void CResponse::SetContentList(const QList<CMediaType>& content)
{
	if (content != m_contentList){
		m_contentList = content;
	}
}


bool CResponse::ReadFromJsonObject(CResponse& object, const QJsonObject& jsonObject)
{
	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	if (jsonObject.contains("headers")){
		const QJsonArray headersArray = jsonObject["headers"].toArray();
		qsizetype headersCount = headersArray.size();
		QList<CHeader> headersList;
		for (int headersIndex = 0; headersIndex < headersCount; ++headersIndex){
			CHeader headers;
			if (!CHeader::ReadFromJsonObject(headers, headersArray[headersIndex].toObject())){
				return false;
			}
			headersList << headers;
		}
		object.SetHeaders(headersList);
	}

	if (jsonObject.contains("content")){
		const QJsonArray contentArray = jsonObject["content"].toArray();
		qsizetype contentCount = contentArray.size();
		QList<CMediaType> contentList;
		for (int contentIndex = 0; contentIndex < contentCount; ++contentIndex){
			CMediaType content;
			if (!CMediaType::ReadFromJsonObject(content, contentArray[contentIndex].toObject())){
				return false;
			}
			contentList << content;
		}
		object.SetContentList(contentList);
	}

	return true;
}


bool CResponse::operator==(const CResponse& other) const
{
	bool retVal = true;
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_headers == other.m_headers);
	retVal = retVal && (m_contentList == other.m_contentList);

	return retVal;
}


} // namespace imtoas

