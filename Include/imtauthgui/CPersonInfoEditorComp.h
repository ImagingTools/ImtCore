#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtauth/IAddress.h>
#include <GeneratedFiles/imtauthgui/ui_CPersonInfoEditorComp.h>


namespace imtauthgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CPersonInfoEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CPersonInfoEditorComp, imtauth::IAddress>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CPersonInfoEditorComp, imtauth::IAddress> BaseClass;

	I_BEGIN_COMPONENT(CPersonInfoEditorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private Q_SLOTS:
	void on_CountryEdit_editingFinished();
	void on_CityEdit_editingFinished();
	void on_PostalCodeEdit_editingFinished();

};


} // namespace imtauthgui


