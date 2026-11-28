// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CRestHttpTransport.h>


// ImtCore includes
#include <imtrest/CHttpClientRequest.h>
#include <imtrest/CHttpClientRequestSender.h>


namespace imtassist
{


// reimplemented (imtassist::IAssistantHttpTransport)

IAssistantHttpTransport::Response CRestHttpTransport::Post(const Request& request, const CCancellationToken& cancellationToken)
{
	QNetworkRequest networkRequest(request.url);
	for (const QPair<QByteArray, QByteArray>& header: request.headers){
		networkRequest.setRawHeader(header.first, header.second);
	}

	imtrest::CHttpClientRequest clientRequest;
	clientRequest.SetRequest(networkRequest);
	clientRequest.SetRequestType(QNetworkAccessManager::PostOperation);
	clientRequest.SetRequestBody(request.body);

	imtrest::CHttpClientRequestSender::SendResult sendResult = imtrest::CHttpClientRequestSender::Send(
				clientRequest,
				request.timeoutMs,
				[cancellationToken](){ return cancellationToken.IsCanceled(); },
				MAX_RESPONSE_SIZE);

	Response retVal;
	switch (sendResult){
	case imtrest::CHttpClientRequestSender::SR_CANCELED:
		retVal.status = TS_CANCELED;
		return retVal;

	case imtrest::CHttpClientRequestSender::SR_TIMEOUT:
		retVal.status = TS_TIMEOUT;
		return retVal;

	case imtrest::CHttpClientRequestSender::SR_FINISHED:
		break;

	default:
		retVal.status = TS_NETWORK_ERROR;
		return retVal;
	}

	QVariant httpStatus = clientRequest.GetAttribute(QNetworkRequest::HttpStatusCodeAttribute);
	if (!httpStatus.isValid()){
		retVal.status = (clientRequest.GetErrorType() == QNetworkReply::TimeoutError) ? TS_TIMEOUT : TS_NETWORK_ERROR;

		return retVal;
	}

	// A successful status with a network error means an incomplete body; error statuses keep the body for the error mapping.
	int httpStatusCode = httpStatus.toInt();
	QNetworkReply::NetworkError errorType = clientRequest.GetErrorType();
	if ((httpStatusCode >= 200) && (httpStatusCode < 300) && (errorType != QNetworkReply::NoError)){
		retVal.status = (errorType == QNetworkReply::TimeoutError) ? TS_TIMEOUT : TS_NETWORK_ERROR;

		return retVal;
	}

	retVal.status = TS_RESPONSE;
	retVal.httpStatus = httpStatusCode;
	retVal.body = clientRequest.GetReplyBody();

	return retVal;
}


} // namespace imtassist


