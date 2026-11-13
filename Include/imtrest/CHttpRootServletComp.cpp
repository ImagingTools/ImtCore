// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpRootServletComp.h>


// Qt includes
#include <QtCore/QDataStream>
#include <QtCore/QRegularExpression>

// 3rdParty includes
#include "zlib.h"

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/ITransport.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/IStreamingResponse.h>


namespace
{


// The body of a streaming response is not in memory. A partial response is described by Content-Range of the uncompressed
// representation (RFC 9110, section 14.4), so it cannot be compressed either.
bool IsCompressionAllowed(const imtrest::IResponse& response)
{
	if (dynamic_cast<const imtrest::IStreamingResponse*>(&response) != nullptr){
		return false;
	}

	const imtrest::IResponse::Headers headers = response.GetHeaders();
	for (imtrest::IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
		if (headerIter.key().toLower() == "content-range"){
			return false;
		}
	}

	return true;
}


} // namespace


namespace imtrest
{


// public methods

CHttpRootServletComp::CHttpRootServletComp()
	// the web clients of all JQML versions in both encodings fit
	:m_compressedDataCache(64 * 1024 * 1024)
{
}


// reimplemented (IRequestHandler)

bool CHttpRootServletComp::IsCommandSupported(const QByteArray& commandId) const
{
	return FindRequestHandler(commandId) != nullptr;
}


imtrest::ConstResponsePtr CHttpRootServletComp::ProcessRequest(const IRequest& request, const QByteArray& /*subCommandId*/) const
{
	QByteArray commandId = request.GetCommandId();
	if (commandId.startsWith('/')){
		commandId = commandId.remove(0, 1);
	}

	if (commandId.endsWith('/')){
		commandId = commandId.remove(commandId.length() - 1, 1);
	}

	const IProtocolEngine& engine = request.GetProtocolEngine();

	const IRequestServlet* handlerPtr = FindRequestHandler(commandId);
	if (handlerPtr != nullptr){
		ConstResponsePtr responsePtr = handlerPtr->ProcessRequest(request, commandId);
		const auto* httpResponseConstPtr = dynamic_cast<const CHttpResponse*>(responsePtr.GetPtr());
		auto* httpResponsePtr = dynamic_cast<CHttpResponse*>(const_cast<CHttpResponse*>(httpResponseConstPtr));

		// An empty body (e.g. 304 Not Modified) must stay empty.
		if (m_autoCompressionAttrPtr->GetValue() == true && httpResponsePtr != nullptr && IsCompressionAllowed(*responsePtr) && !responsePtr->GetData().isEmpty()){
			EncodingType encodingType = ET_NONE;
			const auto* httprequestPtr = dynamic_cast<const CHttpRequest*>(&request);
			if (httprequestPtr != nullptr){
				QByteArray encodingHeader = httprequestPtr->GetHeaderValue("accept-encoding");
				if (encodingHeader.contains("deflate")){
					encodingType = ET_DEFLATE;
				}
				else{
					if (encodingHeader.contains("gzip")){
						encodingType = ET_GZIP;
					}
				}
			}
			if (encodingType != ET_NONE){
				httpResponsePtr->SetData(GetCompressedData(commandId, *responsePtr, encodingType));
				IResponse::Headers headers = responsePtr->GetHeaders();
				headers.insert("content-encoding", encodingType == ET_DEFLATE ? "deflate" : "gzip");
				httpResponsePtr->SetHeaders(headers);
			}
		}

		return responsePtr;
	}
	else if (commandId.isEmpty()){
		QByteArray body = QByteArrayLiteral(R"(<html><head><title>Error</title></head><body><p>Empty command-ID</p></body></html>)");
		QByteArray reponseTypeId = QByteArrayLiteral("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId).PopInterfacePtr());

		return responsePtr;
	}
	else{
		QString commandIdSafe = commandId;
		commandIdSafe = commandIdSafe.replace(QRegularExpression("[<>\":;()= .]"),"_");
		QByteArray body = QStringLiteral("<html><head><title>Error</title></head><body><p>The requested command could not be executed. No servlet was found for the given command: '%1'</p></body></html>").arg(commandIdSafe).toUtf8();
		QByteArray reponseTypeId = QByteArrayLiteral("text/html; charset=utf-8");

		ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId).PopInterfacePtr());

		SendErrorMessage(0, QStringLiteral("No request handler found for: '%1'").arg(commandId));

		return responsePtr;
	}

