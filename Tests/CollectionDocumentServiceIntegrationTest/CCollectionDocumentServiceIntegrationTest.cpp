// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCollectionDocumentServiceIntegrationTest.h"


// Qt includes
#include <QtCore/QCoreApplication>

// ACF includes
#include <idoc/IUndoManager.h>
#include <imod/IModel.h>
#include <iprm/CTextParam.h>
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imtdoc/CEventBase.h>
#include <imtdoc/imtdoc.h>


namespace
{

const QByteArray TEST_USER_ID = QByteArrayLiteral("testUser");
const QByteArray TEST_USER_ID_2 = QByteArrayLiteral("testUser2");
const QByteArray TEST_TYPE_ID = QByteArrayLiteral("TestType");
const QByteArray TEST_TYPE_ID_2 = QByteArrayLiteral("TestType2");
const QByteArray VALIDATED_TYPE_ID = QByteArrayLiteral("ValidatedType");
const QByteArray VALIDATED_TYPE_ID_2 = QByteArrayLiteral("ValidatedType2");
const QByteArray TEST_OBJECT_ID = QByteArrayLiteral("testObject123");
const QByteArray TEST_OBJECT_ID_2 = QByteArrayLiteral("testObject456");
const QString TEST_DOC_NAME = QStringLiteral("Test Document");

// Configured in the TextDocumentValidator* and TextDocumentNameProvider registry elements
const QString INVALID_TEXT = QStringLiteral("invalid");
const QString VALIDATION_ERROR_MESSAGE = QStringLiteral("Custom validation error");
const QString DEFAULT_VALIDATION_ERROR_MESSAGE = QStringLiteral("Document data is invalid");
const QString NAME_PREFIX = QStringLiteral("Auto: ");

const QByteArray CREATED_EVENT = QByteArrayLiteral("DocumentCreatedEvent");
const QByteArray OPENED_EVENT = QByteArrayLiteral("DocumentOpenedEvent");
const QByteArray DATA_LOADED_EVENT = QByteArrayLiteral("DocumentDataLoadedEvent");
const QByteArray RENAMED_EVENT = QByteArrayLiteral("DocumentRenamedEvent");
const QByteArray SAVED_EVENT = QByteArrayLiteral("DocumentSavedEvent");
const QByteArray SAVED_AS_EVENT = QByteArrayLiteral("DocumentSavedAsEvent");
const QByteArray CLOSED_EVENT = QByteArrayLiteral("DocumentClosedEvent");

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


// private slots of the class CCollectionDocumentServiceIntegrationTest

// Setup and teardown

void CCollectionDocumentServiceIntegrationTest::init()
{
	const QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	const QString registryFile = imtCorePath + QStringLiteral("/Tests/CollectionDocumentServiceIntegrationTest/Partitura/CollectionDocumentServiceIntegrationTest.acc");
	const QString configFile = imtCorePath + QStringLiteral("/Config/ImtCoreTest.awc");

	m_accessorPtr.SetPtr(new ipackage::CComponentAccessor(registryFile, configFile));

	m_servicePtr = m_accessorPtr->GetComponentInterface<imtdoc::IDocumentService>(QByteArrayLiteral("CollectionDocumentService"));
	m_singleCopyServicePtr = m_accessorPtr->GetComponentInterface<imtdoc::IDocumentService>(QByteArrayLiteral("SingleCopyCollectionDocumentService"));
	m_collectionPtr = m_accessorPtr->GetComponentInterface<imtbase::IObjectCollection>(QByteArrayLiteral("ObjectCollection"));

	QVERIFY2(m_servicePtr != nullptr, "Collection document service should be created");
	QVERIFY2(m_singleCopyServicePtr != nullptr, "Single-copy collection document service should be created");
	QVERIFY2(m_collectionPtr != nullptr, "Object collection should be created");

	m_servicePtr->RegisterEventHandler(m_eventHandler);
	m_singleCopyServicePtr->RegisterEventHandler(m_eventHandler);

	m_eventHandler.ClearEvents();
}


void CCollectionDocumentServiceIntegrationTest::cleanup()
{
	const QList<imtdoc::IDocumentService*> services = {m_servicePtr, m_singleCopyServicePtr};
	for (imtdoc::IDocumentService* servicePtr : services){
		if (servicePtr == nullptr){
			continue;
		}

		for (const QByteArray& userId : {TEST_USER_ID, TEST_USER_ID_2}){
			for (const imtdoc::IDocumentService::DocumentListItem& item : servicePtr->GetOpenedDocumentList(userId)){
				WaitForDocumentLoaded(*servicePtr, userId, item.documentId);
			}
		}

		servicePtr->UnregisterEventHandler(m_eventHandler);
	}

	m_servicePtr = nullptr;
	m_singleCopyServicePtr = nullptr;
	m_collectionPtr = nullptr;

	m_accessorPtr.Reset();

	m_eventHandler.ClearEvents();
}


// private methods

void CCollectionDocumentServiceIntegrationTest::AddCollectionObject(
			const QByteArray& objectId,
			const QByteArray& typeId,
			const QString& name,
			const QString& text)
{
	if (m_collectionPtr->GetElementIds().contains(objectId)){
		return;
	}

	iprm::CTextParam data;
	data.SetText(text);

	m_collectionPtr->InsertNewObject(typeId, name, QString(), &data, objectId);
}


QByteArray CCollectionDocumentServiceIntegrationTest::CreateDocumentAndWaitForLoad(
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


QByteArray CCollectionDocumentServiceIntegrationTest::OpenDocumentAndWaitForLoad(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& objectId)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.url = imtdoc::ObjectIdToUrl(objectId);

	QByteArray documentId;
	if (ExecuteTask(service, imtdoc::IDocumentService::TT_OPEN, params, &documentId) != imtdoc::IDocumentService::OS_OK){
		return QByteArray();
	}

	if (!WaitForDocumentLoaded(service, userId, documentId)){
		return QByteArray();
	}

	return documentId;
}


bool CCollectionDocumentServiceIntegrationTest::WaitForDocumentLoaded(
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


imtdoc::IDocumentService::OperationStatus CCollectionDocumentServiceIntegrationTest::ExecuteTask(
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


imtdoc::IDocumentService::OperationStatus CCollectionDocumentServiceIntegrationTest::CloseDocument(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& documentId)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.documentId = documentId;

	return ExecuteTask(service, imtdoc::IDocumentService::TT_CLOSE, params);
}


imtdoc::IDocumentService::OperationStatus CCollectionDocumentServiceIntegrationTest::SaveDocument(
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


imtdoc::IDocumentService::DocumentListItem CCollectionDocumentServiceIntegrationTest::FindDocumentInfo(
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


QString CCollectionDocumentServiceIntegrationTest::GetDocumentText(
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



// GetOpenedDocumentList tests

void CCollectionDocumentServiceIntegrationTest::GetOpenedDocumentListEmptyTest()
{
	QVERIFY2(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(), "Document list should be empty initially");
}


void CCollectionDocumentServiceIntegrationTest::GetOpenedDocumentListNonExistentUserTest()
{
	QVERIFY2(m_servicePtr->GetOpenedDocumentList(QByteArrayLiteral("nonexistentUser")).isEmpty(), "Document list for non-existent user should be empty");
}


void CCollectionDocumentServiceIntegrationTest::GetOpenedDocumentListAfterCreateTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY2(!docId.isEmpty(), "Document should be created");

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId);
	QCOMPARE(list[0].typeId, TEST_TYPE_ID);
	QVERIFY2(list[0].isDirty, "New document should be dirty");
	QVERIFY2(!list[0].isLoading, "Document should be loaded");
	QVERIFY2(list[0].url.isEmpty(), "New document should not have a URL");
}


void CCollectionDocumentServiceIntegrationTest::GetOpenedDocumentListMultipleDocumentsTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID_2);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());
	QVERIFY(docId1 != docId2);

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 2);
	QVERIFY(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId1).typeId == TEST_TYPE_ID);
	QVERIFY(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId2).typeId == TEST_TYPE_ID_2);
}


