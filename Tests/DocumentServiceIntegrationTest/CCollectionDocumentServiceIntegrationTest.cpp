// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCollectionDocumentServiceIntegrationTest.h"


// ACF includes
#include <idoc/IUndoManager.h>
#include <iprm/CTextParam.h>
#include <iprm/IIdParam.h>
#include <imeas/INumericValue.h>

// ImtCore includes
#include <imtdoc/imtdoc.h>


namespace
{

const QByteArray TEST_USER_ID = QByteArrayLiteral("testUser");
const QByteArray TEST_USER_ID_2 = QByteArrayLiteral("testUser2");
const QByteArray TEST_TYPE_ID = QByteArrayLiteral("TestType");
// Name provider of this type appends the object ID and the revision from the document source parameters
const QByteArray SOURCE_NAME_TYPE_ID = QByteArrayLiteral("TestType2");
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

const QByteArray OPENED_EVENT = QByteArrayLiteral("DocumentOpenedEvent");
const QByteArray DATA_LOADED_EVENT = QByteArrayLiteral("DocumentDataLoadedEvent");
const QByteArray RENAMED_EVENT = QByteArrayLiteral("DocumentRenamedEvent");
const QByteArray SAVED_EVENT = QByteArrayLiteral("DocumentSavedEvent");
const QByteArray SAVED_AS_EVENT = QByteArrayLiteral("DocumentSavedAsEvent");
const QByteArray UNDO_REDO_CHANGED_EVENT = QByteArrayLiteral("DocumentUndoRedoChangedEvent");

const int WAIT_TIMEOUT = 5000;


}


// private slots of the class CCollectionDocumentServiceIntegrationTest

// Setup and teardown

void CCollectionDocumentServiceIntegrationTest::init()
{
	const QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	const QString registryFile = imtCorePath + QStringLiteral("/Tests/DocumentServiceIntegrationTest/Partitura/CollectionDocumentServiceIntegrationTest.acc");
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


QByteArray CCollectionDocumentServiceIntegrationTest::OpenDocumentAndWaitForLoad(
			imtdoc::IDocumentService& service,
			const QByteArray& userId,
			const QByteArray& objectId,
			int revision)
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = userId;
	params.documentSourceRef = imtdoc::ObjectIdToSourceRef(objectId, revision);

	QByteArray documentId;
	if (ExecuteTask(service, imtdoc::IDocumentService::TT_OPEN, params, &documentId) != imtdoc::IDocumentService::OS_OK){
		return QByteArray();
	}

	if (!WaitForDocumentLoaded(service, userId, documentId)){
		return QByteArray();
	}

	return documentId;
}


int CCollectionDocumentServiceIntegrationTest::CreateObjectRevision(
			const QByteArray& objectId,
			const QString& revisionText,
			const QString& currentText)
{
	const imtbase::IRevisionController* revisionControllerPtr = m_collectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		return -1;
	}

	iprm::CTextParam data;
	data.SetText(revisionText);
	if (!m_collectionPtr->SetObjectData(objectId, data)){
		return -1;
	}

	const int revision = revisionControllerPtr->BackupRevision(*m_collectionPtr, objectId);

	data.SetText(currentText);
	if (!m_collectionPtr->SetObjectData(objectId, data)){
		return -1;
	}

	return revision;
}


// CreateNewDocument tests

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
	QCOMPARE(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef, TEST_OBJECT_ID);
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
	QCOMPARE(info.documentSourceRef, TEST_OBJECT_ID);
	QVERIFY(!info.isDirty);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("testData"));
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentEmptySourceRefTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentInvalidRevisionTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentSourceRef = TEST_OBJECT_ID + QByteArrayLiteral("/abc");

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentNonExistentObjectTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentSourceRef = imtdoc::ObjectIdToSourceRef(QByteArrayLiteral("nonExistentObject"));

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
	params.documentSourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID);

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
	params.documentSourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID);

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