	return ConstResponsePtr();
}


// protected methods

IRequestServlet* CHttpRootServletComp::FindRequestHandler(const QByteArray& commandId) const
{
	for (int i = 0; i < m_requestHandlersCompPtr.GetCount(); ++i){
		IRequestServlet* handlerPtr = m_requestHandlersCompPtr[i];
		if (handlerPtr != nullptr){
			if (handlerPtr->IsCommandSupported(commandId)){
				return handlerPtr;
			}
		}
	}

	return nullptr;
}


QByteArray CHttpRootServletComp::GetCompressedData(const QByteArray& commandId, const IResponse& response, EncodingType encodingType) const
{
	QByteArray eTag;
	const IResponse::Headers headers = response.GetHeaders();
	for (IResponse::Headers::ConstIterator headerIter = headers.constBegin(); headerIter != headers.constEnd(); ++headerIter){
		if (headerIter.key().toLower() == "etag"){
			eTag = headerIter.value();
			break;
		}
	}

	if (eTag.isEmpty()){
		return Compress(response.GetData(), encodingType);
	}

	const QByteArray cacheKey = commandId + '\n' + eTag + '\n' + QByteArray::number(encodingType);
	{
		QMutexLocker cacheLock(&m_compressedDataCacheMutex);
		const QByteArray* cachedDataPtr = m_compressedDataCache.object(cacheKey);
		if (cachedDataPtr != nullptr){
			return *cachedDataPtr;
		}
	}

	// two requests of a new body may both compress it, the later one replaces the earlier in the cache
	const QByteArray compressedData = Compress(response.GetData(), encodingType);
	{
		QMutexLocker cacheLock(&m_compressedDataCacheMutex);
		m_compressedDataCache.insert(cacheKey, new QByteArray(compressedData), compressedData.size());
	}

	return compressedData;
}


QByteArray CHttpRootServletComp::Compress(const QByteArray& data, EncodingType encodingType)
{
	if (encodingType == ET_DEFLATE){
		QByteArray qData = qCompress(data, 8);
		qData.remove(0, 4);

		return qData;
	}

	QByteArray qData = qCompress(data, 8);
	qData.remove(0, 6);
	qData.remove(qData.length() - 4, 4);
	int crc = (int)crc32(0, (unsigned char *)qData.data(), qData.length());
	const char gzipheader[] = {
			 0x1f, static_cast<char>(0x8b)      // gzip magic number
			 , 8             // compress method "defalte"
			 , 1             // text data
			 , 0, 0, 0, 0    // timestamp is not set
			 , 2             // maximum compression flag
			 , static_cast<char>(255)           // unknown OS
		 };
	QByteArray outData;
	QDataStream datastream(&outData, QIODevice::WriteOnly);
	datastream.device()->write(gzipheader,10);
	datastream.device()->write(qData);
	datastream << crc;
	datastream << qData.length();

	return outData;
}


// reimplemented (icomp::CComponentBase)

void CHttpRootServletComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	{
		QMutexLocker lockHandlerMap(&m_handlersMapMutex);
		m_handlersMap.clear();

		int handlersCount = qMin(m_commandIdsAttrPtr.GetCount(), m_requestHandlersCompPtr.GetCount());

		for (int i = 0; i < handlersCount; ++i){
			QByteArray registeredCommandId = m_commandIdsAttrPtr[i];
			IRequestServlet* handlerPtr = m_requestHandlersCompPtr[i];

			if ((handlerPtr != nullptr)){
				m_handlersMap[registeredCommandId] = handlerPtr;
			}
		}
	}
}


} // namespace imtrest

