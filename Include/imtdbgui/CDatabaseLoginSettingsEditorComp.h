#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
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
	I_END_COMPONENT;

protected:
	// reimplemented (imod::IModelEditor)
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

private Q_SLOTS:
	void on_HostEdit_editingFinished();
	void on_PortEdit_editingFinished();
	void on_DatabaseNameEdit_editingFinished();
	void on_UserEdit_editingFinished();
	void on_PasswordEdit_editingFinished();
};


} // namespace imtdbgui