void CCollectionDocumentServiceIntegrationTest::OpenDocumentNegativeRevisionTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentSourceRef = TEST_OBJECT_ID + QByteArrayLiteral("/-1");

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentTooManySegmentsTest()
{
	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentSourceRef = TEST_OBJECT_ID + QByteArrayLiteral("/1/2");

	QCOMPARE(ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params), imtdoc::IDocumentService::OS_FAILED);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentWithoutSingleInstanceModeFailsWhenSingleInstanceAlreadyOpenedTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams singleInstanceParams;
	singleInstanceParams.userId = TEST_USER_ID;
	singleInstanceParams.documentSourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID);
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

	const QByteArray objectId = imtdoc::SourceRefToObjectId(info.documentSourceRef);
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
	QCOMPARE(info.documentSourceRef, TEST_OBJECT_ID);
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


void CCollectionDocumentServiceIntegrationTest::SaveDocumentNewDocumentUpdatesSourceRefTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());
	QVERIFY(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef.isEmpty());

	const imtbase::ICollectionInfo::Ids idsBefore = m_collectionPtr->GetElementIds();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	const QByteArray objectId = imtdoc::SourceRefToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef);
	QVERIFY2(!objectId.isEmpty(), "Saved document should have a source reference");
	QVERIFY(!idsBefore.contains(objectId));
	QVERIFY(m_collectionPtr->GetElementIds().contains(objectId));
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentSaveAsUpdatesSourceRefTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("Copy Name")), imtdoc::IDocumentService::OS_OK);

	const QByteArray objectId = imtdoc::SourceRefToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef);
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
	QVERIFY(info.documentSourceRef.isEmpty());
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
	QVERIFY(info.documentSourceRef.isEmpty());

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

	const QByteArray objectId = imtdoc::SourceRefToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef);
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


// Undo/redo tests

void CCollectionDocumentServiceIntegrationTest::UndoRedoOpenedDocumentDirtyFlagTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("original"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());
	QVERIFY(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(TEST_USER_ID, docId, undoManagerPtr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(undoManagerPtr != nullptr);
	QCOMPARE(undoManagerPtr->GetAvailableUndoSteps(), 0);

	QCOMPARE(SetDocumentText(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("modified")), imtdoc::IDocumentService::OS_OK);
	QVERIFY2(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty, "Document should be dirty after change");

	QVERIFY(undoManagerPtr->DoUndo());
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("original"));
	QVERIFY2(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty, "Document should not be dirty after undo to the opened state");

	QVERIFY(undoManagerPtr->DoRedo());
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("modified"));
	QVERIFY2(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty, "Document should be dirty after redo");
}


void CCollectionDocumentServiceIntegrationTest::UndoAfterSaveMakesDocumentDirtyTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("original"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(TEST_USER_ID, docId, undoManagerPtr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(undoManagerPtr != nullptr);

	QCOMPARE(SetDocumentText(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("modified")), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty);

	m_eventHandler.ClearEvents();
	QVERIFY(undoManagerPtr->DoUndo());
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("original"));
	QVERIFY2(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty, "Document should be dirty after undo behind the saved state");
	QVERIFY2(m_eventHandler.HasEvent(UNDO_REDO_CHANGED_EVENT, TEST_USER_ID), "DocumentUndoRedoChanged event should be fired on undo");

	const QList<CRecordingEventHandler::RecordedEvent> events = m_eventHandler.GetEvents();
	for (const CRecordingEventHandler::RecordedEvent& event : events){
		if (event.type == UNDO_REDO_CHANGED_EVENT){
			QVERIFY(event.isDirty);
		}
	}

	m_eventHandler.ClearEvents();
	QVERIFY(undoManagerPtr->DoRedo());
	QVERIFY2(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).isDirty, "Document should not be dirty after redo to the saved state");
}


// Document source reference tests

void CCollectionDocumentServiceIntegrationTest::OpenedDocumentSourceRefTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef, TEST_OBJECT_ID);
}


void CCollectionDocumentServiceIntegrationTest::SavedDocumentSourceRefTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	QVERIFY(!docId.isEmpty());
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	// Source reference of the saved document is the ID of the inserted collection object
	const QByteArray objectId = imtdoc::SourceRefToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef);
	QVERIFY(!objectId.isEmpty());
	QVERIFY(m_collectionPtr->GetElementIds().contains(objectId));

	// The inserted object can be opened
	QByteArray openedDocId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID_2, objectId);
	QVERIFY(!openedDocId.isEmpty());
	QCOMPARE(FindDocumentInfo(*m_servicePtr, TEST_USER_ID_2, openedDocId).documentSourceRef, objectId);
}


