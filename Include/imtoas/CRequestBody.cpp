// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CRequestBody.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


CRequestBody::CRequestBody()
	:	m_isRequired(false)
{
}


QString CRequestBody::GetId() const
{
	return m_id;
}

void CRequestBody::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
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


bool CRequestBody::ReadFromJsonObject(CRequestBody& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	if (!jsonObject.contains("content")){
		return false;
	}
	const QJsonObject contentArray = jsonObject["content"].toObject();
	qsizetype contentCount = contentArray.size();
	if (contentCount <= 0){
		return false;
	}
	QList<CMediaType> contentList;
	if (!COasTools::ExtractItemsFromObject(contentList, contentArray, globalObject)){
		return false;
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
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_description == other.m_description);
	retVal = retVal && (m_content == other.m_content);
	retVal = retVal && (m_isRequired == other.m_isRequired);

	return retVal;
}


} // namespace imtoas

