// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CObjectCollectionSubscriberComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CObjectCollectionSubscriberComp::OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData)
{
	if (!m_subscriptionIds.contains(subscriptionId)){
		SendErrorMessage(0, QString("Unable to response received for subscription ID '%1'").arg(qPrintable(subscriptionId)), "CObjectCollectionSubscriberComp");
		return;
	}

	istd::IChangeable::ChangeSet changeSet;
	imtbase::ICollectionInfo::NotifierInfo notifierInfo;

	QJsonDocument document = QJsonDocument::fromJson(subscriptionData);
	QJsonObject subscriptionObject = document.object();

	if (subscriptionObject.contains("data")){
		subscriptionObject = subscriptionObject.value("data").toObject();
	}

	QByteArray commandId = GetCommandId(subscriptionId);
	if (commandId.isEmpty()){
		return;
	}

	if (subscriptionObject.contains(commandId)){
		subscriptionObject = subscriptionObject.value(commandId).toObject();
	}

	QByteArray itemId;
	if (subscriptionObject.contains("itemId")){
		itemId = subscriptionObject.value("itemId").toString().toUtf8();
	}

	if (subscriptionObject.contains("operationContext")){
		imtbase::IOperationContext::OperationContextInfo info;

		QJsonObject operationContextObj = subscriptionObject.value("operationContext").toObject();
		if (operationContextObj.contains("ownerId")){
			QString ownerId = operationContextObj.value("ownerId").toString();
			info.id = ownerId.toUtf8();
		}

		if (operationContextObj.contains("ownerName")){
			QString ownerName = operationContextObj.value("ownerName").toString();
			info.name = ownerName;
		}

		changeSet.SetChangeInfo(
			imtbase::IOperationContext::OPERATION_CONTEXT_INFO,
			QVariant::fromValue<imtbase::IOperationContext::OperationContextInfo>(info));
	}

	if (subscriptionObject.contains("typeOperation")){
		QByteArray typeOperation = subscriptionObject.value("typeOperation").toString().toUtf8();
		if (typeOperation == "inserted"){
			changeSet += imtbase::ICollectionInfo::CF_ADDED;
			changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_ELEMENT_INSERTED, itemId);
		}
		else if (typeOperation == "removed"){
			if (subscriptionObject.contains("itemIds")){
				QJsonArray itemIdsArr = subscriptionObject.value("itemIds").toArray();

				imtbase::ICollectionInfo::MultiElementNotifierInfo multiElementNotifierInfo;
				for (const auto& itemIdValue : itemIdsArr){
					multiElementNotifierInfo.elementIds.append(itemIdValue.toString().toUtf8());
				}

				changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_ELEMENTS_REMOVED, QVariant::fromValue(multiElementNotifierInfo));
			}

			changeSet += imtbase::ICollectionInfo::CF_REMOVED;
		}
		else if (typeOperation == "updated"){
			changeSet += imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED;
			changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED, itemId);
		}
	}

	if (m_objectCollectionCompPtr.IsValid()){
		if (changeSet.IsEmpty()){
			changeSet = istd::IChangeable::GetAnyChange();
		}

		istd::CChangeNotifier notifier(m_objectCollectionCompPtr.GetPtr(), &changeSet);
	}
}


} // namespace imtclientgql


