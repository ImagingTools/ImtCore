// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <istd/TOptInterfacePtr.h>
#include <iser/IObject.h>
#include <imod/TModelWrap.h>
#include <imod/CModelUpdateBridge.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>

// Acula includes
#include <imthype/ITaskCollection.h>


namespace imthype
{


/**
	Basic implementation of the \c ITaskCollection interface.
	The component supports definition of static or dynamic task pipelines.
*/
class CTaskCollectionCompBase:
			public ilog::CLoggerComponentBase,
			virtual public ITaskCollection,
			virtual public iser::ISerializable
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CTaskCollectionCompBase);
		I_REGISTER_INTERFACE(ITaskCollection);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN_MULTI_0(m_fixedTasksCompPtr, "FixedTasks", "List of static defined tasks", false);
		I_ASSIGN_MULTI_0(m_fixedTaskIdsAttrPtr, "FixedTaskIds", "List of IDs for corresponding fixed task", false);
		I_ASSIGN_MULTI_0(m_fixedTaskTypeIdsAttrPtr, "FixedTaskTypeIds", "List of type IDs for corresponding fixed task", false);
		I_ASSIGN_MULTI_0(m_fixedTaskTypeNamesAttrPtr, "FixedTaskTypeNames", "List of names for corresponding fixed task", false);
		I_ASSIGN_MULTI_0(m_fixedTaskUserIdsAttrPtr, "FixedTaskUserIds", "List of user ids for corresponding fixed task", false);
		I_ASSIGN(m_taskInputsCompPtr, "TaskInputs", "Container of task inputs related to the task collection", false, "TaskInputs");
		I_ASSIGN_TO(m_taskInputsModelCompPtr, m_taskInputsCompPtr, false);
		I_ASSIGN(m_allowAddTasksAttrPtr, "AllowAdd", "If enabled the new tasks can be added to the manager", true, true);
	I_END_COMPONENT;

	CTaskCollectionCompBase();

	virtual QString GetTaskTypeName(const QByteArray& taskTypeId) const = 0;

	// reimplemented (ITaskCollection)
	virtual QByteArray GetUserTaskId(const QByteArray& taskUuid) const override;
	virtual bool SetUserTaskId(const QByteArray& taskUuid, const QByteArray& userTaskId) override;
	virtual QByteArray GetTaskInputId(const QByteArray& taskUuid) const override;
	virtual bool SetTaskInputId(const QByteArray& taskUuid, const QByteArray& inputId) override;
	virtual const iinsp::ISupplier* GetTask(const QByteArray& taskId) const override;
	virtual const ITaskResultCollection* GetTaskResults(const QByteArray& taskId) const override;
	virtual const imtbase::IObjectCollection* GetTaskInputs() const override;

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElements(
				const Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElementSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjects(
				const Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjectSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData(
				const QByteArray& objectId,
				DataPtr& dataPtr,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool SetObjectData(
				const QByteArray& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtbase::IObjectCollectionUniquePtr CreateSubCollection(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual void ResetObjectIds() override;

	// reimplemented (IObjectCollectionInfo)
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				imtbase::ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;
	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

protected:
	virtual iinsp::ISupplierUniquePtr CreateTaskInstance(const QByteArray& taskTypeId) const = 0;
	virtual void OnTaskCreated(iinsp::ISupplier& task);
	virtual void OnTaskRemoved(const QByteArray& taskId);

	/**
		Generating of a unique task name.
		\param newName	Preferred new task name.
		\param oldName	Previous task name (if any).

		Note: For newly created object, oldName must be empty.
	*/
	virtual QString GenerateUniqueObjectName(const QString& newName, const QString& oldName) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	typedef istd::TOptInterfacePtr<iinsp::ISupplier> OptionalTaskPtr;

	struct Task
	{
		Task()
			:isEnabled(true),
			taskFlags(OF_ALL & ~OF_SUPPORT_PAGINATION)
		{
			uuid = QUuid::createUuid().toByteArray();
		}

		QByteArray uuid;
		QByteArray typeId;
		QString typeName;
		OptionalTaskPtr taskPtr;
		QString name;
		bool isEnabled;
		int taskFlags;
		QString description;
		QByteArray userDefinedTaskId;
		QByteArray inputId;
	};

	typedef QVector<Task> TaskList;

protected:
	TaskList m_tasks;
	typedef QMap<QByteArray, TaskList::Iterator> TaskIdMap;

private:
	void OnTaskInputsUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* objectCollectionPtr);

protected:
	I_MULTIREF(iinsp::ISupplier, m_fixedTasksCompPtr);
	I_MULTIATTR(QByteArray, m_fixedTaskIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_fixedTaskTypeIdsAttrPtr);
	I_MULTITEXTATTR(m_fixedTaskTypeNamesAttrPtr);
	I_MULTITEXTATTR(m_fixedTaskUserIdsAttrPtr);
	I_REF(imtbase::IObjectCollection, m_taskInputsCompPtr);
	I_REF(imod::IModel, m_taskInputsModelCompPtr);
	I_ATTR(bool, m_allowAddTasksAttrPtr);

	mutable imod::CModelUpdateBridge m_updateBridge;
	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CTaskCollectionCompBase> m_taskInputsObserver;
};


} // namespace imthype


