// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthypegui/CTaskManagerGuiComp.h>


// Qt includes
#include <QtWidgets/QMenu>
#include <QKeyEvent>


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
	connect(&m_showTriggerTableCommand, SIGNAL(triggered()), this, SLOT(OnShowTriggerTable()));
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
		m_commands.InsertChild(&m_showTriggerTableCommand);

		if (!toggled){
			m_commands.InsertChild(&m_executeAllTasksCommand);
			m_commands.InsertChild(&m_addCommand);
			m_commands.InsertChild(&m_deleteCommand);
			m_commands.InsertChild(&m_duplicateCommand);

			if (ShowListButton->menu() == nullptr) {
				auto listMenu = new QMenu(ShowListButton);
				listMenu->addActions(AddButton->actions());
				listMenu->addAction(&m_executeAllTasksCommand);
				ShowListButton->setMenu(listMenu);
			}

			MicroTaskList->setFocus();
		}
		else {
			TaskList->setFocus();
		}

		TaskActionFrame->setVisible(toggled);
	}
}


void CTaskManagerGuiComp::OnAddMenuOptionClicked(QAction* action)
{
	Q_ASSERT(action != nullptr);

	QByteArray typeId = action->data().toByteArray();

	bool autoAssignUserId = false;

	if (m_autoAssignUserIdAttrPtr.IsValid()) {
		autoAssignUserId = *m_autoAssignUserIdAttrPtr;
	}

	AddTask(typeId, autoAssignUserId);
}


void CTaskManagerGuiComp::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	OnTaskSelectionChanged(selected, deselected);

	if (selected.indexes().count())
		TaskList->setCurrentIndex(selected.indexes().first());

	// sync with micro tasks
	MicroTaskList->selectionModel()->select(selected, QItemSelectionModel::ClearAndSelect);
}


void CTaskManagerGuiComp::OnMicroSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	TaskList->selectionModel()->select(selected, QItemSelectionModel::ClearAndSelect);

	if (selected.indexes().count())
		MicroTaskList->setCurrentIndex(selected.indexes().first());
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

		itemMenu.addAction(GetIcon(":/Icons/Edit"), tr("Rename"), QKeySequence(Qt::Key_F2), this, SLOT(OnRenameTask()));

		if (*m_allowAddTasksAttrPtr) {
			itemMenu.addAction(m_duplicateCommand.icon(), tr("Duplicate"), this, SLOT(OnDuplicateTask()));
			itemMenu.addAction(m_deleteCommand.icon(), tr("Delete"), QKeySequence(Qt::Key_Delete), this, SLOT(OnDeleteTask()));
		}

		itemMenu.addSeparator();

		const imtbase::IObjectCollection* inputsCollectionPtr = GetObservedObject()->GetTaskInputs();
		if (inputsCollectionPtr != nullptr) {
			itemMenu.addAction(m_showInputsManagerCommand.icon(), tr("Input Manager..."), this, SLOT(OnShowInputsManager()));
		}

		// trigger table
		if (*m_showTriggerTableAttrPtr && m_triggerTableGuiCompPtr.IsValid()) {
			itemMenu.addAction(m_showTriggerTableCommand.icon(), tr("Trigger Table..."), this, SLOT(OnShowTriggerTable()));
		}

		if (m_taskSettingsGuiCompPtr.IsValid()) {
			QAction* settingsAction = itemMenu.addAction(GetIcon(":/Icons/Settings"), tr("Settings..."));
			connect(settingsAction, &QAction::triggered, [this, index]() { on_TaskList_doubleClicked(index); });
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
	imtbase::ICollectionInfo::Ids selectedIds = GetSelectedTaskIds();

	if (selectedIds.isEmpty()) {
		// No checkbox-selected tasks: fall back to deleting the currently Qt-selected task
		DeleteTask();
		return;
	}

	// Delete all checkbox-selected tasks
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr) {
		return;
	}

	QItemSelectionModel* selectionModelPtr = GetTaskSelectionModel();
	if (selectionModelPtr == nullptr) {
		return;
	}

	// Find the lowest selected row index to restore selection afterwards
	int lowestSelectedRow = m_itemModel.rowCount();
	for (const QByteArray& taskId : selectedIds) {
		const int rowCount = m_itemModel.rowCount();
		for (int i = 0; i < rowCount; ++i) {
			const QModelIndex index = m_itemModel.index(i, 0);
			if (index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray() == taskId) {
				lowestSelectedRow = qMin(lowestSelectedRow, i);
				break;
			}
		}
	}

	// Deselect all before deletion to avoid stale selection state
	selectionModelPtr->clearSelection();

	// Set target index: the item just before the first deleted one,
	// clamped to the remaining count after deletion
	const int remainingCount = m_itemModel.rowCount() - selectedIds.count();
	m_currentSelectedIndex = (remainingCount > 0) ? qMax(0, lowestSelectedRow - 1) : 0;

	{
		istd::CChangeGroup changeGroup(objectPtr);
		objectPtr->RemoveElements(selectedIds);
	}

	// UpdateGui() rebuilds the model and restores selection to m_currentSelectedIndex
	// Clamp in case the estimated index is now out of range
	const int rowCount = m_itemModel.rowCount();
	if (rowCount > 0) {
		m_currentSelectedIndex = qMin(m_currentSelectedIndex, rowCount - 1);
		selectionModelPtr->setCurrentIndex(
			selectionModelPtr->model()->index(m_currentSelectedIndex, 0),
			QItemSelectionModel::SelectCurrent);
	}
}


