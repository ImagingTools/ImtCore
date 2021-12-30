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

	CDatabaseLoginSettingsEditorComp();

protected:
	// reimplemented (imod::IModelEditor)
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
};


} // namespace imtdbgui


