// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CSessionModelSubscriberControllerComp.h>


// ImtCore includes
#include <imtgql/IGqlRequestProvider.h>
#include<imtrest/IProtocolEngine.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CSessionModelSubscriberControllerComp::OnSessionModelChanged(const istd::IChangeable::ChangeSet& changeSet, const QByteArray& sessionId)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return;
	}

	if (sessionId.isEmpty()){
		return;
	}

	QSet<int> ids = changeSet.GetIds();
	QJsonArray idsArray;
	for (int id : ids){
		QJsonObject obj;
		obj["id"] = QJsonValue(QString::number(id));
		idsArray.append(obj);
	}

	QJsonObject changeObj;
	const istd::IChangeable::ChangeInfoMap changeMap = changeSet.GetChangeInfoMap();
	for (const QByteArray& key : changeMap.keys()){
		QByteArray value = changeSet.GetChangeInfo(key).toByteArray();
		changeObj.insert(key, QJsonValue(QString(value)));
	}

	QJsonObject rootObject;
	rootObject["ids"] = idsArray;
	rootObject["infoMap"] = changeObj;
	rootObject["description"] = changeSet.GetDescription();

	QJsonDocument changeSetDocument;
	changeSetDocument.setObject(rootObject);

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequest = requestNetworks.networkRequests[id];
			QByteArray body = QString(R"({"type": "data", "id": "%1","payload":{"data": {"token": "%2", "changeSet": %3}}})")
								.arg(QString(id))
								.arg(QString(sessionId))
								.arg(qPrintable(changeSetDocument.toJson(QJsonDocument::Compact))).toUtf8();
			QByteArray responseTypeId("application/json; charset=utf-8");
			const imtrest::IProtocolEngine& engine = networkRequest->GetProtocolEngine();

			imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequest, imtrest::IProtocolEngine::SC_OK, body, responseTypeId).PopInterfacePtr());
			if (responsePtr.IsValid()){
				const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequest->GetRequestId());
				if (sender != nullptr){
					sender->SendResponse(responsePtr);
				}
			}
		}
	}
}


} // namespace imtauthgql


