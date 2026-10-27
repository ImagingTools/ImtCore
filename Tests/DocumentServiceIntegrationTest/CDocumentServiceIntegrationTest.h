// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtdoc/IDocumentService.h>

// Local includes
#include "CDocumentServiceIntegrationTestBase.h"


/**
	Integration test for CDocumentServiceComp.
	The service is created from a Partitura registry with real components, the documents
	are \c iprm::ITextParam objects. The service is not connected to a collection, so the
	open, save and rename operations are not supported.
*/
class CDocumentServiceIntegrationTest: public CDocumentServiceIntegrationTestBase
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

	// GetDocumentName tests
	void GetDocumentNameSuccessTest();
	void GetDocumentNameInvalidUserTest();
	void GetDocumentNameInvalidDocumentTest();

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

	// Async document loading completion tests
	void CreateNewDocumentAsyncCompletionTest();

	// Multi-user scenario tests
	void MultiUserCreateAndCloseTest();
	void MultiUserIsolationTest();

	// Collection related operations
	void OpenDocumentNotSupportedTest();
	void SaveDocumentNotSupportedTest();
	void SetDocumentNameNotSupportedTest();

private:
	istd::TDelPtr<ipackage::CComponentAccessor> m_accessorPtr;
	imtdoc::IDocumentService* m_servicePtr = nullptr;
};


