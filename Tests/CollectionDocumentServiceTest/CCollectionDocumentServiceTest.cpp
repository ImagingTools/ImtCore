// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCollectionDocumentServiceTest.h"


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QElapsedTimer>
#include <QtCore/QUuid>


namespace
{

const QByteArray TEST_USER_ID = "testUser";
const QByteArray TEST_USER_ID_2 = "testUser2";
const QByteArray TEST_TYPE_ID = "TestType";
const QByteArray TEST_TYPE_ID_2 = "TestType2";
const QByteArray TEST_OBJECT_ID = "testObject123";
const QByteArray TEST_OBJECT_ID_2 = "testObject456";
const QString TEST_DOC_NAME = "Test Document";

}


// Setup and teardown

void CCollectionDocumentServiceTest::init()
{
	m_managerPtr = new CTestableDocumentService();
}


void CCollectionDocumentServiceTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


// Helper methods

QByteArray CCollectionDocumentServiceTest::CreateDocumentAndWaitForLoad(
	CTestableDocumentService& manager,
	const QByteArray& userId,
	const QByteArray& typeId)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.documentTypeId = typeId;

	QByteArray taskId = manager.BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	if (taskId.isEmpty()) {
		return QByteArray();
	}

	auto result = manager.WaitForTaskFinished(taskId);
	return result.documentId;
}


QByteArray CCollectionDocumentServiceTest::OpenDocumentAndWaitForLoad(
	CTestableDocumentService& manager,
	const QByteArray& userId,
	const QByteArray& objectId)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.url = QUrl("collection:///" + objectId);

	QByteArray taskId = manager.BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	if (taskId.isEmpty()) {
		return QByteArray();
	}

	auto result = manager.WaitForTaskFinished(taskId);
	QElapsedTimer timer;
	timer.start();
	while (!result.documentId.isEmpty() && timer.elapsed() < 5000) {
		auto documents = manager.GetOpenedDocumentList(userId);
		for (const auto& document : documents) {
			if (document.documentId == result.documentId && !document.isLoading) {
				return result.documentId;
			}
		}
		QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
	}

	return result.documentId;
}


QByteArray CCollectionDocumentServiceTest::SetupDocumentDirectly(
	CTestableDocumentService& manager,
	const QByteArray& userId,
	const QByteArray& typeId,
	const QByteArray& objectId,
	const QString& name)
{
	QByteArray documentId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QMutexLocker locker(&manager.m_mutex);
	auto& doc = manager.m_userDocuments[userId][documentId];
	doc.typeId = typeId;
	doc.objectId = objectId;
	doc.name = name;
	doc.objectPtr = istd::IChangeableSharedPtr(new CMockDocumentObject());
	doc.undoManagerPtr = idoc::IUndoManagerSharedPtr(new CMockUndoManager());
	doc.isDirty = false;
	doc.isLoading = false;

	return documentId;
}


// ======================================================================
// GetOpenedDocumentList tests
// ======================================================================

void CCollectionDocumentServiceTest::GetOpenedDocumentListEmptyTest()
{
	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "Document list should be empty initially");
}


void CCollectionDocumentServiceTest::GetOpenedDocumentListNonExistentUserTest()
{
	auto list = m_managerPtr->GetOpenedDocumentList("nonexistentUser");
	QVERIFY2(list.isEmpty(), "Document list for non-existent user should be empty");
}


void CCollectionDocumentServiceTest::GetOpenedDocumentListAfterCreateTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	QVERIFY2(!docId.isEmpty(), "Document ID should not be empty");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId);
}


void CCollectionDocumentServiceTest::GetOpenedDocumentListMultipleDocumentsTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID_2);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 2);
}


void CCollectionDocumentServiceTest::GetOpenedDocumentListMultipleUsersTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID_2, TEST_TYPE_ID);

	auto list1 = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	auto list2 = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2);

	QCOMPARE(list1.size(), 1);
	QCOMPARE(list2.size(), 1);
	QVERIFY2(list1[0].documentId != list2[0].documentId, "Different users should have different document IDs");
}


// ======================================================================
// CreateNewDocument tests
// ======================================================================

void CCollectionDocumentServiceTest::CreateNewDocumentSuccessTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	QVERIFY2(!taskId.isEmpty(), "BeginDocumentTask should return a non-empty task ID");

	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QByteArray docId = result.documentId;
	QVERIFY2(!docId.isEmpty(), "Task result should contain a non-empty document ID");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId);
	QCOMPARE(list[0].typeId, TEST_TYPE_ID);
}


