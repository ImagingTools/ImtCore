// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CTaskCollectionCompBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/MetaTypes.h>
#include <imthype/ITaskCollectionContext.h>
#include <imthype/CTaskCollectionUpdateEvent.h>
#include <imtcore/Version.h>


namespace imthype
{


static QByteArray s_userDefinedTaskId = "CTaskCollectionCompBase::UserDefinedTaskId";
static QByteArray s_taskInputId = "CTaskCollectionCompBase::TaskInputId";


// public methods

CTaskCollectionCompBase::CTaskCollectionCompBase()
	:m_updateBridge(this),
	m_taskInputsObserver(*this)
{
}


// reimplemented (ITaskCollection)

QByteArray CTaskCollectionCompBase::GetUserTaskId(const QByteArray& taskUuid) const
{
	for (const Task& task : m_tasks){
		if (task.uuid == taskUuid){
			return task.userDefinedTaskId;
		}
	}

	return QByteArray();
}


bool CTaskCollectionCompBase::SetUserTaskId(const QByteArray& taskUuid, const QByteArray& userTaskId)
{
	// Check for existing user-defined task-ID in some other tasks:
	for (const Task& task : m_tasks){
		if (!userTaskId.isEmpty()){
			if ((task.userDefinedTaskId == userTaskId) && (task.uuid != taskUuid)){
				return false;
			}
		}
	}

	// Set the new user-defined task-ID:
	for (Task& task : m_tasks){
		if (task.uuid == taskUuid){
			if (task.userDefinedTaskId != userTaskId){
				istd::TSmartPtr<istd::IChangeable> eventPtr;
				eventPtr.SetPtr(new imthype::CTaskCollectionUpdateEvent(
							taskUuid,
							imthype::CTaskCollectionUpdateEvent::UT_USER_TASK_ID,
							task.userDefinedTaskId,
							userTaskId));

				istd::IChangeable::ChangeSet changeSet(CF_OBJECT_DATA_CHANGED);
				changeSet.SetChangeInfo(s_userDefinedTaskId, QVariant::fromValue<imtbase::IChangeablePtr>(eventPtr));
				istd::CChangeNotifier changeNotifier(this, &changeSet);

				task.userDefinedTaskId = userTaskId;
			}

			return true;
		}
	}

	return false;
}

QByteArray CTaskCollectionCompBase::GetTaskInputId(const QByteArray& taskUuid) const
{
	for (const Task& task : m_tasks){
		if (task.uuid == taskUuid){
			return task.inputId;
		}
	}

	return QByteArray();
}


bool CTaskCollectionCompBase::SetTaskInputId(const QByteArray& taskUuid, const QByteArray& inputId)
{
	// Set the new user-defined task-ID:
	for (Task& task : m_tasks){
		if (task.uuid == taskUuid){
			if (task.inputId != inputId){
				istd::TSmartPtr<istd::IChangeable> eventPtr;
				eventPtr.SetPtr(new imthype::CTaskCollectionUpdateEvent(
							taskUuid,
							imthype::CTaskCollectionUpdateEvent::UT_TASK_INPUT,
							task.inputId,
							inputId));

				istd::IChangeable::ChangeSet changeSet(CF_OBJECT_DATA_CHANGED);
				changeSet.SetChangeInfo(s_taskInputId, QVariant::fromValue<imtbase::IChangeablePtr>(eventPtr));
				istd::CChangeNotifier changeNotifier(this, &changeSet);

				task.inputId = inputId;

				imthype::ITaskCollectionContext* contextPtr = QueryInterface<imthype::ITaskCollectionContext>(task.taskPtr.GetPtr());
				if (contextPtr != nullptr){
					contextPtr->SetTaskInputId(inputId);
				}
			}

			return true;
		}
	}

	return false;
}


const iinsp::ISupplier* CTaskCollectionCompBase::GetTask(const QByteArray& taskId) const
{
	for (const Task& task : m_tasks){
		if (task.uuid == taskId){
			return task.taskPtr.GetPtr();
		}
	}

	return nullptr;
}


const ITaskResultCollection* CTaskCollectionCompBase::GetTaskResults(const QByteArray& taskId) const
{
	for (const Task& task : m_tasks){
		if (task.uuid == taskId){
			ITaskResultCollection* resultsPtr = CompCastPtr<ITaskResultCollection>(const_cast<iinsp::ISupplier*>(task.taskPtr.GetPtr()));

			return resultsPtr;
		}
	}

	return nullptr;
}


const imtbase::IObjectCollection* CTaskCollectionCompBase::GetTaskInputs() const
{
	return m_taskInputsCompPtr.GetPtr();
}


QString CTaskCollectionCompBase::GenerateUniqueObjectName(const QString& newName, const QString& oldName) const
{
	imtbase::ICollectionInfo::Ids ids = GetElementIds();

	QVector<QString> names;
	for (const QByteArray& id : ids){
		names.append(GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString());
	}

	QString uniqueName = newName;
	int counterName = 1;
	while (names.contains(uniqueName)){
		if (uniqueName == oldName){
			break;
		}

		uniqueName = newName + QString(" - %1").arg(counterName++);
	}

	return uniqueName;
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CTaskCollectionCompBase::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CTaskCollectionCompBase::GetDataController() const
{
	return nullptr;
}


int CTaskCollectionCompBase::GetOperationFlags(const QByteArray& objectId) const
{
	if (objectId.isEmpty()){
		int flags = 0;

		if (*m_allowAddTasksAttrPtr){
			flags |= OF_SUPPORT_INSERT;
		}

		flags |= OF_SUPPORT_PAGINATION;

		return flags;
	}

	for (const Task& task : m_tasks){
		if (task.uuid == objectId){
			return task.taskFlags;
		}
	}

	return 0;
}


idoc::MetaInfoPtr CTaskCollectionCompBase::GetDataMetaInfo(const Id& /*objectId*/) const
{
	return idoc::MetaInfoPtr();
}


QByteArray CTaskCollectionCompBase::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const Id& proposedObjectId,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	iinsp::ISupplierUniquePtr newTaskPtr(CreateTaskInstance(typeId));
	if (newTaskPtr.IsValid()){
		if (defaultValuePtr != nullptr){
			if (!newTaskPtr->CopyFrom(*defaultValuePtr)){
				return QByteArray();
			}
		}

		OnTaskCreated(*newTaskPtr);

		Task newTask;

		QString newName = name.isEmpty() ? GetTaskTypeName(typeId) : name;

		imtbase::ICollectionInfo::Ids ids = GetElementIds();
		QVector<QString> names;

		for(const QByteArray& elementId : ids){
			names.append(GetElementInfo(elementId, imtbase::ICollectionInfo::EIT_NAME).toString());
		}

		QString workingName = newName;
		int nameCounter = 1;
		while (names.contains(workingName)){
			workingName = newName + QString(" - %1").arg(nameCounter++);
		}

		newTask.taskPtr.TakeOver(newTaskPtr);
		newTask.typeId = typeId;
		newTask.name = workingName;
		newTask.description = description;

		if (!proposedObjectId.isEmpty() && !ids.contains(proposedObjectId)){
			newTask.uuid = proposedObjectId;
		}

		istd::IChangeable::ChangeSet changeSet(CF_ADDED);
		changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_ELEMENT_INSERTED, newTask.uuid);
		istd::CChangeNotifier changeNotifier(this, &changeSet);

		imthype::ITaskCollectionContext* contextPtr = QueryInterface<imthype::ITaskCollectionContext>(newTask.taskPtr.GetPtr());
		if (contextPtr != nullptr){
			contextPtr->SetTaskId(newTask.uuid);
		}

		m_tasks.push_back(newTask);

		return newTask.uuid;
	}