void CTaskManagerGuiComp::OnDuplicateTask()
{
	imtbase::ICollectionInfo::Ids selectedIds = GetSelectedTaskIds();

	if (selectedIds.isEmpty()){
		// No checkbox-selected tasks: fall back to duplicating the currently Qt-selected task
		DuplicateTask();
		return;
	}

	// Duplicate all checkbox-selected tasks
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return;
	}

	{
		istd::CChangeGroup changeGroup(objectPtr);

		for (const QByteArray& taskId : selectedIds){
			const QByteArray taskTypeId = objectPtr->GetObjectTypeId(taskId);
			const QString sourceTaskName = objectPtr->GetElementInfo(
				taskId,
				imtbase::ICollectionInfo::EIT_NAME).toString();
			const bool sourceEnabled = objectPtr->GetElementInfo(
				taskId,
				imtbase::ICollectionInfo::EIT_ENABLED).toBool();

			QByteArray newTaskId = objectPtr->InsertNewObject(
				taskTypeId,
				QString("Copy of %1").arg(sourceTaskName),
				QString(),
				objectPtr->GetObjectPtr(taskId));

			if (newTaskId.isEmpty()){
				QMessageBox::critical(
					GetWidget(),
					tr("Task Error"),
					tr("Task '%1' could not be duplicated!").arg(sourceTaskName));
				continue;
			}

			// Copy enabled state from source task
			objectPtr->SetElementEnabled(newTaskId, sourceEnabled);

			if (!objectPtr->GetUserTaskId(taskId).isEmpty()){
				imthype::ITaskCollection::AssignNextNumberedTaskId(*objectPtr, newTaskId);
			}
		}
	}

	// Select the last duplicated task
	QItemSelectionModel* selectionModelPtr = GetTaskSelectionModel();
	if (selectionModelPtr != nullptr){
		const int lastRow = selectionModelPtr->model()->rowCount() - 1;
		m_currentSelectedIndex = lastRow;
		selectionModelPtr->setCurrentIndex(
			selectionModelPtr->model()->index(lastRow, 0),
			QItemSelectionModel::SelectCurrent);
	}
}


void CTaskManagerGuiComp::OnToggleTask()
{
	imtbase::ICollectionInfo::Ids selectedIds = GetSelectedTaskIds();

	if (selectedIds.isEmpty()){
		// No checkbox-selected tasks: fall back to toggling the currently Qt-selected task
		ToggleTask();
		return;
	}

	// Toggle enabled state of all checkbox-selected tasks
	imthype::ITaskCollection* objectPtr = BaseClass::GetObjectPtr();
	if (objectPtr == nullptr){
		return;
	}

	// Determine new state from the first selected task
	const bool currentState = objectPtr->GetElementInfo(
		selectedIds.first(),
		imtbase::ICollectionInfo::EIT_ENABLED).toBool();
	const bool newState = !currentState;

	istd::CChangeGroup changeGroup(objectPtr);
	for (const QByteArray& taskId : selectedIds){
		objectPtr->SetElementEnabled(taskId, newState);
	}
}


