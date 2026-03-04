// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CResponse.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


QString CResponse::GetId() const
{
	return m_id;
}


void CResponse::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


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


bool CResponse::ReadFromJsonObject(CResponse& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	if (jsonObject.contains("headers")){
		const QJsonObject headersArray = jsonObject["headers"].toObject();
		QList<CHeader> headersList;
		if (!COasTools::ExtractItemsFromObject(headersList, headersArray, globalObject)){
			return false;
		}
		object.SetHeaders(headersList);
	}

	if (jsonObject.contains("content")){
		const QJsonObject contentArray = jsonObject["content"].toObject();
		QList<CMediaType> contentList;
		if (!COasTools::ExtractItemsFromObject(contentList, contentArray, globalObject)){
			return false;
		}
		object.SetContentList(contentList);
	}

	return true;
}


bool CResponse::operator==(const CResponse& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_headers == other.m_headers);
	retVal = retVal && (m_contentList == other.m_contentList);

	return retVal;
}


} // namespace imtoas

