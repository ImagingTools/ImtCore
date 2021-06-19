#include <imtgql/CApiClientComp.h>


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QThread>


namespace imtgql
{


// public methods
CApiClientComp::CApiClientComp()
	:m_timeout(30000),
	m_progressManagerPtr(nullptr)
{
}


// reimplemented (IGqlClient)

bool CApiClientComp::SendRequest(const IGqlRequest& request, const QByteArray& data, ResponseHandler& responseHandler) const
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return false;
	}

	// Move processing instance to the current thread:
	(const_cast<CApiClientComp*>(this))->moveToThread(QThread::currentThread());

	// Each request with its own instance of the network management:
	QNetworkAccessManager* networkManagerPtr = new QNetworkAccessManager;

	QEventLoop connectionLoop;

	// If the network reply is finished, the internal event loop will be finished:
	QObject::connect(networkManagerPtr, &QNetworkAccessManager::finished, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

	QTimer timer;
	timer.setSingleShot(true);

	// If the timer is running out, the internal event loop will be finished:
	QObject::connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

	// If the application will be finished, the internal event loop will be also finished:
	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

	// If timeout for the request was defgined, start the timer:
	if (m_timeout > 0){
		timer.start(m_timeout);
	}

	QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(request);
	if (networkRequestPtr != nullptr){
		IGqlRequest::RequestType requestType = request.GetRequestType();
		if ((requestType == IGqlRequest::RT_QUERY) || (requestType == IGqlRequest::RT_MUTATION)){
			QNetworkReply* replyPtr = networkManagerPtr->post(*networkRequestPtr, data);
			if (replyPtr != nullptr){
				connect(replyPtr, &QNetworkReply::finished, this, &CApiClientComp::OnReply, Qt::DirectConnection);

				RequestInfo requestInfo;
				requestInfo.replyPtr = replyPtr;
				requestInfo.responseHandlerPtr = &responseHandler;

				m_requestInfos.push_back(requestInfo);

				connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);

				timer.stop();

				bool retVal = true;

				// Check if the reply was timed out and cancel further network processing:
				if (replyPtr->isRunning()){
					replyPtr->abort();

					retVal = false;
				}

				// Check if the reply was failed:
				if (replyPtr->error()){
					SendErrorMessage(0, replyPtr->errorString(), "API client");

					retVal = false;
				}

				networkManagerPtr->deleteLater();

				return retVal;
			}
		}
	}

	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CApiClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


// private slots

void CApiClientComp::OnReply()
{
	QNetworkReply* replyPtr = dynamic_cast<QNetworkReply*>(sender());
	if (replyPtr != nullptr){
		for (const RequestInfo& requestInfo : m_requestInfos){
			if (requestInfo.replyPtr == replyPtr){
				QByteArray payload = replyPtr->readAll();

				if (replyPtr->error() == QNetworkReply::NoError){
					if (requestInfo.responseHandlerPtr != nullptr){
						requestInfo.responseHandlerPtr->OnReply(payload);
					}

					qDebug() << "*** SERVER RESPONSE: " << payload;
				}
				else{
					qDebug() << "*** NETWORK ERROR: " << replyPtr->errorString();
				}
			}
		}

		replyPtr->deleteLater();
	}
}

} // namespace imtgql