void CTaskManagerGuiComp::OnToggleTaskSelection()
{
	auto index = TaskList->hasFocus() ? TaskList->currentIndex() : MicroTaskList->currentIndex();

	if (index.isValid()){
		bool selected = index.data(CTaskItemDelegate::DR_TASK_SELECTED).toBool();

		// Set the new selection state
		if (m_itemModel.setData(index, !selected, CTaskItemDelegate::DR_TASK_SELECTED))
		{
			// Update both list views
			TaskList->update(index);
			MicroTaskList->update(index);
		}
	}
}


void CTaskManagerGuiComp::OnSelectAllTasks()
{
	const int rowCount = m_itemModel.rowCount();
	if (rowCount == 0) {
		return;
	}

	// Determine if all tasks are already selected - if so, deselect all
	bool allSelected = true;
	for (int i = 0; i < rowCount; ++i) {
		const QModelIndex index = m_itemModel.index(i, 0);
		if (!index.data(CTaskItemDelegate::DR_TASK_SELECTED).toBool()) {
			allSelected = false;
			break;
		}
	}

	// Toggle: select all if not all selected, deselect all if all were selected
	const bool newState = !allSelected;
	for (int i = 0; i < rowCount; ++i) {
		const QModelIndex index = m_itemModel.index(i, 0);
		m_itemModel.setData(index, newState, CTaskItemDelegate::DR_TASK_SELECTED);
	}

	// Refresh both views
	TaskList->update();
	MicroTaskList->update();
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
	imthype::ITaskCollection* taskCollectionPtr = BaseClass::GetObjectPtr();
	if (!taskCollectionPtr || !m_taskInputManagerGuiCompPtr.IsValid() || !m_taskInputManagerObserverCompPtr.IsValid())
		return;

	imod::IModel* taskInputsModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(taskCollectionPtr->GetTaskInputs()));
	if (!taskInputsModelPtr || !taskInputsModelPtr->AttachObserver(m_taskInputManagerObserverCompPtr.GetPtr()))
		return;

	iqtgui::CGuiComponentDialog dialog(m_taskInputManagerGuiCompPtr.GetPtr(), QDialogButtonBox::Close);
	dialog.SetDialogGeometry(0.5);
	dialog.setWindowTitle(tr("Task Input Manager"));
	dialog.exec();

	taskInputsModelPtr->DetachObserver(m_taskInputManagerObserverCompPtr.GetPtr());
}


