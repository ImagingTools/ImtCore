// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthypegui/CTaskManagerGuiComp.h>


// Qt includes
#include <QtWidgets/QMenu>


namespace imthypegui
{


// public methods

CTaskManagerGuiComp::CTaskManagerGuiComp()
{
	m_showTaskListCommand.setChecked(true);

	connect(&m_showTaskListCommand, SIGNAL(toggled(bool)), this, SLOT(OnToggleTaskList(bool)));
	connect(&m_executeAllTasksCommand, SIGNAL(triggered()), this, SLOT(OnTestAll()));
	connect(&m_addCommand, SIGNAL(triggered()), this, SLOT(OnAddTask()));
	connect(&m_deleteCommand, SIGNAL(triggered()), this, SLOT(OnDeleteTask()));
	connect(&m_duplicateCommand, SIGNAL(triggered()), this, SLOT(OnDuplicateTask()));
	connect(&m_showInputsManagerCommand, SIGNAL(triggered()), this, SLOT(OnShowInputsManager()));
}


// protected slots

void CTaskManagerGuiComp::OnToggleTaskList(bool toggled)
{
	if (IsGuiCreated()){
		LeftFrame->setVisible(toggled);
		SideFrame->setVisible(!toggled);

		istd::CChangeNotifier notifier(&m_commandsProvider);
		m_commands.ResetChilds();
		m_commands.InsertChild(&m_showTaskListCommand);
		m_commands.InsertChild(&m_showInputsManagerCommand);

		if (toggled){
			m_commands.InsertChild(&m_executeAllTasksCommand);
			m_commands.InsertChild(&m_addCommand);
			m_commands.InsertChild(&m_deleteCommand);
			m_commands.InsertChild(&m_duplicateCommand);
		}
		else {
			if (ShowListButton->menu() == nullptr) {
				QMenu* listMenu = new QMenu(ShowListButton);
				listMenu->addActions(AddButton->actions());
				listMenu->addAction(&m_executeAllTasksCommand);
				ShowListButton->setMenu(listMenu);
			}
		}

		TaskActionFrame->setVisible(toggled);
	}
}


void CTaskManagerGuiComp::OnTestAll()
{
	TestAll();

	QCoreApplication::removePostedEvents(this, QEvent::MetaCall); // prevents endless testing if test button is clicked continuously
}


void CTaskManagerGuiComp::OnAddTask()
{
	bool autoAssignUserId = false;

	if (m_autoAssignUserIdAttrPtr.IsValid()){
		autoAssignUserId = *m_autoAssignUserIdAttrPtr;
	}

	AddTask(QByteArray(), autoAssignUserId);
}


void CTaskManagerGuiComp::OnDeleteTask()
{
	DeleteTask();
}


void CTaskManagerGuiComp::OnDuplicateTask()
{
	DuplicateTask();
}


void CTaskManagerGuiComp::OnAddMenuOptionClicked(QAction* action)
{
	Q_ASSERT(action != nullptr);

	QByteArray typeId = action->data().toByteArray();

	bool autoAssignUserId = false;

	if (m_autoAssignUserIdAttrPtr.IsValid()){
		autoAssignUserId = *m_autoAssignUserIdAttrPtr;
	}

	AddTask(typeId, autoAssignUserId);
}


void CTaskManagerGuiComp::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	OnTaskSelectionChanged(selected, deselected);

	MicroTaskList->selectionModel()->select(selected, QItemSelectionModel::ClearAndSelect);
}


void CTaskManagerGuiComp::OnMicroSelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
	TaskList->selectionModel()->select(selected, QItemSelectionModel::ClearAndSelect);
}


void CTaskManagerGuiComp::on_TaskList_doubleClicked(const QModelIndex& index)
{
	EditTask(index);
}


void CTaskManagerGuiComp::on_MicroTaskList_doubleClicked(const QModelIndex& index)
{
	EditTask(index);
}


