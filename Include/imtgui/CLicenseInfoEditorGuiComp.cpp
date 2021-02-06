#include <imtgui/CLicenseInfoEditorGuiComp.h>


namespace imtgui
{


// public methods

CLicenseInfoEditorGuiComp::CLicenseInfoEditorGuiComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CLicenseInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtbase::ILicenseInfo* objectProviderPtr = GetObservedObject();
	Q_ASSERT(objectProviderPtr != nullptr);
}


void CLicenseInfoEditorGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CLicenseInfoEditorGuiComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


} // namespace imtgui


