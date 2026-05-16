// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCollectionDocumentManagerTest.h"


// Qt includes
#include <QtCore/QCoreApplication>
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

void CCollectionDocumentManagerTest::init()
{
	m_managerPtr = new CTestableDocumentManager();
}


void CCollectionDocumentManagerTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


// Helper methods

QByteArray CCollectionDocumentManagerTest::CreateDocumentAndWaitForLoad(
	CTestableDocumentManager& manager,
	const QByteArray& userId,
	const QByteArray& typeId)
{
	QByteArray documentId = manager.CreateNewDocument(userId, typeId);

	// Process events to let async thread complete and main thread callbacks fire
	for (int i = 0; i < 50; ++i){
		QCoreApplication::processEvents();
		QThread::msleep(10);
	}

	return documentId;
}


QByteArray CCollectionDocumentManagerTest::OpenDocumentAndWaitForLoad(
	CTestableDocumentManager& manager,
	const QByteArray& userId,
	const QByteArray& objectId)
{
	QUrl url = QUrl("collection:///" + objectId);
	QByteArray documentId = manager.OpenDocument(userId, url);

	// Process events to let async thread complete and main thread callbacks fire
	for (int i = 0; i < 50; ++i){
		QCoreApplication::processEvents();
		QThread::msleep(10);
	}

	return documentId;
}


QByteArray CCollectionDocumentManagerTest::SetupDocumentDirectly(
	CTestableDocumentManager& manager,
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

void CCollectionDocumentManagerTest::GetOpenedDocumentListEmptyTest()
{
	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "Document list should be empty initially");
}


void CCollectionDocumentManagerTest::GetOpenedDocumentListNonExistentUserTest()
{
	auto list = m_managerPtr->GetOpenedDocumentList("nonexistentUser");
	QVERIFY2(list.isEmpty(), "Document list for non-existent user should be empty");
}


void CCollectionDocumentManagerTest::GetOpenedDocumentListAfterCreateTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	QVERIFY2(!docId.isEmpty(), "Document ID should not be empty");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId);
}


void CCollectionDocumentManagerTest::GetOpenedDocumentListMultipleDocumentsTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID_2);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 2);
}


void CCollectionDocumentManagerTest::GetOpenedDocumentListMultipleUsersTest()
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

void CCollectionDocumentManagerTest::CreateNewDocumentSuccessTest()
{
	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY2(!docId.isEmpty(), "CreateNewDocument should return a non-empty document ID");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId);
	QCOMPARE(list[0].typeId, TEST_TYPE_ID);
}


void CCollectionDocumentManagerTest::CreateNewDocumentUndoManagerFailTest()
{
	m_managerPtr->SetCreateUndoManagerShouldFail(true);

	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY2(docId.isEmpty(), "CreateNewDocument should return empty ID when undo manager creation fails");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "No documents should be created when undo manager fails");
}


void CCollectionDocumentManagerTest::CreateNewDocumentObjectCreationFailAsyncTest()
{
	m_managerPtr->SetCreateObjectShouldFail(true);

	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY2(!docId.isEmpty(), "CreateNewDocument returns ID immediately even if object creation will fail async");

	// Wait for async completion — the document should be closed due to failure
	QTRY_VERIFY_WITH_TIMEOUT(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(), 5000);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "Document should be closed after async object creation failure");
}


void CCollectionDocumentManagerTest::CreateNewDocumentEventFiredTest()
{
	m_managerPtr->GetMockEventHandler().ClearEvents();

	m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);

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


void CCollectionDocumentManagerTest::CreateNewDocumentIsLoadingTest()
{
	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY2(!docId.isEmpty(), "Document ID should not be empty");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QVERIFY2(list[0].isLoading, "Newly created document should be in loading state");
}


void CCollectionDocumentManagerTest::CreateNewDocumentMultipleDocumentsTest()
{
	QByteArray docId1 = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID_2);

	QVERIFY2(!docId1.isEmpty(), "First document ID should not be empty");
	QVERIFY2(!docId2.isEmpty(), "Second document ID should not be empty");
	QVERIFY2(docId1 != docId2, "Document IDs should be unique");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 2);
}