	return QByteArray();
}


bool CTaskCollectionCompBase::RemoveElements(const Ids& objectIds, const imtbase::IOperationContext* /*operationContextPtr*/)
{
	if (objectIds.isEmpty()){
		return false;
	}

	MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = objectIds;

	TaskList::iterator iter = m_tasks.begin();
	while (iter != m_tasks.end()){
		const QByteArray& taskId = iter->uuid;
		if (objectIds.contains(taskId)){
			OnTaskRemoved(taskId);
			iter = m_tasks.erase(iter);
		}
		else{
			++iter;
		}
	}

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENTS_REMOVED, QVariant::fromValue(notifierInfo));
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	return true;
}


bool CTaskCollectionCompBase::RemoveElementSet(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return false;
}


bool CTaskCollectionCompBase::RestoreObjects(
			const Ids& /*objectIds*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return false;
}


bool CTaskCollectionCompBase::RestoreObjectSet(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return false;
}


const istd::IChangeable* CTaskCollectionCompBase::GetObjectPtr(const QByteArray& objectId) const
{
	// TODO: think about using this code for staticaly defined tasks only!
	return GetTask(objectId);
}


bool CTaskCollectionCompBase::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	for (const Task& task : m_tasks){
		if ((task.uuid == objectId) && task.taskPtr.IsValid()){
			if (!dataPtr.IsValid()){
				iinsp::ISupplierUniquePtr newInstancePtr = CreateTaskInstance(task.typeId);
				if (newInstancePtr.IsValid()){
					if (newInstancePtr->CopyFrom(*task.taskPtr)){
						dataPtr.MoveCastedPtr(newInstancePtr);

						return true;
					}
				}
			}
			else{
				return dataPtr->CopyFrom(*task.taskPtr);
			}
		}
	}

	return false;
}


bool CTaskCollectionCompBase::SetObjectData(
			const QByteArray& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	for (Task& task : m_tasks){
		if ((task.uuid == objectId) && task.taskPtr.IsValid()){
			istd::IChangeable::ChangeSet changeSet(CF_OBJECT_DATA_CHANGED);
			changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED, objectId);

			istd::CChangeNotifier changeNotifier(this, &changeSet);

			return task.taskPtr->CopyFrom(object, mode);
		}
	}

	return false;
}


imtbase::IObjectCollectionUniquePtr CTaskCollectionCompBase::CreateSubCollection(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


imtbase::IObjectCollectionIterator* CTaskCollectionCompBase::CreateObjectCollectionIterator(
			const QByteArray& /*objectId*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


void CTaskCollectionCompBase::ResetObjectIds()
{
	istd::CChangeNotifier changeNotifier(this);

	QMutableVectorIterator<Task> taskIterator(m_tasks);
	while (taskIterator.hasNext()){
		Task& taskItem = taskIterator.next();

		taskItem.uuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		imthype::ITaskCollectionContext* contextPtr = QueryInterface<imthype::ITaskCollectionContext>(taskItem.taskPtr.GetPtr());
		if (contextPtr != nullptr){
			contextPtr->SetTaskId(taskItem.uuid);
		}
	}
}


// reimplemented (IObjectCollectionInfo)

imtbase::ICollectionInfo::Id CTaskCollectionCompBase::GetObjectTypeId(const QByteArray& objectId) const
{
	for (const Task& task : m_tasks){
		if (task.uuid == objectId){
			return task.typeId;
		}
	}

	return Id();
}


// reimplemented (ICollectionInfo)

int CTaskCollectionCompBase::GetElementsCount(const iprm::IParamsSet* /*selectionParamPtr*/, ilog::IMessageConsumer* /*logPtr*/) const
{
	return imtbase::narrow_cast<int>(m_tasks.count());
}


