// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpStreamingResponse.h>


namespace imtrest
{


// public methods

CHttpStreamingResponse::CHttpStreamingResponse(
			int statusCode,
			const QByteArray& dataTypeId,
			const IProtocolEngine& engine,
			qint64 bodySize,
			const SourceFactory& sourceFactory)
	:CHttpResponse(statusCode, QByteArray(), dataTypeId, engine),
	m_bodySize(bodySize),
	m_sourceFactory(sourceFactory)
{
}


// reimplemented (imtrest::IStreamingResponse)

qint64 CHttpStreamingResponse::GetBodySize() const
{
	return m_bodySize;
}


IResponseBodySourcePtr CHttpStreamingResponse::CreateBodySource() const
{
	if (!m_sourceFactory){
		return nullptr;
	}

	return m_sourceFactory();
}


} // namespace imtrest