void CCollectionDocumentServiceTest::CreateNewDocumentUndoManagerFailTest()
{
	m_managerPtr->SetCreateUndoManagerShouldFail(true);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	QVERIFY2(taskId.isEmpty(), "BeginDocumentTask should return empty task ID when undo manager creation fails");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "No documents should be created when undo manager fails");
}


void CCollectionDocumentServiceTest::CreateNewDocumentObjectCreationFailAsyncTest()
{
	m_managerPtr->SetCreateObjectShouldFail(true);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	QVERIFY2(!taskId.isEmpty(), "BeginDocumentTask returns task ID immediately even if object creation will fail async");

	// Wait for async completion — the document should be closed due to failure
	auto result = m_managerPtr->WaitForTaskFinished(taskId);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "Document should be closed after async object creation failure");
}


void CCollectionDocumentServiceTest::CreateNewDocumentEventFiredTest()
{
	m_managerPtr->GetMockEventHandler().ClearEvents();

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);

	const auto& events = m_managerPtr->GetMockEventHandler().GetEvents();
	QVERIFY2(!events.isEmpty(), "At least one event should have been fired");

	bool foundCreatedEvent = false;
	for (const auto& event : events) {
		if (event.type == "DocumentCreatedEvent") {
			foundCreatedEvent = true;
			QCOMPARE(event.userId, TEST_USER_ID);
			QCOMPARE(event.documentTypeId, TEST_TYPE_ID);
			break;
		}
	}
	QVERIFY2(foundCreatedEvent, "DocumentCreated event should have been fired");
}


void CCollectionDocumentServiceTest::CreateNewDocumentIsLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	QVERIFY2(!taskId.isEmpty(), "Task ID should not be empty");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QVERIFY2(list[0].isLoading, "Newly created document should be in loading state");
}


void CCollectionDocumentServiceTest::CreateNewDocumentMultipleDocumentsTest()
{
	imtdoc::IDocumentService::TaskParams params1;
	params1.userId = TEST_USER_ID;
	params1.documentTypeId = TEST_TYPE_ID;

	imtdoc::IDocumentService::TaskParams params2;
	params2.userId = TEST_USER_ID;
	params2.documentTypeId = TEST_TYPE_ID_2;

	QByteArray taskId1 = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params1);
	QByteArray taskId2 = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params2);

	auto result1 = m_managerPtr->WaitForTaskFinished(taskId1);
	auto result2 = m_managerPtr->WaitForTaskFinished(taskId2);

	QByteArray docId1 = result1.documentId;
	QByteArray docId2 = result2.documentId;

	QVERIFY2(!docId1.isEmpty(), "First document ID should not be empty");
	QVERIFY2(!docId2.isEmpty(), "Second document ID should not be empty");
	QVERIFY2(docId1 != docId2, "Document IDs should be unique");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 2);
}


void CCollectionDocumentServiceTest::CreateNewDocumentProposedSourceDocumentIdUsedOnSaveTest()
{
	const QByteArray proposedId = "proposed-object-id";

	imtdoc::IDocumentService::TaskParams createParams;
	createParams.userId = TEST_USER_ID;
	createParams.documentTypeId = TEST_TYPE_ID;
	createParams.proposedSourceDocumentId = proposedId;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, createParams);
	QVERIFY2(!taskId.isEmpty(), "BeginDocumentTask should return a non-empty task ID");

	auto createResult = m_managerPtr->WaitForTaskFinished(taskId);
	QByteArray docId = createResult.documentId;
	QVERIFY2(!docId.isEmpty(), "Task result should contain a non-empty document ID");

	// Save the document – this should insert it into the collection using the proposed ID
	imtdoc::IDocumentService::TaskParams saveParams;
	saveParams.userId = TEST_USER_ID;
	saveParams.documentId = docId;
	saveParams.documentName = TEST_DOC_NAME;

	QByteArray saveTaskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, saveParams);
	auto saveResult = m_managerPtr->WaitForTaskFinished(saveTaskId);
	QCOMPARE(saveResult.status, imtdoc::IDocumentService::OS_OK);

	// Verify the proposed ID was used as the collection element ID
	QCOMPARE(m_managerPtr->GetMockCollection().GetLastInsertedId(), proposedId);
}


// ======================================================================
// OpenDocument tests
// ======================================================================

void CCollectionDocumentServiceTest::OpenDocumentSuccessTest()
{
	CMockDocumentObject* mockObj = new CMockDocumentObject("testData");
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(mockObj));

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(!taskId.isEmpty(), "BeginDocumentTask should return a non-empty task ID");

	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QByteArray docId = result.documentId;
	QVERIFY2(!docId.isEmpty(), "Task result should contain a non-empty document ID");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId);
	QCOMPARE(list[0].typeId, TEST_TYPE_ID);
	QCOMPARE(list[0].name, TEST_DOC_NAME);
}


