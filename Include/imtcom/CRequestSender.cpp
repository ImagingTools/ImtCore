#include <imtcom/CRequestSender.h>


// Qt includes
#include <QtNetwork/QNetworkReply>


namespace imtcom
{


// public methods

QNetworkReply* CRequestSender::DoSyncGet(const QNetworkRequest& request, int timeout)
{
	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = networkOperation.networkManagerPtr->get(request);
	if (replyPtr != nullptr){
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
	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = networkOperation.networkManagerPtr->put(request, data);
	if (replyPtr != nullptr){
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
	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = networkOperation.networkManagerPtr->post(request, data);
	if (replyPtr != nullptr){
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
	NetworkOperation networkOperation(timeout);

	QNetworkReply* replyPtr = networkOperation.networkManagerPtr->sendCustomRequest(request, verb, data);
	if (replyPtr != nullptr){
		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		networkOperation.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


// public methods of the embedded class NetworkOperation

CRequestSender::NetworkOperation::NetworkOperation(int timeout)
{
	networkManagerPtr = new QNetworkAccessManager();

	// If the network reply is finished, the internal event loop will be finished:
	QObject::connect(networkManagerPtr, &QNetworkAccessManager::finished, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

	// If the application will be finished, the internal event loop will be also finished:
	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

	// If a timeout for the request was defined, start the timer:
	if (timeout > 0){
		timer.setSingleShot(true);

		// If the timer is running out, the internal event loop will be finished:
		QObject::connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

		timer.start(timeout);
	}
}

CRequestSender::NetworkOperation::~NetworkOperation()
{
	networkManagerPtr->deleteLater();
}


} // namespace imtcom


