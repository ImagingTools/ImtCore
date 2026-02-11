// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlQueryBasedPublisherCompBase.h>


// ACF includes
#include<istd/TDelPtr.h>

// ImtCore includes
#include<imtrest/IProtocolEngine.h>
#include<imtrest/ISender.h>
#include<imtrest/CWebSocketRequest.h>


namespace imtservergql
{


// protected methods

bool CGqlQueryBasedPublisherCompBase::Publish(bool useAwsStyle)
{
	if (*m_requestHandlerCommandIdAtrPtr == ""){
		return false;
	}

	if (!m_requestHandlerCompPtr.IsValid()){
		return false;
	}

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		imtgql::CGqlRequest clonedRequest;
		clonedRequest.CopyFrom(requestNetworks.gqlRequest);
		clonedRequest.SetRequestType(imtgql::CGqlRequest::RT_QUERY);
		clonedRequest.SetCommandId(*m_requestHandlerCommandIdAtrPtr);

		QString errorMessage;
		istd::TDelPtr<imtbase::CTreeItemModel> resultModelPtr = m_requestHandlerCompPtr->CreateResponse(clonedRequest, errorMessage);
		if (!resultModelPtr.IsValid()){
			errorMessage = QString("Unable to send response to the subscribers result model is invalid");
			SendErrorMessage(0, errorMessage, "CGqlQueryBasedPublisherCompBase");

			return false;
		}

		imtbase::CTreeItemModel* dataModelPtr = resultModelPtr->GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			errorMessage = QString("Unable to send response to the subscribers result model is invalid");
			SendErrorMessage(0, errorMessage, "CGqlQueryBasedPublisherCompBase");

			return false;
		}

		QByteArray data =  dataModelPtr->ToJson().toUtf8();

		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequestPtr = requestNetworks.networkRequests[id];
			if (networkRequestPtr != nullptr){
				PushDataToSubscriber(id, "", data, *networkRequestPtr, useAwsStyle);
			}
		}
	}

	return true;
}


} // namespace imtservergql