// Async document loading completion tests

void CCollectionDocumentServiceIntegrationTest::OpenDocumentAsyncCompletionTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("testData"));

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentSourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID);

	QByteArray taskId = m_servicePtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	QVERIFY(!taskId.isEmpty());

	imtdoc::IDocumentService::TaskResult result = m_servicePtr->WaitForTaskFinished(taskId);
	QCOMPARE(result.status, imtdoc::IDocumentService::OS_OK);
	QVERIFY(!result.documentId.isEmpty());

	QTRY_VERIFY_WITH_TIMEOUT(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, result.documentId).isLoading, WAIT_TIMEOUT);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, result.documentId), QStringLiteral("testData"));
	QTRY_VERIFY_WITH_TIMEOUT(m_eventHandler.HasEvent(DATA_LOADED_EVENT, TEST_USER_ID), WAIT_TIMEOUT);
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
	params.documentSourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID);

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

	const QByteArray copySourceRef = FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID, docId).documentSourceRef;
	QVERIFY(copySourceRef != TEST_OBJECT_ID);
	QVERIFY(m_collectionPtr->GetElementIds().contains(copySourceRef));

	// Both users hold the same document, which is now backed by the copy
	QCOMPARE(FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID_2, docId).documentSourceRef, copySourceRef);
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



