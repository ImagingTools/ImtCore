#include <imtservergql/CCollectionDocumentManagerPublisherComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>


namespace imtservergql
{


// protected methods

// reimplemented (iomod::CSingleModelObserverBase)

void CCollectionDocumentManagerPublisherComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	QVariant varChanged;
	QVariant varClosed;

	const istd::IChangeable::ChangeInfoMap& map = changeSet.GetChangeInfoMap();
	Q_ASSERT(map.count() == 1);

	if (map.contains(imtdoc::ICollectionDocumentManager::CN_NEW_DOCUMENT_CREATED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_NEW_DOCUMENT_CREATED) == 1);
		varChanged = map.value(imtdoc::ICollectionDocumentManager::CN_NEW_DOCUMENT_CREATED);
	}
	else if (map.contains(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_OPENED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_OPENED) == 1);
		varChanged = map.value(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_OPENED);
	}
	else if (map.contains(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CHANGED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CHANGED) == 1);
		varChanged = map.value(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CHANGED);
	}
	else if (map.contains(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CLOSED)){
		Q_ASSERT(map.values(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CLOSED) == 1);
		varClosed = map.value(imtdoc::ICollectionDocumentManager::CN_DOCUMENT_CLOSED);
	}
	else{
		return;
	}

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification sdlNotification;
	sdlNotification.Version_1_0.emplace();
	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0& sdlNotificationV1 = *sdlNotification.Version_1_0;

	if (varChanged.canConvert<imtdoc::ICollectionDocumentManager::DocumentNotification>()){
		imtdoc::ICollectionDocumentManager::DocumentNotification notification =
			varChanged.value<imtdoc::ICollectionDocumentManager::DocumentNotification>();

		sdlNotificationV1.documentId = varClosed.toByteArray();
		sdlNotificationV1.objectId = notification.objectId;
		sdlNotificationV1.hasChanges = notification.hasChanges;
		sdlNotificationV1.isClosed = false;

		sdlNotificationV1.undoInfo.emplace();
		sdlNotificationV1.undoInfo->availableUndoSteps = notification.availableUndoSteps;
		sdlNotificationV1.undoInfo->availableRedoSteps = notification.availableRedoSteps;

		sdlNotificationV1.undoInfo->undoLevelDescriptions.emplace();
		for (const QString& description : notification.undoLevelDescriptions){
			sdlNotificationV1.undoInfo->undoLevelDescriptions->append(description);
		}

		sdlNotificationV1.undoInfo->redoLevelDescriptions.emplace();
		for (const QString& description : notification.redoLevelDescriptions){
			sdlNotificationV1.undoInfo->redoLevelDescriptions->append(description);
		}
	}
	else if (varClosed.metaType().id() == QMetaType::QByteArray){
		sdlNotificationV1.documentId = varClosed.toByteArray();
		sdlNotificationV1.isClosed = true;
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