void CTaskManagerGuiComp::OnShowTriggerTable()
{
	imthype::ITaskCollection* taskCollectionPtr = BaseClass::GetObjectPtr();
	imod::IObserver* observerPtr = dynamic_cast<imod::IObserver*>(m_triggerTableGuiCompPtr.GetPtr());
	if (!observerPtr || !taskCollectionPtr)
		return;

	// copy the task collection (CloneMe will NOT copy the acquisitions, so we need to do it separately)
	auto modelCopy(taskCollectionPtr->CloneMe());
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(modelCopy.GetPtr());
	if (!modelCopy || !modelPtr)
		return;

	// copy the acquisitions here
	auto inputsPtr = taskCollectionPtr->GetTaskInputs();
	if (!inputsPtr)
		return;

	auto inputsCopy(inputsPtr->CloneMe());

	if (!modelPtr->AttachObserver(observerPtr))
		return;

	istd::CChangeGroup changeGroup(taskCollectionPtr);

	iqtgui::CGuiComponentDialog dialog(m_triggerTableGuiCompPtr.GetPtr(), QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	dialog.SetDialogGeometry(0.75);
	dialog.setWindowTitle(tr("Trigger-Inspection Mapping"));
	auto r = dialog.exec();

	// copy model changes back if accepted
	if (r == QDialog::Accepted) {
		taskCollectionPtr->CopyFrom(*modelCopy);
	}
	else {
		// reject input changes forcefully
		const_cast<imtbase::IObjectCollection*>(inputsPtr)->CopyFrom(*inputsCopy);
	}

	modelPtr->DetachObserver(observerPtr);
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

	// inspection count
	QString text = tr("Total Inspections: %1").arg(m_itemModel.rowCount());
	QFontMetrics fm(InspectionCount->font());
	QString elided = fm.elidedText(text, Qt::ElideLeft, InspectionCount->width());
	InspectionCount->setText(elided);
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

	TaskList->setFocus();


	const Qt::ConnectionType uniqueQueued = static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection);
	AddButton->setDefaultAction(&m_addCommand);
	connect(RemoveButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnDeleteTask, uniqueQueued);
	connect(DuplicateButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnDuplicateTask, uniqueQueued);
	connect(ExecuteAllButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnTestAll, uniqueQueued);

	m_showInputsManagerCommand.setVisible(m_taskInputManagerGuiCompPtr.IsValid() && m_taskInputManagerObserverCompPtr.IsValid());
	ShowInputManagerButton->setVisible(m_showInputsManagerCommand.isVisible());
	connect(ShowInputManagerButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnShowInputsManager, uniqueQueued);

	m_showTriggerTableCommand.setVisible(*m_showTriggerTableAttrPtr && m_triggerTableGuiCompPtr.IsValid());
	ShowTriggerTableButton->setVisible(m_showTriggerTableCommand.isVisible());
	if (m_showTriggerTableCommand.isVisible()) {
		connect(ShowTriggerTableButton, &QToolButton::clicked, this, &CTaskManagerGuiComp::OnShowTriggerTable, uniqueQueued);
	}

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
	m_showTriggerTableCommand.SetVisuals(tr("&Show Trigger Table"), tr("Show Trigger Table"), tr("Show editor of the triggers"), GetIcon(":/Icons/Production"));
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
	m_showTriggerTableCommand.setIcon(GetIcon(":/Icons/Table"));

	AddButton->setIcon(GetIcon(":/Icons/Add"));
	DuplicateButton->setIcon(GetIcon(":/Icons/Duplicate"));
	ExecuteAllButton->setIcon(GetIcon(":/Icons/Play"));
	RemoveButton->setIcon(GetIcon(":/Icons/Delete"));
	ShowInputManagerButton->setIcon(GetIcon(":/Icons/Capture"));
	ShowTriggerTableButton->setIcon(GetIcon(":/Icons/Table"));
}


void CTaskManagerGuiComp::OnGuiShown()
{
	BaseClass::OnGuiShown();

	// Reinstall event filter when the widget is shown
	if (TaskList != nullptr)
	{
		TaskList->installEventFilter(this);
		TaskList->viewport()->installEventFilter(this);
	}

	if (MicroTaskList != nullptr)
	{
		MicroTaskList->installEventFilter(this);
		MicroTaskList->viewport()->installEventFilter(this);
	}
}


void CTaskManagerGuiComp::OnGuiHidden()
{
	// Remove event filter when the widget is hidden
	if (TaskList != nullptr)
	{
		TaskList->viewport()->removeEventFilter(this);
		TaskList->removeEventFilter(this);
	}

	if (MicroTaskList != nullptr)
	{
		MicroTaskList->viewport()->removeEventFilter(this);
		MicroTaskList->removeEventFilter(this);
	}

	BaseClass::OnGuiHidden();
}


