// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <imod/IModel.h>
#include <imod/IObserver.h>
#include <idoc/IUndoManager.h>
#include <idoc/IDocumentStateComparator.h>
#include <istd/IChangeable.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/CCollectionDocumentManagerBase.h>
#include <imtdoc/IDocumentManagerEventHandler.h>


/**
	Mock object that implements both IChangeable and IModel
	so it can be used as a document object in the document manager.
*/
class CMockDocumentObject:
	public imod::IModel,
	virtual public istd::IChangeable
{
public:
	CMockDocumentObject()
		: m_data("default")
	{
	}

	explicit CMockDocumentObject(const QByteArray& data)
		: m_data(data)
	{
	}

	QByteArray GetData() const { return m_data; }
	void SetData(const QByteArray& data) { m_data = data; }

	// reimplemented (imod::IModel)
	virtual bool AttachObserver(imod::IObserver* observerPtr) override
	{
		if (observerPtr == nullptr || m_observers.contains(observerPtr)) {
			return false;
		}
		m_observers.append(observerPtr);
		return true;
	}

	virtual void DetachObserver(imod::IObserver* observerPtr) override
	{
		m_observers.removeAll(observerPtr);
	}

	virtual void DetachAllObservers() override
	{
		m_observers.clear();
	}

	virtual bool IsAttached(const imod::IObserver* observerPtr) const override
	{
		return m_observers.contains(const_cast<imod::IObserver*>(observerPtr));
	}

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override { return SO_CLONE | SO_COPY | SO_COMPARE; }

	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) override
	{
		const CMockDocumentObject* otherPtr = dynamic_cast<const CMockDocumentObject*>(&object);
		if (otherPtr == nullptr) {
			return false;
		}

		m_data = otherPtr->m_data;
		return true;
	}

	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override
	{
		istd::IChangeableUniquePtr clonePtr(new CMockDocumentObject);
		if (clonePtr->CopyFrom(*this, mode)){
			return clonePtr;
		}
	
		return nullptr;
	}

	virtual bool IsEqual(const IChangeable& object) const override
	{
		const CMockDocumentObject* otherPtr = dynamic_cast<const CMockDocumentObject*>(&object);
		if (otherPtr == nullptr) {
			return false;
		}

		return m_data == otherPtr->m_data;
	}

private:
	QByteArray m_data;
	QList<imod::IObserver*> m_observers;
};


/**
	Mock undo manager that implements IUndoManager and IModel.
	The document manager requires the undo manager to be castable to both.
*/
class CMockUndoManager:
	public imod::IModel,
	virtual public idoc::IUndoManager
{
public:
	CMockUndoManager()
		: m_documentChangeFlag(idoc::IDocumentStateComparator::DCF_EQUAL)
		, m_storedState(false)
	{
	}

	bool WasStateStored() const { return m_storedState; }
	void ResetStoredFlag() { m_storedState = false; }

	// reimplemented (imod::IModel)
	virtual bool AttachObserver(imod::IObserver* /*observerPtr*/) override { return true; }
	virtual void DetachObserver(imod::IObserver* /*observerPtr*/) override {}
	virtual void DetachAllObservers() override {}
	virtual bool IsAttached(const imod::IObserver* /*observerPtr*/) const override { return true; }

	// reimplemented (idoc::IUndoManager)
	virtual bool DoUndo(int steps = 1) override { return true; }
	virtual bool DoRedo(int steps = 1) override { return true; }
	virtual int GetAvailableUndoSteps() const override { return 0; }
	virtual int GetAvailableRedoSteps() const override { return 0; }
	virtual QString GetUndoLevelDescription(int stepIndex) const override { return QString(); }
	virtual QString GetRedoLevelDescription(int stepIndex) const override { return QString(); }
	virtual void ResetUndo() override {}

	// reimplemented (idoc::IDocumentStateComparator)
	virtual bool HasStoredDocumentState() const override { return true; }
	virtual bool StoreDocumentState() override { return true; }
	virtual bool RestoreDocumentState() override { return true; }
	virtual DocumentChangeFlag GetDocumentChangeFlag() const override { return m_documentChangeFlag; }

	void SetDocumentChangeFlag(idoc::IDocumentStateComparator::DocumentChangeFlag flag){ m_documentChangeFlag = flag; }

private:
	idoc::IDocumentStateComparator::DocumentChangeFlag m_documentChangeFlag;
	bool m_storedState;
};