void CCollectionDocumentServiceIntegrationTest::SingleCopyUndoRedoSharedTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME, QStringLiteral("original"));

	QByteArray docId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QByteArray docId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!docId1.isEmpty());
	QVERIFY(!docId2.isEmpty());

	idoc::IUndoManager* undoManager1Ptr = nullptr;
	idoc::IUndoManager* undoManager2Ptr = nullptr;
	QCOMPARE(m_singleCopyServicePtr->GetDocumentUndoManager(TEST_USER_ID, docId1, undoManager1Ptr), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(m_singleCopyServicePtr->GetDocumentUndoManager(TEST_USER_ID_2, docId2, undoManager2Ptr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(undoManager1Ptr != nullptr);
	QCOMPARE(undoManager1Ptr, undoManager2Ptr);

	m_eventHandler.ClearEvents();
	QCOMPARE(SetDocumentText(*m_singleCopyServicePtr, TEST_USER_ID, docId1, QStringLiteral("modified")), imtdoc::IDocumentService::OS_OK);
	QVERIFY2(m_eventHandler.HasEvent(UNDO_REDO_CHANGED_EVENT, TEST_USER_ID), "DocumentUndoRedoChanged event should be fired for the first user");
	QVERIFY2(m_eventHandler.HasEvent(UNDO_REDO_CHANGED_EVENT, TEST_USER_ID_2), "DocumentUndoRedoChanged event should be fired for the second user");

	// Undo by the second user reverts the change of the first user
	m_eventHandler.ClearEvents();
	QVERIFY(undoManager2Ptr->DoUndo());
	QCOMPARE(GetDocumentText(*m_singleCopyServicePtr, TEST_USER_ID, docId1), QStringLiteral("original"));
	QVERIFY(!FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID, docId1).isDirty);
	QVERIFY(!FindDocumentInfo(*m_singleCopyServicePtr, TEST_USER_ID_2, docId2).isDirty);
	QVERIFY2(m_eventHandler.HasEvent(UNDO_REDO_CHANGED_EVENT, TEST_USER_ID), "DocumentUndoRedoChanged event should be fired for the first user on undo");
	QVERIFY2(m_eventHandler.HasEvent(UNDO_REDO_CHANGED_EVENT, TEST_USER_ID_2), "DocumentUndoRedoChanged event should be fired for the second user on undo");
}


I_ADD_TEST(CCollectionDocumentServiceIntegrationTest);


// Object revision tests

void CCollectionDocumentServiceIntegrationTest::RevisionSourceRefHelpersTest()
{
	const QByteArray sourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID, 3);
	QCOMPARE(sourceRef, TEST_OBJECT_ID + QByteArrayLiteral("/3"));
	QCOMPARE(imtdoc::SourceRefToObjectId(sourceRef), TEST_OBJECT_ID);
	QCOMPARE(imtdoc::SourceRefToRevision(sourceRef), 3);

	const QByteArray headSourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID);
	QCOMPARE(headSourceRef, TEST_OBJECT_ID);
	QCOMPARE(imtdoc::SourceRefToObjectId(headSourceRef), TEST_OBJECT_ID);
	QCOMPARE(imtdoc::SourceRefToRevision(headSourceRef), -1);

	QVERIFY(imtdoc::ObjectIdToSourceRef(QByteArray(), 3).isEmpty());

	const QByteArray invalidRevisionRef = TEST_OBJECT_ID + QByteArrayLiteral("/abc");
	QVERIFY(imtdoc::SourceRefToObjectId(invalidRevisionRef).isEmpty());
	QCOMPARE(imtdoc::SourceRefToRevision(invalidRevisionRef), -1);

	QVERIFY(imtdoc::SourceRefToObjectId(TEST_OBJECT_ID + QByteArrayLiteral("/-1")).isEmpty());

	const QByteArray tooManySegmentsRef = TEST_OBJECT_ID + QByteArrayLiteral("/1/2");
	QVERIFY(imtdoc::SourceRefToObjectId(tooManySegmentsRef).isEmpty());
	QCOMPARE(imtdoc::SourceRefToRevision(tooManySegmentsRef), -1);

	QVERIFY(imtdoc::ObjectIdToSourceRef(QByteArray()).isEmpty());
	QVERIFY(imtdoc::SourceRefToObjectId(QByteArray()).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentRevisionTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("revisionData"));

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QCOMPARE(info.documentSourceRef, imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID, revision));
	QCOMPARE(info.typeId, TEST_TYPE_ID);
	QVERIFY(!info.isDirty);

	const QByteArray revisionSourceRef = TEST_OBJECT_ID + '/' + QByteArray::number(revision);
	bool hasOpenedEvent = false;
	for (const CRecordingEventHandler::RecordedEvent& event : m_eventHandler.GetEvents()){
		if (event.type == OPENED_EVENT && event.documentId == docId){
			QCOMPARE(event.documentSourceRef, revisionSourceRef);
			hasOpenedEvent = true;
		}
	}
	QVERIFY(hasOpenedEvent);
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentRevisionListNameSuffixTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	const QString expectedName = TEST_DOC_NAME + QStringLiteral(" (Rev #%1)").arg(revision);

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QCOMPARE(info.name, expectedName);
	QVERIFY(info.isReadOnly);

	// The suffix is a part of the document name
	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, expectedName);

	// The collection object is not renamed
	QCOMPARE(m_collectionPtr->GetElementInfo(TEST_OBJECT_ID, imtbase::ICollectionInfo::EIT_NAME).toString(), TEST_DOC_NAME);
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentNonExistentRevisionTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);

	imtdoc::IDocumentService::TaskParams params;
	params.userId = TEST_USER_ID;
	params.documentSourceRef = imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID, 42);

	ExecuteTask(*m_servicePtr, imtdoc::IDocumentService::TT_OPEN, params);

	QTRY_VERIFY_WITH_TIMEOUT(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty(), WAIT_TIMEOUT);
}


void CCollectionDocumentServiceIntegrationTest::OpenDocumentRevisionAndHeadSeparateTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray headDocId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!headDocId.isEmpty());

	QByteArray revisionDocId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!revisionDocId.isEmpty());
	QVERIFY(revisionDocId != headDocId);

	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, headDocId), QStringLiteral("currentData"));
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, revisionDocId), QStringLiteral("revisionData"));

	QCOMPARE(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, headDocId).name, TEST_DOC_NAME);
	QVERIFY(!FindDocumentInfo(*m_servicePtr, TEST_USER_ID, headDocId).isReadOnly);
	QVERIFY(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, revisionDocId).isReadOnly);
	QCOMPARE(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).count(), 2);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentRevisionFailsTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	const int elementCount = m_collectionPtr->GetElementIds().count();

	m_eventHandler.ClearEvents();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_FAILED);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount);
	QVERIFY(!m_eventHandler.HasEvent(SAVED_EVENT, TEST_USER_ID));

	// The current object data is not changed
	imtbase::IObjectCollection::DataPtr dataPtr;
	QVERIFY(m_collectionPtr->GetObjectData(TEST_OBJECT_ID, dataPtr));
	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(dataPtr.GetPtr());
	QVERIFY(textPtr != nullptr);
	QCOMPARE(textPtr->GetText(), QStringLiteral("currentData"));

	// The document still addresses the revision
	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QCOMPARE(info.documentSourceRef, imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID, revision));
	QVERIFY(info.isReadOnly);
}


