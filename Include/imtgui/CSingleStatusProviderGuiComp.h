#pragma once


// Acf includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <istd/IInformationProvider.h>
#include <GeneratedFiles/imtgui/ui_CSingleStatusProviderGuiComp.h>


namespace imtgui
{


class CSingleStatusProviderGuiComp: public iqtgui::TDesignerGuiObserverCompBase<Ui::SingleStatusProviderGui, istd::IInformationProvider>
{
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::SingleStatusProviderGui, istd::IInformationProvider> BaseClass;

	I_BEGIN_COMPONENT(CSingleStatusProviderGuiComp);
		I_ASSIGN(m_showStatusAttrPtr, "ShowStatus", "Show status text", false, true);
		I_ASSIGN(m_showDescriptionAttrPtr, "ShowDescription", "Show description text", false, true);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	I_ATTR(bool, m_showStatusAttrPtr);
	I_ATTR(bool, m_showDescriptionAttrPtr);

	static QIcon GetStatusIcon(istd::IInformationProvider::InformationCategory status);
};


} // namespace imtgui