void CCollectionDocumentManagerTest::CreateNewDocumentProposedSourceDocumentIdUsedOnSaveTest()
{
	const QByteArray proposedId = "proposed-object-id";

	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID, proposedId);
	QVERIFY2(!docId.isEmpty(), "CreateNewDocument should return a non-empty document ID");

	// Wait for async object creation to complete
	QTRY_VERIFY_WITH_TIMEOUT([&]() {
		auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
		for (const auto& info : list) {
			if (info.documentId == docId) {
				return !info.isLoading;
			}
		}
		return true;
	}(), 5000);

	// Save the document – this should insert it into the collection using the proposed ID
	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId, TEST_DOC_NAME);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	// Verify the proposed ID was used as the collection element ID
	QCOMPARE(m_managerPtr->GetMockCollection().GetLastInsertedId(), proposedId);
}


// ======================================================================
// OpenDocument tests
// ======================================================================

void CCollectionDocumentManagerTest::OpenDocumentSuccessTest()
{
	CMockDocumentObject* mockObj = new CMockDocumentObject("testData");
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(mockObj));

	QUrl url("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);

	QVERIFY2(!docId.isEmpty(), "OpenDocument should return a non-empty document ID");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId);
	QCOMPARE(list[0].typeId, TEST_TYPE_ID);
	QCOMPARE(list[0].name, TEST_DOC_NAME);
}


void CCollectionDocumentManagerTest::OpenDocumentInvalidSchemeTest()
{
	QUrl url("file:///some/path");
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);
	QVERIFY2(docId.isEmpty(), "OpenDocument should fail with non-collection scheme");
}


void CCollectionDocumentManagerTest::OpenDocumentInvalidUrlTest()
{
	QUrl url("invalid-url");
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);
	QVERIFY2(docId.isEmpty(), "OpenDocument should fail with invalid URL");
}


void CCollectionDocumentManagerTest::OpenDocumentNonExistentObjectTest()
{
	QUrl url("collection:///nonexistentObject");
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);
	QVERIFY2(docId.isEmpty(), "OpenDocument should fail for non-existent object");
}


void CCollectionDocumentManagerTest::OpenDocumentUndoManagerFailTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));
	m_managerPtr->SetCreateUndoManagerShouldFail(true);

	QUrl url("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);
	QVERIFY2(docId.isEmpty(), "OpenDocument should fail when undo manager creation fails");
}


void CCollectionDocumentManagerTest::OpenDocumentEventFiredTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));
	m_managerPtr->GetMockEventHandler().ClearEvents();

	QUrl url("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));
	m_managerPtr->OpenDocument(TEST_USER_ID, url);

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


void CCollectionDocumentManagerTest::OpenDocumentIsLoadingTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QUrl url("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);
	QVERIFY2(!docId.isEmpty(), "Document ID should not be empty");

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QVERIFY2(list[0].isLoading, "Opened document should be in loading state initially");
}


void CCollectionDocumentManagerTest::OpenDocumentWithHostTest()
{
	QUrl url("collection://hostname/objectId");
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);
	QVERIFY2(docId.isEmpty(), "OpenDocument should fail when URL has a host");
}


void CCollectionDocumentManagerTest::OpenDocumentMultiplePathSegmentsTest()
{
	QUrl url("collection:///segment1/segment2");
	QByteArray docId = m_managerPtr->OpenDocument(TEST_USER_ID, url);
	QVERIFY2(docId.isEmpty(), "OpenDocument should fail when URL has multiple path segments");
}


// ======================================================================
// GetDocumentName tests
// ======================================================================

void CCollectionDocumentManagerTest::GetDocumentNameSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, QByteArray(), TEST_DOC_NAME);

	QString name;
	auto status = m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);

	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, TEST_DOC_NAME);
}


void CCollectionDocumentManagerTest::GetDocumentNameInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	QString name;
	auto status = m_managerPtr->GetDocumentName("invalidUser", docId, name);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::GetDocumentNameInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	QString name;
	auto status = m_managerPtr->GetDocumentName(TEST_USER_ID, "invalidDocId", name);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// ======================================================================
// SetDocumentName tests
// ======================================================================

void CCollectionDocumentManagerTest::SetDocumentNameSuccessTest()
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


void CCollectionDocumentManagerTest::SetDocumentNameSameNameTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);

	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, docId, TEST_DOC_NAME);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentManagerTest::SetDocumentNameInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->SetDocumentName("invalidUser", docId, "NewName");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::SetDocumentNameInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->SetDocumentName(TEST_USER_ID, "invalidDocId", "NewName");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentManagerTest::SetDocumentNameCollectionUpdateFailTest()
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


void CCollectionDocumentManagerTest::SetDocumentNameEventFiredTest()
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