void CCollectionDocumentServiceIntegrationTest::SaveDocumentSaveAsRevisionFailsTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	const int elementCount = m_collectionPtr->GetElementIds().count();

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("Copy Name")), imtdoc::IDocumentService::OS_FAILED);
	QCOMPARE(m_collectionPtr->GetElementIds().count(), elementCount);

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QCOMPARE(info.documentSourceRef, imtdoc::ObjectIdToSourceRef(TEST_OBJECT_ID, revision));
	QCOMPARE(info.name, TEST_DOC_NAME + QStringLiteral(" (Rev #%1)").arg(revision));
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentNameRevisionFailsTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(m_servicePtr->SetDocumentName(TEST_USER_ID, docId, QStringLiteral("New Name")), imtdoc::IDocumentService::OS_FAILED);

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, TEST_DOC_NAME + QStringLiteral(" (Rev #%1)").arg(revision));
	QCOMPARE(m_collectionPtr->GetElementInfo(TEST_OBJECT_ID, imtbase::ICollectionInfo::EIT_NAME).toString(), TEST_DOC_NAME);
}


void CCollectionDocumentServiceIntegrationTest::SetDocumentDataRevisionFailsTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(SetDocumentText(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("modified")), imtdoc::IDocumentService::OS_FAILED);
	QCOMPARE(GetDocumentText(*m_servicePtr, TEST_USER_ID, docId), QStringLiteral("revisionData"));

	imtdoc::IDocumentService::DocumentListItem info = FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId);
	QVERIFY(!info.isDirty);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	QCOMPARE(m_servicePtr->GetDocumentUndoManager(TEST_USER_ID, docId, undoManagerPtr), imtdoc::IDocumentService::OS_OK);
	QVERIFY(undoManagerPtr != nullptr);
	QCOMPARE(undoManagerPtr->GetAvailableUndoSteps(), 0);
}


void CCollectionDocumentServiceIntegrationTest::CloseDocumentRevisionTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(CloseDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);
	QVERIFY(m_servicePtr->GetOpenedDocumentList(TEST_USER_ID).isEmpty());
}


void CCollectionDocumentServiceIntegrationTest::SingleCopyRevisionSharedTest()
{
	AddCollectionObject(TEST_OBJECT_ID, TEST_TYPE_ID, TEST_DOC_NAME);
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray revisionDocId1 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!revisionDocId1.isEmpty());

	QByteArray revisionDocId2 = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID, revision);
	QCOMPARE(revisionDocId2, revisionDocId1);

	QByteArray headDocId = OpenDocumentAndWaitForLoad(*m_singleCopyServicePtr, TEST_USER_ID_2, TEST_OBJECT_ID);
	QVERIFY(!headDocId.isEmpty());
	QVERIFY(headDocId != revisionDocId1);

	QCOMPARE(GetDocumentText(*m_singleCopyServicePtr, TEST_USER_ID_2, revisionDocId2), QStringLiteral("revisionData"));
	QCOMPARE(GetDocumentText(*m_singleCopyServicePtr, TEST_USER_ID_2, headDocId), QStringLiteral("currentData"));
}


// Document source parameters tests

