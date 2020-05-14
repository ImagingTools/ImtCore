#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiStatusProvider.h>
#include <GeneratedFiles/imtgui/ui_CMultiStatusProviderGuiComp.h>


namespace imtgui
{


class CMultiStatusProviderGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::MultiStatusProviderGui, imtbase::IMultiStatusProvider>
{
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::MultiStatusProviderGui, imtbase::IMultiStatusProvider> BaseClass;

	I_BEGIN_COMPONENT(CMultiStatusProviderGuiComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiRetranslate() override;

private:
	void UpdateCommonStatusGui(const istd::IInformationProvider& status);
	void UpdateStatusesGui(const imtbase::IMultiStatusProvider& statuses);

	static QIcon GetStatusIcon(istd::IInformationProvider::InformationCategory status);
};


} // namespace imtgui