/**
	Mock object collection for testing the document manager.
	Stores objects in memory and provides all the collection operations.
*/
class CMockObjectCollection: public imtbase::IObjectCollection
{
public:
	struct ObjectEntry
	{
		QByteArray typeId;
		QString name;
		QString description;
		istd::IChangeableSharedPtr dataPtr;
	};

	CMockObjectCollection()
		: m_insertShouldFail(false)
		, m_setObjectDataShouldFail(false)
		, m_getObjectDataShouldFail(false)
		, m_setElementNameShouldFail(false)
	{
	}

	// Control flags for simulating failures
	void SetInsertShouldFail(bool fail) { m_insertShouldFail = fail; }
	void SetSetObjectDataShouldFail(bool fail) { m_setObjectDataShouldFail = fail; }
	void SetGetObjectDataShouldFail(bool fail) { m_getObjectDataShouldFail = fail; }
	void SetSetElementNameShouldFail(bool fail) { m_setElementNameShouldFail = fail; }

	void AddObject(const QByteArray& objectId, const QByteArray& typeId, const QString& name,
		istd::IChangeableSharedPtr dataPtr = istd::IChangeableSharedPtr())
	{
		ObjectEntry entry;
		entry.typeId = typeId;
		entry.name = name;
		entry.dataPtr = dataPtr;
		m_objects[objectId] = entry;
	}

	bool HasObject(const QByteArray& objectId) const
	{
		return m_objects.contains(objectId);
	}

	const QMap<QByteArray, ObjectEntry>& GetObjects() const { return m_objects; }

	QByteArray GetLastInsertedId() const { return m_lastInsertedId; }

	// reimplemented (imtbase::IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override { return nullptr; }

