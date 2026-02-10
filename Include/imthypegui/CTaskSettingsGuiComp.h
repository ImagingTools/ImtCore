// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#if QT_VERSION > 0x060000
#include <QtGui/QRegularExpressionValidator>
#endif

// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>

// Acula includes
#include <imthype/ITaskSettings.h>
#include <GeneratedFiles/imthypegui/ui_CTaskSettingsGuiComp.h>


namespace imthypegui
{


class CTaskSettingsGuiComp: public iqtgui::TDesignerGuiObserverCompBase<Ui::CTaskSettingsGuiComp, imthype::ITaskSettings>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CTaskSettingsGuiComp, imthype::ITaskSettings> BaseClass;

	I_BEGIN_COMPONENT(CTaskSettingsGuiComp);
		I_ASSIGN(m_taskInputManagerGuiCompPtr, "TaskInputEditor", "Editor of task inputs", false, "TaskInputEditor");
		I_ASSIGN_TO(m_taskInputManagerObserverCompPtr, m_taskInputManagerGuiCompPtr, true);
	I_END_COMPONENT;

	CTaskSettingsGuiComp();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const override;
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDesignChanged() override;

	// reimplemented (iqtgui::IGuiObject)
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;

private Q_SLOTS:
	void on_TaskNameEdit_editingFinished();
	void on_TaskDescriptionEdit_editingFinished();
	void on_TaskUserIdEdit_editingFinished();
	void on_InputIdEdit_editingFinished();
	void on_InputSelector_currentTextChanged(const QString& text);
	void on_ActiveCheck_stateChanged(int state);
	void on_ShowAcquisitionManagerButton_clicked();

private:
	void OnInputsChanged(const istd::IChangeable::ChangeSet& /*changeset*/, const imtbase::IObjectCollection* inputsCollectionPtr);
	void ShowInputsManager();

private:
	I_REF(iqtgui::IGuiObject, m_taskInputManagerGuiCompPtr);
	I_REF(imod::IObserver, m_taskInputManagerObserverCompPtr);

#if QT_VERSION > 0x060000
	QRegularExpressionValidator m_taskNameValidator;
#else
	QRegExpValidator m_taskNameValidator;
#endif
	mutable QByteArray m_taskId;

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CTaskSettingsGuiComp> m_inputsObserver;
};


} // namespace imthypegui