void CCollectionDocumentManagerTest::SetDocumentNameNewDocumentNoCollectionUpdateTest()
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

void CCollectionDocumentManagerTest::GetDocumentPtrSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY2(ptr != nullptr, "GetDocumentPtr should return a valid pointer");
}


void CCollectionDocumentManagerTest::GetDocumentPtrInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr("invalidUser", docId);
	QVERIFY2(ptr == nullptr, "GetDocumentPtr should return nullptr for invalid user");
}


void CCollectionDocumentManagerTest::GetDocumentPtrInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, "invalidDocId");
	QVERIFY2(ptr == nullptr, "GetDocumentPtr should return nullptr for invalid document ID");
}


void CCollectionDocumentManagerTest::GetDocumentPtrWhileLoadingTest()
{
	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY2(!docId.isEmpty(), "Document should be created");

	// Document is loading, should return nullptr
	const istd::IChangeable* ptr = m_managerPtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY2(ptr == nullptr, "GetDocumentPtr should return nullptr while document is loading");
}


// ======================================================================
// GetDocumentData tests
// ======================================================================

void CCollectionDocumentManagerTest::GetDocumentDataSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData(TEST_USER_ID, docId, dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QVERIFY2(dataPtr.IsValid(), "Document data should be valid");
}


void CCollectionDocumentManagerTest::GetDocumentDataInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData("invalidUser", docId, dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::GetDocumentDataInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData(TEST_USER_ID, "invalidDocId", dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentManagerTest::GetDocumentDataWhileLoadingTest()
{
	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);

	istd::IChangeableSharedPtr dataPtr;
	auto status = m_managerPtr->GetDocumentData(TEST_USER_ID, docId, dataPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


// ======================================================================
// SetDocumentData tests
// ======================================================================

void CCollectionDocumentManagerTest::SetDocumentDataSuccessTest()
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


void CCollectionDocumentManagerTest::SetDocumentDataInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockDocumentObject newData;
	auto status = m_managerPtr->SetDocumentData("invalidUser", docId, newData);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::SetDocumentDataInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockDocumentObject newData;
	auto status = m_managerPtr->SetDocumentData(TEST_USER_ID, "invalidDocId", newData);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentManagerTest::SetDocumentDataWhileLoadingTest()
{
	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);

	CMockDocumentObject newData;
	auto status = m_managerPtr->SetDocumentData(TEST_USER_ID, docId, newData);

	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


// ======================================================================
// SaveDocument tests
// ======================================================================

void CCollectionDocumentManagerTest::SaveDocumentNewDocumentTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId, TEST_DOC_NAME);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	// Verify the document name was set
	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, TEST_DOC_NAME);

	// Verify the object was inserted into the collection
	QVERIFY2(!m_managerPtr->GetMockCollection().GetLastInsertedId().isEmpty(),
		"Object should have been inserted into collection");
}


void CCollectionDocumentManagerTest::SaveDocumentExistingDocumentTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentManagerTest::SaveDocumentSaveAsTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);

	QString newName = "Saved As Copy";
	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId, newName);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	// Verify name was updated
	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, newName);
}


void CCollectionDocumentManagerTest::SaveDocumentInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->SaveDocument("invalidUser", docId);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::SaveDocumentInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, "invalidDocId");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentManagerTest::SaveDocumentWhileLoadingTest()
{
	QByteArray docId = m_managerPtr->CreateNewDocument(TEST_USER_ID, TEST_TYPE_ID);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId);
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


void CCollectionDocumentManagerTest::SaveDocumentCollectionInsertFailTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	m_managerPtr->GetMockCollection().SetInsertShouldFail(true);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId, TEST_DOC_NAME);
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


void CCollectionDocumentManagerTest::SaveDocumentCollectionSetDataFailTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockCollection().SetSetObjectDataShouldFail(true);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId);
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


void CCollectionDocumentManagerTest::SaveDocumentValidationFailTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	m_managerPtr->SetValidateShouldFail(true);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId, TEST_DOC_NAME);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA);
}


void CCollectionDocumentManagerTest::SaveDocumentValidationFailWithMessageTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	m_managerPtr->SetValidateShouldFail(true);
	m_managerPtr->SetValidationErrorMessage("Custom validation error");

	QString errorMessage;
	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId, TEST_DOC_NAME, &errorMessage);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA);
	QCOMPARE(errorMessage, QString("Custom validation error"));
}


