#include <imtconn/CRequestSender.h>


// Qt includes
#include <QtNetwork/QNetworkReply>


namespace imtconn
{


// public methods

QNetworkReply* CRequestSender::Get(const QNetworkRequest& request, int timeout)
{
	WorkingObjects workingObjects(timeout);

	QNetworkReply* replyPtr = workingObjects.networkManagerPtr->get(request);
	if (replyPtr != nullptr){
		workingObjects.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		workingObjects.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::Put(const QNetworkRequest& request, const QByteArray& data, int timeout)
{
	WorkingObjects workingObjects(timeout);

	QNetworkReply* replyPtr = workingObjects.networkManagerPtr->put(request, data);
	if (replyPtr != nullptr){
		workingObjects.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		workingObjects.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::Post(const QNetworkRequest& request, const QByteArray& data, int timeout)
{
	WorkingObjects workingObjects(timeout);

	QNetworkReply* replyPtr = workingObjects.networkManagerPtr->post(request, data);
	if (replyPtr != nullptr){
		workingObjects.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		workingObjects.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


QNetworkReply* CRequestSender::CustomRequest(const QNetworkRequest& request, const QByteArray& verb, const QByteArray& data, int timeout)
{
	WorkingObjects workingObjects(timeout);

	QNetworkReply* replyPtr = workingObjects.networkManagerPtr->sendCustomRequest(request, verb, data);
	if (replyPtr != nullptr){
		workingObjects.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		workingObjects.timer.stop();

		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	}

	return replyPtr;
}


// public methods of the embedded class WorkingObjects

CRequestSender::WorkingObjects::WorkingObjects(int timeout)
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

CRequestSender::WorkingObjects::~WorkingObjects()
{
	networkManagerPtr->deleteLater();
}


} // namespace imtconn


