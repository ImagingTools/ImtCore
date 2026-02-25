// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CRequestSender.h>


// Qt includes
#include <QtNetwork/QNetworkReply>


namespace imtcom
{


// public methods


thread_local std::unique_ptr<QNetworkAccessManager> CRequestSender::s_networkManagerPtr;


QNetworkReply* CRequestSender::DoSyncGet(const QNetworkRequest& request, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->get(request);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		replyPtr->ignoreSslErrors();

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		networkOperation.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::DoSyncPut(const QNetworkRequest& request, const QByteArray& data, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->put(request, data);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		networkOperation.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::DoSyncPost(const QNetworkRequest& request, const QByteArray& data, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->post(request, data);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		replyPtr->ignoreSslErrors();

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		networkOperation.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::DoSyncCustomRequest(const QNetworkRequest& request, const QByteArray& verb, const QByteArray& data, int timeout)
{
	EnsureNetworkAccessManager();

	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = s_networkManagerPtr->sendCustomRequest(request, verb, data);
	if (replyPtr != nullptr){
		QObject::connect(replyPtr, &QNetworkReply::finished, &networkOperation.connectionLoop, &QEventLoop::quit);

		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		networkOperation.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


// private methods

void CRequestSender::EnsureNetworkAccessManager()
{
	if (s_networkManagerPtr == nullptr){
		s_networkManagerPtr.reset(new QNetworkAccessManager());
	}
}


// public methods of the embedded class NetworkOperation

CRequestSender::NetworkOperation::NetworkOperation(int timeout)
{
	// If the application will be finished, the internal event loop will be also finished:
	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit);

	// If a timeout for the request was defined, start the timer:
	if (timeout > 0){
		timer.setSingleShot(true);

		// If the timer is running out, the internal event loop will be finished:
		QObject::connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit);

		timer.start(timeout);
	}
}


CRequestSender::NetworkOperation::~NetworkOperation()
{
	timer.stop();

	QObject::disconnect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit);
	QObject::disconnect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit);

	Q_ASSERT(!timer.isActive());
	Q_ASSERT(!connectionLoop.isRunning());
}


} // namespace imtcom


