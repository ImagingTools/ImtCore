// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlQueryBasedPublisherCompBase.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

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
		QJsonObject resultObj = m_requestHandlerCompPtr->CreateResponse(clonedRequest, errorMessage);
		if (resultObj.isEmpty()){
			errorMessage = QString("Unable to send response to the subscribers result model is invalid");
			SendErrorMessage(0, errorMessage, "CGqlQueryBasedPublisherCompBase");

			return false;
		}

		QJsonObject dataObj = resultObj.value(QStringLiteral("data")).toObject();
		if (dataObj.isEmpty()){
			errorMessage = QString("Unable to send response to the subscribers result model is invalid");
			SendErrorMessage(0, errorMessage, "CGqlQueryBasedPublisherCompBase");

			return false;
		}

		QJsonDocument doc(dataObj);
		QByteArray data = doc.toJson(QJsonDocument::Compact);

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


