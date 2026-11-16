// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CApiClientComp.h>


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtclientgql/imtclientgql.h>
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

IGqlClient::GqlResult CApiClientComp::SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParam) const
{
	if (!requestPtr.IsValid()){
		return {{}, EC_INVALID_REQUEST, QStringLiteral("Request is invalid")};
	}

	GqlResult retVal;

	if (m_protocolEngineCompPtr.IsValid()){
		imtgql::IGqlRequest::RequestType requestType = requestPtr->GetRequestType();
		if ((requestType == imtgql::IGqlRequest::RT_QUERY) || (requestType == imtgql::IGqlRequest::RT_MUTATION)){
			QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(*requestPtr, urlParam);
			if (networkRequestPtr != nullptr){
				QByteArray uuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

				istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_INFO;
				QString message = QStringLiteral("Send request with ID %1, command %2").arg(QString::fromUtf8(uuid), QString::fromUtf8(requestPtr->GetCommandId()));
				SendVerboseMessage(message, "API client");

				QElapsedTimer requestTimer;
				requestTimer.start();
				QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPost(*networkRequestPtr, requestPtr->GetQuery(), m_timeout);
				if (replyPtr != nullptr){
					QNetworkReply::NetworkError error = replyPtr->error();
					if (error == QNetworkReply::NoError){
						QByteArray payload = replyPtr->readAll();

						imtgql::CGqlResponse* responsePtr(new imtgql::CGqlResponse(requestPtr));
						responsePtr->SetResponseData(payload);
						retVal.responsePtr.SetPtr(responsePtr);

						category = istd::IInformationProvider::IC_INFO;
					}
					else{
						const int status = replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
						// DoSyncPost aborts a running reply when its wait expires.
						const bool isTimeout = error == QNetworkReply::TimeoutError
									|| (error == QNetworkReply::OperationCanceledError && m_timeout > 0 && requestTimer.elapsed() >= m_timeout);
						retVal.errorCategory = status == 401 || status == 403 ? EC_ACCESS_DENIED
									: isTimeout ? EC_TIMEOUT : EC_NETWORK;
						category = istd::IInformationProvider::IC_ERROR;
						message = "Response for request-ID " + uuid + "\n" + replyPtr->errorString();
						retVal.errorMessage = GetReplyErrorMessage(*replyPtr, replyPtr->readAll());
					}
				}
				else{
					category = istd::IInformationProvider::IC_ERROR;
					message = "Null response for request-ID " + uuid;
					retVal.errorCategory = EC_NETWORK;
					retVal.errorMessage = message;
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
				retVal.errorCategory = EC_INTERNAL;
				retVal.errorMessage = QStringLiteral("Failed to create network request");
			}

			delete(networkRequestPtr);
		}
		else{
			SendErrorMessage(0, "Invalid request type", "API Client");
			retVal.errorCategory = EC_INVALID_REQUEST;
			retVal.errorMessage = QStringLiteral("Invalid request type");
		}
	}
	else{
		retVal.errorCategory = EC_INTERNAL;
		retVal.errorMessage = QStringLiteral("Protocol engine is not configured");
		SendErrorMessage(0, retVal.errorMessage, "API Client");
	}

	return retVal;
}


bool CApiClientComp::SendRequestNoWait(GqlRequestPtr /*requestPtr*/, imtbase::IUrlParam* /*urlParamPtr*/) const
{
	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CApiClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeout = *m_timeoutAttrPtr * 1000;
}


} // namespace imtclientgql
