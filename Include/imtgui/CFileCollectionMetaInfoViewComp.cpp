#include <imtgui/CFileCollectionMetaInfoViewComp.h>


namespace imtgui
{


// protected methods

CFileCollectionMetaInfoViewComp::CFileCollectionMetaInfoViewComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFileCollectionMetaInfoViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFileCollectionMetaInfoViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CFileCollectionMetaInfoViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAnyChange());
}


} // namespace imtgui


