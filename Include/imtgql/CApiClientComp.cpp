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

	(const_cast<CApiClientComp*>(this))->moveToThread(QThread::currentThread());

	QNetworkAccessManager* networkManagerPtr = new QNetworkAccessManager;

	QEventLoop connectionLoop;
	QObject::connect(networkManagerPtr, SIGNAL(finished(QNetworkReply*)), &connectionLoop, SLOT(quit()), Qt::DirectConnection);

	QTimer timer;
	timer.setSingleShot(true);
	QObject::connect(&timer, SIGNAL(timeout()), &connectionLoop, SLOT(quit()), Qt::DirectConnection);
	QObject::connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), &connectionLoop, SLOT(quit()), Qt::DirectConnection);

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

				// Check if the reply was timed out and cancel further network processing:
				if (replyPtr->isRunning()){
					replyPtr->abort();
				}

				bool retVal = true;

				// Copy task output to processor output:
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


