// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CObjectCollectionChangeNotifierComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CObjectCollectionChangeNotifierComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = false;
	if (m_collectionIdAttrPtr.IsValid()){
		QByteArray collectionId = *m_collectionIdAttrPtr;
		QByteArray gqlCommandId = gqlRequest.GetCommandId();
		
		isSupported = gqlCommandId == QByteArrayLiteral("On") + collectionId + QByteArrayLiteral("CollectionChanged");
	}

	if (isSupported){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CObjectCollectionChangeNotifierComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
}


void CObjectCollectionChangeNotifierComp::OnComponentDestroyed()
{
	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CObjectCollectionChangeNotifierComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return;
	}

	QByteArray collectionChangedCommandId;
	if (m_collectionIdAttrPtr.IsValid() && !(*m_collectionIdAttrPtr).isEmpty()){
		collectionChangedCommandId = QByteArrayLiteral("On") + *m_collectionIdAttrPtr + QByteArrayLiteral("CollectionChanged");
	}

	if (collectionChangedCommandId.isEmpty() && m_commandIdsAttrPtr.GetCount() <= 0){
		return;
	}

	QByteArray itemId;
	QJsonObject dataObject;
	bool isRemoved = false;
	bool recognized = false;

	if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
		itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
		dataObject.insert("typeOperation", "inserted");
		recognized = true;
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
		QVariant changeInfo = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED);
		if (changeInfo.isValid()){
			imtbase::ICollectionInfo::MultiElementNotifierInfo info = changeInfo.value<imtbase::ICollectionInfo::MultiElementNotifierInfo>();

			QJsonArray removedItemsArray;
			for (const QByteArray& elementId: info.elementIds){
				removedItemsArray.append(QJsonValue(QString(elementId)));
			}

			dataObject.insert("itemIds", removedItemsArray);
		}
		dataObject.insert("typeOperation", "removed");
		isRemoved = true;
		recognized = true;
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_DESCRIPTION_CHANGED)){
		itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_DESCRIPTION_CHANGED).toByteArray();
		dataObject.insert("typeOperation", "updated");
		recognized = true;
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_RENAMED)){
		itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_RENAMED).toByteArray();
		dataObject.insert("typeOperation", "updated");
		recognized = true;
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_STATE)){
		itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_STATE).toByteArray();
		dataObject.insert("typeOperation", "updated");
		recognized = true;
	}
	else if (changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED)){
		itemId = changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray();
		dataObject.insert("typeOperation", "updated");
		recognized = true;
	}

	if (!recognized){
		return;
	}

	QVariant operationContext = changeSet.GetChangeInfo(imtbase::IOperationContext::OPERATION_CONTEXT_INFO);
	if (operationContext.isValid()){
		QJsonObject operationContextObject;

		imtbase::IOperationContext::OperationContextInfo info = operationContext.value<imtbase::IOperationContext::OperationContextInfo>();
		operationContextObject.insert("ownerId", QString(info.id));
		operationContextObject.insert("ownerName", info.name);

		dataObject.insert("operationContext", operationContextObject);
	}

	if (!isRemoved){
		dataObject.insert("itemId", QString(itemId));

		// Send serialized item payload only when itemId is valid.
		if (!itemId.isEmpty() && m_isSendItemSource.IsValid() && m_objectCollectionCompPtr.IsValid() && *m_isSendItemSource){
			imtbase::IObjectCollection::DataPtr dataPtr;
			m_objectCollectionCompPtr->GetObjectData(itemId, dataPtr);
			QByteArray representationData;
			iser::ISerializable* objectPtr = dynamic_cast<iser::ISerializable*>(dataPtr.GetPtr());
			if (objectPtr != nullptr){
				iser::CJsonMemWriteArchive archive;

				if (objectPtr->Serialize(archive)){
					representationData = archive.GetData();
				}
				else{
					Q_ASSERT(false);
				}
			}
			dataObject.insert("item", QString(representationData));
		}
	}

	QJsonDocument jsonDocument;
	jsonDocument.setObject(dataObject);
	const QByteArray data = jsonDocument.toJson(QJsonDocument::Compact);

	if (!collectionChangedCommandId.isEmpty()){
		PublishData(collectionChangedCommandId, data);
	}

	for (int index = 0; index < m_commandIdsAttrPtr.GetCount(); ++index){
		const QByteArray& commandId = m_commandIdsAttrPtr[index];
		if (commandId != collectionChangedCommandId){
			PublishData(commandId, data);
		}
	}
}


} // namespace imtservergql

