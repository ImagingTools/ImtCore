#pragma once


// Acf includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiStatusProvider.h>
#include <GeneratedFiles/imtgui/ui_CMultiStatusProviderGuiComp.h>


namespace imtgui
{


class CMultiStatusProviderGuiComp: public iqtgui::TDesignerGuiObserverCompBase<Ui::MultiStatusProviderGui, imtbase::IMultiStatusProvider>
{
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::MultiStatusProviderGui, imtbase::IMultiStatusProvider> BaseClass;

	I_BEGIN_BASE_COMPONENT(CMultiStatusProviderGuiComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	void UpdateCommonStatusGui(imtbase::IMultiStatusProvider& object);
	void UpdateSubStatusesGui(imtbase::IMultiStatusProvider& object);

	static QIcon GetStatusIcon(istd::IInformationProvider::InformationCategory status);
};


} // namespace imtgui
