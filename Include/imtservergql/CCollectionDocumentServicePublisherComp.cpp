// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionDocumentServicePublisherComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPair>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlParamObject.h>
#include <imtdoc/CDocumentChangedEvent.h>
#include <imtdoc/CDocumentClosedEvent.h>
#include <imtdoc/CDocumentCreatedEvent.h>
#include <imtdoc/CDocumentDataLoadedEvent.h>
#include <imtdoc/CDocumentOpenedEvent.h>
#include <imtdoc/CDocumentRenamedEvent.h>
#include <imtdoc/CDocumentSavedAsEvent.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtdoc/CDocumentUndoRedoChangedEvent.h>


namespace imtservergql
{


namespace CDM = sdl::V1_0::imtbase;


// public methods

// reimplemented (icomp::CComponentBase)

void CCollectionDocumentServicePublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QObject::connect(
				&m_closeIdleDocumentsTimer,
				&QTimer::timeout,
				[this]{ CloseIdleDocuments(); });
	m_closeIdleDocumentsTimer.start(1000);
}


void CCollectionDocumentServicePublisherComp::OnComponentDestroyed()
{
	m_closeIdleDocumentsTimer.stop();
	m_closeIdleDocumentsTimer.disconnect();

	BaseClass::OnComponentDestroyed();
}


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CCollectionDocumentServicePublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return BaseClass::IsRequestSupported(gqlRequest);
	}

	QByteArray collectionId = *m_collectionIdAttrPtr;
	QByteArray gqlCommandId = gqlRequest.GetCommandId();

	if (gqlCommandId == sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId() ||
		gqlCommandId == sdl::V1_0::imtbase::COnUndoRedoChangedGqlRequest::GetCommandId()){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		QByteArray requestCollectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();

		return requestCollectionId == collectionId;
	}

	if (gqlCommandId == sdl::V1_0::imtbase::COnDocumentChangedGqlRequest::GetCommandId()){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		QByteArray requestCollectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
		QByteArray documentId = inputParamPtr->GetParamArgumentValue("id").toByteArray();

		return requestCollectionId == collectionId && !documentId.isEmpty();
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (imtdoc::IDocumentServiceEventHandler)

bool CCollectionDocumentServicePublisherComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
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
	retVal = retVal || OnDocumentDataLoaded(eventPtr);

	return retVal;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentCreated(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentCreatedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentCreatedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentService::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::NewDocumentCreated, sdlNotification);

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);

	TrackDocument(notification.userId, notification.documentId);

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentOpened(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentOpenedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentOpenedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentService::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentOpened, sdlNotification);

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);

	TrackDocument(notification.userId, notification.documentId);

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentRenamed(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentRenamedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentRenamedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentService::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentRenamed, sdlNotification);

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentChanged(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentChangedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentChangedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentService::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentChanged, sdlNotification);

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);
	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentUndoRedoChanged(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentUndoRedoChangedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentUndoRedoChangedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	sdl::V1_0::imtbase::CUndoInfo sdlNotification;

	sdlNotification.documentId = concreteEventPtr->GetDocumentId();
	sdlNotification.status.emplace().status = sdl::V1_0::imtbase::EUndoStatus::Success;
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
		sdl::V1_0::imtbase::COnUndoRedoChangedGqlRequest::GetCommandId(),
		concreteEventPtr->GetUserId(),
		sdlNotification);

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentSaved(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentSavedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentSavedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentService::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentSaved, sdlNotification);

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentSavedAs(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentSavedAsEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentSavedAsEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentService::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentSavedAs, sdlNotification);

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentClosed(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentClosedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentClosedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	sdlNotification.documentOperation = CDM::EDocumentOperation::DocumentClosed;
	sdlNotification.documentId = concreteEventPtr->GetDocumentId();
	sdlNotification.documentName.emplace();

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), concreteEventPtr->GetUserId(), sdlNotification);

	UntrackDocument(concreteEventPtr->GetDocumentId());

	return true;
}


bool CCollectionDocumentServicePublisherComp::OnDocumentDataLoaded(imtdoc::CEventBase* eventPtr) const
{
	imtdoc::CDocumentDataLoadedEvent* concreteEventPtr = dynamic_cast<imtdoc::CDocumentDataLoadedEvent*>(eventPtr);
	if (concreteEventPtr == nullptr){
		return false;
	}

	imtdoc::IDocumentService::DocumentNotification notification;
	FillDocumentNotification(concreteEventPtr, notification);

	sdl::V1_0::imtbase::CDocumentServiceNotification sdlNotification;
	FillSdlNotification(notification, CDM::EDocumentOperation::DocumentDataLoaded, sdlNotification);

	PublishRepresentation(sdl::V1_0::imtbase::COnDocumentManagerChangedGqlRequest::GetCommandId(), notification.userId, sdlNotification);

	return true;
}


