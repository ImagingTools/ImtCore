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
		isSupported = isSupported || gqlCommandId == QByteArrayLiteral("On") + collectionId + QByteArrayLiteral("UndoChanged");
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

	if (map.contains(imtdoc::IDocumentManager::CN_NEW_DOCUMENT_CREATED)){
		Q_ASSERT(map.values(imtdoc::IDocumentManager::CN_NEW_DOCUMENT_CREATED).size() == 1);
		varChanged = map.value(imtdoc::IDocumentManager::CN_NEW_DOCUMENT_CREATED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::NewDocumentCreated;
	}
	else if (map.contains(imtdoc::IDocumentManager::CN_DOCUMENT_OPENED)){
		Q_ASSERT(map.values(imtdoc::IDocumentManager::CN_DOCUMENT_OPENED).size() == 1);
		varChanged = map.value(imtdoc::IDocumentManager::CN_DOCUMENT_OPENED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentOpened;
	}
	else if (map.contains(imtdoc::IDocumentManager::CN_DOCUMENT_CHANGED)){
		Q_ASSERT(map.values(imtdoc::IDocumentManager::CN_DOCUMENT_CHANGED).size() == 1);
		varChanged = map.value(imtdoc::IDocumentManager::CN_DOCUMENT_CHANGED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentChanged;
	}
	else if (map.contains(imtdoc::IDocumentManager::CN_DOCUMENT_UNDO_CHANGED)){
		Q_ASSERT(map.values(imtdoc::IDocumentManager::CN_DOCUMENT_UNDO_CHANGED).size() == 1);
		varChanged = map.value(imtdoc::IDocumentManager::CN_DOCUMENT_UNDO_CHANGED);

		if (!varChanged.canConvert<imtdoc::IDocumentManager::DocumentUndoNotification>()){
			Q_ASSERT(false);

			return;
		}

		imtdoc::IDocumentManager::DocumentUndoNotification notification =
			varChanged.value<imtdoc::IDocumentManager::DocumentUndoNotification>();

		sdl::imtbase::UndoManager::CUndoInfo sdlNotification;
		sdl::imtbase::UndoManager::CUndoInfo::V1_0& sdlNotificationV1 = sdlNotification.Version_1_0.emplace();

		sdlNotificationV1.documentId = notification.documentId;
		sdlNotificationV1.status.emplace().status = sdl::imtbase::UndoManager::EUndoStatus::Success;
		sdlNotificationV1.isDirty = notification.isDirty;
		sdlNotificationV1.availableUndoSteps = notification.availableUndoSteps;
		sdlNotificationV1.availableRedoSteps = notification.availableRedoSteps;
		sdlNotificationV1.undoLevelDescriptions.emplace();
		sdlNotificationV1.redoLevelDescriptions.emplace();
		for (int i = 0; i < notification.undoLevelDescriptions.count(); i++){
			sdlNotificationV1.undoLevelDescriptions->append(notification.undoLevelDescriptions[i]);
		}
		for (int i = 0; i < notification.redoLevelDescriptions.count(); i++){
			sdlNotificationV1.redoLevelDescriptions->append(notification.redoLevelDescriptions[i]);
		}

		PublishRepresentation(
			QByteArrayLiteral("On") + *m_collectionIdAttrPtr + QByteArrayLiteral("UndoChanged"),
			notification.userId,
			sdlNotification);

		return;
	}
	else if (map.contains(imtdoc::IDocumentManager::CN_DOCUMENT_SAVED)){
		Q_ASSERT(map.values(imtdoc::IDocumentManager::CN_DOCUMENT_SAVED).size() == 1);
		varChanged = map.value(imtdoc::IDocumentManager::CN_DOCUMENT_SAVED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentSaved;
	}
	else if (map.contains(imtdoc::IDocumentManager::CN_DOCUMENT_CLOSED)){
		Q_ASSERT(map.values(imtdoc::IDocumentManager::CN_DOCUMENT_CLOSED).size() == 1);
		varClosed = map.value(imtdoc::IDocumentManager::CN_DOCUMENT_CLOSED);

		sdlNotificationV1.documentOperation = sdl::imtbase::CollectionDocumentManager::EDocumentOperation::DocumentClosed;
	}
	else{
		return;
	}

	QByteArray userId;

	if (varChanged.canConvert<imtdoc::IDocumentManager::DocumentNotification>()){
		imtdoc::IDocumentManager::DocumentNotification notification =
			varChanged.value<imtdoc::IDocumentManager::DocumentNotification>();

		userId = notification.userId;

		sdlNotificationV1.documentId = notification.documentId;
		sdlNotificationV1.objectId = notification.objectId;
	}
	else if (varClosed.canConvert<imtdoc::IDocumentManager::DocumentClosedNotification>()){
		imtdoc::IDocumentManager::DocumentClosedNotification notification =
			varClosed.value<imtdoc::IDocumentManager::DocumentClosedNotification>();

		userId = notification.userId;

		sdlNotificationV1.documentId = notification.documentId;
	}
	else{
		return;
	}

	Q_ASSERT(!userId.isEmpty());

	PublishRepresentation(
		QByteArrayLiteral("On") + *m_collectionIdAttrPtr + QByteArrayLiteral("DocumentChanged"),
		userId,
		sdlNotification);
}


} // namespace imtservergql

