// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqtgui/TMakeStateIconWrapper.h>

// ImtCore includes
#include <imthypegui/TTaskCollectionEditorCompBase.h>
#include <GeneratedFiles/imthypegui/ui_CTaskManagerGuiComp.h>


namespace imthypegui
{


class CTaskManagerGuiComp: public TTaskCollectionEditorCompBase<Ui::CTaskManagerGuiComp>
{
	Q_OBJECT

public:
	typedef TTaskCollectionEditorCompBase<Ui::CTaskManagerGuiComp> BaseClass;

	I_BEGIN_COMPONENT(CTaskManagerGuiComp);
		I_ASSIGN(m_autoAssignUserIdAttrPtr, "AutoAssignUserId", "Automatically assign new User-ID for created and duplicated tasks", true, false);
		I_ASSIGN(m_autoRenameAttrPtr, "AutoRename", "Automatically renaming if the new task name entered by user already exists", true, false);
		I_ASSIGN_MULTI_0(m_taskEditorsFactCompPtr, "TaskEditors", "List of task editors", true);
		I_ASSIGN_TO(m_taskObserversFactCompPtr, m_taskEditorsFactCompPtr, true);
		I_ASSIGN_MULTI_0(m_editorTypeIdsAttrPtr, "TaskEditorTypeIds", "List of type IDs for corresponding task editors", true);
		I_ASSIGN(m_showAllExecuteButtonAttrPtr, "ShowAllExecute", "Show all execute button in the tool bar", true, true);
	I_END_COMPONENT;

	CTaskManagerGuiComp();

protected Q_SLOTS:
	void OnToggleTaskList(bool toggled);
	void OnTestAll();
	void OnAddTask();
	void OnDeleteTask();
	void OnDuplicateTask();
	void OnAddMenuOptionClicked(QAction* action);
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnMicroSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void on_TaskList_doubleClicked(const QModelIndex& index);
	void on_MicroTaskList_doubleClicked(const QModelIndex& index);
	void OnItemContextMenu(const QPoint& position);
	void OnMicroItemContextMenu(const QPoint& position);
	void OnToggleTask();
	void OnRenameTask();
	void OnShowInputsManager();

protected:
	// reimplemented (TTaskCollectionEditorCompBase)
	virtual QItemSelectionModel* GetTaskSelectionModel() const override;
	virtual void UpdateCommands() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;
	virtual void OnGuiDesignChanged() override;

private:
	void ShowContextMenu(const QPoint& position, QListView& list);

private:
	I_ATTR(bool, m_autoAssignUserIdAttrPtr);
	I_ATTR(bool, m_autoRenameAttrPtr);
	I_ATTR(bool, m_showAllExecuteButtonAttrPtr);
	I_MULTIFACT(iqtgui::IGuiObject, m_taskEditorsFactCompPtr);
	I_MULTIFACT(imod::IObserver, m_taskObserversFactCompPtr);
	I_MULTIATTR(QByteArray, m_editorTypeIdsAttrPtr);
};


} // namespace imthypegui