void CCollectionDocumentServiceIntegrationTest::DocumentSourceParamsHelpersTest()
{
	iprm::IParamsSetUniquePtr paramsPtr = imtdoc::CreateObjectSourceParams(TEST_OBJECT_ID, 3);
	QVERIFY(paramsPtr.IsValid());
	QCOMPARE(imtdoc::GetObjectIdFromParams(paramsPtr.GetPtr()), TEST_OBJECT_ID);
	QCOMPARE(imtdoc::GetRevisionFromParams(paramsPtr.GetPtr()), 3);

	const iprm::IIdParam* objectIdParamPtr = dynamic_cast<const iprm::IIdParam*>(paramsPtr->GetParameter(imtdoc::s_objectIdParamId));
	QVERIFY(objectIdParamPtr != nullptr);
	QCOMPARE(objectIdParamPtr->GetId(), TEST_OBJECT_ID);

	const imeas::INumericValue* revisionParamPtr = dynamic_cast<const imeas::INumericValue*>(paramsPtr->GetParameter(imtdoc::s_revisionParamId));
	QVERIFY(revisionParamPtr != nullptr);
	QCOMPARE(revisionParamPtr->GetValues().GetElementsCount(), 1);
	QCOMPARE(revisionParamPtr->GetValues()[0], 3.0);

	iprm::IParamsSetUniquePtr headParamsPtr = imtdoc::CreateObjectSourceParams(TEST_OBJECT_ID);
	QVERIFY(headParamsPtr.IsValid());
	QCOMPARE(imtdoc::GetObjectIdFromParams(headParamsPtr.GetPtr()), TEST_OBJECT_ID);
	QCOMPARE(imtdoc::GetRevisionFromParams(headParamsPtr.GetPtr()), -1);
	QVERIFY(headParamsPtr->GetParameter(imtdoc::s_revisionParamId) == nullptr);

	iprm::IParamsSetUniquePtr emptyParamsPtr = imtdoc::CreateObjectSourceParams(QByteArray(), 3);
	QVERIFY(!emptyParamsPtr.IsValid());
	QVERIFY(imtdoc::GetObjectIdFromParams(nullptr).isEmpty());
	QCOMPARE(imtdoc::GetRevisionFromParams(nullptr), -1);
}


void CCollectionDocumentServiceIntegrationTest::NameProviderSourceParamsNewDocumentTest()
{
	QByteArray docId = CreateDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, SOURCE_NAME_TYPE_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(SetDocumentText(*m_servicePtr, TEST_USER_ID, docId, QStringLiteral("new")), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	// The name provider gets the ID of the inserted object
	const QByteArray objectId = imtdoc::SourceRefToObjectId(FindDocumentInfo(*m_servicePtr, TEST_USER_ID, docId).documentSourceRef);
	QVERIFY(!objectId.isEmpty());

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, NAME_PREFIX + QStringLiteral("new [%1]").arg(QString(objectId)));
}


void CCollectionDocumentServiceIntegrationTest::NameProviderSourceParamsOpenedDocumentTest()
{
	AddCollectionObject(TEST_OBJECT_ID, SOURCE_NAME_TYPE_ID, TEST_DOC_NAME, QStringLiteral("head"));

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID);
	QVERIFY(!docId.isEmpty());

	QCOMPARE(SaveDocument(*m_servicePtr, TEST_USER_ID, docId), imtdoc::IDocumentService::OS_OK);

	const QString expectedName = NAME_PREFIX + QStringLiteral("head [%1]").arg(QString(TEST_OBJECT_ID));

	QString name;
	QCOMPARE(m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name), imtdoc::IDocumentService::OS_OK);
	QCOMPARE(name, expectedName);
	QCOMPARE(m_collectionPtr->GetElementInfo(TEST_OBJECT_ID, imtbase::ICollectionInfo::EIT_NAME).toString(), expectedName);
}


void CCollectionDocumentServiceIntegrationTest::NameProviderSourceParamsRevisionTest()
{
	// An object without a name gets the document name from the name provider on opening
	AddCollectionObject(TEST_OBJECT_ID, SOURCE_NAME_TYPE_ID, QString());
	const int revision = CreateObjectRevision(TEST_OBJECT_ID, QStringLiteral("revisionData"), QStringLiteral("currentData"));
	QVERIFY(revision > 0);

	QByteArray docId = OpenDocumentAndWaitForLoad(*m_servicePtr, TEST_USER_ID, TEST_OBJECT_ID, revision);
	QVERIFY(!docId.isEmpty());

	// The name provider gets the revision source parameters, the name has the revision suffix
	const QString expectedName = NAME_PREFIX
				+ QStringLiteral("revisionData [%1/%2] (Rev #%2)").arg(QString(TEST_OBJECT_ID)).arg(revision);

	QString name;
	QTRY_VERIFY_WITH_TIMEOUT(
				m_servicePtr->GetDocumentName(TEST_USER_ID, docId, name) == imtdoc::IDocumentService::OS_OK && name == expectedName,
				WAIT_TIMEOUT);
}


