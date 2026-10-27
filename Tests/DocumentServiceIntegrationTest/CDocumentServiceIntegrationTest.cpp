// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDocumentServiceIntegrationTest.h"


// ACF includes
#include <idoc/IUndoManager.h>
#include <iprm/CTextParam.h>
#include <iser/CMemoryWriteArchive.h>


namespace
{

const QByteArray TEST_USER_ID = QByteArrayLiteral("testUser");
const QByteArray TEST_USER_ID_2 = QByteArrayLiteral("testUser2");
const QByteArray TEST_TYPE_ID = QByteArrayLiteral("TestType");
const QByteArray TEST_TYPE_ID_2 = QByteArrayLiteral("TestType2");
const QByteArray TEST_OBJECT_ID = QByteArrayLiteral("testObject123");
const QString TEST_DOC_NAME = QStringLiteral("Test Document");

const QByteArray CREATED_EVENT = QByteArrayLiteral("DocumentCreatedEvent");
const QByteArray DATA_LOADED_EVENT = QByteArrayLiteral("DocumentDataLoadedEvent");
const QByteArray CLOSED_EVENT = QByteArrayLiteral("DocumentClosedEvent");

const int WAIT_TIMEOUT = 5000;

}


// private slots of the class CDocumentServiceIntegrationTest

// Setup and teardown

void CDocumentServiceIntegrationTest::init()
{
	const QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	const QString registryFile = imtCorePath + QStringLiteral("/Tests/DocumentServiceIntegrationTest/Partitura/DocumentServiceIntegrationTest.acc");
	const QString configFile = imtCorePath + QStringLiteral("/Config/ImtCoreTest.awc");

	m_accessorPtr.SetPtr(new ipackage::CComponentAccessor(registryFile, configFile));

	m_servicePtr = m_accessorPtr->GetComponentInterface<imtdoc::IDocumentService>(QByteArrayLiteral("DocumentService"));
	QVERIFY2(m_servicePtr != nullptr, "Document service should be created");

	m_servicePtr->RegisterEventHandler(m_eventHandler);

	m_eventHandler.ClearEvents();
}


void CDocumentServiceIntegrationTest::cleanup()
{
	if (m_servicePtr != nullptr){
		for (const QByteArray& userId : {TEST_USER_ID, TEST_USER_ID_2}){
			for (const imtdoc::IDocumentService::DocumentListItem& item : m_servicePtr->GetOpenedDocumentList(userId)){
				WaitForDocumentLoaded(*m_servicePtr, userId, item.documentId);
			}
		}

		m_servicePtr->UnregisterEventHandler(m_eventHandler);
	}

	m_servicePtr = nullptr;

	m_accessorPtr.Reset();

	m_eventHandler.ClearEvents();
}


// GetOpenedDocumentList tests

void CDocumentServiceIntegrationTest::GetOpenedDocumentListEmptyTest()
{
	QVERIFY2(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(), "Document list should be empty initially");
}


void CDocumentServiceIntegrationTest::GetOpenedDocumentListNonExistentUserTest()
{
	QVERIFY2(m_servicePtr->GetOpenedDocumentList(QByteArrayLiteral("nonexistentUser")).isEmpty(), "Document list for non-existent user should be empty");
}


void CDocumentServiceIntegrationTest::GetOpenedDocumentListAfterCreateTest()
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


void CDocumentServiceIntegrationTest::GetOpenedDocumentListMultipleDocumentsTest()
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


void CDocumentServiceIntegrationTest::GetOpenedDocumentListMultipleUsersTest()
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

void CDocumentServiceIntegrationTest::CreateNewDocumentSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY2(!docId.isEmpty(), "Document should be created");

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QCOMPARE(info.documentId, docId);
	QCOMPARE(info.name, TEST_DOC_NAME);
	QVERIFY(m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId) != nullptr);
}


void CDocumentServiceIntegrationTest::CreateNewDocumentUndoManagerFailTest()
{
	QSKIP("The undo manager factory is always configured in the integration registry");
}


void CDocumentServiceIntegrationTest::CreateNewDocumentObjectCreationFailAsyncTest()
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


void CDocumentServiceIntegrationTest::CreateNewDocumentEventFiredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QVERIFY2(m_eventHandler.HasEvent(CREATED_EVENT, TEST_USER_ID), "DocumentCreated event should be fired");
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);
}


