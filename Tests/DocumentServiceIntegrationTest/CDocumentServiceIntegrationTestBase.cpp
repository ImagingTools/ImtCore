// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDocumentServiceIntegrationTestBase.h"


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDeadlineTimer>

// ACF includes
#include <imod/IModel.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace
{

const int WAIT_TIMEOUT = 5000;

}


// public methods of the class CRecordingEventHandler

QList<CRecordingEventHandler::RecordedEvent> CRecordingEventHandler::GetEvents() const
{
	QMutexLocker locker(&m_mutex);

	return m_events;
}


void CRecordingEventHandler::ClearEvents()
{
	QMutexLocker locker(&m_mutex);

	m_events.clear();
}


int CRecordingEventHandler::CountEventsOfType(const QByteArray& type) const
{
	QMutexLocker locker(&m_mutex);

	int count = 0;
	for (const RecordedEvent& event : m_events){
		if (event.type == type){
			++count;
		}
	}

	return count;
}


bool CRecordingEventHandler::HasEvent(const QByteArray& type, const QByteArray& userId) const
{
	QMutexLocker locker(&m_mutex);

	for (const RecordedEvent& event : m_events){
		if ((event.type == type) && (event.userId == userId)){
			return true;
		}
	}

	return false;
}


// reimplemented (imtdoc::IDocumentServiceEventHandler)

bool CRecordingEventHandler::ProcessEvent(imtdoc::CEventBase* eventPtr)
{
	if (eventPtr == nullptr){
		return false;
	}

	RecordedEvent event;
	event.type = eventPtr->Type();
	event.userId = eventPtr->GetUserId();
	event.documentId = eventPtr->GetDocumentId();
	event.documentTypeId = eventPtr->GetDocumentTypeId();
	event.documentName = eventPtr->GetDocumentName();
	event.documentUrl = eventPtr->GetDocumentUrl();
	event.isDirty = eventPtr->IsDocumentDirty();

	QMutexLocker locker(&m_mutex);

	m_events.append(event);

	return true;
}


// public methods of the class CTestObserver

CTestObserver::~CTestObserver()
{
	const QList<imod::IModel*> models = m_models;
	for (imod::IModel* modelPtr : models){
		modelPtr->DetachObserver(this);
	}
}


// reimplemented (imod::IObserver)

bool CTestObserver::IsModelAttached(const imod::IModel* modelPtr) const
{
	if (modelPtr == nullptr){
		return !m_models.isEmpty();
	}

	for (const imod::IModel* attachedModelPtr : m_models){
		if (attachedModelPtr == modelPtr){
			return true;
		}
	}

	return false;
}


bool CTestObserver::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	changeMask += istd::IChangeable::CF_ANY;

	m_models.append(modelPtr);

	return true;
}


bool CTestObserver::OnModelDetached(imod::IModel* modelPtr)
{
	return m_models.removeOne(modelPtr);
}


void CTestObserver::BeforeUpdate(imod::IModel* /*modelPtr*/)
{
}


void CTestObserver::AfterUpdate(imod::IModel* /*modelPtr*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
}


// protected methods of the class CDocumentServiceIntegrationTestBase

QByteArray CDocumentServiceIntegrationTestBase::CreateDocumentAndWaitForLoad(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& typeId,
			const QString& name)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.documentTypeId = typeId;
	params.documentName = name;

	QByteArray documentId;
	if (ExecuteTask(service, imtdoc::IDocumentService::TT_NEW, params, &documentId) != imtdoc::IDocumentService::OS_OK){
		return QByteArray();
	}

	if (!WaitForDocumentLoaded(service, userId, documentId)){
		return QByteArray();
	}

	return documentId;
}


bool CDocumentServiceIntegrationTestBase::WaitForDocumentLoaded(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& documentId)
{
	if (documentId.isEmpty()){
		return false;
	}

	QDeadlineTimer deadline(WAIT_TIMEOUT);
	while (!deadline.hasExpired()){
		bool isFound = false;
		for (const imtdoc::IDocumentService::DocumentListItem& item : service.GetOpenedDocumentList(userId)){
			if (item.documentId != documentId){
				continue;
			}

			isFound = true;
			if (!item.isLoading){
				return true;
			}
		}

		// Document was closed (e.g. data loading failed)
		if (!isFound){
			return false;
		}

		QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
	}

	return false;
}


imtdoc::IDocumentService::OperationStatus CDocumentServiceIntegrationTestBase::ExecuteTask(
			imtdoc::IDocumentService& service,
			imtdoc::IDocumentService::TaskType taskType,
			const imtdoc::IDocumentService::TaskParams& params,
			QByteArray* documentIdPtr)
{
	imtdoc::IDocumentService::Error error;
	QByteArray taskId = service.BeginDocumentTask(taskType, params, &error);
	if (taskId.isEmpty()){
		return error.HasError() ? error.status : imtdoc::IDocumentService::OS_FAILED;
	}

	imtdoc::IDocumentService::TaskResult result = service.WaitForTaskFinished(taskId);
	if (documentIdPtr != nullptr){
		*documentIdPtr = result.documentId;
	}

	return result.status;
}


imtdoc::IDocumentService::OperationStatus CDocumentServiceIntegrationTestBase::CloseDocument(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& documentId)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.documentId = documentId;

	return ExecuteTask(service, imtdoc::IDocumentService::TT_CLOSE, params);
}


imtdoc::IDocumentService::OperationStatus CDocumentServiceIntegrationTestBase::SaveDocument(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& documentId,
			const QString& documentName)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.documentId = documentId;
	params.documentName = documentName;

	return ExecuteTask(service, imtdoc::IDocumentService::TT_SAVE, params);
}


imtdoc::IDocumentService::DocumentListItem CDocumentServiceIntegrationTestBase::FindDocumentInfo(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& documentId) const
{
	for (const imtdoc::IDocumentService::DocumentListItem& item : service.GetOpenedDocumentList(userId)){
		if (item.documentId == documentId){
			return item;
		}
	}

	return imtdoc::IDocumentService::DocumentListItem();
}


QString CDocumentServiceIntegrationTestBase::GetDocumentText(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& documentId) const
{
	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(service.GetDocumentPtr(userId, documentId));
	if (textPtr == nullptr){
		return QString();
	}

	return textPtr->GetText();
}


imtdoc::IDocumentService::OperationStatus CDocumentServiceIntegrationTestBase::SetDocumentText(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& documentId,
			const QString& text)
{
	iprm::CTextParam data;
	data.SetText(text);

	return service.SetDocumentData(userId, documentId, data);
}