void CTaskManagerGuiComp::ShowContextMenu(const QPoint& position, QListView& list)
{
	QModelIndex index = list.indexAt(position);
	if (index.isValid()) {
		bool isEnabled = index.data(CTaskItemDelegate::DR_TASK_ENABLED).toBool();

		QString actionText = isEnabled ? tr("Disable") : tr("Enable");

		QMenu itemMenu;
		itemMenu.addAction(actionText, this, SLOT(OnToggleTask()));
		itemMenu.addSeparator();

		itemMenu.addAction(GetIcon(":/Icons/Edit"), tr("Rename"), this, SLOT(OnRenameTask()), QKeySequence(Qt::Key_F2));

		if (*m_allowAddTasksAttrPtr){
			itemMenu.addAction(m_duplicateCommand.icon(), tr("Duplicate"), this, SLOT(OnDuplicateTask()));
			itemMenu.addAction(m_deleteCommand.icon(), tr("Delete"), this, SLOT(OnDeleteTask()), QKeySequence(Qt::Key_Delete));
		}

		const imtbase::IObjectCollection* inputsCollectionPtr = GetObservedObject()->GetTaskInputs();
		if (inputsCollectionPtr != nullptr){
			itemMenu.addAction(m_showInputsManagerCommand.icon(), tr("Show Input Manager..."), this, SLOT(OnShowInputsManager()));
		}

		if (m_taskSettingsGuiCompPtr.IsValid()){
			QAction* settingsAction = itemMenu.addAction(GetIcon(":/Icons/Settings"), tr("Settings"));
			connect(settingsAction, &QAction::triggered, [this, index](){ on_TaskList_doubleClicked(index); });
		}

		itemMenu.exec(list.mapToGlobal(position));
	}
}


void CTaskManagerGuiComp::OnMicroItemContextMenu(const QPoint& position)
{
	ShowContextMenu(position, *MicroTaskList);
}


void CTaskManagerGuiComp::OnItemContextMenu(const QPoint& position)
{
	ShowContextMenu(position, *TaskList);
}


void CTaskManagerGuiComp::OnToggleTask()
{
	ToggleTask();
}


void CTaskManagerGuiComp::OnRenameTask()
{
	bool autoRename = true;

	if (m_autoRenameAttrPtr.IsValid()){
		autoRename = *m_autoRenameAttrPtr;
	}

	RenameTask(autoRename);
}


void CTaskManagerGuiComp::OnShowInputsManager()
{
	ShowInputsManager();
}


// protected methods

// reimplemented (TTaskCollectionEditorCompBase)

QItemSelectionModel* CTaskManagerGuiComp::GetTaskSelectionModel() const
{
	return TaskList->selectionModel();
}


void CTaskManagerGuiComp::UpdateCommands()
{
	BaseClass::UpdateCommands();

	AddButton->setVisible(m_addCommand.isVisible());
	AddButton->setEnabled(m_addCommand.isEnabled());

	RemoveButton->setVisible(m_deleteCommand.isVisible());
	RemoveButton->setEnabled(m_deleteCommand.isEnabled());

	DuplicateButton->setVisible(m_duplicateCommand.isVisible());
	DuplicateButton->setEnabled(m_duplicateCommand.isEnabled());

	ExecuteAllButton->setVisible(*m_showAllExecuteButtonAttrPtr && m_executeAllTasksCommand.isVisible());
	ExecuteAllButton->setEnabled(*m_showAllExecuteButtonAttrPtr && m_executeAllTasksCommand.isEnabled());
}


// reimplemented (iqtgui::CGuiComponentBase)

void CTaskManagerGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	const int taskEditorsCount = qMin(m_taskEditorsFactCompPtr.GetCount(), m_editorTypeIdsAttrPtr.GetCount());
	for (int i = 0; i < taskEditorsCount; ++i){
		const QByteArray editorTypeId = m_editorTypeIdsAttrPtr[i];
		Q_ASSERT(!editorTypeId.isEmpty());

		iqtgui::IGuiObjectSharedPtr editorPtr = m_taskEditorsFactCompPtr.CreateInstance(i);
		if (!editorPtr.IsValid()){
			m_editorsMap.clear();
			m_observersMap.clear();
			m_typeToStackIndexMap.clear();

			return;
		}

		icomp::IComponent* editorComponentPtr = dynamic_cast<icomp::IComponent*>(editorPtr.GetBasePtr().get());

		imod::IObserver* observerPtr = m_taskObserversFactCompPtr.ExtractInterface(editorComponentPtr);
		Q_ASSERT(observerPtr != NULL);

		m_editorsMap[editorTypeId] = editorPtr;
		m_observersMap[editorTypeId] = observerPtr;
		m_typeToStackIndexMap[editorTypeId] = i + 1;

		istd::TDelPtr<QWidget> pageWidgetPtr(new QWidget(TaskEditorStack));
		QVBoxLayout* layoutPtr = new QVBoxLayout(pageWidgetPtr.GetPtr());
		layoutPtr->setContentsMargins(0,0,0,0);

		if (editorPtr->CreateGui(pageWidgetPtr.GetPtr())){
			TaskEditorStack->addWidget(pageWidgetPtr.PopPtr());
		}
	}

	TaskEditorStack->setCurrentIndex(0);

	TaskList->setModel(&m_itemModel);
	TaskList->setItemDelegate(new CTaskItemDelegate(this));
	TaskList->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(TaskList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CTaskManagerGuiComp::OnSelectionChanged);
	connect(TaskList, &QWidget::customContextMenuRequested, this, &CTaskManagerGuiComp::OnItemContextMenu);


	MicroTaskList->setModel(&m_itemModel);
	MicroTaskList->setItemDelegate(new CMicroTaskItemDelegate(this));
	MicroTaskList->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(MicroTaskList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CTaskManagerGuiComp::OnMicroSelectionChanged);
	connect(MicroTaskList, &QWidget::customContextMenuRequested, this, &CTaskManagerGuiComp::OnItemContextMenu);


	const Qt::ConnectionType uniqueQueued = static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection);
	AddButton->setDefaultAction(&m_addCommand);
	connect(RemoveButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnDeleteTask, uniqueQueued);
	connect(DuplicateButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnDuplicateTask, uniqueQueued);
	connect(ExecuteAllButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnTestAll, uniqueQueued);

	m_showInputsManagerCommand.setVisible(m_taskInputManagerGuiCompPtr.IsValid() && m_taskInputManagerObserverCompPtr.IsValid());
	connect(ShowInputManagerButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnShowInputsManager, uniqueQueued);

	AddButton->setPopupMode(QToolButton::InstantPopup);

	HideListButton->setIcon(GetIcon(":/Icons/Menu"));
	connect(HideListButton, &QToolButton::clicked, [=]() {m_showTaskListCommand.toggle(); });
	ShowListButton->setIcon(GetIcon(":/Icons/Menu"));
	connect(ShowListButton, &QToolButton::clicked, [=]() {m_showTaskListCommand.toggle(); });
	SideFrame->hide();
}


void CTaskManagerGuiComp::OnGuiDestroyed()
{
	for (EditorsMap::Iterator iter = m_editorsMap.begin(); iter != m_editorsMap.end(); ++iter){
		iqtgui::IGuiObject* editorPtr = iter.value().GetPtr();
		Q_ASSERT(editorPtr != NULL);

		if (editorPtr->IsGuiCreated()){
			editorPtr->DestroyGui();
		}
	}

	while (TaskEditorStack->count() > 1){
		for (int i = 1; i < TaskEditorStack->count(); ++i){
			TaskEditorStack->removeWidget(TaskEditorStack->widget(i));
		}
	}

	BaseClass::OnGuiDestroyed();
}


void CTaskManagerGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_showTaskListCommand.SetVisuals(tr("&Show Task List"), tr("Show Tasks"), tr("Show task list"), GetIcon(":/Icons/Menu"));
	m_executeAllTasksCommand.SetVisuals(tr("&Execute All Tasks"), tr("Execute"), tr("Execute all tasks"), GetIcon(":/Icons/Play"));
	m_addCommand.SetVisuals(tr("&Add Task"), tr("Add"), tr("Add a new task"), GetIcon(":/Icons/Add"));
	m_deleteCommand.SetVisuals(tr("&Remove Task"), tr("Remove"), tr("Remove current task"), GetIcon(":/Icons/Delete"));
	m_duplicateCommand.SetVisuals(tr("&Duplicate Task"), tr("Duplicate"), tr("Duplicate current task"), GetIcon(":/Icons/Duplicate"));
	m_showInputsManagerCommand.SetVisuals(tr("&Show Input Manager"), tr("Show Input Manager"), tr("Show editor of the task inputs"), GetIcon(":/Icons/Capture"));
}


void CTaskManagerGuiComp::OnGuiDesignChanged()
{
	BaseClass::OnGuiDesignChanged();

	m_showTaskListCommand.setIcon(GetIcon(":/Icons/Menu"));
	m_executeAllTasksCommand.setIcon(GetIcon(":/Icons/Play"));
	m_addCommand.setIcon(GetIcon(":/Icons/Add"));
	m_deleteCommand.setIcon(GetIcon(":/Icons/Delete"));
	m_duplicateCommand.setIcon(GetIcon(":/Icons/Duplicate"));
	m_showInputsManagerCommand.setIcon(GetIcon(":/Icons/Capture"));

	AddButton->setIcon(GetIcon(":/Icons/Add"));
	DuplicateButton->setIcon(GetIcon(":/Icons/Duplicate"));
	ExecuteAllButton->setIcon(GetIcon(":/Icons/Play"));
	RemoveButton->setIcon(GetIcon(":/Icons/Delete"));
	ShowInputManagerButton->setIcon(GetIcon(":/Icons/Capture"));
}


} // namespace imthypegui


