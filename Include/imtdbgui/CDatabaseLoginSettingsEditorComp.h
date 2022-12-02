#pragma once


// ACF includes
#include <iprm/IEnableableParam.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtdb/IDatabaseLoginSettings.h>
#include <GeneratedFiles/imtdbgui/ui_CDatabaseLoginSettingsEditorComp.h>


namespace imtdbgui
{


class CDatabaseLoginSettingsEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CDatabaseLoginSettingsEditorComp, imtdb::IDatabaseLoginSettings>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CDatabaseLoginSettingsEditorComp, imtdb::IDatabaseLoginSettings> BaseClass;

	I_BEGIN_COMPONENT(CDatabaseLoginSettingsEditorComp);
		I_ASSIGN(m_addressEditEnablerCompPtr, "AddressFieldEditEnabler", "Enable/disable host and port editors", false, "AddressFieldEditEnabler");
		I_ASSIGN(m_databaseNameEditEnablerCompPtr, "DatabaseNameFieldEditEnabler", "Enable/disable editor for database name", false, "DatabaseNameFieldEditEnabler");
		I_ASSIGN(m_generalEditEnablerCompPtr, "GeneralEditEnabler", "Enable/disable whole editor for database parameter", false, "GeneralEditEnabler");
	I_END_COMPONENT;

	CDatabaseLoginSettingsEditorComp();

protected:
	// reimplemented (imod::IModelEditor)
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated () override;
	virtual void OnGuiDestroyed() override;

private Q_SLOTS:
	void on_HostEdit_editingFinished();
	void on_PortEdit_editingFinished();
	void on_DatabaseNameEdit_editingFinished();
	void on_UserEdit_editingFinished();
	void on_PasswordEdit_editingFinished();

private:
	void OnAddressEditEnabled(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* objectPtr);
	void OnDatabaseNameEditEnabled(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* objectPtr);
	void OnGeneralEditEnabled(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* objectPtr);

private:
	I_REF(iprm::IEnableableParam, m_addressEditEnablerCompPtr);
	I_REF(iprm::IEnableableParam, m_databaseNameEditEnablerCompPtr);
	I_REF(iprm::IEnableableParam, m_generalEditEnablerCompPtr);

	imtbase::TModelUpdateBinder<iprm::IEnableableParam, CDatabaseLoginSettingsEditorComp> m_addressEditEnablerObserver;
	imtbase::TModelUpdateBinder<iprm::IEnableableParam, CDatabaseLoginSettingsEditorComp> m_databaseNameEditEnablerObserver;
	imtbase::TModelUpdateBinder<iprm::IEnableableParam, CDatabaseLoginSettingsEditorComp> m_generalEditEnablerObserver;
};


} // namespace imtdbgui