void CCollectionDocumentServiceIntegrationTest::GetOpenedDocumentListMultipleUsersTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, TEST_TYPE_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	imtdoc::IDocumentService::DocumentList list1 = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	imtdoc::IDocumentService::DocumentList list2 = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID_2);
	QCOMPARE(list1.size(), 1);
	QCOMPARE(list2.size(), 1);
	QCOMPARE(list1[0].documentId, docId1);
	QCOMPARE(list2[0].documentId, docId2);
}


// CreateNewDocument tests

void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY2(!docId.isEmpty(), "Document should be created");

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QCOMPARE(info.documentId, docId);
	QCOMPARE(info.name, TEST_DOC_NAME);
	QVERIFY(m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId) != nullptr);
}


void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentUndoManagerFailTest()
{
	QSKIP("The undo manager factory is always configured in the integration registry");
}


void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentObjectCreationFailAsyncTest()
{
#ifndef QT_NO_DEBUG
	QSKIP("Missing object factory triggers Q_ASSERT in debug builds");
#endif

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = QByteArrayLiteral("UnknownType");

	QByteArray docId;
	imtdoc::IDocumentService::OperationStatus status = ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_NEW, params, &docId);
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(docId.isEmpty());
	QTRY_VERIFY_WITH_TIMEOUT(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(), WAIT_TIMEOUT);
}


void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentEventFiredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QVERIFY2(m_eventHandler.HasEvent(CREATED_EVENT, TEST_USER_ID), "DocumentCreated event should be fired");
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);
}


void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentIsLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	QVERIFY(!taskId.isEmpty());

	// Loading finishes in the main thread, no events were processed yet
	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QVERIFY2(list[0].isLoading, "Document should be loading right after the task was started");

	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
	QVERIFY(WaitForDocumentLoaded(*m_servicePtr, TEST_USER_ID, result.documentId));
}


void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentMultipleDocumentsTest()
{
	QSet<QByteArray> ids;
	for (int i = 0; i < 3; ++i){
		QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
		QVERIFY(!docId.isEmpty());

		ids.insert(docId);
	}

	QCOMPARE(ids.size(), 3);
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 3);
}


