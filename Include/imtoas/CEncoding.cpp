// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CEncoding.h>


// ImtCore includes
#include <imtoas/COasTools.h>


namespace imtoas
{


CEncoding::CEncoding()
	:	m_isExplode(false),
	m_allowReserved(false)
{
}


QString CEncoding::GetId() const
{
	return m_id;
}

void CEncoding::SetId(const QString& id)
{
	if (id != m_id){
		m_id = id;
	}
}


QString CEncoding::GetContentType() const
{
	return m_contentType;
}


void CEncoding::SetContentType(const QString& contentType)
{
	if (contentType != m_contentType){
		m_contentType = contentType;
	}
}


QList<CHeader> CEncoding::GetHeaders() const
{
	return m_headers;
}


void CEncoding::SetHeaders(const QList<CHeader>& headers)
{
	if (headers != m_headers){
		m_headers = headers;
	}
}


QString CEncoding::GetStyle() const
{
	return m_style;
}


void CEncoding::SetStyle(const QString& style)
{
	if (style != m_style){
		m_style = style;
	}
}


bool CEncoding::IsExplode() const
{
	return m_isExplode;
}


void CEncoding::SetExplode(bool explode)
{
	if (explode != m_isExplode){
		m_isExplode = explode;
	}
}


bool CEncoding::IsAllowReserved() const
{
	return m_allowReserved;
}


void CEncoding::SetAllowReserved(bool allowReserved)
{
	if (allowReserved != m_allowReserved){
		m_allowReserved = allowReserved;
	}
}


bool CEncoding::ReadFromJsonObject(CEncoding& object, const QJsonObject& jsonObject, const QJsonObject& globalObject)
{
	QVariant contentTypeData = jsonObject.value("contentType").toVariant();
	if (!contentTypeData.isNull()){
		object.SetContentType(contentTypeData.toString());
	}

	if (jsonObject.contains("headers")){
		const QJsonObject headersArray = jsonObject["headers"].toObject();
		QList<CHeader> headersList;
		if (!COasTools::ExtractItemsFromObject(headersList, headersArray, globalObject)){
			return false;
		}
		object.SetHeaders(headersList);
	}

	QVariant styleData = jsonObject.value("style").toVariant();
	if (!styleData.isNull()){
		object.SetStyle(styleData.toString());
	}

	QVariant explodeData = jsonObject.value("explode").toVariant();
	if (!explodeData.isNull()){
		object.SetExplode(explodeData.toBool());
	}

	QVariant allowReservedData = jsonObject.value("allowReserved").toVariant();
	if (!allowReservedData.isNull()){
		object.SetAllowReserved(allowReservedData.toBool());
	}

	return true;
}


bool CEncoding::operator==(const CEncoding& other) const
{
	bool retVal = true;
	retVal = retVal && (m_id == other.m_id);
	retVal = retVal && (m_contentType == other.m_contentType);
	retVal = retVal && (m_headers == other.m_headers);
	retVal = retVal && (m_style == other.m_style);
	retVal = retVal && (m_isExplode == other.m_isExplode);
	retVal = retVal && (m_allowReserved == other.m_allowReserved);

	return retVal;
}


} // namespace imtoas

