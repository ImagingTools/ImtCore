// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CObjectCollectionChangeNotifierComp.h>


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

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequest = requestNetworks.networkRequests[id];
			QByteArray data;
			QByteArray itemId;
			QJsonObject dataObject;
			
			bool isRemoved = false;

			if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
				itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
				dataObject.insert("typeOperation", "inserted");
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
			}
			else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_DESCRIPTION_CHANGED)){
				itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_DESCRIPTION_CHANGED).toByteArray();

				dataObject.insert("typeOperation", "updated");
			}
			else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_RENAMED)){
				itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_RENAMED).toByteArray();

				dataObject.insert("typeOperation", "updated");
			}
			else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_STATE)){
				itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_STATE).toByteArray();

				dataObject.insert("typeOperation", "updated");
			}
			else if (changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED)){
				itemId = changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray();

				dataObject.insert("typeOperation", "updated");
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
				
				if (itemId.isEmpty() && m_isSendItemSource.IsValid() && m_objectCollectionCompPtr.IsValid() && *m_isSendItemSource){
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
			data = jsonDocument.toJson(QJsonDocument::Compact);

			PushDataToSubscriber(id, networkRequest->GetCommandId(), data, *networkRequest);
		}
	}
}


} // namespace imtservergql