void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentProposedSourceDocumentIdUsedOnSaveTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;
	params.proposedSourceDocumentId = TEST_OBJECT_ID;
	params.documentName = TEST_DOC_NAME;

	QByteArray docId;
	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_NEW, params, &docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY(WaitForDocumentLoaded(*m_servicePtr, TEST_USER_ID, docId));

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	QVERIFY2(m_collectionPtr->GetElementIds().contains(TEST_OBJECT_ID), "Proposed ID should be used as the collection object ID");
	QCOMPARE(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).url, imtdoc::ObjectIdToUrl(TEST_OBJECT_ID));
}


// OpenDocument tests

void CCollectionDocumentServiceIntegrationTest::OpenDocumentSuccessTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("testData"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY2(!docId.isEmpty(), "Document should be opened");

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QCOMPARE(info.typeId, TEST_TYPE_ID);
	QCOMPARE(info.name, TEST_DOC_NAME);
	QCOMPARE(info.url, imtdoc::ObjectIdToUrl(TEST_OBJECT_ID));
	QVERIFY(!info.isDirty);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("testData"));
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentInvalidSchemeTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl(QStringLiteral("file:///") + TEST_OBJECT_ID);

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentInvalidUrlTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl(QStringLiteral("collection:///"));

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentNonExistentObjectTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = imtdoc::ObjectIdToUrl(QByteArrayLiteral("nonExistentObject"));

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentUndoManagerFailTest()
{
	QSKIP("The undo manager factory is always configured in the integration registry");
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentEventFiredTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	QVERIFY2(m_eventHandler.HasEvent(OPENED_EVENT, TEST_USER_ID), "DocumentOpened event should be fired");
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentIsLoadingTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = imtdoc::ObjectIdToUrl(TEST_OBJECT_ID);

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY(!taskId.isEmpty());

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QVERIFY2(list[0].isLoading, "Document should be loading right after the open task was started");

	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
	QVERIFY(WaitForDocumentLoaded(*m_servicePtr, TEST_USER_ID, result.documentId));
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentCompletesBeforeDataLoadedTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("testData"));

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = imtdoc::ObjectIdToUrl(TEST_OBJECT_ID);

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
	QVERIFY(!result.documentId.isEmpty());

	// Data loading is finished in the main thread only
	QVERIFY2(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, result.documentId).isLoading, "Open task should complete before the data is loaded");
	QCOMPARE(m_eventHandler.CountEventsOfType(DATA_LOADED_EVENT), 0);

	QVERIFY(WaitForDocumentLoaded(*m_servicePtr, TEST_USER_ID, result.documentId));
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, result.documentId), QStringLiteral("testData"));
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentDataLoadFailClosesDocumentTest()
{
	QSKIP("Collection data loading failure cannot be injected with real components");
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentWithHostTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl(QStringLiteral("collection://host/") + TEST_OBJECT_ID);

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentMultiplePathSegmentsTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl(QStringLiteral("collection:///segment1/segment2"));

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentWithoutSingleInstanceModeFailsWhenSingleInstanceAlreadyOpenedTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams singleInstanceParams;
	singleInstanceParams.userId = TEST_USER_ID;
	singleInstanceParams.url = imtdoc::ObjectIdToUrl(TEST_OBJECT_ID);
	singleInstanceParams.singleDocumentInstance = true;

	QByteArray firstDocId;
	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, singleInstanceParams, &firstDocId), imtdoc::IDocumentService::OS_OK);
	QVERIFY(!firstDocId.isEmpty());

	imtdoc::IDocumentService::TaskParams regularParams = singleInstanceParams;
	regularParams.singleDocumentInstance = false;

	QByteArray secondDocId;
	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, regularParams, &secondDocId), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY2(secondDocId.isEmpty(), "Open without single-instance mode must fail when the document is already open in single-instance mode");
}


// GetDocumentName tests

void CCollectionDocumentServiceIntegrationTest::GetDocumentNameSuccessTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, TEST_DOC_NAME);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentNameInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(QByteArrayLiteral("invalidUser"), docId, name), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentNameInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// SetDocumentName tests