void CDocumentServiceIntegrationTest::CreateNewDocumentIsLoadingTest()
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


void CDocumentServiceIntegrationTest::CreateNewDocumentMultipleDocumentsTest()
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


// GetDocumentName tests

void CDocumentServiceIntegrationTest::GetDocumentNameSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, TEST_DOC_NAME);
}


void CDocumentServiceIntegrationTest::GetDocumentNameInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(QByteArrayLiteral("invalidUser"), docId, name), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CDocumentServiceIntegrationTest::GetDocumentNameInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// GetDocumentPtr tests

void CDocumentServiceIntegrationTest::GetDocumentPtrSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	const istd::IChangeable* ptr = m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId);
	QVERIFY(ptr != nullptr);
	QVERIFY(dynamic_cast<const iprm::ITextParam*>(ptr) != nullptr);
}


void CDocumentServiceIntegrationTest::GetDocumentPtrInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QVERIFY(m_servicePtr->GetDocumentPtr(QByteArrayLiteral("invalidUser"), docId) == nullptr);
}


void CDocumentServiceIntegrationTest::GetDocumentPtrInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QVERIFY(m_servicePtr->GetDocumentPtr(TEST_USER_ID, QByteArrayLiteral("invalidDocId")) == nullptr);
}


void CDocumentServiceIntegrationTest::GetDocumentPtrWhileLoadingTest()
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

void CDocumentServiceIntegrationTest::GetDocumentDataSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("testData"));
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, newData), imtdoc::IDocumentService::OS_OK);

	istd::IChangeableSharedPtr dataPtr;
	QCOMPARE(m_servicePtr->GetDocumentData(TEST_USER_ID, docId, dataPtr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(dataPtr.IsValid());

	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(dataPtr.GetPtr());
	QVERIFY(textPtr != nullptr);
	QCOMPARE(textPtr->GetText(), QStringLiteral("testData"));

	// Returned data is a copy
	QVERIFY(dataPtr.GetPtr() != m_servicePtr->GetDocumentPtr(TEST_USER_ID, docId));
}


void CDocumentServiceIntegrationTest::GetDocumentDataInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	istd::IChangeableSharedPtr dataPtr;
	QCOMPARE(m_servicePtr->GetDocumentData(QByteArrayLiteral("invalidUser"), docId, dataPtr), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CDocumentServiceIntegrationTest::GetDocumentDataInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	istd::IChangeableSharedPtr dataPtr;
	QCOMPARE(m_servicePtr->GetDocumentData(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), dataPtr), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CDocumentServiceIntegrationTest::GetDocumentDataWhileLoadingTest()
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

void CDocumentServiceIntegrationTest::SetDocumentDataSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	newData.SetText(QStringLiteral("newData"));

	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, docId, newData), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("newData"));
}


void CDocumentServiceIntegrationTest::SetDocumentDataInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	iprm::CTextParam newData;
	QCOMPARE(m_servicePtr->SetDocumentData(QByteArrayLiteral("invalidUser"), docId, newData), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CDocumentServiceIntegrationTest::SetDocumentDataInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	iprm::CTextParam newData;
	QCOMPARE(m_servicePtr->SetDocumentData(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), newData), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CDocumentServiceIntegrationTest::SetDocumentDataWhileLoadingTest()
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


// CloseDocument tests

void CDocumentServiceIntegrationTest::CloseDocumentSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
}


void CDocumentServiceIntegrationTest::CloseDocumentInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, QByteArrayLiteral("invalidUser"), docId), imtdoc::IDocumentService::OS_INVALID_USER_ID);
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
}


void CDocumentServiceIntegrationTest::CloseDocumentInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, QByteArrayLiteral("invalidDocId")), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).size(), 1);
}


void CDocumentServiceIntegrationTest::CloseDocumentEventFiredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	m_eventHandler.ClearEvents();

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY2(m_eventHandler.HasEvent(CLOSED_EVENT, TEST_USER_ID), "DocumentClosed event should be fired");
}


void CDocumentServiceIntegrationTest::CloseDocumentRemovesFromListTest()
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


void CDocumentServiceIntegrationTest::CloseDocumentLastDocForUserTest()
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

