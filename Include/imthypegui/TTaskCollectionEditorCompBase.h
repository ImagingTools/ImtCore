// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QItemSelection>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLineEdit>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <iimg/CBitmap.h>
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/CGuiComponentDialog.h>
#include <iqtgui/TMakeStateIconWrapper.h>
#include <imod/IModel.h>
#include <iprm/IOptionsList.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>

// ImtCore
#include <imtbase/TModelUpdateBinder.h>

// Acula includes
#include <imthype/ITaskCollection.h>
#include <imthype/ITaskResultCollection.h>
#include <imthype/ITaskSettings.h>
#include <imthypegui/CTaskItemDelegate.h>
#include <imthypegui/CTaskSettingsDialog.h>
#include <imthypegui/CTaskSettingsGuiComp.h>


namespace imthypegui
{


template <class UI>
class TTaskCollectionEditorCompBase:
	public iqtgui::StateIconWrapper<
			iqtgui::TDesignerGuiObserverCompBase<
							UI,
							imthype::ITaskCollection> >,
	protected imod::CMultiModelDispatcherBase,
	virtual public ibase::ICommandsProvider
{
	Q_DECLARE_TR_FUNCTIONS(TTaskCollectionEditorCompBase)

public:
	typedef iqtgui::StateIconWrapper< iqtgui::TDesignerGuiObserverCompBase<UI, imthype::ITaskCollection> > BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_BASE_COMPONENT(TTaskCollectionEditorCompBase);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_REGISTER_SUBELEMENT(TaskSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(TaskSettings, imthype::ITaskSettings, ExtractTaskSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(TaskSettings, istd::IChangeable, ExtractTaskSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(TaskSettings, imod::IModel, ExtractTaskSettings);
		I_ASSIGN(m_previewArtifactIdAttrPtr, "PreviewArtifactId", "ID of the artifact used for task tumbnail", true, "Preview");
		I_ASSIGN(m_allowAddTasksAttrPtr, "AllowAddTasks", "If enabled, adding the new task by user will be possible", true, true);
		I_ASSIGN(m_taskInputManagerGuiCompPtr, "TaskInputEditor", "Editor of task inputs", false, "TaskInputEditor");
		I_ASSIGN_TO(m_taskInputManagerObserverCompPtr, m_taskInputManagerGuiCompPtr, true);
		I_ASSIGN(m_taskSettingsGuiCompPtr, "TaskSettingsEditor", "Editor of task settings", false, "TaskSettingsGui");
		I_ASSIGN_TO(m_taskSettingsObserverCompPtr, m_taskSettingsGuiCompPtr, true);
	I_END_COMPONENT;

	TTaskCollectionEditorCompBase();
	~TTaskCollectionEditorCompBase();

protected:
	void TestAll();
	void AddTask(const QByteArray& typeId = QByteArray(), bool initUserId = false);
	void DeleteTask();
	void DuplicateTask();
	void OnTaskSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void EditTask(const QModelIndex& index);
	void ToggleTask();
	void RenameTask(bool autoRename = true);
	void ShowInputsManager();

	int GetLastNumberedUserId();

protected:
	virtual QItemSelectionModel* GetTaskSelectionModel() const = 0;

	virtual void UpdateCommands();
	void UpdateTaskItemState(const QModelIndex& index);
	imod::IModel* GetTaskModelFromSelection(const QItemSelection& itemSelection) const;
	imod::IObserver* GetTaskModelObserverFromSelection(const QItemSelection& itemSelection) const;

	void InfoToTaskSettings();
	void InfoFromTaskSettings();
	void OnTaskSettingsChanged(const istd::IChangeable::ChangeSet& /*changeset*/, const imthype::ITaskSettings* taskSettingsPtr);

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	class TaskSettings: virtual public imthype::ITaskSettings
	{
	public:
		TaskSettings();

		void SetParent(TTaskCollectionEditorCompBase* parentPtr);

		// reimplemented (imthype::ITaskSettings)
		virtual QString GetTaskName() const override;
		virtual void SetTaskName(const QString& taskName) override;
		virtual QString GetTaskDescription() const override;
		virtual void SetTaskDescription(const QString& taskDescription) override;
		virtual bool GetTaskEnabled() const override;
		virtual void SetTaskEnabled(bool isEnabled) override;
		virtual QByteArray GetUserTaskId() const override;
		virtual void SetUserTaskId(const QByteArray& userTaskId) override;
		virtual QByteArray GetTaskInputId() const override;
		virtual void SetTaskInputId(const QByteArray& inputId) override;
		virtual const imtbase::IObjectCollection* GetTaskInputs() const override;

	private:
		TTaskCollectionEditorCompBase* m_parentPtr = nullptr;

		QString m_taskName;
		QString m_taskDescription;
		bool m_isEnabled = false;
		QByteArray m_userTaskId;
		QByteArray m_inputId;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractTaskSettings(TTaskCollectionEditorCompBase& component)
	{
		return &component.m_taskSettings;
	}


	class Commands: virtual public ibase::ICommandsProvider
	{
	public:
		Commands();

		void SetParent(TTaskCollectionEditorCompBase* parentPtr);

	protected:
		// reimplemented (ibase::ICommandsProvider)
		virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	private:
		TTaskCollectionEditorCompBase* m_parentPtr = nullptr;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractCommands(TTaskCollectionEditorCompBase& component)
	{
		return &component.m_commandsProvider;
	}

protected:
	QStandardItemModel m_itemModel;

	typedef QMap<QByteArray, imod::IObserver*> ObserversMap;
	ObserversMap m_observersMap;

	typedef QMap<QByteArray, iqtgui::IGuiObjectSharedPtr> EditorsMap;
	EditorsMap m_editorsMap;

	typedef QMap<QByteArray, qsizetype> TypeToIndexMap;
	TypeToIndexMap m_typeToStackIndexMap;

	QMenu m_startVariableMenus;

	int m_currentSelectedIndex = -1;

	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_commands;
	iqtgui::CHierarchicalCommand m_showTaskListCommand;
	iqtgui::CHierarchicalCommand m_executeAllTasksCommand;
	iqtgui::CHierarchicalCommand m_addCommand;
	iqtgui::CHierarchicalCommand m_deleteCommand;
	iqtgui::CHierarchicalCommand m_duplicateCommand;
	iqtgui::CHierarchicalCommand m_showInputsManagerCommand;

	imod::TModelWrap<Commands> m_commandsProvider;

	imod::TModelWrap<TaskSettings> m_taskSettings;
	bool m_isTaskSettingsUpdating = false;
	QByteArray m_selectedTaskId;

	imtbase::TModelUpdateBinder<imthype::ITaskSettings, TTaskCollectionEditorCompBase<UI>> m_taskSettingsObserver;

	I_ATTR(QByteArray, m_previewArtifactIdAttrPtr);
	I_ATTR(bool, m_allowAddTasksAttrPtr);
	I_REF(iqtgui::IGuiObject, m_taskInputManagerGuiCompPtr);
	I_REF(imod::IObserver, m_taskInputManagerObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_taskSettingsGuiCompPtr);
	I_REF(imod::IObserver, m_taskSettingsObserverCompPtr);
};



template <class UI>
TTaskCollectionEditorCompBase<UI>::TTaskCollectionEditorCompBase()
	:m_currentSelectedIndex(0),
	m_showTaskListCommand("Show Task List", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_ONOFF, 1977),
	m_executeAllTasksCommand("Execute", 100, ibase::ICommand::CF_GLOBAL_MENU, 1978),
	m_addCommand("Add Task", 100, ibase::ICommand::CF_GLOBAL_MENU, 1979),
	m_deleteCommand("Remove Task", 100, ibase::ICommand::CF_GLOBAL_MENU, 1979),
	m_duplicateCommand("Duplicate Task", 100, ibase::ICommand::CF_GLOBAL_MENU, 1979),
	m_showInputsManagerCommand("Edit Inputs", 100, ibase::ICommand::CF_GLOBAL_MENU, 1980),
	m_isTaskSettingsUpdating(false),
	m_taskSettingsObserver(*this)
{
	m_rootCommands.InsertChild(&m_commands, false);

	m_commands.InsertChild(&m_showTaskListCommand);
	m_commands.InsertChild(&m_executeAllTasksCommand);
	m_commands.InsertChild(&m_addCommand);
	m_commands.InsertChild(&m_deleteCommand);
	m_commands.InsertChild(&m_duplicateCommand);
	m_commands.InsertChild(&m_showInputsManagerCommand);

	m_commandsProvider.SetParent(this);

	m_taskSettings.SetParent(this);
	m_taskSettingsObserver.RegisterObject(&m_taskSettings, &TTaskCollectionEditorCompBase<UI>::OnTaskSettingsChanged);
}


template <class UI>
TTaskCollectionEditorCompBase<UI>::~TTaskCollectionEditorCompBase()
{
	m_taskSettingsObserver.UnregisterAllObjects();
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::TestAll()
{
	imthype::ITaskCollection* objectPtr = BaseClass::BaseClass::GetObjectPtr();
	if (objectPtr != nullptr){
		imtbase::ICollectionInfo::Ids taskIds = objectPtr->GetElementIds();
		for (const QByteArray& taskId : taskIds){
			iinsp::ISupplier* taskPtr = const_cast<iinsp::ISupplier*>(objectPtr->GetTask(taskId));
			bool isTaskEnabled = objectPtr->GetElementInfo(taskId, imtbase::ICollectionInfo::EIT_ENABLED).toBool();

			if ((taskPtr != nullptr) && isTaskEnabled){
				taskPtr->InvalidateSupplier();

				taskPtr->EnsureWorkInitialized();
			}
		}

		for (const QByteArray& taskId : taskIds){
			iinsp::ISupplier* taskPtr = const_cast<iinsp::ISupplier*>(objectPtr->GetTask(taskId));
			bool isTaskEnabled = objectPtr->GetElementInfo(taskId, imtbase::ICollectionInfo::EIT_ENABLED).toBool();

			if ((taskPtr != nullptr) && isTaskEnabled){
				taskPtr->EnsureWorkFinished();
			}
		}
	}
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::AddTask(const QByteArray& typeId, bool initUserId)
{
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return;
	}

	const iprm::IOptionsList* taskTypesPtr = objectPtr->GetObjectTypesInfo();
	if (taskTypesPtr == nullptr || taskTypesPtr->GetOptionsCount() < 1){
		return;
	}

	QByteArray workingTypeId = typeId;
	if (workingTypeId.isEmpty()){
		workingTypeId = taskTypesPtr->GetOptionId(0);
	}

	istd::CChangeGroup changeGroup(objectPtr);
	Q_UNUSED(changeGroup);

	QByteArray taskId = objectPtr->InsertNewObject(workingTypeId, QString(), QString());

	if (taskId.isEmpty()){
		QMessageBox::critical(BaseClass::GetWidget(), QObject::tr("Task Manager"), QObject::tr("Task could not be created!"));
		return;
	}

	if (initUserId){
		int lastUserId = GetLastNumberedUserId();

		if (lastUserId >= 0){
			objectPtr->SetUserTaskId(taskId, QByteArray(QString::number(lastUserId + 1).toUtf8()));
		}
	}

	// select the recent task
	QItemSelectionModel* selectionModelPtr = GetTaskSelectionModel();
	int lastRow = selectionModelPtr->model()->rowCount() - 1;
	m_currentSelectedIndex = lastRow;
	selectionModelPtr->setCurrentIndex(selectionModelPtr->model()->index(lastRow, 0), QItemSelectionModel::SelectCurrent);
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::DeleteTask()
{
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return;
	}

	QItemSelectionModel* selectionModelPtr = GetTaskSelectionModel();
	if (selectionModelPtr == nullptr){
		return;
	}

	const QItemSelection currentSelection = selectionModelPtr->selection();
	const QModelIndexList indexes = currentSelection.indexes();
	if (indexes.isEmpty()){
		return;
	}

	int selectionIndex = m_currentSelectedIndex;
	selectionModelPtr->select(currentSelection, QItemSelectionModel::Deselect);

	imtbase::ICollectionInfo::Ids taskIds = objectPtr->GetElementIds();

	if (taskIds.count() > 1){
		m_currentSelectedIndex = selectionIndex > 0 ? selectionIndex - 1 : 0;
	}

	const QModelIndex index = indexes[0];

	QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();

	imtbase::ICollectionInfo::Ids elementIds;
	elementIds << taskId;

	objectPtr->RemoveElements(elementIds);
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::DuplicateTask()
{
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return;
	}

	QItemSelectionModel* selectionModelPtr = GetTaskSelectionModel();
	if (selectionModelPtr == nullptr){
		return;
	}

	const QItemSelection currentSelection = selectionModelPtr->selection();
	const QModelIndexList selectedIndexes = currentSelection.indexes();

	if (!selectedIndexes.isEmpty()){
		const QModelIndex index = selectedIndexes[0];

		QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();
		QByteArray taskTypeId = index.data(CTaskItemDelegate::DR_TYPE_ID).toByteArray();
		QString sourceTaskName = index.data(CTaskItemDelegate::DR_TASK_NAME).toString();

		istd::CChangeGroup changeGroup(objectPtr);
		Q_UNUSED(changeGroup);

		QByteArray newTaskId = objectPtr->InsertNewObject(taskTypeId, QString("Copy of %1").arg(sourceTaskName), "", objectPtr->GetObjectPtr(taskId));
		if (newTaskId.isEmpty()){
			QMessageBox::critical(BaseClass::GetWidget(), QObject::tr("Task Error"), QObject::tr("Task could not be duplicated!"));
			return;
		}

		if (!objectPtr->GetUserTaskId(taskId).isEmpty()){
			int lastUserId = GetLastNumberedUserId();
			if (lastUserId >= 0){
				objectPtr->SetUserTaskId(newTaskId, QByteArray(QString::number(lastUserId + 1).toUtf8()));
			}
		}
	}

	// select the recent task
	int lastRow = selectionModelPtr->model()->rowCount() - 1;
	m_currentSelectedIndex = lastRow;
	selectionModelPtr->setCurrentIndex(selectionModelPtr->model()->index(lastRow, 0), QItemSelectionModel::SelectCurrent);
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::OnTaskSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	int pageIndex = 0;
	m_currentSelectedIndex = 0;
	QString taskName;

	m_selectedTaskId.clear();

	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr != nullptr){
		imod::IModel* taskModelPtr = GetTaskModelFromSelection(deselected);
		imod::IObserver* observerPtr = GetTaskModelObserverFromSelection(deselected);
		if (taskModelPtr != nullptr && observerPtr != nullptr){
				taskModelPtr->DetachObserver(observerPtr);
		}

		const QModelIndexList selectedIndexes = selected.indexes();
		if (!selectedIndexes.isEmpty()){
			const QModelIndex index = selectedIndexes[0];

			QStandardItem* itemPtr = m_itemModel.itemFromIndex(index);
			if (itemPtr != nullptr){
				m_currentSelectedIndex = itemPtr->row();

				QByteArray typeId = itemPtr->data(CTaskItemDelegate::DR_TYPE_ID).toByteArray();
				pageIndex = m_typeToStackIndexMap[typeId];
				taskName = itemPtr->data(CTaskItemDelegate::DR_TASK_NAME).toString();
				m_selectedTaskId = itemPtr->data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();

				const iinsp::ISupplier* supplierPtr = objectPtr->GetTask(m_selectedTaskId);
				imod::IModel* selectedTaskModelPtr = dynamic_cast<imod::IModel*>(const_cast<iinsp::ISupplier*>(supplierPtr));
				imod::IObserver* selectedObserverPtr = m_observersMap[typeId];

				if (selectedTaskModelPtr && selectedObserverPtr && !selectedTaskModelPtr->IsAttached(selectedObserverPtr)){
					selectedTaskModelPtr->AttachObserver(selectedObserverPtr);
				}
			}
		}
	}

	BaseClass::TaskEditorStack->setCurrentIndex(pageIndex);

	BaseClass::SetStatusText(taskName);

	UpdateCommands();

	InfoToTaskSettings();
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::EditTask(const QModelIndex& /*index*/)
{
	imthype::ITaskCollection* objectPtr = BaseClass::GetObservedObject();
	if (objectPtr != nullptr && m_taskSettingsGuiCompPtr.IsValid() && m_taskSettingsObserverCompPtr.IsValid()){
		iqtgui::CGuiComponentDialog dialog(m_taskSettingsGuiCompPtr.GetPtr(), QDialogButtonBox::Ok);
		dialog.SetDialogGeometry(0.15);

		dialog.exec();
	}
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::ToggleTask()
{
	QItemSelectionModel* selectionModelPtr = GetTaskSelectionModel();
	if (selectionModelPtr == nullptr){
		return;
	}

    imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr != nullptr){
		const QItemSelection currentSelection = selectionModelPtr->selection();
		const QModelIndexList selectedIndexes = currentSelection.indexes();

		if (!selectedIndexes.isEmpty()){
			const QModelIndex index = selectedIndexes[0];

			QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();
			bool isEnabled = index.data(CTaskItemDelegate::DR_TASK_ENABLED).toBool();

			objectPtr->SetElementEnabled(taskId, !isEnabled);
		}
	}
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::RenameTask(bool autoRename)
{
	QItemSelectionModel* selectionModelPtr = GetTaskSelectionModel();
	if (selectionModelPtr == nullptr){
		return;
	}

	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr != nullptr){
		const QItemSelection currentSelection = selectionModelPtr->selection();
		const QModelIndexList selectedIndexes = currentSelection.indexes();

		if (!selectedIndexes.isEmpty()){
			const QModelIndex index = selectedIndexes[0];

			QString currentTaskName = index.data(CTaskItemDelegate::DR_TASK_NAME).toString();

			bool renamingFinished = false;

			while (!renamingFinished){
				bool isGetNameOk = false;
				const QString newName = QInputDialog::getText(BaseClass::GetWidget(), QObject::tr("Enter new task name"), QObject::tr("Name"), QLineEdit::Normal, currentTaskName, &isGetNameOk).trimmed();

				if (isGetNameOk){
					if (!newName.isEmpty()){
#if QT_VERSION > 0x060000
						QRegularExpressionValidator inputValidator(QRegularExpression("^[^\\\\/:\\*\\?\"\\<\\>\\|\\+]+$"));
#else
						QRegExpValidator inputValidator(QRegExp("^[^\\\\/:\\*\\?\"\\<\\>\\|\\+]+$"));
#endif
						int pos;
						QString name = newName;
						if (inputValidator.validate(name, pos) != QValidator::Acceptable){
							QMessageBox::critical(nullptr, tr("Error"), tr("The task name contains some not allowed characters"));

							return;
						}

						QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();

						bool isNewNameExists = false;

						if (!autoRename){
							imtbase::IObjectCollection::Ids ids = objectPtr->GetElementIds();
							for (const imtbase::IObjectCollection::Id& id : ids){
								QString taskName = objectPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();

								if (newName == taskName){
									isNewNameExists = true;
									break;
								}
							}
						}

						if (!isNewNameExists){
							objectPtr->SetElementName(taskId, newName);
							renamingFinished = true;
						}
						else {
							QMessageBox::warning(
								BaseClass::GetQtWidget(),
								QObject::tr("Warning"),
								QObject::tr("Task name already exist"));
						}
					}
					else{
						QMessageBox::warning(BaseClass::GetWidget(), QObject::tr("Wrong task name"), QObject::tr("Empty task name cannot be used"));
					}
				}
				else{
					renamingFinished = true;
				}
			}
		}
	}
}


template<class UI>
void TTaskCollectionEditorCompBase<UI>::ShowInputsManager()
{
	if (m_taskInputManagerGuiCompPtr.IsValid() && m_taskInputManagerObserverCompPtr.IsValid()){
		imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
		if (objectPtr != nullptr){
			const imtbase::IObjectCollection* taskInputsPtr = objectPtr->GetTaskInputs();
			if (taskInputsPtr != nullptr){
				const imod::IModel* taskInputsModelPtr = dynamic_cast<const imod::IModel*>(taskInputsPtr);
				if (taskInputsModelPtr != nullptr){
					if ((const_cast<imod::IModel*>(taskInputsModelPtr))->AttachObserver(m_taskInputManagerObserverCompPtr.GetPtr())){
						iqtgui::CGuiComponentDialog dialog(m_taskInputManagerGuiCompPtr.GetPtr(), QDialogButtonBox::Close, false, BaseClass::GetWidget());
						dialog.SetDialogGeometry(0.5);
						dialog.setWindowTitle(tr("Task Input Manager"));

						dialog.exec();

						(const_cast<imod::IModel*>(taskInputsModelPtr))->DetachObserver(m_taskInputManagerObserverCompPtr.GetPtr());
					}
				}
			}
		}
	}
}


template <class UI>
int TTaskCollectionEditorCompBase<UI>::GetLastNumberedUserId()
{
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return -1;
	}

	int lastUserId = 0;

	imtbase::IObjectCollection::Ids ids = objectPtr->GetElementIds();
	for (const imtbase::IObjectCollection::Id& id : ids){
		QByteArray userId = objectPtr->GetUserTaskId(id);

		bool isOk = false;
		int convertedUserId = userId.toInt(&isOk);

		if (isOk){
			if (convertedUserId > lastUserId){
				lastUserId = convertedUserId;
			}
		}
	}

	return lastUserId;
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::UpdateCommands()
{
	m_addCommand.setEnabled(false);
	m_deleteCommand.setEnabled(false);
	m_duplicateCommand.setEnabled(false);
	m_showInputsManagerCommand.setEnabled(false);

	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr != nullptr){
		m_addCommand.setEnabled(true);

		m_addCommand.setVisible(*m_allowAddTasksAttrPtr && (objectPtr->GetOperationFlags() & imthype::ITaskCollection::OF_SUPPORT_INSERT));
		m_duplicateCommand.setVisible(*m_allowAddTasksAttrPtr && (objectPtr->GetOperationFlags() & imthype::ITaskCollection::OF_SUPPORT_INSERT));

		const QModelIndexList selection = BaseClass::TaskList->selectionModel()->selectedIndexes();
		if (!selection.isEmpty()){
			const QModelIndex index = selection[0];

			QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();
			int flags = objectPtr->GetOperationFlags(taskId);

			m_deleteCommand.setVisible(*m_allowAddTasksAttrPtr);
			m_deleteCommand.setEnabled(*m_allowAddTasksAttrPtr && (flags & imthype::ITaskCollection::OF_SUPPORT_DELETE));

			m_duplicateCommand.setEnabled(*m_allowAddTasksAttrPtr);
		}

		m_showInputsManagerCommand.setEnabled(objectPtr->GetTaskInputs() != nullptr);
	}
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::InfoToTaskSettings()
{
	m_isTaskSettingsUpdating = true;

	QString name;
	QString description;
	bool isEnabled = false;
	QByteArray userTaskId;
	QByteArray inputId;

	imthype::ITaskCollection* objectPtr = BaseClass::GetObservedObject();
	if (objectPtr != nullptr && objectPtr->GetElementIds().contains(m_selectedTaskId)){
		name = objectPtr->GetElementInfo(m_selectedTaskId, imtbase::ICollectionInfo::EIT_NAME).toString();
		description = objectPtr->GetElementInfo(m_selectedTaskId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
		isEnabled = objectPtr->GetElementInfo(m_selectedTaskId, imtbase::ICollectionInfo::EIT_ENABLED).toBool();
		userTaskId = objectPtr->GetUserTaskId(m_selectedTaskId);
		inputId = objectPtr->GetTaskInputId(m_selectedTaskId);
	}

	{
		istd::CChangeGroup group(&m_taskSettings);

		m_taskSettings.SetTaskName(name);
		m_taskSettings.SetTaskDescription(description);
		m_taskSettings.SetTaskEnabled(isEnabled);
		m_taskSettings.SetUserTaskId(userTaskId);
		m_taskSettings.SetTaskInputId(inputId);
	}

	m_isTaskSettingsUpdating = false;
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::InfoFromTaskSettings()
{
	if (!m_isTaskSettingsUpdating){
		QString name = m_taskSettings.GetTaskName();
		QString description = m_taskSettings.GetTaskDescription();
		bool isEnabled = m_taskSettings.GetTaskEnabled();
		QByteArray userTaskId = m_taskSettings.GetUserTaskId();
		QByteArray inputId = m_taskSettings.GetTaskInputId();

		imthype::ITaskCollection* taskCollectionPtr = BaseClass::GetObservedObject();
		if (taskCollectionPtr != nullptr && taskCollectionPtr->GetElementIds().contains(m_selectedTaskId)){
			istd::CChangeGroup group(taskCollectionPtr);

			taskCollectionPtr->SetElementName(m_selectedTaskId, name);
			taskCollectionPtr->SetElementDescription(m_selectedTaskId, description);
			taskCollectionPtr->SetElementEnabled(m_selectedTaskId, isEnabled);
			taskCollectionPtr->SetTaskInputId(m_selectedTaskId, inputId);
			if (!taskCollectionPtr->SetUserTaskId(m_selectedTaskId, userTaskId)){
				QByteArray userId = taskCollectionPtr->GetUserTaskId(m_selectedTaskId);
				m_taskSettings.SetUserTaskId(userId);
				//taskCollectionPtr->SetUserTaskId(m_selectedTaskId, userId);
			}
		}
	}
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::UpdateTaskItemState(const QModelIndex& index)
{
	istd::IInformationProvider::InformationCategory taskState = istd::IInformationProvider::IC_NONE;

	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return;
	}

	QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();

	const iinsp::ISupplier* taskPtr = objectPtr->GetTask(taskId);
	if (taskPtr == nullptr){
		m_itemModel.setData(index, taskState, CTaskItemDelegate::DR_TASK_PROCESSING_STATE);
		m_itemModel.setData(index, this->GetCategoryPixmap(taskState), CTaskItemDelegate::DR_TASK_PROCESSING_STATE_ICON);
		return;
	}

	const istd::IInformationProvider* infoProviderPtr = dynamic_cast<const istd::IInformationProvider*>(taskPtr);
	if (infoProviderPtr != nullptr){
		taskState = infoProviderPtr->GetInformationCategory();
	}

	const imthype::ITaskResultCollection* taskResultsProviderPtr = CompCastPtr<const imthype::ITaskResultCollection>(taskPtr);
	if (taskResultsProviderPtr != nullptr){
		// \todo Call the rendering processor for preview generation at this place. Input is the artifact object, output is a bitmap!
		const iimg::IBitmap* inputBitmapPtr = dynamic_cast<const iimg::IBitmap*>(taskResultsProviderPtr->GetObjectPtr(*m_previewArtifactIdAttrPtr));
		if (inputBitmapPtr != nullptr){
			// check if this is already QImage, to avoid extra copying
			if (const iimg::CBitmap* imagePtr = dynamic_cast<const iimg::CBitmap*>(inputBitmapPtr)) {
				m_itemModel.setData(index, imagePtr->GetQImage(), CTaskItemDelegate::DR_TASK_PREVIEW_OBJECT);
			}
			else { // na ja, then copy it to QImage
				iimg::CBitmap previewBitmap;
				previewBitmap.CopyFrom(*inputBitmapPtr, istd::IChangeable::CM_CONVERT);

				m_itemModel.setData(index, previewBitmap.GetQImage(), CTaskItemDelegate::DR_TASK_PREVIEW_OBJECT);
			}
		}
	}

	m_itemModel.setData(index, taskState, CTaskItemDelegate::DR_TASK_PROCESSING_STATE);
	m_itemModel.setData(index, this->GetCategoryPixmap(taskState), CTaskItemDelegate::DR_TASK_PROCESSING_STATE_ICON);
}


template <class UI>
imod::IModel* TTaskCollectionEditorCompBase<UI>::GetTaskModelFromSelection(const QItemSelection& itemSelection) const
{
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return nullptr;
	}

	const QModelIndexList indexes = itemSelection.indexes();
	if (indexes.isEmpty()){
		return nullptr;
	}

	const QModelIndex index = indexes[0];

	QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();

	const iinsp::ISupplier* supplierPtr = objectPtr->GetTask(taskId);

	return dynamic_cast<imod::IModel*>(const_cast<iinsp::ISupplier*>(supplierPtr));
}


template <class UI>
imod::IObserver* TTaskCollectionEditorCompBase<UI>::GetTaskModelObserverFromSelection(const QItemSelection& itemSelection) const
{
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return nullptr;
	}

	const QModelIndexList indexes = itemSelection.indexes();
	if (indexes.isEmpty()){
		return nullptr;
	}

	const QModelIndex index = indexes[0];

	const QStandardItem* itemPtr = m_itemModel.itemFromIndex(index);
	Q_ASSERT(itemPtr != nullptr);

	QByteArray typeId = itemPtr->data(CTaskItemDelegate::DR_TYPE_ID).toByteArray();

	return m_observersMap[typeId];
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::OnTaskSettingsChanged(
			const istd::IChangeable::ChangeSet& /*changeset*/,
			const imthype::ITaskSettings* /*taskSettingsPtr*/)
{
	InfoFromTaskSettings();
}


// reimplemented (iqtgui::TGuiObserverWrap)

template <class UI>
void TTaskCollectionEditorCompBase<UI>::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr != nullptr){
		const iprm::IOptionsList* taskTypesPtr = objectPtr->GetObjectTypesInfo();
		if (taskTypesPtr != nullptr){
			int taskTypesCount = taskTypesPtr->GetOptionsCount();
			for (int i = 0; i < taskTypesCount; ++i){
				if (taskTypesPtr->IsOptionEnabled(i)){
					const QString typeName = taskTypesPtr->GetOptionName(i);

					QAction* action = m_startVariableMenus.addAction(typeName);
					action->setData(taskTypesPtr->GetOptionId(i));
				}
			}

			m_addCommand.setMenu(&m_startVariableMenus);

			QObject::connect(&m_startVariableMenus, SIGNAL(triggered(QAction*)), this, SLOT(OnAddMenuOptionClicked(QAction*)));
		}
	}
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT(BaseClass::IsGuiCreated());
	int currentSelectedIndex = m_currentSelectedIndex;

	if (changeSet.ContainsExplicit(istd::IChangeable::CF_DELEGATED, true)){
		return;
	}

	const QItemSelection currentSelection = BaseClass::TaskList->selectionModel()->selection();
	const QModelIndexList indexes = currentSelection.indexes();
	if (!indexes.isEmpty()){
		BaseClass::TaskList->selectionModel()->select(currentSelection, QItemSelectionModel::Deselect);
	}

	BaseClass::TaskList->selectionModel()->clear();
	m_itemModel.clear();
	m_itemModel.setColumnCount(1);

	BaseClass2::UnregisterAllModels();

	const imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		UpdateCommands();
		return;
	}

	int modelId = 0;

	const iprm::IOptionsList* typesInfoPtr = objectPtr->GetObjectTypesInfo();

	imtbase::ICollectionInfo::Ids taskIds = objectPtr->GetElementIds();
	for (const QByteArray& taskId : taskIds){
		QByteArray taskTypeId = objectPtr->GetObjectTypeId(taskId);
		QString taskTypeName = taskTypeId;
		if (typesInfoPtr != nullptr){
			for (int i = 0; i < typesInfoPtr->GetOptionsCount(); ++i){
				if (typesInfoPtr->GetOptionId(i) == taskTypeId){
					taskTypeName = typesInfoPtr->GetOptionName(i);
					break;
				}
			}
		}

		QStandardItem* taskItemPtr = new QStandardItem();
		taskItemPtr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		taskItemPtr->setData(taskId, CTaskItemDelegate::DR_TASK_UUID);
		taskItemPtr->setData(taskTypeId, CTaskItemDelegate::DR_TYPE_ID);
		taskItemPtr->setData(taskTypeName, CTaskItemDelegate::DR_TYPE_NAME);
		QString taskName = objectPtr->GetElementInfo(taskId, imtbase::ICollectionInfo::EIT_NAME).toString();
		taskItemPtr->setData(taskName, CTaskItemDelegate::DR_TASK_NAME);
		taskItemPtr->setData(objectPtr->GetElementInfo(taskId, imtbase::ICollectionInfo::EIT_ENABLED), CTaskItemDelegate::DR_TASK_ENABLED);
		taskItemPtr->setData(objectPtr->GetUserTaskId(taskId), CTaskItemDelegate::DR_TASK_USERID);
		taskItemPtr->setData(objectPtr->GetTaskInputId(taskId), CTaskItemDelegate::DR_TASK_INPUTID);

		taskItemPtr->setToolTip(
					tr("Task name: ") + taskName
					+ tr("\nUser-ID: ") + objectPtr->GetUserTaskId(taskId)
					+ tr("\nInput-ID: ") + objectPtr->GetTaskInputId(taskId));

		m_itemModel.appendRow(taskItemPtr);

		const iinsp::ISupplier* taskPtr = objectPtr->GetTask(taskId);
		imod::IModel* taskModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(taskPtr));
		if (taskModelPtr != nullptr){
			BaseClass2::RegisterModel(taskModelPtr, modelId++);
		}
	}

	int rowCount = m_itemModel.rowCount();

	if (currentSelectedIndex > rowCount - 1){
		currentSelectedIndex = rowCount - 1;
	}

	for (int i = 0; i < rowCount; ++i){
		const QModelIndex index = m_itemModel.index(i, 0);
		if (i == currentSelectedIndex){
			BaseClass::TaskList->selectionModel()->select(index, QItemSelectionModel::Select);
		}

		UpdateTaskItemState(index);
	}

	UpdateCommands();
}


// reimplemented (imod::CMultiModelDispatcherBase)

template <class UI>
void TTaskCollectionEditorCompBase<UI>::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const QModelIndex index = m_itemModel.index(modelId, 0);

	UpdateTaskItemState(index);
}


// public methods of the embedded class Commands

template <class UI>
TTaskCollectionEditorCompBase<UI>::Commands::Commands()
	:m_parentPtr(nullptr)
{
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::Commands::SetParent(TTaskCollectionEditorCompBase<UI>* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (ibase::ICommandsProvider)

template <class UI>
const ibase::IHierarchicalCommand* TTaskCollectionEditorCompBase<UI>::Commands::GetCommands() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	return &m_parentPtr->m_rootCommands;
}


// public methods of the embedded class TaskSettings

template <class UI>
TTaskCollectionEditorCompBase<UI>::TaskSettings::TaskSettings()
	:m_parentPtr(nullptr),
	m_isEnabled(false)
{
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::TaskSettings::SetParent(TTaskCollectionEditorCompBase* parentPtr)
{
	Q_ASSERT(parentPtr != nullptr);

	m_parentPtr = parentPtr;
}


// reimplemented (imthype::ITaskSettings)

template <class UI>
QString TTaskCollectionEditorCompBase<UI>::TaskSettings::GetTaskName() const
{
	return m_taskName;
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::TaskSettings::SetTaskName(const QString& taskName)
{
	if (m_taskName != taskName){
		istd::CChangeNotifier notifier(this);

		m_taskName = taskName;
	}
}


template <class UI>
QString TTaskCollectionEditorCompBase<UI>::TaskSettings::GetTaskDescription() const
{
	return m_taskDescription;
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::TaskSettings::SetTaskDescription(const QString& taskDescription)
{
	if (m_taskDescription != taskDescription){
		istd::CChangeNotifier notifier(this);

		m_taskDescription = taskDescription;
	}
}


template <class UI>
bool TTaskCollectionEditorCompBase<UI>::TaskSettings::GetTaskEnabled() const
{
	return m_isEnabled;
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::TaskSettings::SetTaskEnabled(bool isEnabled)
{
	if (m_isEnabled!= isEnabled){
		istd::CChangeNotifier notifier(this);

		m_isEnabled = isEnabled;
	}
}


template <class UI>
QByteArray TTaskCollectionEditorCompBase<UI>::TaskSettings::GetUserTaskId() const
{
	return m_userTaskId;
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::TaskSettings::SetUserTaskId(const QByteArray& userTaskId)
{
	if (m_userTaskId != userTaskId){
		istd::CChangeNotifier notifier(this);

		m_userTaskId = userTaskId;
	}
}


template <class UI>
QByteArray TTaskCollectionEditorCompBase<UI>::TaskSettings::GetTaskInputId() const
{
	return m_inputId;
}


template <class UI>
void TTaskCollectionEditorCompBase<UI>::TaskSettings::SetTaskInputId(const QByteArray& inputId)
{
	if (m_inputId != inputId){
		istd::CChangeNotifier notifier(this);

		m_inputId = inputId;
	}
}


template <class UI>
const imtbase::IObjectCollection* TTaskCollectionEditorCompBase<UI>::TaskSettings::GetTaskInputs() const
{
	Q_ASSERT(m_parentPtr != nullptr);

	imthype::ITaskCollection* taskCollectionPtr = m_parentPtr->GetObservedObject();
	if (taskCollectionPtr != nullptr){
		return taskCollectionPtr->GetTaskInputs();
	}

	return nullptr;
}


} // namespace imthypegui