void CCollectionDocumentServiceIntegrationTest::SetDocumentNameSuccessTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	const QString newName = QStringLiteral("New Name");
	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, docId, newName), imtdoc::IDocumentService::OS_OK);

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, newName);
	QCOMPARE(m_collectionPtr->GetElementInfo(TEST_OBJECT_ID, imtbase::ICollectionInfo::EIT_NAME).toString(), newName);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentNameSameNameTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	m_eventHandler.ClearEvents();

	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, docId, TEST_DOC_NAME), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_eventHandler.CountEventsOfType(RENAMED_EVENT), 0);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentNameInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(m_servicePtr->SetDocumentName(QByteArrayLiteral("invalidUser"), docId, QStringLiteral("NewName")), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentNameInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), QStringLiteral("NewName")), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentNameCollectionUpdateFailTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	// Renaming fails in the collection when the object was removed
	QVERIFY(m_collectionPtr->RemoveElements({TEST_OBJECT_ID}));

	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, docId, QStringLiteral("NewName")), imtdoc::IDocumentService::OS_FAILED);

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, TEST_DOC_NAME);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentNameEventFiredTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	m_eventHandler.ClearEvents();

	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, docId, QStringLiteral("NewName")), imtdoc::IDocumentService::OS_OK);
	QVERIFY2(m_eventHandler.HasEvent(RENAMED_EVENT, TEST_USER_ID), "DocumentRenamed event should be fired");

	const QList<CRecordingEventHandler::RecordedEvent> events = m_eventHandler.GetEvents();
	for (const CRecordingEventHandler::RecordedEvent& event : events){
		if (event.type == RENAMED_EVENT){
			QCOMPARE(event.documentId, docId);
			QCOMPARE(event.documentName, QStringLiteral("NewName"));
		}
	}
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentNameNewDocumentNoCollectionUpdateTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	const int elementCount = m_collectionPtr->GetElementIds().count();

	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, docId, QStringLiteral("NewName")), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount);

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, QStringLiteral("NewName"));
}


// GetDocumentPtr tests

void CCollectionDocumentServiceIntegrationTest::GetDocumentPtrSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	const istd::IChangeable* ptr = m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY(ptr != nullptr);
	QVERIFY(dynamic_cast<const iprm::ITextParam*>(ptr) != nullptr);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentPtrInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QVERIFY(m_servicePtr->GetDocumentPtr(QByteArrayLiteral("invalidUser"), docId) == nullptr);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentPtrInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QVERIFY(m_servicePtr->GetDocumentPtr(TEST_USER_ID, QByteArrayLiteral("invalidDocId")) == nullptr);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentPtrWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QVERIFY(!m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params).isEmpty());

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);

	QVERIFY2(m_servicePtr->GetDocumentPtr(TEST_USER_ID, list[0].documentId) == nullptr, "GetDocumentPtr should return nullptr while document is loading");
}


// GetDocumentData tests

void CCollectionDocumentServiceIntegrationTest::GetDocumentDataSuccessTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("testData"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	istd::IChangeableSharedPtr dataPtr;
	QCOMPARE(m_servicePtr->GetDocumentData(TEST_USER_ID, docId, dataPtr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(dataPtr.IsValid());

	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(dataPtr.GetPtr());
	QVERIFY(textPtr != nullptr);
	QCOMPARE(textPtr->GetText(), QStringLiteral("testData"));

	// Returned data is a copy
	QVERIFY(dataPtr.GetPtr() != m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId));
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentDataInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	istd::IChangeableSharedPtr dataPtr;
	QCOMPARE(m_servicePtr->GetDocumentData(QByteArrayLiteral("invalidUser"), docId, dataPtr), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentDataInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	istd::IChangeableSharedPtr dataPtr;
	QCOMPARE(m_servicePtr->GetDocumentData(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), dataPtr), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentDataWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QVERIFY(!m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params).isEmpty());

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);

	istd::IChangeableSharedPtr dataPtr;
	QCOMPARE(m_servicePtr->GetDocumentData(TEST_USER_ID, list[0].documentId, dataPtr), imtdoc::IDocumentService::OS_FAILED);
}


// SetDocumentData tests

void CCollectionDocumentServiceIntegrationTest::SetDocumentDataSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("newData"));

	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, newData), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("newData"));
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentDataInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	QCOMPARE(m_servicePtr->SetDocumentData(QByteArrayLiteral("invalidUser"), docId, newData), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentDataInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	iprm::CTextParam newData;
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), newData), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentDataWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QVERIFY(!m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params).isEmpty());

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);

	iprm::CTextParam newData;
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, list[0].documentId, newData), imtdoc::IDocumentService::OS_FAILED);
}


// SaveDocument tests

