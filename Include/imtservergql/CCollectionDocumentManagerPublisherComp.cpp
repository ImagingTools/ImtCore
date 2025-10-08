#include <imtservergql/CCollectionDocumentManagerPublisherComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CCollectionDocumentManagerPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = false;
	if (m_collectionIdAttrPtr.IsValid()){
		QByteArray collectionId = *m_collectionIdAttrPtr;
		QByteArray gqlCommandId = gqlRequest.GetCommandId();

		isSupported = gqlCommandId == QByteArrayLiteral("On") + collectionId + QByteArrayLiteral("DocumentChanged");
	}

	if (isSupported){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (iomod::CSingleModelObserverBase)

void CCollectionDocumentManagerPublisherComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	QVariant varChanged;
	QVariant varClosed;

	const istd::IChangeable::ChangeInfoMap& map = changeSet.GetChangeInfoMap();
	Q_ASSERT(map.count() <= 1);

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification sdlNotification;
	sdlNotification.Version_1_0.emplace();
	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0& sdlNotificationV1 = *sdlNotification.Version_1_0;

	if (map.contains(imtdoc::ICollectionDocumentManager::CN_NEW_DOCUMENT_CREATED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_NEW_DOCUMENT_CREATED).size() == 1);
		varChanged = map.value(imtdoc::ICollectionDocumentManager::CN_NEW_DOCUMENT_CREATED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::NewDocumentCreated;
	}
	else if (map.contains(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_OPENED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_OPENED).size() == 1);
		varChanged = map.value(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_OPENED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentOpened;
	}
	else if (map.contains(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CHANGED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CHANGED).size() == 1);
		varChanged = map.value(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CHANGED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentChanged;
	}
	else if (map.contains(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CLOSED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CLOSED).size() == 1);
		varClosed = map.value(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CLOSED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentClosed;
	}
	else{
		return;
	}

	if (varChanged.canConvert<imtdoc::ICollectionDocumentManager::DocumentNotification>()){
		imtdoc::ICollectionDocumentManager::DocumentNotification notification =
			varChanged.value<imtdoc::ICollectionDocumentManager::DocumentNotification>();

		sdlNotificationV1.documentId = notification.documentId;
		sdlNotificationV1.objectId = notification.objectId;
		sdlNotificationV1.hasChanges = notification.hasChanges;
	}
	else if (varClosed.metaType().id() == QMetaType::QByteArray){
		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentClosed;
		sdlNotificationV1.documentId = varClosed.toByteArray();
	}
	else{
		return;
	}

	QJsonObject jsonObject;
	if (!sdlNotification.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);
	}

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);

	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

	for (const RequestNetworks& networkRequest : m_registeredSubscribers){
		QByteArray commandId = networkRequest.gqlRequest.GetCommandId();

		PublishData(commandId, data);
	}
}


} // namespace imtservergql