imtbase::ICollectionInfo::Ids CTaskCollectionCompBase::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	Ids retVal;

	qsizetype objectsCount = m_tasks.size();
	if( count >= 0) {
		objectsCount = qMin(count, m_tasks.size());
	}

	for (qsizetype i = offset; i < objectsCount; ++i){
		retVal.push_back(m_tasks[i].uuid);
	}

	return retVal;
}

bool CTaskCollectionCompBase::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CTaskCollectionCompBase::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	int taskIndex = -1;

	for (int i = 0; i < m_tasks.count(); ++i){
		if (m_tasks[i].uuid == elementId){
			 taskIndex = i;
			 break;
		}
	}

	if (taskIndex >= 0){
		switch (infoType){
		case EIT_DESCRIPTION:
			return m_tasks[taskIndex].description;

		case EIT_NAME:
			return m_tasks[taskIndex].name;

		case EIT_ENABLED:
			return m_tasks[taskIndex].isEnabled;
		default:
			I_IF_DEBUG(qWarning() << __FILE__ << __LINE__ << "Unexpected info type: " << infoType;)
			break;
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CTaskCollectionCompBase::GetElementMetaInfo(const Id& /*elementId*/, ilog::IMessageConsumer* /*logPtr*/) const
{
	return idoc::MetaInfoPtr();
}


bool CTaskCollectionCompBase::SetElementName(const QByteArray& elementId, const QString& objectName, ilog::IMessageConsumer* /*logPtr*/)
{
	for (Task& task : m_tasks){
		if (task.uuid == elementId){
			if (task.name != objectName){
				QString uniqueObjectName = GenerateUniqueObjectName(objectName, task.name);

				if (task.name != uniqueObjectName){
					istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_RENAMED);
					changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_ELEMENT_RENAMED, elementId);
					istd::CChangeNotifier changeNotifier(this, &changeSet);

					task.name = uniqueObjectName;
				}
			}

			return true;
		}
	}

	return false;
}