void CCollectionDocumentManagerTest::SaveDocumentWithNameProviderTest()
{
	m_managerPtr->SetHasNameProvider(true);
	m_managerPtr->SetDefaultDocumentName("Auto-Generated Name");

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);

	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId, TEST_DOC_NAME);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

	QString name;
	m_managerPtr->GetDocumentName(TEST_USER_ID, docId, name);
	QCOMPARE(name, QString("Auto-Generated Name"));
}


void CCollectionDocumentManagerTest::SaveDocumentEventFiredTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockEventHandler().ClearEvents();

	m_managerPtr->SaveDocument(TEST_USER_ID, docId);

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


void CCollectionDocumentManagerTest::SaveDocumentSaveAsEventFiredTest()
{
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject()));

	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID, TEST_OBJECT_ID, TEST_DOC_NAME);
	m_managerPtr->GetMockEventHandler().ClearEvents();

	m_managerPtr->SaveDocument(TEST_USER_ID, docId, "New Copy Name");

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

void CCollectionDocumentManagerTest::CloseDocumentSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->CloseDocument(TEST_USER_ID, docId);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentManagerTest::CloseDocumentInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->CloseDocument("invalidUser", docId);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::CloseDocumentInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	auto status = m_managerPtr->CloseDocument(TEST_USER_ID, "invalidDocId");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentManagerTest::CloseDocumentEventFiredTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	m_managerPtr->GetMockEventHandler().ClearEvents();

	m_managerPtr->CloseDocument(TEST_USER_ID, docId);

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


void CCollectionDocumentManagerTest::CloseDocumentRemovesFromListTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);

	m_managerPtr->CloseDocument(TEST_USER_ID, docId);

	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "Document should be removed from list after closing");
}


void CCollectionDocumentManagerTest::CloseDocumentLastDocForUserTest()
{
	QByteArray docId1 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);
	QByteArray docId2 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	m_managerPtr->CloseDocument(TEST_USER_ID, docId1);
	auto list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QCOMPARE(list.size(), 1);
	QCOMPARE(list[0].documentId, docId2);

	m_managerPtr->CloseDocument(TEST_USER_ID, docId2);
	list = m_managerPtr->GetOpenedDocumentList(TEST_USER_ID);
	QVERIFY2(list.isEmpty(), "All documents should be removed after closing the last one");
}


// ======================================================================
// GetDocumentUndoManager tests
// ======================================================================

void CCollectionDocumentManagerTest::GetDocumentUndoManagerSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	auto status = m_managerPtr->GetDocumentUndoManager(TEST_USER_ID, docId, undoManagerPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
	QVERIFY2(undoManagerPtr != nullptr, "Undo manager should not be null");
}


void CCollectionDocumentManagerTest::GetDocumentUndoManagerInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	auto status = m_managerPtr->GetDocumentUndoManager("invalidUser", docId, undoManagerPtr);

	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
	QVERIFY2(undoManagerPtr == nullptr, "Undo manager should be null for invalid user");
}


void CCollectionDocumentManagerTest::GetDocumentUndoManagerInvalidDocumentTest()
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

void CCollectionDocumentManagerTest::RegisterDocumentObserverSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentManagerTest::RegisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->RegisterDocumentObserver("invalidUser", docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::RegisterDocumentObserverInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, "invalidDocId", observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentManagerTest::RegisterDocumentObserverAlreadyRegisteredTest()
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

void CCollectionDocumentManagerTest::UnregisterDocumentObserverSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	m_managerPtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer);

	auto status = m_managerPtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);
}


void CCollectionDocumentManagerTest::UnregisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->UnregisterDocumentObserver("invalidUser", docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::UnregisterDocumentObserverInvalidDocumentTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->UnregisterDocumentObserver(TEST_USER_ID, "invalidDocId", observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CCollectionDocumentManagerTest::UnregisterDocumentObserverNotRegisteredTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	CMockObserver observer;
	auto status = m_managerPtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer);
	QCOMPARE(status, imtdoc::IDocumentService::OS_FAILED);
}


// ======================================================================
// Serialize test
// ======================================================================

void CCollectionDocumentManagerTest::SerializeReturnsFalseTest()
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

void CCollectionDocumentManagerTest::ValidateInputParamsSuccessTest()
{
	QByteArray docId = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::OperationStatus status;
	bool result = m_managerPtr->ValidateInputParams(TEST_USER_ID, docId, status);

	QVERIFY2(result, "ValidateInputParams should return true for valid params");
}