	virtual Id GetObjectTypeId(const Id& objectId) const override
	{
		if (m_objects.contains(objectId)) {
			return m_objects[objectId].typeId;
		}
		return QByteArray();
	}

	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& /*objectId*/) const override
	{
		return idoc::MetaInfoPtr();
	}

	// reimplemented (imtbase::ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* /*selectionParamsPtr*/ = nullptr,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) const override
	{
		return m_objects.size();
	}

	virtual Ids GetElementIds(int /*offset*/ = 0, int /*count*/ = -1,
		const iprm::IParamsSet* /*selectionParamsPtr*/ = nullptr,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) const override
	{
		return Ids(m_objects.keys().begin(), m_objects.keys().end());
	}

	virtual bool GetSubsetInfo(ICollectionInfo& /*subsetInfo*/, int /*offset*/ = 0, int /*count*/ = -1,
		const iprm::IParamsSet* /*selectionParamsPtr*/ = nullptr,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) const override
	{
		return false;
	}

	virtual QVariant GetElementInfo(const Id& elementId, int infoType,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) const override
	{
		if (!m_objects.contains(elementId)) {
			return QVariant();
		}

		switch (infoType) {
		case EIT_NAME:
			return m_objects[elementId].name;
		case EIT_DESCRIPTION:
			return m_objects[elementId].description;
		default:
			return QVariant();
		}
	}

	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& /*elementId*/,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) const override
	{
		return idoc::MetaInfoPtr();
	}

	virtual bool SetElementName(const Id& elementId, const QString& name,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) override
	{
		if (m_setElementNameShouldFail || !m_objects.contains(elementId)) {
			return false;
		}

		m_objects[elementId].name = name;
		return true;
	}

	virtual bool SetElementDescription(const Id& elementId, const QString& description,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) override
	{
		if (!m_objects.contains(elementId)) {
			return false;
		}

		m_objects[elementId].description = description;
		return true;
	}

	virtual bool SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/ = true,
		ilog::IMessageConsumer* /*logPtr*/ = nullptr) override
	{
		return true;
	}

	// reimplemented (imtbase::IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override { return nullptr; }
	virtual const imtbase::ICollectionDataController* GetDataController() const override { return nullptr; }

	virtual int GetOperationFlags(const Id& /*elementId*/ = Id()) const override
	{
		return OF_ALL;
	}

	virtual Id InsertNewObject(
		const QByteArray& typeId,
		const QString& name,
		const QString& description,
		const istd::IChangeable* defaultValuePtr = nullptr,
		const Id& proposedElementId = Id(),
		const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/ = nullptr,
		const idoc::IDocumentMetaInfo* /*elementMetaInfoPtr*/ = nullptr,
		const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) override
	{
		if (m_insertShouldFail) {
			return QByteArray();
		}

		QByteArray objectId = proposedElementId.isEmpty()
			? QUuid::createUuid().toByteArray(QUuid::WithoutBraces)
			: proposedElementId;

		ObjectEntry entry;
		entry.typeId = typeId;
		entry.name = name;
		entry.description = description;
		if (defaultValuePtr != nullptr) {
			entry.dataPtr = istd::IChangeableSharedPtr(defaultValuePtr->CloneMe().PopInterfacePtr());
		}

		m_objects[objectId] = entry;
		m_lastInsertedId = objectId;

		return objectId;
	}

	virtual bool RemoveElements(const Ids& elementIds,
		const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) override
	{
		for (const auto& id : elementIds) {
			m_objects.remove(id);
		}
		return true;
	}

	virtual bool RemoveElementSet(const iprm::IParamsSet* /*selectionParamsPtr*/ = nullptr,
		const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) override
	{
		m_objects.clear();
		return true;
	}

	virtual bool RestoreObjects(const Ids& /*objectIds*/,
		const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) override
	{
		return false;
	}

	virtual bool RestoreObjectSet(const iprm::IParamsSet* /*selectionParamsPtr*/ = nullptr,
		const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) override
	{
		return false;
	}

	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const override
	{
		if (m_objects.contains(objectId)) {
			return m_objects[objectId].dataPtr.GetPtr();
		}
		return nullptr;
	}

	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr,
		const iprm::IParamsSet* /*dataConfigurationPtr*/ = nullptr) const override
	{
		if (m_getObjectDataShouldFail) {
			return false;
		}

		if (!m_objects.contains(objectId) || !m_objects[objectId].dataPtr.IsValid()) {
			return false;
		}

		dataPtr = istd::IChangeableSharedPtr(m_objects[objectId].dataPtr->CloneMe().PopInterfacePtr());
		return dataPtr.IsValid();
	}

	virtual bool SetObjectData(
		const Id& objectId,
		const istd::IChangeable& object,
		CompatibilityMode /*mode*/ = CM_WITHOUT_REFS,
		const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) override
	{
		if (m_setObjectDataShouldFail || !m_objects.contains(objectId)) {
			return false;
		}

		if (m_objects[objectId].dataPtr.IsValid()) {
			return m_objects[objectId].dataPtr->CopyFrom(object);
		}

		m_objects[objectId].dataPtr = istd::IChangeableSharedPtr(object.CloneMe().PopInterfacePtr());
		return m_objects[objectId].dataPtr.IsValid();
	}

	virtual istd::TUniqueInterfacePtr<IObjectCollection> CreateSubCollection(
		int /*offset*/ = 0, int /*count*/ = -1,
		const iprm::IParamsSet* /*selectionParamsPtr*/ = nullptr) const override
	{
		return istd::TUniqueInterfacePtr<IObjectCollection>();
	}

	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
		const QByteArray& /*objectId*/ = QByteArray(), int /*offset*/ = 0, int /*count*/ = -1,
		const iprm::IParamsSet* /*selectionParamsPtr*/ = nullptr) const override
	{
		return nullptr;
	}

private:
	QMap<QByteArray, ObjectEntry> m_objects;
	QByteArray m_lastInsertedId;
	bool m_insertShouldFail;
	bool m_setObjectDataShouldFail;
	bool m_getObjectDataShouldFail;
	bool m_setElementNameShouldFail;
};


