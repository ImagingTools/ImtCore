#include <imtgql/CApiClientComp.h>


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QUuid>


namespace imtgql
{


class RequestSender: public QObject
{
public:
	RequestSender(const CApiClientComp& client)
		:m_client(client)
	{
	}

	bool SendRequest(const IGqlRequest& request, IGqlClient::ResponseHandler& responseHandler) const
	{
		if (!m_client.m_protocolEngineCompPtr.IsValid()){
			m_client.SendCriticalMessage(0, "Client engine component was not set. Check the component configuration", "API client");

			return false;
		}

		// Each request with its own instance of the network management:
		QNetworkAccessManager* networkManagerPtr = new QNetworkAccessManager;

		QEventLoop connectionLoop;

		// If the network reply is finished, the internal event loop will be finished:
		QObject::connect(networkManagerPtr, &QNetworkAccessManager::finished, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

		// If the application will be finished, the internal event loop will be also finished:
		QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

		// If a timeout for the request was defined, start the timer:
		QTimer timer;
		if (m_client.m_timeout > 0){
			timer.setSingleShot(true);

			// If the timer is running out, the internal event loop will be finished:
			QObject::connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

			timer.start(m_client.m_timeout);
		}

		bool retVal = true;

		QNetworkRequest* networkRequestPtr = m_client.m_protocolEngineCompPtr->CreateNetworkRequest(request);
		if (networkRequestPtr != nullptr){
			IGqlRequest::RequestType requestType = request.GetRequestType();
			if ((requestType == IGqlRequest::RT_QUERY) || (requestType == IGqlRequest::RT_MUTATION)){
				QNetworkReply* replyPtr = networkManagerPtr->post(*networkRequestPtr, request.GetQuery());
				if (replyPtr != nullptr){
					QByteArray uuid = QUuid::createUuid().toByteArray();
					QString message = "Send request with ID " + uuid + "\n" + request.GetQuery();
					m_client.SendInfoMessage(0, message, "API client");

					connect(replyPtr, &QNetworkReply::finished, this, &RequestSender::OnReply, Qt::DirectConnection);

					{
						QWriteLocker mapLock(&m_client.m_requestMapMutex);

						CApiClientComp::Response response;

						response.responseHandlerPtr = &responseHandler;
						response.requestPtr.SetCastedOrRemove(request.CloneMe());
						response.uuid = uuid;

						Q_ASSERT(response.requestPtr.IsValid());

						m_client.m_requestMap[replyPtr] = response;
					}

					connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);

					timer.stop();

					// Check if the reply was timed out and cancel further network processing:
					if (replyPtr->isRunning()){
						replyPtr->abort();

						retVal = false;
					}

					// Check if the reply was failed:
					if (replyPtr->error()){
						message = "Response for request ID " + uuid + "\n" + replyPtr->errorString();
						m_client.SendErrorMessage(0, message, "API client");

						retVal = false;
					}

					{
						QWriteLocker mapLock(&m_client.m_requestMapMutex);

						m_client.m_requestMap.remove(replyPtr);
					}
				}
				else{
					qDebug("POST operation failed");

					retVal = false;
				}
			}
		}
		else{
			qDebug("Network request could not be created by the client engine");

			retVal = false;
		}

		networkManagerPtr->deleteLater();

		return retVal;
	}

	void OnReply()
	{
		QNetworkReply* replyPtr = dynamic_cast<QNetworkReply*>(sender());
		if (replyPtr != nullptr){
			QString message;
			istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_NONE;

			{
				QReadLocker mapLock(&m_client.m_requestMapMutex);

				if (m_client.m_requestMap.contains(replyPtr)) {
					IGqlClient::ResponseHandler* responseHandlerPtr = m_client.m_requestMap[replyPtr].responseHandlerPtr;
					Q_ASSERT(responseHandlerPtr != nullptr);

					IGqlRequest* requestPtr = m_client.m_requestMap[replyPtr].requestPtr.GetPtr();
					Q_ASSERT(requestPtr != nullptr);

					mapLock.unlock();

					QByteArray payload = replyPtr->readAll();

					if (replyPtr->error() == QNetworkReply::NoError) {
						responseHandlerPtr->OnReply(*requestPtr, payload);

						category = istd::IInformationProvider::IC_INFO;
						message = "Response for request ID " + m_client.m_requestMap[replyPtr].uuid + "\n" + payload;

						qDebug() << "*** SERVER RESPONSE: " << payload;
					}
					else {
						category = istd::IInformationProvider::IC_ERROR;
						message = "Response for request ID " + m_client.m_requestMap[replyPtr].uuid + "\n" + replyPtr->errorString() + (payload.isEmpty() ? QString("") : QString("\n") + payload);

						qDebug() << "*** NETWORK ERROR: " << replyPtr->errorString();
						qDebug() << payload;
					}
				}
			}

			if (!message.isEmpty()) {
				m_client.SendLogMessage(category, 0, message, "API client");
			}
		}

		replyPtr->deleteLater();
	}

private:
	const CApiClientComp& m_client;
};


// public methods

CApiClientComp::CApiClientComp()
	:m_timeout(30000)
{
}


// reimplemented (IGqlClient)

bool CApiClientComp::SendRequest(const IGqlRequest& request, ResponseHandler& responseHandler) const
{
	RequestSender* senderPtr = new RequestSender(*this);

	// Move processing instance to the current thread:
	senderPtr->moveToThread(QThread::currentThread());

	bool retVal = senderPtr->SendRequest(request, responseHandler);
	
	senderPtr->deleteLater();

	return retVal;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CApiClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeout = *m_timeoutAttrPtr * 1000;
}


} // namespace imtgql