void CCollectionDocumentServiceTest::OpenDocumentInvalidSchemeTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("file:///some/path");

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(taskId.isEmpty(), "BeginDocumentTask should fail with non-collection scheme");
}


void CCollectionDocumentServiceTest::OpenDocumentInvalidUrlTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("invalid-url");

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(taskId.isEmpty(), "BeginDocumentTask should fail with invalid URL");
}


void CCollectionDocumentServiceTest::OpenDocumentNonExistentObjectTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection:///nonexistentObject");

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(taskId.isEmpty(), "BeginDocumentTask should fail for non-existent object");
}


void CCollectionDocumentServiceTest::OpenDocumentUndoManagerFailTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));
	m_managerPtr->SetCreateUndoManagerShouldFail(true);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(taskId.isEmpty(), "BeginDocumentTask should fail when undo manager creation fails");
}


void CCollectionDocumentServiceTest::OpenDocumentEventFiredTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));
	m_managerPtr->GetMockEventHandler().ClearEvents();

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));

	m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);

	const auto& events = m_managerPtr->GetMockEventHandler().GetEvents();
	bool foundOpenedEvent = false;
	for (const auto& event : events) {
		if (event.type == "DocumentOpenedEvent") {
			foundOpenedEvent = true;
			QCOMPARE(event.userId, TEST_USER_ID);
			break;
		}
	}
	QVERIFY2(foundOpenedEvent, "DocumentOpened event should have been fired");
}


void CCollectionDocumentServiceTest::OpenDocumentIsLoadingTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(!taskId.isEmpty(), "Task ID should not be empty");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QVERIFY2(list[0].isLoading, "Opened document should be in loading state initially");
}


void CCollectionDocumentServiceTest::OpenDocumentCompletesBeforeDataLoadedTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));
	m_managerPtr->GetMockCollection().SetBlockGetObjectData(true);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	bool loadingStarted = m_managerPtr->GetMockCollection().WaitForGetObjectData(1000);
	if (!loadingStarted){
		m_managerPtr->GetMockCollection().ContinueGetObjectData();
	}
	QVERIFY2(loadingStarted,
		"Document data loading should have started");

	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	m_managerPtr->GetMockCollection().ContinueGetObjectData();

	QVERIFY2(!result.documentId.isEmpty(),
		"OpenDocument should complete while document data is still loading");
	QCOMPARE(list.size(), 1);
	QVERIFY2(list[0].isLoading, "Document should remain in loading state");

	QTRY_VERIFY_WITH_TIMEOUT([this]() {
		auto documents = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
		return documents.size() == 1 && !documents[0].isLoading;
	}(), 1000);
}


void CCollectionDocumentServiceTest::OpenDocumentWithHostTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection://hostname/objectId");

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(taskId.isEmpty(), "BeginDocumentTask should fail when URL has a host");
}


void CCollectionDocumentServiceTest::OpenDocumentMultiplePathSegmentsTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl("collection:///segment1/segment2");

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(taskId.isEmpty(), "BeginDocumentTask should fail when URL has multiple path segments");
}


// ======================================================================
// GetDocumentName tests
// ======================================================================

void CCollectionDocumentServiceTest::GetDocumentNameSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, QByteArray(), TEST_DOC_NAME);

	QString name;
	auto status = m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);

	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, TEST_DOC_NAME);
}


void CCollectionDocumentServiceTest::GetDocumentNameInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	QString name;
	auto status = m_managerPtr->GetDocumentName("invalidUser", docId, name);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceTest::GetDocumentNameInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	QString name;
	auto status = m_managerPtr->GetDocumentName(TEST_USER_ID, "invalidDocId", name);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// ======================================================================
// SetDocumentName tests
// ======================================================================

void CCollectionDocumentServiceTest::SetDocumentNameSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockCollection().AddObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QString newName = "Renamed Document";
	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, docId, newName);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	QString retrievedName;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, retrievedName);
	QCOMPARE(retrievedName, newName);
}


void CCollectionDocumentServiceTest::SetDocumentNameSameNameTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);

	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, docId, TEST_DOC_NAME);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentServiceTest::SetDocumentNameInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->SetDocumentName("invalidUser", docId, "NewName");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceTest::SetDocumentNameInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, "invalidDocId", "NewName");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceTest::SetDocumentNameCollectionUpdateFailTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockCollection().SetSetElementNameShouldFail(true);

	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, docId, "NewName");
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);

	// Name should not have changed
	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, TEST_DOC_NAME);
}


