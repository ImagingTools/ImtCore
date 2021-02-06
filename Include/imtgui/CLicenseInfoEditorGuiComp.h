#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/ILicenseInfo.h>
#include <GeneratedFiles/imtgui/ui_CLicenseInfoEditorGuiComp.h>


namespace imtgui
{


class CLicenseInfoEditorGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CLicenseInfoEditorGuiComp, imtbase::ILicenseInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CLicenseInfoEditorGuiComp, imtbase::ILicenseInfo> BaseClass;

	I_BEGIN_COMPONENT(CLicenseInfoEditorGuiComp);
	I_END_COMPONENT;

	CLicenseInfoEditorGuiComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
};


} // namespace imtgui