/**
	Mock event handler that records all events for verification.
*/
class CMockEventHandler: public imtdoc::IDocumentManagerEventHandler
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
		bool isDirty;
		bool accepted;
	};

	CMockEventHandler()
		: m_shouldReject(false)
	{
	}

	void SetShouldReject(bool reject) { m_shouldReject = reject; }
	const QList<RecordedEvent>& GetEvents() const { return m_events; }
	void ClearEvents() { m_events.clear(); }

	int CountEventsOfType(const QByteArray& type) const
	{
		int count = 0;
		for (const auto& event : m_events) {
			if (event.type == type) {
				count++;
			}
		}
		return count;
	}

	// reimplemented (imtdoc::IDocumentManagerEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override
	{
		if (eventPtr == nullptr) {
			return false;
		}

		RecordedEvent record;
		record.type = eventPtr->Type();
		record.userId = eventPtr->GetUserId();
		record.documentId = eventPtr->GetDocumentId();
		record.documentTypeId = eventPtr->GetDocumentTypeId();
		record.documentName = eventPtr->GetDocumentName();
		record.documentUrl = eventPtr->GetDocumentUrl();
		record.isDirty = eventPtr->IsDocumentDirty();

		if (m_shouldReject) {
			eventPtr->Reject();
		}

		record.accepted = eventPtr->IsAccepted();
		m_events.append(record);

		return true;
	}

private:
	QList<RecordedEvent> m_events;
	bool m_shouldReject;
};


/**
	Concrete subclass of CCollectionDocumentManager for testing.
	Provides mock implementations for the pure virtual methods.
*/
class CTestableDocumentManager: public imtdoc::CCollectionDocumentManagerBase
{
public:
	CTestableDocumentManager()
		: m_collectionPtr(&m_mockCollection)
		, m_createObjectShouldFail(false)
		, m_createUndoManagerShouldFail(false)
		, m_hasNameProvider(false)
		, m_defaultDocumentName()
		, m_validateShouldFail(false)
		, m_validationErrorMessage()
		, m_singleCopyMode(false)
	{
	}

	// Access to mock objects for test configuration
	CMockObjectCollection& GetMockCollection() { return m_mockCollection; }
	CMockEventHandler& GetMockEventHandler() { return m_mockEventHandler; }

	void SetCreateObjectShouldFail(bool fail) { m_createObjectShouldFail = fail; }
	void SetCreateUndoManagerShouldFail(bool fail) { m_createUndoManagerShouldFail = fail; }
	void SetHasNameProvider(bool has) { m_hasNameProvider = has; }
	void SetDefaultDocumentName(const QString& name) { m_defaultDocumentName = name; }
	void SetValidateShouldFail(bool fail) { m_validateShouldFail = fail; }
	void SetValidationErrorMessage(const QString& msg) { m_validationErrorMessage = msg; }
	void SetSingleCopyMode(bool enabled) { m_singleCopyMode = enabled; }

	// Expose protected methods for testing
	using imtdoc::CDocumentManagerBase::FindDocument;
	using imtdoc::CDocumentManagerBase::ValidateInputParams;
	using imtdoc::CDocumentManagerBase::ObjectIdToUrl;
	using imtdoc::CDocumentManagerBase::m_userDocuments;
	using imtdoc::CDocumentManagerBase::m_mutex;
	using imtdoc::CDocumentManagerBase::m_sharedDocuments;

protected:
	// reimplemented (CCollectionDocumentManagerBase)
	virtual imtbase::IObjectCollection* GetCollection() const override
	{
		return m_collectionPtr;
	}