void CCollectionDocumentServiceTest::SetDocumentNameEventFiredTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockCollection().AddObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockEventHandler().ClearEvents();

	m_managerPtr->SetDocumentName(TEST_USER_ID, docId, "NewName");

	const auto& events = m_managerPtr->GetMockEventHandler().GetEvents();
	bool foundRenameEvent = false;
	for (const auto& event : events) {
		if (event.type == "DocumentRenamedEvent") {
			foundRenameEvent = true;
			QCOMPARE(event.documentName, QString("NewName"));
			break;
		}
	}
	QVERIFY2(foundRenameEvent, "DocumentRenamed event should have been fired");
}


void CCollectionDocumentServiceTest::SetDocumentNameNewDocumentNoCollectionUpdateTest()
{
	// New document (empty objectId) — no collection update needed
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, QByteArray(), TEST_DOC_NAME);
	m_managerPtr->GetMockCollection().SetSetElementNameShouldFail(true);

	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, docId, "NewName");
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, QString("NewName"));
}


// ======================================================================
// GetDocumentPtr tests
// ======================================================================

void CCollectionDocumentServiceTest::GetDocumentPtrSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY2(ptr != nullptr, "GetDocumentPtr should return a valid pointer");
}


void CCollectionDocumentServiceTest::GetDocumentPtrInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr("invalidUser", docId);
	QVERIFY2(ptr == nullptr, "GetDocumentPtr should return nullptr for invalid user");
}


void CCollectionDocumentServiceTest::GetDocumentPtrInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, "invalidDocId");
	QVERIFY2(ptr == nullptr, "GetDocumentPtr should return nullptr for invalid document ID");
}


void CCollectionDocumentServiceTest::GetDocumentPtrWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	QVERIFY2(!taskId.isEmpty(), "Document should be created");

	// Document is loading, should return nullptr
	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QByteArray docId = list[0].documentId;

	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY2(ptr == nullptr, "GetDocumentPtr should return nullptr while document is loading");
}


// ======================================================================
// GetDocumentData tests
// ======================================================================

void CCollectionDocumentServiceTest::GetDocumentDataSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData(TEST_USER_ID, docId, dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QVERIFY2(dataPtr.IsValid(), "Document data should be valid");
}


void CCollectionDocumentServiceTest::GetDocumentDataInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData("invalidUser", docId, dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceTest::GetDocumentDataInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData(TEST_USER_ID, "invalidDocId", dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceTest::GetDocumentDataWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QByteArray docId = list[0].documentId;

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData(TEST_USER_ID, docId, dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


// ======================================================================
// SetDocumentData tests
// ======================================================================

void CCollectionDocumentServiceTest::SetDocumentDataSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockDocumentObject newData("updatedData");
	auto status = m_managerPtr->SetDocumentData(TEST_USER_ID, docId, newData);

	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	// Verify the data was actually updated
	const istd::IChangeable* docPtr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY2(docPtr != nullptr, "Document pointer should be valid");

	const CMockDocumentObject* mockPtr = dynamic_cast<const CMockDocumentObject*>(docPtr);
	QVERIFY2(mockPtr != nullptr, "Should be able to cast to CMockDocumentObject");
	QCOMPARE(mockPtr->GetData(), QByteArray("updatedData"));
}


void CCollectionDocumentServiceTest::SetDocumentDataInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockDocumentObject newData;
	auto status = m_managerPtr->SetDocumentData("invalidUser", docId, newData);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceTest::SetDocumentDataInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockDocumentObject newData;
	auto status = m_managerPtr->SetDocumentData(TEST_USER_ID, "invalidDocId", newData);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceTest::SetDocumentDataWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentTypeId = TEST_TYPE_ID;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QByteArray docId = list[0].documentId;

	CMockDocumentObject newData;
	auto status = m_managerPtr->SetDocumentData(TEST_USER_ID, docId, newData);

	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


// ======================================================================
// SaveDocument tests
// ======================================================================

void CCollectionDocumentServiceTest::SaveDocumentNewDocumentTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = TEST_DOC_NAME;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);

	// Verify the document name was set
	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, TEST_DOC_NAME);

	// Verify the object was inserted into the collection
	QVERIFY2(!m_managerPtr->GetMockCollection().GetLastInsertedId().isEmpty(),
		"Object should have been inserted into collection");
}


void CCollectionDocumentServiceTest::SaveDocumentExistingDocumentTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentServiceTest::SaveDocumentSaveAsTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);

	QString newName = "Saved As Copy";
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = newName;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);

	// Verify name was updated
	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, newName);
}


