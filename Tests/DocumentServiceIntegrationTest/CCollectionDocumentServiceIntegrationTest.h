// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentService.h>

// Local includes
#include "CDocumentServiceIntegrationTestBase.h"


/**
	Integration test for CCollectionDocumentServiceComp.
	The services are created from a Partitura registry with real components: two
	collection document services (regular and single-copy mode) sharing one object
	collection, the documents are \c iprm::ITextParam objects.
	Document validators and name providers of the validated types are provided by
	DocumentServiceIntegrationTestPck.
	Contains the test cases of the interaction with the collection; the common document
	service functionality is covered by CDocumentServiceIntegrationTest.
*/
class CCollectionDocumentServiceIntegrationTest: public CDocumentServiceIntegrationTestBase
{
	Q_OBJECT

private slots:
	// Setup and teardown
	void init();
	void cleanup();

	// CreateNewDocument tests
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

	// SetDocumentName tests
	void SetDocumentNameSuccessTest();
	void SetDocumentNameSameNameTest();
	void SetDocumentNameInvalidUserTest();
	void SetDocumentNameInvalidDocumentTest();
	void SetDocumentNameCollectionUpdateFailTest();
	void SetDocumentNameEventFiredTest();
	void SetDocumentNameNewDocumentNoCollectionUpdateTest();

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

	// ObjectIdToUrl tests
	void ObjectIdToUrlTest();
	void UrlToObjectIdTest();

	// Async document loading completion tests
	void OpenDocumentAsyncCompletionTest();

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
		Opens the collection object as a document and waits until its data is loaded.
		Returns the document ID.
	*/
	QByteArray OpenDocumentAndWaitForLoad(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& objectId);

private:
	istd::TDelPtr<ipackage::CComponentAccessor> m_accessorPtr;
	imtdoc::IDocumentService* m_servicePtr = nullptr;
	imtdoc::IDocumentService* m_singleCopyServicePtr = nullptr;
	imtbase::IObjectCollection* m_collectionPtr = nullptr;
};