void CCollectionDocumentServicePublisherComp::FillDocumentNotification(
			const imtdoc::CEventBase* eventPtr,
			imtdoc::IDocumentService::DocumentNotification& notification) const
{
	notification.userId = eventPtr->GetUserId();
	notification.documentId = eventPtr->GetDocumentId();
	notification.typeId = eventPtr->GetDocumentTypeId();
	notification.url = eventPtr->GetDocumentUrl();
	notification.name = eventPtr->GetDocumentName();
	notification.isDirty = eventPtr->IsDocumentDirty();
}


void CCollectionDocumentServicePublisherComp::FillSdlNotification(
			const imtdoc::IDocumentService::DocumentNotification& notification,
			sdl::V1_0::imtbase::EDocumentOperation operation,
			sdl::V1_0::imtbase::CDocumentServiceNotification& sdlNotification) const
{
	sdlNotification.documentOperation = operation;
	sdlNotification.documentId = notification.documentId;
	sdlNotification.documentName = notification.name;
	sdlNotification.objectId = ConvertUrlToObjectId(notification.url);
	sdlNotification.isDirty = notification.isDirty;
}


QByteArray CCollectionDocumentServicePublisherComp::ConvertUrlToObjectId(const QUrl& url) const
{
	QString path = url.path();

	QStringList parts = path.split('/', Qt::SkipEmptyParts);
	if (parts.count() == 1){
		return parts.first().toUtf8();
	}

	return QByteArray();
}


void CCollectionDocumentServicePublisherComp::TrackDocument(
			const QByteArray& userId,
			const QByteArray& documentId) const
{
	if (documentId.isEmpty()){
		return;
	}

	QMutexLocker locker(&m_trackedDocumentsMutex);

	TrackedDocument& trackedDocument = m_trackedDocuments[documentId];
	trackedDocument.userId = userId;
	trackedDocument.lastSubscriberSeenMs = QDateTime::currentMSecsSinceEpoch();
}


void CCollectionDocumentServicePublisherComp::UntrackDocument(const QByteArray& documentId) const
{
	QMutexLocker locker(&m_trackedDocumentsMutex);

	m_trackedDocuments.remove(documentId);
}


bool CCollectionDocumentServicePublisherComp::HasActiveSingleDocumentChangedSubscriber(const QByteArray& documentId) const
{
	const QByteArray commandId = sdl::V1_0::imtbase::COnDocumentChangedGqlRequest::GetCommandId();

	QMutexLocker locker(&m_mutex);

	for (const RequestNetworks& entry : m_registeredSubscribers){
		if (entry.gqlRequest.GetCommandId() != commandId){
			continue;
		}

		if (entry.networkRequests.isEmpty()){
			continue;
		}

		const imtgql::CGqlParamObject* inputParamPtr = entry.gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			continue;
		}

		if (inputParamPtr->GetParamArgumentValue("id").toByteArray() == documentId){
			return true;
		}
	}

	return false;
}


void CCollectionDocumentServicePublisherComp::CloseIdleDocuments()
{
	if (!m_documentServiceCompPtr.IsValid()){
		return;
	}

	const qint64 timeout = *m_closeDocumentTimeoutAttrPtr;
	const qint64 now = QDateTime::currentMSecsSinceEpoch();

	QList<QPair<QByteArray, QByteArray> > documentsToClose; // userId, documentId

	{
		QMutexLocker locker(&m_trackedDocumentsMutex);

		for (auto it = m_trackedDocuments.begin(); it != m_trackedDocuments.end(); ){
			if (HasActiveSingleDocumentChangedSubscriber(it.key())){
				it.value().lastSubscriberSeenMs = now;
				++it;

				continue;
			}

			if ((now - it.value().lastSubscriberSeenMs) < timeout){
				++it;

				continue;
			}

			documentsToClose.append(qMakePair(it.value().userId, it.key()));
			it = m_trackedDocuments.erase(it);
		}
	}

	if (documentsToClose.isEmpty()){
		return;
	}

	for (const QPair<QByteArray, QByteArray>& document : documentsToClose){
		imtdoc::IDocumentService::TaskParams taskParams;
		taskParams.userId = document.first;
		taskParams.documentId = document.second;

		QByteArray taskId = m_documentServiceCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, taskParams);
		m_documentServiceCompPtr->WaitForTaskFinished(taskId);
	}
}


} // namespace imtservergql