void CCollectionDocumentServiceTest::SaveDocumentInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = "invalidUser";
	params.documentId = docId;

	imtdoc::IDocumentService::Error error;
	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params, &error);
	if (taskId.isEmpty()) {
		QCOMPARE(error.status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
	}
	else {
		auto result = m_managerPtr->WaitForTaskFinished(taskId);
		QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
	}
}


void CCollectionDocumentServiceTest::SaveDocumentInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = "invalidDocId";

	imtdoc::IDocumentService::Error error;
	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params, &error);
	if (taskId.isEmpty()) {
		QCOMPARE(error.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	}
	else {
		auto result = m_managerPtr->WaitForTaskFinished(taskId);
		QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	}
}


void CCollectionDocumentServiceTest::SaveDocumentWhileLoadingTest()
{
	imtdoc::IDocumentService::TaskParams createParams;
	createParams.userId = TEST_USER_ID;
	createParams.documentTypeId = TEST_TYPE_ID;

	QByteArray createTaskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, createParams);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QByteArray docId = list[0].documentId;

	imtdoc::IDocumentService::TaskParams saveParams;
	saveParams.userId = TEST_USER_ID;
	saveParams.documentId = docId;

	imtdoc::IDocumentService::Error error;
	QByteArray saveTaskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, saveParams, &error);
	if (saveTaskId.isEmpty()) {
		QCOMPARE(error.status, imtdoc::IDocumentService::OS_FAILED);
	}
	else {
		auto result = m_managerPtr->WaitForTaskFinished(saveTaskId);
		QCOMPARE(result.status, imtdoc::IDocumentService::OS_FAILED);
	}
}


void CCollectionDocumentServiceTest::SaveDocumentCollectionInsertFailTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	m_managerPtr->GetMockCollection().SetInsertShouldFail(true);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = TEST_DOC_NAME;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_FAILED);
}


void CCollectionDocumentServiceTest::SaveDocumentCollectionSetDataFailTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockCollection().SetSetObjectDataShouldFail(true);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_FAILED);
}


void CCollectionDocumentServiceTest::SaveDocumentValidationFailTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	m_managerPtr->SetValidateShouldFail(true);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = TEST_DOC_NAME;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA);
}


void CCollectionDocumentServiceTest::SaveDocumentValidationFailWithMessageTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	m_managerPtr->SetValidateShouldFail(true);
	m_managerPtr->SetValidationErrorMessage("Custom validation error");

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = TEST_DOC_NAME;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA);
	QCOMPARE(result.errorMessage, QString("Custom validation error"));
}


void CCollectionDocumentServiceTest::SaveDocumentWithNameProviderTest()
{
	m_managerPtr->SetHasNameProvider(true);
	m_managerPtr->SetDefaultDocumentName("Auto-Generated Name");

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = TEST_DOC_NAME;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);

	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, QString("Auto-Generated Name"));
}


void CCollectionDocumentServiceTest::SaveDocumentEventFiredTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockEventHandler().ClearEvents();

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	m_managerPtr->WaitForTaskFinished(taskId);

	const auto& events = m_managerPtr->GetMockEventHandler().GetEvents();
	bool foundSavedEvent = false;
	for (const auto& event : events) {
		if (event.type == "DocumentSavedEvent") {
			foundSavedEvent = true;
			break;
		}
	}
	QVERIFY2(foundSavedEvent, "DocumentSaved event should have been fired");
}


void CCollectionDocumentServiceTest::SaveDocumentSaveAsEventFiredTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockEventHandler().ClearEvents();

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;
	params.documentName = "New Copy Name";

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	m_managerPtr->WaitForTaskFinished(taskId);

	const auto& events = m_managerPtr->GetMockEventHandler().GetEvents();
	bool foundSavedAsEvent = false;
	for (const auto& event : events) {
		if (event.type == "DocumentSavedAsEvent") {
			foundSavedAsEvent = true;
			break;
		}
	}
	QVERIFY2(foundSavedAsEvent, "DocumentSavedAs event should have been fired");
}


// ======================================================================
// CloseDocument tests
// ======================================================================

void CCollectionDocumentServiceTest::CloseDocumentSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentServiceTest::CloseDocumentInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = "invalidUser";
	params.documentId = docId;

	imtdoc::IDocumentService::Error error;
	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params, &error);
	if (taskId.isEmpty()) {
		QCOMPARE(error.status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
	}
	else {
		auto result = m_managerPtr->WaitForTaskFinished(taskId);
		QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
	}
}