bool CTaskCollectionCompBase::SetElementDescription(const QByteArray& elementId, const QString& objectDescription, ilog::IMessageConsumer* /*logPtr*/)
{
	for (Task& task : m_tasks){
		if (task.uuid == elementId){
			if (task.description != objectDescription){
				istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_DESCRIPTION_CHANGED);
				changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_ELEMENT_DESCRIPTION_CHANGED, elementId);
				istd::CChangeNotifier changeNotifier(this, &changeSet);

				task.description = objectDescription;
			}

			return true;
		}
	}

	return false;
}


bool CTaskCollectionCompBase::SetElementEnabled(const QByteArray& objectId, bool isEnabled, ilog::IMessageConsumer* /*logPtr*/)
{
	for (Task& task : m_tasks){
		if (task.uuid == objectId){
			if (task.isEnabled != isEnabled){
				istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_STATE);
				changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_ELEMENT_STATE, objectId);
				istd::CChangeNotifier changeNotifier(this, &changeSet);

				task.isEnabled = isEnabled;
			}

			return true;
		}
	}

	return false;
}


// reimplemented (istd::IChangeable)

int CTaskCollectionCompBase::GetSupportedOperations() const
{
	return SO_COPY;
}


bool CTaskCollectionCompBase::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTaskCollectionCompBase* sourcePtr = dynamic_cast<const CTaskCollectionCompBase*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		QMutableVectorIterator<Task> taskIterator(m_tasks);
		while (taskIterator.hasNext()){
			Task& task = taskIterator.next();

			OnTaskRemoved(task.uuid);
		}

		m_tasks.clear();

		QVectorIterator<Task> sourceTaskIterator(sourcePtr->m_tasks);
		while (sourceTaskIterator.hasNext()){
			const Task& sourceTask = sourceTaskIterator.next();

			Task newTask;
			iinsp::ISupplierUniquePtr newTaskPtr = CreateTaskInstance(sourceTask.typeId);
			if (newTaskPtr.IsValid()){
				OnTaskCreated(*newTaskPtr);

				newTask.typeId = sourceTask.typeId;
				newTask.uuid = sourceTask.uuid;
				newTask.name = sourceTask.name;
				newTask.isEnabled = sourceTask.isEnabled;
				newTask.taskFlags = sourceTask.taskFlags;
				newTask.description = sourceTask.description;
				newTask.userDefinedTaskId = sourceTask.userDefinedTaskId;
				newTask.inputId = sourceTask.inputId;

				newTask.taskPtr.TakeOver(newTaskPtr);
				if (!newTask.taskPtr->CopyFrom(*sourceTask.taskPtr)){
					return false;
				}

				imthype::ITaskCollectionContext* contextPtr = QueryInterface<imthype::ITaskCollectionContext>(newTask.taskPtr.GetPtr());
				if (contextPtr != nullptr){
					contextPtr->SetTaskId(newTask.uuid);
					contextPtr->SetTaskInputId(newTask.inputId);
				}

				m_tasks.push_back(newTask);
			}
			else {
				Q_ASSERT(false);

				return false;
			}
		}

		return true;
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CTaskCollectionCompBase::Serialize(iser::IArchive& archive)
{
	static const iser::CArchiveTag tasksTag("Tasks", "List of tasks", iser::CArchiveTag::TT_MULTIPLE);
	static const iser::CArchiveTag taskTag("Task", "Single task", iser::CArchiveTag::TT_GROUP, &tasksTag);
	static const iser::CArchiveTag taskTypeIdTag("Type", "Type of the task", iser::CArchiveTag::TT_LEAF, &taskTag);
	static const iser::CArchiveTag taskUuidTag("Uuid", "UUID of the task", iser::CArchiveTag::TT_LEAF, &taskTag);
	static const iser::CArchiveTag taskNameTag("Name", "Name of the task", iser::CArchiveTag::TT_LEAF, &taskTag);
	static const iser::CArchiveTag taskDescriptionTag("Description", "Description of the task", iser::CArchiveTag::TT_LEAF, &taskTag);
	static const iser::CArchiveTag taskEnabledTag("IsEnabled", "Status of the task", iser::CArchiveTag::TT_LEAF, &taskTag);
	static const iser::CArchiveTag taskFlagsTag("Flags", "Flags of the task", iser::CArchiveTag::TT_LEAF, &taskTag);
	static const iser::CArchiveTag taskInputsTag("TaskInputs", "Collection of the task inputs", iser::CArchiveTag::TT_GROUP);

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	TaskList emptyList;
	TaskList& tasks = archive.IsStoring() ? m_tasks : emptyList;

	TaskIdMap taskIdMap;
	if (!archive.IsStoring()){
		for (TaskList::Iterator it = m_tasks.begin(); it != m_tasks.end(); ++it){
			taskIdMap.insert(it->uuid, it);
		}
	}

	int tasksCount = imtbase::narrow_cast<int>(tasks.size());

	bool retVal = true;
	retVal = retVal && archive.BeginMultiTag(tasksTag, taskTag, tasksCount);

	quint32 imtcoreVersion = 0xffffffff;
	archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, imtcoreVersion);

	for (int i = 0; i < tasksCount; ++i){
		if(!archive.IsStoring()){
			tasks.push_back(Task());
		}

		retVal = retVal && archive.BeginTag(taskTag);

		Task& task = tasks[i];

		retVal = retVal && archive.BeginTag(taskTypeIdTag);
		retVal = retVal && archive.Process(task.typeId);
		retVal = retVal && archive.EndTag(taskTypeIdTag);

		retVal = retVal && archive.BeginTag(taskUuidTag);
		retVal = retVal && archive.Process(task.uuid);
		retVal = retVal && archive.EndTag(taskUuidTag);

		retVal = retVal && archive.BeginTag(taskNameTag);
		retVal = retVal && archive.Process(task.name);
		retVal = retVal && archive.EndTag(taskNameTag);

		retVal = retVal && archive.BeginTag(taskDescriptionTag);
		retVal = retVal && archive.Process(task.description);
		retVal = retVal && archive.EndTag(taskDescriptionTag);

		retVal = retVal && archive.BeginTag(taskEnabledTag);
		retVal = retVal && archive.Process(task.isEnabled);
		retVal = retVal && archive.EndTag(taskEnabledTag);

		retVal = retVal && archive.BeginTag(taskFlagsTag);
		retVal = retVal && I_SERIALIZE_FLAG(OperationalFlags,archive, task.taskFlags);
		retVal = retVal && archive.EndTag(taskFlagsTag);

		static const iser::CArchiveTag userDefinedTaskIdTag("TaskId", "User-defined ID of the task", iser::CArchiveTag::TT_LEAF, &taskTag);
		static const iser::CArchiveTag inputIdTag("InputId", "Input-ID of the task", iser::CArchiveTag::TT_LEAF, &taskTag);

		retVal = retVal && archive.BeginTag(inputIdTag);
		retVal = retVal && archive.Process(task.inputId);
		retVal = retVal && archive.EndTag(inputIdTag);

		retVal = retVal && archive.BeginTag(userDefinedTaskIdTag);
		retVal = retVal && archive.Process(task.userDefinedTaskId);
		retVal = retVal && archive.EndTag(userDefinedTaskIdTag);

		static iser::CArchiveTag taskParametersTag("TaskParameters", "Task parameters", iser::CArchiveTag::TT_GROUP);
		retVal = retVal && archive.BeginTag(taskParametersTag);

		iser::ISerializable* serializablePtr = nullptr;
		if (archive.IsStoring()){
			serializablePtr = dynamic_cast<iser::ISerializable*>(task.taskPtr.GetPtr());
		}
		else{
			TaskIdMap::Iterator existingTaskIter = taskIdMap.find(task.uuid);
			if (existingTaskIter != taskIdMap.end()){
				Task& existingTask = *existingTaskIter.value();

				task.taskPtr = existingTask.taskPtr;

				serializablePtr = dynamic_cast<iser::ISerializable*>(task.taskPtr.GetPtr());

				imthype::ITaskCollectionContext* contextPtr = QueryInterface<imthype::ITaskCollectionContext>(task.taskPtr.GetPtr());
				if (contextPtr != nullptr){
					contextPtr->SetTaskId(task.uuid);
					contextPtr->SetTaskInputId(task.inputId);
				}
			}
			else{
				iinsp::ISupplierUniquePtr newTaskPtr = CreateTaskInstance(task.typeId);
				if (newTaskPtr.IsValid()){
					OnTaskCreated(*newTaskPtr);

					serializablePtr = dynamic_cast<iser::ISerializable*>(newTaskPtr.GetPtr());
					task.taskPtr.TakeOver(newTaskPtr);

					imthype::ITaskCollectionContext* contextPtr = QueryInterface<imthype::ITaskCollectionContext>(task.taskPtr.GetPtr());
					if (contextPtr != nullptr){
						contextPtr->SetTaskId(task.uuid);
						contextPtr->SetTaskInputId(task.inputId);
					}
				}
				else{
					SendErrorMessage(0, QString("Task (type-ID: '%1') could not be created").arg(task.typeId.constData()));
				}
			}
		}

		if (serializablePtr != nullptr){
			retVal = retVal && serializablePtr->Serialize(archive);
		}

		retVal = retVal && archive.EndTag(taskParametersTag);

		retVal = retVal && archive.EndTag(taskTag);

		if (!retVal){
			return false;
		}
	}

	if (!archive.IsStoring()){
		m_tasks = tasks;
	}

	retVal = retVal && archive.EndTag(tasksTag);

	iser::ISerializable* taskInputsSerializablePtr = dynamic_cast<iser::ISerializable*>(m_taskInputsCompPtr.GetPtr());
	if (taskInputsSerializablePtr != nullptr){
		retVal = retVal && archive.BeginTag(taskInputsTag);
		retVal = retVal && taskInputsSerializablePtr->Serialize(archive);
		retVal = retVal && archive.EndTag(taskInputsTag);
	}

	return retVal;
}


