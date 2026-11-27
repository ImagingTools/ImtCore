// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CQtHttpTransport.h>


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>


namespace imtassist
{


// reimplemented (imtassist::IAssistantHttpTransport)

IAssistantHttpTransport::Response CQtHttpTransport::Post(const Request& request, const CCancellationToken& cancellationToken)
{
	Response retVal;

	if (cancellationToken.IsCanceled()){
		retVal.status = TS_CANCELED;

		return retVal;
	}

	QNetworkAccessManager networkManager;

	QNetworkRequest networkRequest(request.url);
	for (const QPair<QByteArray, QByteArray>& header: request.headers){
		networkRequest.setRawHeader(header.first, header.second);
	}

	networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

	QNetworkReply* replyPtr = networkManager.post(networkRequest, request.body);

	QEventLoop eventLoop;
	QObject::connect(replyPtr, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);

	QTimer pollTimer;
	pollTimer.setInterval(CANCELLATION_POLL_INTERVAL_MS);
	QObject::connect(&pollTimer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
	pollTimer.start();

	QElapsedTimer elapsedTimer;
	elapsedTimer.start();

	bool isTooLarge = false;
	while (!replyPtr->isFinished()){
		if (cancellationToken.IsCanceled()){
			retVal.status = TS_CANCELED;
			break;
		}

		if (elapsedTimer.elapsed() >= request.timeoutMs){
			retVal.status = TS_TIMEOUT;
			break;
		}

		if (replyPtr->bytesAvailable() > MAX_RESPONSE_SIZE){
			isTooLarge = true;
			break;
		}

		eventLoop.exec();
	}

	if (!replyPtr->isFinished()){
		QObject::disconnect(replyPtr, nullptr, &eventLoop, nullptr);
		replyPtr->abort();
		replyPtr->deleteLater();

		if (isTooLarge){
			retVal.status = TS_NETWORK_ERROR;
		}

		return retVal;
	}

	QVariant httpStatus = replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	if (httpStatus.isValid()){
		retVal.status = TS_RESPONSE;
		retVal.httpStatus = httpStatus.toInt();
		retVal.body = replyPtr->read(MAX_RESPONSE_SIZE + 1);
		if (retVal.body.size() > MAX_RESPONSE_SIZE){
			retVal.status = TS_NETWORK_ERROR;
			retVal.httpStatus = 0;
			retVal.body.clear();
		}
	}
	else{
		retVal.status = (replyPtr->error() == QNetworkReply::TimeoutError) ? TS_TIMEOUT : TS_NETWORK_ERROR;
	}

	replyPtr->deleteLater();

	return retVal;
}


} // namespace imtassist