void CCollectionDocumentServiceIntegrationTest::SaveDocumentNewDocumentTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("savedData"));
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, newData), imtdoc::IDocumentService::OS_OK);

	const int elementCount = m_collectionPtr->GetElementIds().count();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, TEST_DOC_NAME), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount + 1);

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QVERIFY2(!info.isDirty, "Document should not be dirty after save");

	const QByteArray objectId = imtdoc::UrlToObjectId(info.url);
	QVERIFY(!objectId.isEmpty());
	QCOMPARE(m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString(), TEST_DOC_NAME);

	imtbase::IObjectCollection::DataPtr dataPtr;
	QVERIFY(m_collectionPtr->GetObjectData(objectId, dataPtr));
	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(dataPtr.GetPtr());
	QVERIFY(textPtr != nullptr);
	QCOMPARE(textPtr->GetText(), QStringLiteral("savedData"));
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentExistingDocumentTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("original"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("modified"));
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, newData), imtdoc::IDocumentService::OS_OK);

	const int elementCount = m_collectionPtr->GetElementIds().count();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount);

	imtbase::IObjectCollection::DataPtr dataPtr;
	QVERIFY(m_collectionPtr->GetObjectData(TEST_OBJECT_ID, dataPtr));
	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(dataPtr.GetPtr());
	QVERIFY(textPtr != nullptr);
	QCOMPARE(textPtr->GetText(), QStringLiteral("modified"));

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QVERIFY(!info.isDirty);
	QCOMPARE(info.url, imtdoc::ObjectIdToUrl(TEST_OBJECT_ID));
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentSaveAsTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("original"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	const int elementCount = m_collectionPtr->GetElementIds().count();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("Copy Name")), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount + 1);

	// Original object is not changed
	QCOMPARE(m_collectionPtr->GetElementInfo(TEST_OBJECT_ID, imtbase::ICollectionInfo::EIT_NAME).toString(), TEST_DOC_NAME);

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, QStringLiteral("Copy Name"));
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentNewDocumentUpdatesUrlTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());
	QVERIFY(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).url.isEmpty());

	const imtbase::ICollectionInfo::Ids idsBefore = m_collectionPtr->GetElementIds();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	const QByteArray objectId = imtdoc::UrlToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).url);
	QVERIFY2(!objectId.isEmpty(), "Saved document should have a collection URL");
	QVERIFY(!idsBefore.contains(objectId));
	QVERIFY(m_collectionPtr->GetElementIds().contains(objectId));
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentSaveAsUpdatesUrlTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("Copy Name")), imtdoc::IDocumentService::OS_OK);

	const QByteArray objectId = imtdoc::UrlToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).url);
	QVERIFY(!objectId.isEmpty());
	QVERIFY2(objectId != TEST_OBJECT_ID, "Document should be backed by the copy after Save As");
	QCOMPARE(m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString(), QStringLiteral("Copy Name"));
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(SaveDocument(*m_servicePtr, QByteArrayLiteral("invalidUser"), docId), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, QByteArrayLiteral("invalidDocId")), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QVERIFY(!m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params).isEmpty());

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);

	// Save task is completed synchronously, the event loop is not spun before
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, list[0].documentId), imtdoc::IDocumentService::OS_FAILED);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentCollectionInsertFailTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	// Insertion fails because the proposed ID is already used in the collection
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;
	params.proposedSourceDocumentId = TEST_OBJECT_ID;

	QByteArray docId;
	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_NEW, params, &docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY(WaitForDocumentLoaded(*m_servicePtr, TEST_USER_ID, docId));

	const int elementCount = m_collectionPtr->GetElementIds().count();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_FAILED);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount);

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QVERIFY2(info.isDirty, "Document should stay dirty after failed save");
	QVERIFY(info.url.isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentCollectionSetDataFailTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("modified"));
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, newData), imtdoc::IDocumentService::OS_OK);

	// Updating fails because the object was removed from the collection
	QVERIFY(m_collectionPtr->RemoveElements({TEST_OBJECT_ID}));

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_FAILED);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentValidationFailTest()
{
	// Validator of this type has no error message configured
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, VALIDATED_TYPE_ID_2);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam invalidData;
	invalidData.SetText(INVALID_TEXT);
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, invalidData), imtdoc::IDocumentService::OS_OK);

	const int elementCount = m_collectionPtr->GetElementIds().count();

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = TEST_DOC_NAME;

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA);
	QCOMPARE(result.errorMessage, DEFAULT_VALIDATION_ERROR_MESSAGE);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount);

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QVERIFY2(info.isDirty, "Document should stay dirty after failed validation");
	QVERIFY(info.url.isEmpty());

	// Document is saved after correction of the data
	iprm::CTextParam validData;
	validData.SetText(QStringLiteral("valid"));
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, validData), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, TEST_DOC_NAME), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount + 1);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentValidationFailWithMessageTest()
{
	AddCollectionObject(TEST_OBJECT_ID, VALIDATED_TYPE_ID, TEST_DOC_NAME, QStringLiteral("original"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam invalidData;
	invalidData.SetText(INVALID_TEXT);
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, invalidData), imtdoc::IDocumentService::OS_OK);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA);
	QCOMPARE(result.errorMessage, VALIDATION_ERROR_MESSAGE);

	// Collection object is not changed
	imtbase::IObjectCollection::DataPtr storedDataPtr;
	QVERIFY(m_collectionPtr->GetObjectData(TEST_OBJECT_ID, storedDataPtr));
	const iprm::ITextParam* storedTextPtr = dynamic_cast<const iprm::ITextParam*>(storedDataPtr.GetPtr());
	QVERIFY(storedTextPtr != nullptr);
	QCOMPARE(storedTextPtr->GetText(), QStringLiteral("original"));
	QCOMPARE(m_collectionPtr->GetElementInfo(TEST_OBJECT_ID, imtbase::ICollectionInfo::EIT_NAME).toString(), TEST_DOC_NAME);

	QVERIFY2(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty, "Document should stay dirty after failed validation");
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentWithNameProviderTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, VALIDATED_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam data;
	data.SetText(QStringLiteral("Generated"));
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, data), imtdoc::IDocumentService::OS_OK);

	// Name provided by the name provider overrides the requested name of a new document
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, TEST_DOC_NAME), imtdoc::IDocumentService::OS_OK);

	const QString expectedName = NAME_PREFIX + QStringLiteral("Generated");

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, expectedName);

	const QByteArray objectId = imtdoc::UrlToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).url);
	QVERIFY(!objectId.isEmpty());
	QCOMPARE(m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString(), expectedName);

	// Name of an existing document is updated on save
	data.SetText(QStringLiteral("Updated"));
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, data), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	const QString updatedName = NAME_PREFIX + QStringLiteral("Updated");

	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, updatedName);
	QCOMPARE(m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString(), updatedName);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentEventFiredTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	m_eventHandler.ClearEvents();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY2(m_eventHandler.HasEvent(SAVED_EVENT, TEST_USER_ID), "DocumentSaved event should be fired");
	QCOMPARE(m_eventHandler.CountEventsOfType(SAVED_AS_EVENT), 0);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentSaveAsEventFiredTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	m_eventHandler.ClearEvents();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("Copy Name")), imtdoc::IDocumentService::OS_OK);
	QVERIFY2(m_eventHandler.HasEvent(SAVED_AS_EVENT, TEST_USER_ID), "DocumentSavedAs event should be fired");
	QCOMPARE(m_eventHandler.CountEventsOfType(SAVED_EVENT), 0);
}