void CTaskCollectionCompBase::OnTaskCreated(iinsp::ISupplier& task)
{
	imod::IModel* parameterModelPtr = dynamic_cast<imod::IModel*>(task.GetModelParametersSet());
	if (parameterModelPtr != nullptr && !m_updateBridge.IsModelAttached(parameterModelPtr)){
		parameterModelPtr->AttachObserver(&m_updateBridge);
	}
}


void CTaskCollectionCompBase::OnTaskRemoved(const QByteArray& taskId)
{
	for (int i = 0; i < m_tasks.count(); ++i){
		if (m_tasks[i].uuid == taskId){
			Q_ASSERT(m_tasks[i].taskPtr.IsValid());

			imod::IModel* taskModelPtr = dynamic_cast<imod::IModel*>(m_tasks[i].taskPtr->GetModelParametersSet());
			if (taskModelPtr != nullptr){
				taskModelPtr->DetachObserver(&m_updateBridge);
			}

			break;
		}
	}
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CTaskCollectionCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int fixedTasksCount = qMin(qMin(m_fixedTasksCompPtr.GetCount(), m_fixedTaskIdsAttrPtr.GetCount()), m_fixedTaskTypeIdsAttrPtr.GetCount());

	for (int i = 0; i < fixedTasksCount; ++i){
		iinsp::ISupplier* supplierPtr = m_fixedTasksCompPtr[i];
		if (supplierPtr != nullptr){
			QByteArray uuid = m_fixedTaskIdsAttrPtr[i];
			Q_ASSERT(!uuid.isEmpty());
			if (uuid.isEmpty()){
				continue;
			}

			QByteArray typeId = m_fixedTaskTypeIdsAttrPtr[i];
			Q_ASSERT(!typeId.isEmpty());
			if (typeId.isEmpty()){
				continue;
			}

			QString name = "<unnamed>";
			QString typeName;
			if (i < m_fixedTaskTypeNamesAttrPtr.GetCount()){
				name = typeName = m_fixedTaskTypeNamesAttrPtr[i];
			}

			QString userId = "";
			if (i < m_fixedTaskUserIdsAttrPtr.GetCount()){
				userId = m_fixedTaskUserIdsAttrPtr[i];
			}

			Task task;
			task.isEnabled = true;
			task.name = name;
			task.taskFlags = OF_ALL & ~OF_SUPPORT_DELETE;
			task.taskPtr.SetUnmanagedPtr(supplierPtr);
			task.typeId = typeId;
			task.typeName = typeName;
			task.uuid = uuid;
			task.userDefinedTaskId = userId.toUtf8();

			m_tasks.push_back(task);

			OnTaskCreated(*supplierPtr);
		}
	}

	if (m_taskInputsModelCompPtr.IsValid() && m_taskInputsCompPtr.IsValid()){
		m_taskInputsModelCompPtr->AttachObserver(&m_updateBridge);
		m_taskInputsObserver.RegisterObject(m_taskInputsCompPtr.GetPtr(), &CTaskCollectionCompBase::OnTaskInputsUpdated);
	}
}