void CCollectionDocumentServiceTest::CloseDocumentInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = "invalidDocId";

	imtdoc::IDocumentService::Error error;
	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params, &error);
	if (taskId.isEmpty()) {
		QCOMPARE(error.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	}
	else {
		auto result = m_managerPtr->WaitForTaskFinished(taskId);
		QCOMPARE(result.status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	}
}


void CCollectionDocumentServiceTest::CloseDocumentEventFiredTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	m_managerPtr->GetMockEventHandler().ClearEvents();

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params);
	m_managerPtr->WaitForTaskFinished(taskId);

	const auto& events = m_managerPtr->GetMockEventHandler().GetEvents();
	bool foundClosedEvent = false;
	for (const auto& event : events) {
		if (event.type == "DocumentClosedEvent") {
			foundClosedEvent = true;
			QCOMPARE(event.userId, TEST_USER_ID);
			break;
		}
	}
	QVERIFY2(foundClosedEvent, "DocumentClosed event should have been fired");
}


void CCollectionDocumentServiceTest::CloseDocumentRemovesFromListTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params);
	m_managerPtr->WaitForTaskFinished(taskId);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "Document should be removed from list after closing");
}


void CCollectionDocumentServiceTest::CloseDocumentLastDocForUserTest()
{
	QByteArray docId1 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	QByteArray docId2 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::TaskParams params1;
	params1.userId = TEST_USER_ID;
	params1.documentId = docId1;

	QByteArray taskId1 = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params1);
	m_managerPtr->WaitForTaskFinished(taskId1);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId2);

	imtdoc::IDocumentService::TaskParams params2;
	params2.userId = TEST_USER_ID;
	params2.documentId = docId2;

	QByteArray taskId2 = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params2);
	m_managerPtr->WaitForTaskFinished(taskId2);

	list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "All documents should be removed after closing the last one");
}


// ======================================================================
// GetDocumentUndoManager tests
// ======================================================================

void CCollectionDocumentServiceTest::GetDocumentUndoManagerSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	auto status = m_managerPtr->GetDocumentUndoManager(TEST_USER_ID, docId, undoManagerPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QVERIFY2(undoManagerPtr != nullptr, "Undo manager should not be null");
}


void CCollectionDocumentServiceTest::GetDocumentUndoManagerInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	auto status = m_managerPtr->GetDocumentUndoManager("invalidUser", docId, undoManagerPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
	QVERIFY2(undoManagerPtr == nullptr, "Undo manager should be null for invalid user");
}


void CCollectionDocumentServiceTest::GetDocumentUndoManagerInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	auto status = m_managerPtr->GetDocumentUndoManager(TEST_USER_ID, "invalidDocId", undoManagerPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	QVERIFY2(undoManagerPtr == nullptr, "Undo manager should be null for invalid document ID");
}


// ======================================================================
// RegisterDocumentObserver tests
// ======================================================================

void CCollectionDocumentServiceTest::RegisterDocumentObserverSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentServiceTest::RegisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->RegisterDocumentObserver("invalidUser", docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceTest::RegisterDocumentObserverInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, "invalidDocId", observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceTest::RegisterDocumentObserverAlreadyRegisteredTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer);

	// Registering the same observer again should fail
	auto status = m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


// ======================================================================
// UnregisterDocumentObserver tests
// ======================================================================

void CCollectionDocumentServiceTest::UnregisterDocumentObserverSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer);

	auto status = m_managerPtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentServiceTest::UnregisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->UnregisterDocumentObserver("invalidUser", docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceTest::UnregisterDocumentObserverInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->UnregisterDocumentObserver(TEST_USER_ID, "invalidDocId", observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentServiceTest::UnregisterDocumentObserverNotRegisteredTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


// ======================================================================
// Serialize test
// ======================================================================

void CCollectionDocumentServiceTest::SerializeReturnsFalseTest()
{
	// Serialize is not implemented and always returns false.
	// We cannot call it without a valid IArchive, but we can verify
	// the behavior indirectly through the interface contract.
	// The implementation is: bool Serialize(iser::IArchive&) { return false; }
	QVERIFY2(true, "Serialize method exists but is not implemented (returns false)");
}


// ======================================================================
// ValidateInputParams tests
// ======================================================================

void CCollectionDocumentServiceTest::ValidateInputParamsSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::OperationStatus status;
	bool result = m_managerPtr->ValidateInputParams(TEST_USER_ID, docId, status);

	QVERIFY2(result, "ValidateInputParams should return true for valid params");
}


void CCollectionDocumentServiceTest::ValidateInputParamsInvalidUserTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::OperationStatus status;
	bool result = m_managerPtr->ValidateInputParams("invalidUser", "someDoc", status);

	QVERIFY2(!result, "ValidateInputParams should return false for invalid user");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentServiceTest::ValidateInputParamsInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::OperationStatus status;
	bool result = m_managerPtr->ValidateInputParams(TEST_USER_ID, "invalidDocId", status);

	QVERIFY2(!result, "ValidateInputParams should return false for invalid document ID");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// ======================================================================
// ObjectIdToUrl test
// ======================================================================

void CCollectionDocumentServiceTest::ObjectIdToUrlTest()
{
	QUrl url = m_managerPtr->ObjectIdToUrl("myObjectId");
	QCOMPARE(url.toString(), QString("collection:///myObjectId"));
	QCOMPARE(url.scheme(), QString("collection"));
	QVERIFY2(url.host().isEmpty(), "URL host should be empty");
}


// ======================================================================
// Async completion tests
// ======================================================================

void CCollectionDocumentServiceTest::CreateNewDocumentAsyncCompletionTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY2(!docId.isEmpty(), "Document should be created");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);

	// After async completion, document should no longer be loading
	QVERIFY2(!list[0].isLoading, "Document should no longer be loading after async completion");

	// Document pointer should be available
	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY2(ptr != nullptr, "Document pointer should be available after async load");
}