// CloseDocument tests

void CCollectionDocumentServiceIntegrationTest::CloseDocumentSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentServiceIntegrationTest::CloseDocumentInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, QByteArrayLiteral("invalidUser"), docId), imtdoc::IDocumentService::OS_INVALID_USER_ID);
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
}


void CCollectionDocumentServiceIntegrationTest::CloseDocumentInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, QByteArrayLiteral("invalidDocId")), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
}


void CCollectionDocumentServiceIntegrationTest::CloseDocumentEventFiredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	m_eventHandler.ClearEvents();

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY2(m_eventHandler.HasEvent(CLOSED_EVENT, TEST_USER_ID), "DocumentClosed event should be fired");
}


void CCollectionDocumentServiceIntegrationTest::CloseDocumentRemovesFromListTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId1), imtdoc::IDocumentService::OS_OK);

	imtdoc::IDocumentService::DocumentList list = m_servicePtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId2);
}


void CCollectionDocumentServiceIntegrationTest::CloseDocumentLastDocForUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());

	// User entry is removed with the last document
	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


// GetDocumentUndoManager tests

void CCollectionDocumentServiceIntegrationTest::GetDocumentUndoManagerSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(TEST_USER_ID, docId, undoManagerPtr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(undoManagerPtr != nullptr);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentUndoManagerInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(QByteArrayLiteral("invalidUser"), docId, undoManagerPtr), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::GetDocumentUndoManagerInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), undoManagerPtr), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// RegisterDocumentObserver tests

void CCollectionDocumentServiceIntegrationTest::RegisterDocumentObserverSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QVERIFY(observer.IsModelAttached(nullptr));
}


void CCollectionDocumentServiceIntegrationTest::RegisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(QByteArrayLiteral("invalidUser"), docId, observer), imtdoc::IDocumentService::OS_INVALID_USER_ID);
	QVERIFY(!observer.IsModelAttached(nullptr));
}


void CCollectionDocumentServiceIntegrationTest::RegisterDocumentObserverInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), observer), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	QVERIFY(!observer.IsModelAttached(nullptr));
}


void CCollectionDocumentServiceIntegrationTest::RegisterDocumentObserverAlreadyRegisteredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_FAILED);
}


// UnregisterDocumentObserver tests

void CCollectionDocumentServiceIntegrationTest::UnregisterDocumentObserverSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QVERIFY(!observer.IsModelAttached(nullptr));
}


void CCollectionDocumentServiceIntegrationTest::UnregisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(QByteArrayLiteral("invalidUser"), docId, observer), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::UnregisterDocumentObserverInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), observer), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceIntegrationTest::UnregisterDocumentObserverNotRegisteredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_FAILED);
}


// Serialize tests

void CCollectionDocumentServiceIntegrationTest::SerializeReturnsFalseTest()
{
	iser::CMemoryWriteArchive archive;
	QVERIFY2(!m_servicePtr->Serialize(archive), "Serialize should return false");
}


// ValidateInputParams tests (checked through GetDocumentName)

void CCollectionDocumentServiceIntegrationTest::ValidateInputParamsSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentServiceIntegrationTest::ValidateInputParamsInvalidUserTest()
{
	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(QByteArrayLiteral("invalidUser"), QByteArrayLiteral("anyDocId"), name), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceIntegrationTest::ValidateInputParamsInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceIntegrationTest::ValidateInputParamsForeignDocumentTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, TEST_TYPE_ID).isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID_2, docId1, name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// Document storage tests

void CCollectionDocumentServiceIntegrationTest::DocumentStorageInsertRemoveTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 2);

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId1), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
	QVERIFY(m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId1) == nullptr);
	QVERIFY(m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId2) != nullptr);

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId2), imtdoc::IDocumentService::OS_OK);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


