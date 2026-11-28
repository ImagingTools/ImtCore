// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpClientRequestSender.h>


// STL includes
#include <limits>

// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

// ImtCore includes
#include <imtrest/CHttpClientRequest.h>


namespace imtrest
{


CHttpClientRequestSender::SendResult CHttpClientRequestSender::Send(
			CHttpClientRequest& request,
			int timeoutMs,
			const CancellationCheck& isCanceled,
			qint64 maxReplySize)
{
	if (maxReplySize < 0 || maxReplySize == std::numeric_limits<qint64>::max()){
		request.SetState(IHttpClientRequest::RS_FINISHED | IHttpClientRequest::RS_ERROR);

		return SR_INVALID_REQUEST;
	}

	if (!request.GetUrl().isValid()){
		request.SetState(IHttpClientRequest::RS_FINISHED | IHttpClientRequest::RS_ERROR);

		return SR_INVALID_REQUEST;
	}

	if (isCanceled && isCanceled()){
		request.SetError(QNetworkReply::OperationCanceledError, QStringLiteral("Canceled"));
		request.SetState(IHttpClientRequest::RS_FINISHED | IHttpClientRequest::RS_ERROR);

		return SR_CANCELED;
	}

	QNetworkAccessManager networkManager;

	QNetworkRequest networkRequest = request.GetRequest();
	networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

	QNetworkReply* replyPtr = nullptr;
	switch (request.GetRequestType()){
	case QNetworkAccessManager::GetOperation:
		replyPtr = networkManager.get(networkRequest);
		break;
	case QNetworkAccessManager::PostOperation:
		replyPtr = networkManager.post(networkRequest, request.GetRequestBody());
		break;
	case QNetworkAccessManager::PutOperation:
		replyPtr = networkManager.put(networkRequest, request.GetRequestBody());
		break;
	case QNetworkAccessManager::DeleteOperation:
		replyPtr = networkManager.deleteResource(networkRequest);
		break;
	case QNetworkAccessManager::HeadOperation:
		replyPtr = networkManager.head(networkRequest);
		break;
	default:
		break;
	}

	if (replyPtr == nullptr){
		request.SetState(IHttpClientRequest::RS_FINISHED | IHttpClientRequest::RS_ERROR);

		return SR_INVALID_REQUEST;
	}

	replyPtr->setReadBufferSize(maxReplySize + 1);
	request.SetReply(replyPtr);

	QEventLoop eventLoop;
	QObject::connect(replyPtr, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);

	QTimer pollTimer;
	pollTimer.setInterval(CANCELLATION_POLL_INTERVAL_MS);
	QObject::connect(&pollTimer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
	pollTimer.start();

	QElapsedTimer elapsedTimer;
	elapsedTimer.start();

	SendResult retVal = SR_FINISHED;
	while (!replyPtr->isFinished()){
		if (isCanceled && isCanceled()){
			retVal = SR_CANCELED;
			break;
		}

		if (elapsedTimer.elapsed() >= timeoutMs){
			retVal = SR_TIMEOUT;
			break;
		}

		if (replyPtr->bytesAvailable() > maxReplySize){
			retVal = SR_TOO_LARGE;
			break;
		}

		eventLoop.exec();
	}

	if (retVal != SR_FINISHED){
		QObject::disconnect(replyPtr, nullptr, &eventLoop, nullptr);
		replyPtr->abort();

		request.SetReply(nullptr);
		if (retVal == SR_TIMEOUT){
			request.SetError(QNetworkReply::TimeoutError, QStringLiteral("Timeout"));
		}
		else if (retVal == SR_CANCELED){
			request.SetError(QNetworkReply::OperationCanceledError, QStringLiteral("Canceled"));
		}
		else{
			request.SetError(QNetworkReply::UnknownContentError, QStringLiteral("Reply too large"));
		}

		request.SetState(IHttpClientRequest::RS_FINISHED | IHttpClientRequest::RS_ERROR);

		return retVal;
	}

	if (replyPtr->bytesAvailable() > maxReplySize){
		request.SetReply(nullptr);
		request.SetError(QNetworkReply::UnknownContentError, QStringLiteral("Reply too large"));
		request.SetState(IHttpClientRequest::RS_FINISHED | IHttpClientRequest::RS_ERROR);

		return SR_TOO_LARGE;
	}

	// Copies status, headers, body and error; the reply is deleted with the network access manager.
	request.SetReply(replyPtr);

	return SR_FINISHED;
}


} // namespace imtrest

