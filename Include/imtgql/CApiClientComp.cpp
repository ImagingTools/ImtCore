#include <imtgql/CApiClientComp.h>


namespace imtgql
{


// public methods

// reimplemented (IGqlClient)

bool CApiClientComp::SendRequest(const IGqlRequest& request, const QByteArray& data, ResponseHandler& responseHandler) const
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return false;
	}

	QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(request);
	if (networkRequestPtr != nullptr){
		IGqlRequest::RequestType requestType = request.GetRequestType();
		if (requestType == IGqlRequest::RT_QUERY){
			QNetworkReply* replyPtr = m_networkManager.post(*networkRequestPtr, data);
			if (replyPtr != nullptr){
				if (!connect(replyPtr, &QNetworkReply::finished, this, &CApiClientComp::OnReply)){
					replyPtr->deleteLater();

					return false;
				}

				RequestInfo requestInfo;
				requestInfo.replyPtr = replyPtr;
				requestInfo.responseHandlerPtr = &responseHandler;

				m_requestInfos.push_back(requestInfo);

				return true;
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
				int status = replyPtr->error();

				if (requestInfo.responseHandlerPtr != nullptr){
					requestInfo.responseHandlerPtr->OnReply(status, payload);
				}

				qDebug() << "*** NETWORK REPLY: " << payload;

			}
		}

		replyPtr->deleteLater();
	}
}

} // namespace imtgql