// ObjectIdToUrl tests

void CCollectionDocumentServiceIntegrationTest::ObjectIdToUrlTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	const QUrl url = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).url;
	QCOMPARE(url.scheme(), QStringLiteral("collection"));
	QVERIFY(url.host().isEmpty());
	QCOMPARE(url.path(), QStringLiteral("/") + TEST_OBJECT_ID);
}


void CCollectionDocumentServiceIntegrationTest::UrlToObjectIdTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	// URL of the saved document references the inserted collection object
	const QByteArray objectId = imtdoc::UrlToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).url);
	QVERIFY(!objectId.isEmpty());
	QVERIFY(m_collectionPtr->GetElementIds().contains(objectId));

	// The URL can be used to open the object
	QByteArray openedDocId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, objectId);
	QVERIFY(!openedDocId.isEmpty());
	QCOMPARE(FindDocumentInfo(*m_servicePtr, TEST_USER_ID_2, openedDocId).url, imtdoc::ObjectIdToUrl(objectId));
}


// Async document loading completion tests

void CCollectionDocumentServiceIntegrationTest::CreateNewDocumentAsyncCompletionTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	QVERIFY(!taskId.isEmpty());

	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
	QVERIFY(!result.documentId.isEmpty());

	QTRY_VERIFY_WITH_TIMEOUT(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, result.documentId).isLoading, WAIT_TIMEOUT);
	QVERIFY(m_servicePtr->GetDocumentPtr(TEST_USER_ID, result.documentId) != nullptr);
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentAsyncCompletionTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("testData"));

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = imtdoc::ObjectIdToUrl(TEST_OBJECT_ID);

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY(!taskId.isEmpty());

	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
	QVERIFY(!result.documentId.isEmpty());

	QTRY_VERIFY_WITH_TIMEOUT(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, result.documentId).isLoading, WAIT_TIMEOUT);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, result.documentId), QStringLiteral("testData"));
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);
}


// Multi-user scenario tests

void CCollectionDocumentServiceIntegrationTest::MultiUserCreateAndCloseTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, TEST_TYPE_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId1), imtdoc::IDocumentService::OS_OK);

	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID_2).size(), 1);

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID_2, docId2), imtdoc::IDocumentService::OS_OK);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID_2).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::MultiUserIsolationTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, QStringLiteral("User1 Doc"));
	QByteArray docId2 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, TEST_TYPE_ID, QStringLiteral("User2 Doc"));
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId2, name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID_2, docId1, name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);

	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId1, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, QStringLiteral("User1 Doc"));

	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID_2, docId2, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, QStringLiteral("User2 Doc"));
}


// Single-copy mode tests

void CCollectionDocumentServiceIntegrationTest::SingleCopyOpenSameObjectByTwoUsersTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("sharedData"));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY2(!docId1.isEmpty(), "First user should open document successfully");

	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY2(!docId2.isEmpty(), "Second user should open document successfully");
	QCOMPARE(docId2, docId1);

	const istd::IChangeable* ptr1 = m_singleCopyServicePtr->GetDocumentPtr(TEST_USER_ID, docId1);
	const istd::IChangeable* ptr2 = m_singleCopyServicePtr->GetDocumentPtr(TEST_USER_ID_2, docId2);
	QVERIFY(ptr1 != nullptr);
	QVERIFY2(ptr1 == ptr2, "Both users should share the same document object");
	QCOMPARE(GetDocumentText(*m_singleCopyServicePtr, TEST_USER_ID_2, docId2), QStringLiteral("sharedData"));
}


void CCollectionDocumentServiceIntegrationTest::SingleCopySecondUserGetsDataLoadedEventTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId1.isEmpty());
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);

	m_eventHandler.ClearEvents();

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID_2;
	params.url = imtdoc::ObjectIdToUrl(TEST_OBJECT_ID);

	QByteArray docId2;
	QCOMPARE(ExecuteTask(*m_singleCopyServicePtr, imtdoc::IDocumentService::TT_OPEN, params, &docId2), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(docId2, docId1);

	// The event for the joining user is posted to the event loop
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID_2), WAIT_TIMEOUT);

	const QList<CRecordingEventHandler::RecordedEvent> events = m_eventHandler.GetEvents();
	for (const CRecordingEventHandler::RecordedEvent& event : events){
		if ((event.type == DATA_LOADED_EVENT) && (event.userId == TEST_USER_ID_2)){
			QCOMPARE(event.documentId, docId2);
		}
	}
}


void CCollectionDocumentServiceIntegrationTest::SingleCopySetDocumentDataSharedTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("original"));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("modified"));
	QCOMPARE(m_singleCopyServicePtr->SetDocumentData(TEST_USER_ID, docId1, newData), imtdoc::IDocumentService::OS_OK);

	QCOMPARE(GetDocumentText(*m_singleCopyServicePtr, TEST_USER_ID_2, docId2), QStringLiteral("modified"));
}


