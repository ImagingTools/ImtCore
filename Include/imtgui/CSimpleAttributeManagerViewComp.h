#pragma once


// ACF includes
#include <iattr/IAttributesProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <GeneratedFiles/imtgui/ui_CSimpleAttributeManagerViewComp.h>


namespace imtgui
{


class CSimpleAttributeManagerViewComp
	: public iqtgui::TDesignerGuiObserverCompBase<Ui::CSimpleAttributeManagerViewComp, iattr::IAttributesProvider>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CSimpleAttributeManagerViewComp, iattr::IAttributesProvider> BaseClass;

	I_BEGIN_COMPONENT(CSimpleAttributeManagerViewComp)
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
};


} // namespace imtgui