	virtual bool IsSingleCopyMode() const override
	{
		return m_singleCopyMode;
	}

	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& /*typeId*/) const override
	{
		if (m_createObjectShouldFail) {
			return istd::IChangeableUniquePtr();
		}

		return istd::IChangeableUniquePtr(new CMockDocumentObject());
	}

	virtual idoc::IUndoManagerUniquePtr CreateUndoManager() const override
	{
		if (m_createUndoManagerShouldFail) {
			return idoc::IUndoManagerUniquePtr();
		}

		return idoc::IUndoManagerUniquePtr(new CMockUndoManager());
	}

	virtual QList<imtdoc::IDocumentManagerEventHandler*> GetDocumentManagerEventHandlers() const override
	{
		return { const_cast<CMockEventHandler*>(&m_mockEventHandler) };
	}

	virtual QString GetDefaultDocumentName(const WorkingDocument& /*document*/) const override
	{
		return m_defaultDocumentName;
	}

	virtual bool HasDocumentNameProvider(const QByteArray& /*typeId*/) const override
	{
		return m_hasNameProvider;
	}

	virtual bool ValidateDocumentData(
		const WorkingDocument& /*document*/,
		OperationStatus& status,
		QString* errorMessage = nullptr) const override
	{
		if (m_validateShouldFail) {
			status = OS_INVALID_DOCUMENT_DATA;
			if (errorMessage != nullptr) {
				*errorMessage = m_validationErrorMessage;
			}
			return false;
		}

		status = OS_OK;
		return true;
	}

private:
	CMockObjectCollection m_mockCollection;
	CMockEventHandler m_mockEventHandler;
	imtbase::IObjectCollection* m_collectionPtr;
	bool m_createObjectShouldFail;
	bool m_createUndoManagerShouldFail;
	bool m_hasNameProvider;
	QString m_defaultDocumentName;
	bool m_validateShouldFail;
	QString m_validationErrorMessage;
	bool m_singleCopyMode;
};


/**
	Mock observer for testing RegisterDocumentObserver/UnregisterDocumentObserver.
*/
class CMockObserver : public imod::IObserver
{
public:
	virtual void BeforeUpdate(imod::IModel* /*modelPtr*/) override {}
	virtual void AfterUpdate(imod::IModel* /*modelPtr*/, const istd::IChangeable::ChangeSet& /*changeSet*/) override {}
	bool IsModelAttached(const imod::IModel* modelPtr) const override { return true; }
	bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override { return true; }
	bool OnModelDetached(imod::IModel* modelPtr) override { return true; }
};


/**
	Test class for CCollectionDocumentManager.
	Tests all synchronous operations and validates edge cases.
*/
class CCollectionDocumentManagerTest: public QObject
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

	// OpenDocument tests
	void OpenDocumentSuccessTest();
	void OpenDocumentInvalidSchemeTest();
	void OpenDocumentInvalidUrlTest();
	void OpenDocumentNonExistentObjectTest();
	void OpenDocumentUndoManagerFailTest();
	void OpenDocumentEventFiredTest();
	void OpenDocumentIsLoadingTest();
	void OpenDocumentWithHostTest();
	void OpenDocumentMultiplePathSegmentsTest();

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

	// ObjectIdToUrl tests
	void ObjectIdToUrlTest();

	// Async document loading completion tests
	void CreateNewDocumentAsyncCompletionTest();
	void OpenDocumentAsyncCompletionTest();

	// Multi-user scenario tests
	void MultiUserCreateAndCloseTest();
	void MultiUserIsolationTest();

	// Single-copy mode tests
	void SingleCopyOpenSameObjectByTwoUsersTest();
	void SingleCopySetDocumentDataSharedTest();
	void SingleCopyCloseOneUserTest();
	void SingleCopyCloseBothUsersTest();
	void SingleCopySaveUpdatesAllTest();
	void SingleCopySetDocumentNameUpdatesAllTest();

private:
	/**
		Helper: creates a document synchronously and waits for async loading.
		Returns the documentId.
	*/
	QByteArray CreateDocumentAndWaitForLoad(
		CTestableDocumentManager& manager,
		const QByteArray& userId,
		const QByteArray& typeId);

	/**
		Helper: opens a document synchronously and waits for async loading.
		Returns the documentId.
	*/
	QByteArray OpenDocumentAndWaitForLoad(
		CTestableDocumentManager& manager,
		const QByteArray& userId,
		const QByteArray& objectId);

	/**
		Helper: directly sets up a document in the manager for synchronous testing.
		Bypasses the async creation flow.
	*/
	QByteArray SetupDocumentDirectly(
		CTestableDocumentManager& manager,
		const QByteArray& userId,
		const QByteArray& typeId = "TestType",
		const QByteArray& objectId = QByteArray(),
		const QString& name = QString());

private:
	CTestableDocumentManager* m_managerPtr;
};