bool CTaskManagerGuiComp::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (watched == TaskList)
		{
			if (HandleListKeyPress(TaskList, keyEvent))
			{
				return true;
			}
		}

		if (watched == MicroTaskList)
		{
			if (HandleListKeyPress(MicroTaskList, keyEvent))
			{
				return true;
			}
		}
	}

	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() == Qt::LeftButton)
		{
			QListView* listView = nullptr;
			bool isMicro = false;

			if (watched == TaskList->viewport())
			{
				listView = TaskList;
				isMicro = false;
			}
			else if (watched == MicroTaskList->viewport())
			{
				listView = MicroTaskList;
				isMicro = true;
			}

			if (listView != nullptr)
			{
				const QModelIndex index = listView->indexAt(mouseEvent->pos());
				if (index.isValid())
				{
					// Compute checkbox rect in viewport coordinates
					const QRect itemRect = listView->visualRect(index);
					const QRect checkboxRect = isMicro
						? CMicroTaskItemDelegate::GetCheckboxRect(itemRect)
						: CTaskItemDelegate::GetCheckboxRect(itemRect);

					auto mpos = listView->mapFromGlobal(mouseEvent->globalPos());

					// If click lands on the checkbox, toggle DR_TASK_SELECTED
					if (checkboxRect.contains(mpos))
					{
						const bool selected = index.data(CTaskItemDelegate::DR_TASK_SELECTED).toBool();
						m_itemModel.setData(index, !selected, CTaskItemDelegate::DR_TASK_SELECTED);

						TaskList->update(index);
						MicroTaskList->update(index);

						// Consume the event so Qt's selection model is not affected
						return true;
					}

					// Ctrl+Click (outside checkbox) also toggles DR_TASK_SELECTED
					if (mouseEvent->modifiers() & Qt::ControlModifier)
					{
						const bool selected = index.data(CTaskItemDelegate::DR_TASK_SELECTED).toBool();
						m_itemModel.setData(index, !selected, CTaskItemDelegate::DR_TASK_SELECTED);

						TaskList->update(index);
						MicroTaskList->update(index);

						// Return false to allow Qt's own selection highlight to also update
						return false;
					}
				}
			}
		}
	}

	return BaseClass::eventFilter(watched, event);
}


bool CTaskManagerGuiComp::HandleListKeyPress(QListView* listView, QKeyEvent* keyEvent)
{
	if (listView == nullptr || keyEvent == nullptr)
	{
		return false;
	}
	
	int key = keyEvent->key();
	Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
	bool isCtrl = modifiers & Qt::ControlModifier;
	
	// Handle Delete key
	if (key == Qt::Key_Delete)
	{
		OnDeleteTask();
		return true;
	}
	
	// Handle F2 key for rename
	if (key == Qt::Key_F2)
	{
		OnRenameTask();
		return true;
	}
	
	// Handle Ctrl+D for duplicate
	if (key == Qt::Key_D && isCtrl)
	{
		OnDuplicateTask();
		return true;
	}
	
	// Handle Enter/Return for editing
	if (key == Qt::Key_Return || key == Qt::Key_Enter)
	{
		QModelIndex currentIndex = listView->currentIndex();
		if (currentIndex.isValid())
		{
			if (listView == TaskList)
			{
				on_TaskList_doubleClicked(currentIndex);
			}
			else if (listView == MicroTaskList)
			{
				on_MicroTaskList_doubleClicked(currentIndex);
			}
			return true;
		}
	}
	
	// Handle E + Ctrl for toggle enabling
	if (key == Qt::Key_E && isCtrl)
	{
		OnToggleTask();
		return true;
	}

	// Handle Space for toggle selection
	if (key == Qt::Key_Space)
	{
		OnToggleTaskSelection();
		return true;
	}

	// Handle A + Ctrl for select all
	if (key == Qt::Key_A && isCtrl)
	{
		OnSelectAllTasks();
		return true;
	}

	// Event not handled
	return false;
}


imtbase::ICollectionInfo::Ids CTaskManagerGuiComp::GetSelectedTaskIds() const
{
	imtbase::ICollectionInfo::Ids selectedIds;

	const int rowCount = m_itemModel.rowCount();
	for (int i = 0; i < rowCount; ++i) {
		const QModelIndex index = m_itemModel.index(i, 0);
		if (index.data(CTaskItemDelegate::DR_TASK_SELECTED).toBool()) {
			QByteArray taskId = index.data(CTaskItemDelegate::DR_TASK_UUID).toByteArray();
			if (!taskId.isEmpty()) {
				selectedIds.append(taskId);
			}
		}
	}

	return selectedIds;
}


} // namespace imthypegui