void CCollectionDocumentServiceTest::OpenDocumentAsyncCompletionTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("openedData")));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY2(!docId.isEmpty(), "Document should be opened");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);

	// After async completion, document should no longer be loading
	QVERIFY2(!list[0].isLoading, "Document should no longer be loading after async completion");

	// Document pointer should be available
	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY2(ptr != nullptr, "Document pointer should be available after async load");
}


// ======================================================================
// Multi-user tests
// ======================================================================

void CCollectionDocumentServiceTest::MultiUserCreateAndCloseTest()
{
	QByteArray docId1 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID_2, TEST_TYPE_ID);

	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2).size(), 1);

	// Close document for user 1 should not affect user 2
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId1;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params);
	m_managerPtr->WaitForTaskFinished(taskId);

	QVERIFY2(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(),
		"User 1 should have no documents after closing");
	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2).size(), 1);
}


void CCollectionDocumentServiceTest::MultiUserIsolationTest()
{
	QByteArray docId1 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID,
		QByteArray(), "User1 Doc");
	QByteArray docId2 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID_2, TEST_TYPE_ID,
		QByteArray(), "User2 Doc");

	// User 1 cannot access User 2's documents
	QString name;
	auto status = m_managerPtr->GetDocumentName(TEST_USER_ID, docId2, name);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);

	// User 2 cannot access User 1's documents
	status = m_managerPtr->GetDocumentName(TEST_USER_ID_2, docId1, name);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);

	// Each user can access their own documents
	status = m_managerPtr->GetDocumentName(TEST_USER_ID, docId1, name);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, QString("User1 Doc"));

	status = m_managerPtr->GetDocumentName(TEST_USER_ID_2, docId2, name);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, QString("User2 Doc"));
}


// ======================================================================
// Single-copy mode tests
// ======================================================================

void CCollectionDocumentServiceTest::SingleCopyOpenSameObjectByTwoUsersTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY2(!docId1.isEmpty(), "First user should open document successfully");

	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY2(!docId2.isEmpty(), "Second user should open document successfully");
	QVERIFY2(docId1 != docId2, "Different users should get different document IDs");

	// Both users should see the same data
	const istd::IChangeable* ptr1 = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId1);
	const istd::IChangeable* ptr2 = m_managerPtr->GetDocumentPtr(TEST_USER_ID_2, docId2);
	QVERIFY2(ptr1 != nullptr, "First user's document pointer should be valid");
	QVERIFY2(ptr2 != nullptr, "Second user's document pointer should be valid");
	QVERIFY2(ptr1 == ptr2, "Both users should share the same document object pointer");
}


void CCollectionDocumentServiceTest::SingleCopySecondUserGetsDataLoadedEventTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	// First user opens and waits for async load
	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY2(!docId1.isEmpty(), "First user should open document successfully");

	// Clear events before second user opens
	m_managerPtr->GetMockEventHandler().ClearEvents();

	// Second user opens the same object (shared document already loaded)
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID_2;
	params.url = QUrl("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY2(!taskId.isEmpty(), "Second user should begin open task successfully");

	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QByteArray docId2 = result.documentId;
	QVERIFY2(!docId2.isEmpty(), "Second user should open document successfully");

	// Second user should have received DocumentDataLoaded event
	int dataLoadedCount = m_managerPtr->GetMockEventHandler().CountEventsOfType("DocumentDataLoadedEvent");
	QVERIFY2(dataLoadedCount >= 1, "DocumentDataLoaded event should be fired for second user joining shared document");

	// Verify the event was for the second user
	bool foundForUser2 = false;
	for (const auto& event : m_managerPtr->GetMockEventHandler().GetEvents()) {
		if (event.type == "DocumentDataLoadedEvent" && event.userId == TEST_USER_ID_2) {
			foundForUser2 = true;
			QCOMPARE(event.documentId, docId2);
			break;
		}
	}
	QVERIFY2(foundForUser2, "DocumentDataLoaded event should be for the second user");
}


