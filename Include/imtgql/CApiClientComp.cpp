#include <imtgql/CApiClientComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtcom/CRequestSender.h>


namespace imtgql
{


// public methods

CApiClientComp::CApiClientComp()
	:m_timeout(30000)
{
}


// reimplemented (IGqlClient)

bool CApiClientComp::SendRequest(const IGqlRequest& request, ResponseHandler& responseHandler) const
{
	bool retVal = false;

	if (m_protocolEngineCompPtr.IsValid()){
		IGqlRequest::RequestType requestType = request.GetRequestType();
		if ((requestType == IGqlRequest::RT_QUERY) || (requestType == IGqlRequest::RT_MUTATION)){
			QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(request);

			if (networkRequestPtr != nullptr){
				QByteArray uuid = QUuid::createUuid().toByteArray();

				istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_INFO;
				QString message = "Send request with ID " + uuid + "\n" + request.GetQuery();

				SendInfoMessage(0, message, "API client");

				imtcom::CRequestSender requestSender;
				QNetworkReply* replyPtr = requestSender.DoSyncPost(*networkRequestPtr, request.GetQuery(), m_timeout);

				if (replyPtr != nullptr){
					QNetworkReply::NetworkError error = replyPtr->error();
					if (error == QNetworkReply::NoError){
						QByteArray payload = replyPtr->readAll();

						responseHandler.OnReply(request, payload);

						category = istd::IInformationProvider::IC_INFO;
						message = "Response for request ID " + uuid + "\n" + payload;

						retVal = true;
					}
					else{
						category = istd::IInformationProvider::IC_ERROR;
						message = "Response for request ID " + uuid + "\n" + replyPtr->errorString();
					}
				}
				else{
					category = istd::IInformationProvider::IC_ERROR;
					message = "Response for request ID " + uuid + "\n" + replyPtr->errorString();
				}

				SendLogMessage(category, 0, message, "API Client");
			}
			else{
				SendErrorMessage(0, "Failed to create network request", "API Client");
			}
		}
		else{
			SendErrorMessage(0, "Invalid request type", "API Client");
		}
	}

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


