#include <imtclientgql/CApiClientComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtcom/CRequestSender.h>


namespace imtclientgql
{


// public methods

CApiClientComp::CApiClientComp()
	:m_timeout(30000)
{
}


// reimplemented (IGqlClient)

bool CApiClientComp::SendRequest(const imtgql::IGqlRequest& request, imtgql::IGqlResponseHandler& responseHandler) const
{
	bool retVal = false;

	if (m_protocolEngineCompPtr.IsValid()){
		imtgql::IGqlRequest::RequestType requestType = request.GetRequestType();
		if ((requestType == imtgql::IGqlRequest::RT_QUERY) || (requestType == imtgql::IGqlRequest::RT_MUTATION)){
			QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(request);
			if (networkRequestPtr != nullptr){
				QByteArray uuid = QUuid::createUuid().toByteArray();

				istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_INFO;
				QString message = "Send request with ID " + uuid + "\n" + request.GetQuery();

				SendVerboseMessage(message, "API client");

				imtcom::CRequestSender requestSender;
				QNetworkReply* replyPtr = requestSender.DoSyncPost(*networkRequestPtr, request.GetQuery(), m_timeout);

				if (replyPtr != nullptr){
					QNetworkReply::NetworkError error = replyPtr->error();
					if (error == QNetworkReply::NoError){
						QByteArray payload = replyPtr->readAll();

						responseHandler.OnReply(request, payload);

						category = istd::IInformationProvider::IC_INFO;

						retVal = true;
					}
					else{
						category = istd::IInformationProvider::IC_ERROR;
						message = "Response for request-ID " + uuid + "\n" + replyPtr->errorString();
					}
				}
				else{
					category = istd::IInformationProvider::IC_ERROR;
					message = "Response for request-ID " + uuid + "\n" + replyPtr->errorString();
				}

				if (category > istd::IInformationProvider::IC_INFO){
					SendLogMessage(category, 0, message, "API Client");
				}
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


} // namespace imtclientgql