void CTaskCollectionCompBase::OnComponentDestroyed()
{
	m_taskInputsObserver.UnregisterAllObjects();
	m_updateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CTaskCollectionCompBase::OnTaskInputsUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* objectCollectionPtr)
{
	const istd::IChangeable::ChangeInfoMap& changeInfoMap = changeSet.GetChangeInfoMap();

	if (changeInfoMap.contains(imtbase::IObjectCollection::CN_ELEMENTS_REMOVED)){
		QSet<QByteArray> inputIds;

		imtbase::IObjectCollection::Ids ids = objectCollectionPtr->GetElementIds();
		for (const QByteArray& id : ids){
			QByteArray inputId = objectCollectionPtr->GetElementInfo(id, imtbase::IObjectCollection::EIT_NAME).toString().toUtf8();
			inputIds += inputId;
		}

		ids = GetElementIds();
		for (const QByteArray& id: ids){
			QByteArray inputId = GetTaskInputId(id);
			if (!inputIds.contains(inputId)){
				SetTaskInputId(id, QByteArray());
			}
		}
	}
	else if (changeInfoMap.contains(imtbase::IObjectCollection::CN_ELEMENT_RENAMED)){
		QVariantList changeInfos = changeInfoMap.values(imtbase::IObjectCollection::CN_ELEMENT_RENAMED);

		Q_ASSERT(changeInfos.count() == 2);

		QByteArray changedElementId = changeInfos[0].toByteArray();
		QString oldInputName = changeInfos[1].toString();

		QString newInputName = objectCollectionPtr->GetElementInfo(changedElementId, imtbase::IObjectCollection::EIT_NAME).toString().toUtf8();

		imtbase::ICollectionInfo::Ids taskIds = GetElementIds();
		for (const QByteArray& taskId : taskIds){
			QByteArray inputId = GetTaskInputId(taskId);
			if (inputId == oldInputName.toUtf8()){
				SetTaskInputId(taskId, newInputName.toUtf8());
			}
		}
	}
	else if (changeInfoMap.contains(s_taskInputId)){
		QVariant var = changeSet.GetChangeInfo(s_taskInputId);
		if (var.canConvert<imtbase::IChangeablePtr>()){
			imthype::CTaskCollectionUpdateEvent* eventPtr = dynamic_cast<imthype::CTaskCollectionUpdateEvent*>(var.value<imtbase::IChangeablePtr>().GetPtr());
			if (eventPtr != nullptr){
				if (eventPtr != nullptr){
					imtbase::IObjectCollection::Ids ids = GetElementIds();
					for (const QByteArray& id : ids){
						if (GetTaskInputId(id) == eventPtr->GetOldValue().toByteArray()){
							SetTaskInputId(id, eventPtr->GetNewValue().toByteArray());
						}
					}
				}
			}
		}
	}
}


} // namespace imthype