void CDocumentServiceIntegrationTest::GetDocumentUndoManagerSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(TEST_USER_ID, docId, undoManagerPtr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(undoManagerPtr != nullptr);
}


void CDocumentServiceIntegrationTest::GetDocumentUndoManagerInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(QByteArrayLiteral("invalidUser"), docId, undoManagerPtr), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CDocumentServiceIntegrationTest::GetDocumentUndoManagerInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), undoManagerPtr), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// RegisterDocumentObserver tests

void CDocumentServiceIntegrationTest::RegisterDocumentObserverSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QVERIFY(observer.IsModelAttached(nullptr));
}


void CDocumentServiceIntegrationTest::RegisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(QByteArrayLiteral("invalidUser"), docId, observer), imtdoc::IDocumentService::OS_INVALID_USER_ID);
	QVERIFY(!observer.IsModelAttached(nullptr));
}


void CDocumentServiceIntegrationTest::RegisterDocumentObserverInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), observer), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
	QVERIFY(!observer.IsModelAttached(nullptr));
}


void CDocumentServiceIntegrationTest::RegisterDocumentObserverAlreadyRegisteredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_FAILED);
}


// UnregisterDocumentObserver tests

void CDocumentServiceIntegrationTest::UnregisterDocumentObserverSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->RegisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_OK);
	QVERIFY(!observer.IsModelAttached(nullptr));
}


void CDocumentServiceIntegrationTest::UnregisterDocumentObserverInvalidUserTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(QByteArrayLiteral("invalidUser"), docId, observer), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CDocumentServiceIntegrationTest::UnregisterDocumentObserverInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), observer), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CDocumentServiceIntegrationTest::UnregisterDocumentObserverNotRegisteredTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	CTestObserver observer;
	QCOMPARE(m_servicePtr->UnregisterDocumentObserver(TEST_USER_ID, docId, observer), imtdoc::IDocumentService::OS_FAILED);
}


// Serialize tests

void CDocumentServiceIntegrationTest::SerializeReturnsFalseTest()
{
	iser::CMemoryWriteArchive archive;
	QVERIFY2(!m_servicePtr->Serialize(archive), "Serialize should return false");
}


// ValidateInputParams tests (checked through GetDocumentName)

void CDocumentServiceIntegrationTest::ValidateInputParamsSuccessTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
}


void CDocumentServiceIntegrationTest::ValidateInputParamsInvalidUserTest()
{
	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(QByteArrayLiteral("invalidUser"), QByteArrayLiteral("anyDocId"), name), imtdoc::IDocumentService::OS_INVALID_USER_ID);
}


void CDocumentServiceIntegrationTest::ValidateInputParamsInvalidDocumentTest()
{
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID).isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, QByteArrayLiteral("invalidDocId"), name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


void CDocumentServiceIntegrationTest::ValidateInputParamsForeignDocumentTest()
{
	QByteArray docId1 = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, TEST_TYPE_ID).isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID_2, docId1, name), imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID);
}


// Document storage tests

void CDocumentServiceIntegrationTest::DocumentStorageInsertRemoveTest()
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


// Async document loading completion tests

void CDocumentServiceIntegrationTest::CreateNewDocumentAsyncCompletionTest()
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


// Multi-user scenario tests

void CDocumentServiceIntegrationTest::MultiUserCreateAndCloseTest()
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


void CDocumentServiceIntegrationTest::MultiUserIsolationTest()
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



// Collection related operations

void CDocumentServiceIntegrationTest::OpenDocumentNotSupportedTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.url = QUrl(QStringLiteral("collection:///") + TEST_OBJECT_ID);

	QByteArray docId;
	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params, &docId), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(docId.isEmpty());
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CDocumentServiceIntegrationTest::SaveDocumentNotSupportedTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_FAILED);

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QVERIFY2(info.isDirty, "Document should stay dirty");
	QVERIFY(info.url.isEmpty());
}


void CDocumentServiceIntegrationTest::SetDocumentNameNotSupportedTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, docId, QStringLiteral("NewName")), imtdoc::IDocumentService::OS_FAILED);

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, TEST_DOC_NAME);
}


I_ADD_TEST(CDocumentServiceIntegrationTest);


