#include <imtgui/CMenuPanelComp.h>


// Qt includes


namespace imtgui
{


// public methods

CMenuPanelComp::CMenuPanelComp()
{

}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
}


void CMenuPanelComp::OnGuiModelAttached()
{
    BaseClass::OnGuiModelAttached();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CMenuPanelComp::OnGuiCreated()
{

}


void CMenuPanelComp::OnGuiDestroyed()
{
    BaseClass::OnGuiDestroyed();
}


void CMenuPanelComp::OnGuiRetranslate()
{
    BaseClass::OnGuiRetranslate();

    UpdateGui(istd::IChangeable::GetAnyChange());
}


// reimplemented (icomp::CComponentBase)

void CMenuPanelComp::OnComponentCreated()
{

}


void CMenuPanelComp::OnComponentDestroyed()
{

}

} // namespace imtgui


