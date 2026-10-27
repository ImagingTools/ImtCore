// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <imod/IObserver.h>
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>


/**
	Event handler that records all events of the document services for verification.
*/
class CRecordingEventHandler: public imtdoc::IDocumentServiceEventHandler
{
public:
	struct RecordedEvent
	{
		QByteArray type;
		QByteArray userId;
		QByteArray documentId;
		QByteArray documentTypeId;
		QString documentName;
		QUrl documentUrl;
		bool isDirty = false;
	};

	QList<RecordedEvent> GetEvents() const;
	void ClearEvents();
	int CountEventsOfType(const QByteArray& type) const;
	bool HasEvent(const QByteArray& type, const QByteArray& userId) const;

	// reimplemented (imtdoc::IDocumentServiceEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

private:
	mutable QMutex m_mutex;
	QList<RecordedEvent> m_events;
};


/**
	Observer for testing RegisterDocumentObserver/UnregisterDocumentObserver.
*/
class CTestObserver: public imod::IObserver
{
public:
	virtual ~CTestObserver();

	// reimplemented (imod::IObserver)
	virtual bool IsModelAttached(const imod::IModel* modelPtr) const override;
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void BeforeUpdate(imod::IModel* modelPtr) override;
	virtual void AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	QList<imod::IModel*> m_models;
};


/**
	Integration test for CCollectionDocumentServiceComp.
	The services are created from a Partitura registry with real components: two
	collection document services (regular and single-copy mode) sharing one object
	collection, the documents are \c iprm::ITextParam objects.
	Contains the test cases of CollectionDocumentServiceTest; cases that need fault
	injection or access to internals are rewritten with the public API or skipped.
*/
class CCollectionDocumentServiceIntegrationTest: public QObject
{
	Q_OBJECT

private slots:
	// Setup and teardown
	void init();
	void cleanup();

	// GetOpenedDocumentList tests
	void GetOpenedDocumentListEmptyTest();
	void GetOpenedDocumentListNonExistentUserTest();
	void GetOpenedDocumentListAfterCreateTest();
	void GetOpenedDocumentListMultipleDocumentsTest();
	void GetOpenedDocumentListMultipleUsersTest();

	// CreateNewDocument tests
	void CreateNewDocumentSuccessTest();
	void CreateNewDocumentUndoManagerFailTest();
	void CreateNewDocumentObjectCreationFailAsyncTest();
	void CreateNewDocumentEventFiredTest();
	void CreateNewDocumentIsLoadingTest();
	void CreateNewDocumentMultipleDocumentsTest();
	void CreateNewDocumentProposedSourceDocumentIdUsedOnSaveTest();

	// OpenDocument tests
	void OpenDocumentSuccessTest();
	void OpenDocumentInvalidSchemeTest();
	void OpenDocumentInvalidUrlTest();
	void OpenDocumentNonExistentObjectTest();
	void OpenDocumentUndoManagerFailTest();
	void OpenDocumentEventFiredTest();
	void OpenDocumentIsLoadingTest();
	void OpenDocumentCompletesBeforeDataLoadedTest();
	void OpenDocumentDataLoadFailClosesDocumentTest();
	void OpenDocumentWithHostTest();
	void OpenDocumentMultiplePathSegmentsTest();
	void OpenDocumentWithoutSingleInstanceModeFailsWhenSingleInstanceAlreadyOpenedTest();

	// GetDocumentName tests
	void GetDocumentNameSuccessTest();
	void GetDocumentNameInvalidUserTest();
	void GetDocumentNameInvalidDocumentTest();

	// SetDocumentName tests
	void SetDocumentNameSuccessTest();
	void SetDocumentNameSameNameTest();
	void SetDocumentNameInvalidUserTest();
	void SetDocumentNameInvalidDocumentTest();
	void SetDocumentNameCollectionUpdateFailTest();
	void SetDocumentNameEventFiredTest();
	void SetDocumentNameNewDocumentNoCollectionUpdateTest();

	// GetDocumentPtr tests
	void GetDocumentPtrSuccessTest();
	void GetDocumentPtrInvalidUserTest();
	void GetDocumentPtrInvalidDocumentTest();
	void GetDocumentPtrWhileLoadingTest();

	// GetDocumentData tests
	void GetDocumentDataSuccessTest();
	void GetDocumentDataInvalidUserTest();
	void GetDocumentDataInvalidDocumentTest();
	void GetDocumentDataWhileLoadingTest();

	// SetDocumentData tests
	void SetDocumentDataSuccessTest();
	void SetDocumentDataInvalidUserTest();
	void SetDocumentDataInvalidDocumentTest();
	void SetDocumentDataWhileLoadingTest();