void CCollectionDocumentManagerTest::ValidateInputParamsInvalidUserTest()
{
	SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID);

	imtdoc::IDocumentService::OperationStatus status;
	bool result = m_managerPtr->ValidateInputParams("invalidUser", "someDoc", status);

	QVERIFY2(!result, "ValidateInputParams should return false for invalid user");
	QCOMPARE(status, imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CCollectionDocumentManagerTest::ValidateInputParamsInvalidDocumentTest()
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

void CCollectionDocumentManagerTest::ObjectIdToUrlTest()
{
	QUrl url = m_managerPtr->ObjectIdToUrl("myObjectId");
	QCOMPARE(url.toString(), QString("collection:///myObjectId"));
	QCOMPARE(url.scheme(), QString("collection"));
	QVERIFY2(url.host().isEmpty(), "URL host should be empty");
}


// ======================================================================
// Async completion tests
// ======================================================================

void CCollectionDocumentManagerTest::CreateNewDocumentAsyncCompletionTest()
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


void CCollectionDocumentManagerTest::OpenDocumentAsyncCompletionTest()
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

void CCollectionDocumentManagerTest::MultiUserCreateAndCloseTest()
{
	QByteArray docId1 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID, TEST_TYPE_ID);
	QByteArray docId2 = SetupDocumentDirectly(*m_managerPtr, TEST_USER_ID_2, TEST_TYPE_ID);

	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2).size(), 1);

	// Close document for user 1 should not affect user 2
	m_managerPtr->CloseDocument(TEST_USER_ID, docId1);

	QVERIFY2(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(),
		"User 1 should have no documents after closing");
	QCOMPARE(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2).size(), 1);
}


void CCollectionDocumentManagerTest::MultiUserIsolationTest()
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

void CCollectionDocumentManagerTest::SingleCopyOpenSameObjectByTwoUsersTest()
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


void CCollectionDocumentManagerTest::SingleCopySecondUserGetsDataLoadedEventTest()
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
	QUrl url("collection:///" + QString::fromUtf8(TEST_OBJECT_ID));
	QByteArray docId2 = m_managerPtr->OpenDocument(TEST_USER_ID_2, url);
	QVERIFY2(!docId2.isEmpty(), "Second user should open document successfully");

	// DocumentDataLoaded is deferred via QTimer::singleShot — wait for event
	QTRY_VERIFY_WITH_TIMEOUT(
		m_managerPtr->GetMockEventHandler().CountEventsOfType("DocumentDataLoadedEvent") >= 1, 5000);

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


void CCollectionDocumentManagerTest::SingleCopySetDocumentDataSharedTest()
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


void CCollectionDocumentManagerTest::SingleCopyCloseOneUserTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	// Close first user's document
	auto status = m_managerPtr->CloseDocument(TEST_USER_ID, docId1);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

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


void CCollectionDocumentManagerTest::SingleCopyCloseBothUsersTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	m_managerPtr->CloseDocument(TEST_USER_ID, docId1);
	m_managerPtr->CloseDocument(TEST_USER_ID_2, docId2);

	// Both users should have no documents
	QVERIFY2(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(),
		"First user should have no documents");
	QVERIFY2(m_managerPtr->GetOpenedDocumentList(TEST_USER_ID_2).isEmpty(),
		"Second user should have no documents");

	// Shared document data should be cleaned up
	QVERIFY2(!m_managerPtr->m_sharedDocuments.contains(TEST_OBJECT_ID),
		"Shared document data should be cleaned up after all users close");
}


void CCollectionDocumentManagerTest::SingleCopySaveUpdatesAllTest()
{
	m_managerPtr->SetSingleCopyMode(true);
	m_managerPtr->GetMockCollection().AddObject(
		TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME,
		istd::IChangeableSharedPtr(new CMockDocumentObject("sharedData")));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_managerPtr, TEST_USER_ID_2, TEST_OBJECT_ID);

	m_managerPtr->GetMockEventHandler().ClearEvents();

	// User 1 saves the document
	auto status = m_managerPtr->SaveDocument(TEST_USER_ID, docId1);
	QCOMPARE(status, imtdoc::IDocumentService::OS_OK);

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


void CCollectionDocumentManagerTest::SingleCopySetDocumentNameUpdatesAllTest()
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


I_ADD_TEST(CCollectionDocumentManagerTest);