void CCollectionDocumentServiceIntegrationTest::SingleCopyCloseOneUserTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	QCOMPARE(CloseDocument(*m_singleCopyServicePtr, TEST_USER_ID, docId1), imtdoc::IDocumentService::OS_OK);

	QVERIFY(m_singleCopyServicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
	QCOMPARE(m_singleCopyServicePtr->GetOpenedDocumentList(TEST_USER_ID_2).size(), 1);
	QVERIFY2(m_singleCopyServicePtr->GetDocumentPtr(TEST_USER_ID_2, docId2) != nullptr, "Document should stay open while one user holds it");
}


void CCollectionDocumentServiceIntegrationTest::SingleCopyCloseBothUsersTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!docId1.isEmpty());
	QCOMPARE(docId2, docId1);

	QCOMPARE(CloseDocument(*m_singleCopyServicePtr, TEST_USER_ID, docId1), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(CloseDocument(*m_singleCopyServicePtr, TEST_USER_ID_2, docId2), imtdoc::IDocumentService::OS_OK);

	QVERIFY(m_singleCopyServicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
	QVERIFY(m_singleCopyServicePtr->GetOpenedDocumentList(TEST_USER_ID_2).isEmpty());

	// The document was really closed: reopening creates a new instance
	QByteArray reopenedDocId = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!reopenedDocId.isEmpty());
	QVERIFY2(reopenedDocId != docId1, "Document should be removed after all users closed it");
}


void CCollectionDocumentServiceIntegrationTest::SingleCopySaveUpdatesAllTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("modified"));
	QCOMPARE(m_singleCopyServicePtr->SetDocumentData(TEST_USER_ID, docId1, newData), imtdoc::IDocumentService::OS_OK);

	m_eventHandler.ClearEvents();

	QCOMPARE(SaveDocument(*m_singleCopyServicePtr, TEST_USER_ID, docId1), imtdoc::IDocumentService::OS_OK);

	QVERIFY(!FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID, docId1).isDirty);
	QVERIFY2(!FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID_2, docId2).isDirty, "Second user's document should not be dirty after shared save");

	QVERIFY(m_eventHandler.HasEvent(SAVED_EVENT, TEST_USER_ID));
	QVERIFY(m_eventHandler.HasEvent(SAVED_EVENT, TEST_USER_ID_2));
}


void CCollectionDocumentServiceIntegrationTest::SingleCopySetDocumentNameUpdatesAllTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	const QString newName = QStringLiteral("Renamed Shared Document");
	QCOMPARE(m_singleCopyServicePtr->SetDocumentName(TEST_USER_ID, docId1, newName), imtdoc::IDocumentService::OS_OK);

	QString name1;
	QString name2;
	QCOMPARE(m_singleCopyServicePtr->GetDocumentName(TEST_USER_ID, docId1, name1), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_singleCopyServicePtr->GetDocumentName(TEST_USER_ID_2, docId2, name2), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name1, newName);
	QCOMPARE(name2, newName);

	QVERIFY(m_eventHandler.HasEvent(RENAMED_EVENT, TEST_USER_ID));
	QVERIFY(m_eventHandler.HasEvent(RENAMED_EVENT, TEST_USER_ID_2));
}


void CCollectionDocumentServiceIntegrationTest::SingleCopySaveAsUpdatesAllTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());
	QCOMPARE(OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID), docId);

	m_eventHandler.ClearEvents();

	QCOMPARE(SaveDocument(*m_singleCopyServicePtr, TEST_USER_ID, docId, QStringLiteral("Saved As Copy")), imtdoc::IDocumentService::OS_OK);

	const QUrl copyUrl = FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID, docId).url;
	QVERIFY(imtdoc::UrlToObjectId(copyUrl) != TEST_OBJECT_ID);
	QVERIFY(m_collectionPtr->GetElementIds().contains(imtdoc::UrlToObjectId(copyUrl)));

	// Both users hold the same document, which is now backed by the copy
	QCOMPARE(FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID_2, docId).url, copyUrl);
	QCOMPARE(m_eventHandler.CountEventsOfType(SAVED_AS_EVENT), 2);
}


void CCollectionDocumentServiceIntegrationTest::SingleCopyReopenBySameUserTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());
	QCOMPARE(OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID), docId);
	QCOMPARE(m_singleCopyServicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
}


void CCollectionDocumentServiceIntegrationTest::SingleCopyOneDocumentForAllUsersTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	AddCollectionObject(TEST_OBJECT_ID_2, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());
	QCOMPARE(OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID), docId);

	// Another object gets its own document
	QByteArray otherDocId = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID_2);
	QVERIFY(!otherDocId.isEmpty());
	QVERIFY(otherDocId != docId);

	QCOMPARE(m_singleCopyServicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
	QCOMPARE(m_singleCopyServicePtr->GetOpenedDocumentList(TEST_USER_ID_2).size(), 2);

	// The regular service creates separate documents for each user
	QByteArray regularDocId1 = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray regularDocId2 = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!regularDocId1.isEmpty());
	QVERIFY(!regularDocId2.isEmpty());
	QVERIFY(regularDocId1 != regularDocId2);
}


I_ADD_TEST(CCollectionDocumentServiceIntegrationTest);
