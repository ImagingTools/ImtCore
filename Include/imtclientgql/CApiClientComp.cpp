// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CApiClientComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtcom/CRequestSender.h>
#include <imtgql/CGqlResponse.h>


namespace imtclientgql
{


// public methods

CApiClientComp::CApiClientComp()
	:m_timeout(30000)
{
}


// reimplemented (IGqlClient)

IGqlClient::GqlResponsePtr CApiClientComp::SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParam) const
{
	if (!requestPtr.IsValid()){
		return GqlResponsePtr(nullptr);
	}

	GqlResponsePtr retVal;

	if (m_protocolEngineCompPtr.IsValid()){
		imtgql::IGqlRequest::RequestType requestType = requestPtr->GetRequestType();
		if ((requestType == imtgql::IGqlRequest::RT_QUERY) || (requestType == imtgql::IGqlRequest::RT_MUTATION)){
			QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(*requestPtr, urlParam);
			if (networkRequestPtr != nullptr){
				QByteArray uuid = QUuid::createUuid().toByteArray();

				istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_INFO;
				QString message = "Send request with ID " + uuid + "\n" + requestPtr->GetQuery();
				SendVerboseMessage(message, "API client");

				QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPost(*networkRequestPtr, requestPtr->GetQuery(), m_timeout);
				if (replyPtr != nullptr){
					QNetworkReply::NetworkError error = replyPtr->error();
					if (error == QNetworkReply::NoError){
						QByteArray payload = replyPtr->readAll();

						imtgql::CGqlResponse* responsePtr(new imtgql::CGqlResponse(requestPtr));
						responsePtr->SetResponseData(payload);
						retVal.SetPtr(responsePtr);

						category = istd::IInformationProvider::IC_INFO;
					}
					else{
						category = istd::IInformationProvider::IC_ERROR;
						message = "Response for request-ID " + uuid + "\n" + replyPtr->errorString();
					}
				}
				else{
					category = istd::IInformationProvider::IC_ERROR;
					message = "Null response for request-ID " + uuid;
				}

				if (replyPtr != nullptr){
					replyPtr->deleteLater();
				}

				if (category > istd::IInformationProvider::IC_INFO){
					SendLogMessage(category, 0, message, "API Client");
				}
			}
			else{
				SendErrorMessage(0, "Failed to create network request", "API Client");
			}

			delete(networkRequestPtr);
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


