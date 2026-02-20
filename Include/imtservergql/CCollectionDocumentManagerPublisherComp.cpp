// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionDocumentManagerPublisherComp.h>


// ImtCore includes
#include <imtdoc/CDocumentChangedEvent.h>
#include <imtdoc/CDocumentClosedEvent.h>
#include <imtdoc/CDocumentCreatedEvent.h>
#include <imtdoc/CDocumentOpenedEvent.h>
#include <imtdoc/CDocumentRenamedEvent.h>
#include <imtdoc/CDocumentSavedAsEvent.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtdoc/CDocumentUndoRedoChangedEvent.h>


namespace imtservergql
{


namespace CDM = sdl::imtbase::CollectionDocumentManager;


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


// reimplemented (imtdoc::IDocumentManagerEventHandler)

bool CCollectionDocumentManagerPublisherComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
{
	eventPtr->Accept();

	bool retVal = false;

	retVal = retVal || OnDocumentCreated(eventPtr);
	retVal = retVal || OnDocumentOpened(eventPtr);
	retVal = retVal || OnDocumentRenamed(eventPtr);
	retVal = retVal || OnDocumentChanged(eventPtr);
	retVal = retVal || OnDocumentUndoRedoChanged(eventPtr);
	retVal = retVal || OnDocumentSaved(eventPtr);
	retVal = retVal || OnDocumentSavedAs(eventPtr);
	retVal = retVal || OnDocumentClosed(eventPtr);

	return retVal;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentCreated(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentCreatedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentCreatedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentManager::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0 sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::NewDocumentCreated, sdlNotification);

	PublishRepresentation(GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentOpened(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentOpenedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentOpenedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentManager::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0 sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentOpened, sdlNotification);

	PublishRepresentation(GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentRenamed(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentRenamedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentRenamedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentManager::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0 sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentRenamed, sdlNotification);

	PublishRepresentation(GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentChanged(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentChangedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentChangedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentManager::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0 sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentChanged, sdlNotification);

	PublishRepresentation(GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentUndoRedoChanged(
	imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentUndoRedoChangedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentUndoRedoChangedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	sdl::imtbase::UndoManager::CUndoInfo::V1_0 sdlNotification;

	sdlNotification.documentId = concreteEventPtr->GetDocumentId();
	sdlNotification.status.emplace().status = sdl::imtbase::UndoManager::EUndoStatus::Success;
	sdlNotification.isDirty = concreteEventPtr->IsDocumentDirty();

	const idoc::IUndoManager& undoManager = concreteEventPtr->GetUndoManager();
	int availableUndoSteps = undoManager.GetAvailableUndoSteps();
	int availableRedoSteps = undoManager.GetAvailableRedoSteps();

	sdlNotification.availableUndoSteps = availableUndoSteps;
	sdlNotification.availableRedoSteps = availableRedoSteps;
	sdlNotification.undoLevelDescriptions.emplace();
	sdlNotification.redoLevelDescriptions.emplace();
	for (int i = 0; i < availableUndoSteps; i++){
		sdlNotification.undoLevelDescriptions->append(undoManager.GetUndoLevelDescription(i));
	}
	for (int i = 0; i < availableRedoSteps; i++){
		sdlNotification.redoLevelDescriptions->append(undoManager.GetRedoLevelDescription(i));
	}

	PublishRepresentation(
		QByteArrayLiteral("On") + *m_collectionIdAttrPtr + QByteArrayLiteral("UndoChanged"),
		concreteEventPtr->GetUserId(),
		sdlNotification);

	return true;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentSaved(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentSavedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentSavedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentManager::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0 sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentSaved, sdlNotification);

	PublishRepresentation(GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentSavedAs(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentSavedAsEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentSavedAsEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentManager::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0 sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentSavedAs, sdlNotification);

	PublishRepresentation(GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentManagerPublisherComp::OnDocumentClosed(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentClosedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentClosedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0 sdlNotification;
	sdlNotification.documentOperation = CDM::EDocumentOperation::DocumentClosed;
	sdlNotification.documentId = concreteEventPtr->GetDocumentId();
	sdlNotification.documentName.emplace();

	PublishRepresentation(GetCommandId(), concreteEventPtr->GetUserId(), sdlNotification);

	return true;
}


void CCollectionDocumentManagerPublisherComp::FillDocumentNotification(
	const imtdoc::CEventBase* eventPtr,
	imtdoc::IDocumentManager::DocumentNotification& notification) const
{
	notification.userId = eventPtr->GetUserId();
	notification.documentId = eventPtr->GetDocumentId();
	notification.typeId = eventPtr->GetDocumentTypeId();
	notification.url = eventPtr->GetDocumentUrl();
	notification.name = eventPtr->GetDocumentName();
	notification.isDirty = eventPtr->IsDocumentDirty();
}


void CCollectionDocumentManagerPublisherComp::FillSdlNotification(
	const imtdoc::IDocumentManager::DocumentNotification& notification,
	sdl::imtbase::CollectionDocumentManager::EDocumentOperation operation,
	sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0& sdlNotification) const
{
	sdlNotification.documentOperation = operation;
	sdlNotification.documentId = notification.documentId;
	sdlNotification.documentName = notification.name;
	sdlNotification.objectId = ConvertUrlToObjectId(notification.url);
	sdlNotification.isDirty = notification.isDirty;
}


QByteArray CCollectionDocumentManagerPublisherComp::ConvertUrlToObjectId(const QUrl& url) const
{
	QString path = url.path();

	QStringList parts = path.split('/', Qt::SkipEmptyParts);
	if (parts.count() == 1){
		return parts.first().toUtf8();
	}

	return QByteArray();
}


QByteArray CCollectionDocumentManagerPublisherComp::GetCommandId() const
{
	return QByteArrayLiteral("On") + *m_collectionIdAttrPtr + QByteArrayLiteral("DocumentChanged");
}


} // namespace imtservergql