	// SaveDocument tests
	void SaveDocumentNewDocumentTest();
	void SaveDocumentExistingDocumentTest();
	void SaveDocumentSaveAsTest();
	void SaveDocumentNewDocumentUpdatesUrlTest();
	void SaveDocumentSaveAsUpdatesUrlTest();
	void SaveDocumentInvalidUserTest();
	void SaveDocumentInvalidDocumentTest();
	void SaveDocumentWhileLoadingTest();
	void SaveDocumentCollectionInsertFailTest();
	void SaveDocumentCollectionSetDataFailTest();
	void SaveDocumentValidationFailTest();
	void SaveDocumentValidationFailWithMessageTest();
	void SaveDocumentWithNameProviderTest();
	void SaveDocumentEventFiredTest();
	void SaveDocumentSaveAsEventFiredTest();

	// CloseDocument tests
	void CloseDocumentSuccessTest();
	void CloseDocumentInvalidUserTest();
	void CloseDocumentInvalidDocumentTest();
	void CloseDocumentEventFiredTest();
	void CloseDocumentRemovesFromListTest();
	void CloseDocumentLastDocForUserTest();

	// GetDocumentUndoManager tests
	void GetDocumentUndoManagerSuccessTest();
	void GetDocumentUndoManagerInvalidUserTest();
	void GetDocumentUndoManagerInvalidDocumentTest();

	// RegisterDocumentObserver tests
	void RegisterDocumentObserverSuccessTest();
	void RegisterDocumentObserverInvalidUserTest();
	void RegisterDocumentObserverInvalidDocumentTest();
	void RegisterDocumentObserverAlreadyRegisteredTest();

	// UnregisterDocumentObserver tests
	void UnregisterDocumentObserverSuccessTest();
	void UnregisterDocumentObserverInvalidUserTest();
	void UnregisterDocumentObserverInvalidDocumentTest();
	void UnregisterDocumentObserverNotRegisteredTest();

	// Serialize tests
	void SerializeReturnsFalseTest();

	// ValidateInputParams tests
	void ValidateInputParamsSuccessTest();
	void ValidateInputParamsInvalidUserTest();
	void ValidateInputParamsInvalidDocumentTest();
	void ValidateInputParamsForeignDocumentTest();

	// Document storage tests
	void DocumentStorageInsertRemoveTest();

	// ObjectIdToUrl tests
	void ObjectIdToUrlTest();
	void UrlToObjectIdTest();

	// Async document loading completion tests
	void CreateNewDocumentAsyncCompletionTest();
	void OpenDocumentAsyncCompletionTest();

	// Multi-user scenario tests
	void MultiUserCreateAndCloseTest();
	void MultiUserIsolationTest();

	// Single-copy mode tests
	void SingleCopyOpenSameObjectByTwoUsersTest();
	void SingleCopySecondUserGetsDataLoadedEventTest();
	void SingleCopySetDocumentDataSharedTest();
	void SingleCopyCloseOneUserTest();
	void SingleCopyCloseBothUsersTest();
	void SingleCopySaveUpdatesAllTest();
	void SingleCopySetDocumentNameUpdatesAllTest();
	void SingleCopySaveAsUpdatesAllTest();
	void SingleCopyReopenBySameUserTest();
	void SingleCopyOneDocumentForAllUsersTest();

private:
	/**
		Inserts a text object into the shared collection if it does not exist yet.
	*/
	void AddCollectionObject(
				const QByteArray& objectId,
				const QByteArray& typeId,
				const QString& name,
				const QString& text = QString());

	/**
		Creates a new document and waits until its data is loaded.
		Returns the document ID.
	*/
	QByteArray CreateDocumentAndWaitForLoad(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& typeId,
				const QString& name = QString());

	/**
		Opens the collection object as a document and waits until its data is loaded.
		Returns the document ID.
	*/
	QByteArray OpenDocumentAndWaitForLoad(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& objectId);

	/**
		Waits until the document is not loading anymore.
	*/
	bool WaitForDocumentLoaded(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId);

	/**
		Starts the task and returns its final status (immediate error or task result).
	*/
	imtdoc::IDocumentService::OperationStatus ExecuteTask(
				imtdoc::IDocumentService& service,
				imtdoc::IDocumentService::TaskType taskType,
				const imtdoc::IDocumentService::TaskParams& params,
				QByteArray* documentIdPtr = nullptr);

	imtdoc::IDocumentService::OperationStatus CloseDocument(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId);

	imtdoc::IDocumentService::OperationStatus SaveDocument(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId,
				const QString& documentName = QString());

	imtdoc::IDocumentService::DocumentListItem FindDocumentInfo(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId) const;

	QString GetDocumentText(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId) const;

	static QUrl ObjectUrl(const QByteArray& objectId);
	static QByteArray UrlObjectId(const QUrl& url);

private:
	istd::TDelPtr<ipackage::CComponentAccessor> m_accessorPtr;
	imtdoc::IDocumentService* m_servicePtr = nullptr;
	imtdoc::IDocumentService* m_singleCopyServicePtr = nullptr;
	imtbase::IObjectCollection* m_collectionPtr = nullptr;
	CRecordingEventHandler m_eventHandler;
};