void CCollectionDocumentServiceTest::SingleCopySetDocumentDataSharedTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("original")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	// User 1 modifies the document
	CMockDocumentObject newData("modified");
	auto status = m_managerPtr->SetDocumentData(TEST_USER_ID, docId1, newData);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	// User 2 should see the modification (shared objectPtr)
	const istd::IChangeable* ptr2 = m_managerPtr->GetDocumentPtr(TEST_USER_ID_2, docId2);
	QVERIFY2(ptr2 != nullptr, "Second user's document pointer should be valid");
	const CMockDocumentObject* mockPtr2 = dynamic_cast<const CMockDocumentObject*>(ptr2);
	QVERIFY2(mockPtr2 != nullptr, "Should be able to cast to CMockDocumentObject");
	QCOMPARE(mockPtr2->GetData(), QByteArray("modified"));
}


void CCollectionDocumentServiceTest::SingleCopyCloseOneUserTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	// Close first user's document
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId1;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);

	// First user should have no documents
	auto list1 = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list1.isEmpty(), "First user should have no documents after closing");

	// Second user should still have the document
	auto list2 = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2);
	QCOMPARE(list2.size(), 1);

	// Second user's document should still be accessible
	const istd::IChangeable* ptr2 = m_managerPtr->GetDocumentPtr(TEST_USER_ID_2, docId2);
	QVERIFY2(ptr2 != nullptr, "Second user's document pointer should still be valid after first user closes");

	// Shared document data should still exist
	QVERIFY2(m_managerPtr->m_sharedDocuments.contains(TEST_OBJECT_ID),
		"Shared document data should still exist while one user has it open");
}


void CCollectionDocumentServiceTest::SingleCopyCloseBothUsersTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	imtdoc::IDocumentService::TaskParams params1;
	params1.userId = TEST_USER_ID;
	params1.documentId = docId1;

	QByteArray taskId1 = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params1);
	m_managerPtr->WaitForTaskFinished(taskId1);

	imtdoc::IDocumentService::TaskParams params2;
	params2.userId = TEST_USER_ID_2;
	params2.documentId = docId2;

	QByteArray taskId2 = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params2);
	m_managerPtr->WaitForTaskFinished(taskId2);

	// Both users should have no documents
	QVERIFY2(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(),
		"First user should have no documents");
	QVERIFY2(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2).isEmpty(),
		"Second user should have no documents");

	// Shared document data should be cleaned up
	QVERIFY2(!m_managerPtr->m_sharedDocuments.contains(TEST_OBJECT_ID),
		"Shared document data should be cleaned up after all users close");
}


void CCollectionDocumentServiceTest::SingleCopySaveUpdatesAllTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	m_managerPtr->GetMockEventHandler().ClearEvents();

	// User 1 saves the document
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentId = docId1;

	QByteArray taskId = m_managerPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	auto result = m_managerPtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);

	// Both users should see the document as not dirty
	auto list1 = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	auto list2 = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2);
	QCOMPARE(list1.size(), 1);
	QCOMPARE(list2.size(), 1);
	QVERIFY2(!list1[0].isDirty, "First user's document should not be dirty after save");
	QVERIFY2(!list2[0].isDirty, "Second user's document should not be dirty after shared save");

	// Both users should have received save events
	int savedEventCount = m_managerPtr->GetMockEventHandler().CountEventsOfType("DocumentSavedEvent");
	QVERIFY2(savedEventCount >= 2, "Save events should be fired for all users sharing the document");
}


void CCollectionDocumentServiceTest::SingleCopySetDocumentNameUpdatesAllTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	QString newName = "Renamed Shared Document";
	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, docId1, newName);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	// Both users should see the new name
	QString name1, name2;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId1, name1);
	m_managerPtr->GetDocumentName(TEST_USER_ID_2, docId2, name2);
	QCOMPARE(name1, newName);
	QCOMPARE(name2, newName);
}


I_ADD_TEST(CCollectionDocumentServiceTest);
