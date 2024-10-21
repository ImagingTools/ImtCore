#include <imtgql/CObjectCollectionSubscriberControllerComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>

// ImtCore includes
#include<imtrest/IProtocolEngine.h>


namespace imtgql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CObjectCollectionSubscriberControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_objectCollectionModelCompPtr.IsValid()) {
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
}


void CObjectCollectionSubscriberControllerComp::OnComponentDestroyed()
{
	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CObjectCollectionSubscriberControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
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

			if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
				itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
				dataObject.insert("typeOperation", "inserted");
			}
			else if (changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
				itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_REMOVED).toByteArray();
				dataObject.insert("typeOperation", "removed");
			}
			else if (changeSet.Contains(imtbase::ICollectionInfo::CF_UPDATED)){
				itemId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_UPDATED).toByteArray();
				dataObject.insert("typeOperation", "updated");
			}
			dataObject.insert("itemId", QString(itemId));

			if (itemId.isEmpty() && m_isSendItemSource.IsValid() && m_objectCollectionCompPtr.IsValid() && *m_isSendItemSource == true){
				imtbase::IObjectCollection::DataPtr dataPtr;
				m_objectCollectionCompPtr->GetObjectData(itemId, dataPtr);
				QByteArray representationData;
				iser::ISerializable* objectPtr = dynamic_cast<iser::ISerializable*>(dataPtr.GetPtr());
				if (objectPtr != nullptr){
					iser::CJsonMemWriteArchive archive(representationData);
					objectPtr->Serialize(archive);
				}
				dataObject.insert("item", QString(representationData));
			}

			QJsonDocument jsonDocument;
			jsonDocument.setObject(dataObject);
			data = jsonDocument.toJson(QJsonDocument::Compact);

			SetData(id, networkRequest->GetCommandId(), data, *networkRequest);
		}
	}
}


bool CObjectCollectionSubscriberControllerComp::SetSubscriptions()
{
	return false;
}


} // namespace imtgql


