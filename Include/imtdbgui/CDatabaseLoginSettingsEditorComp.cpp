#include <imtdbgui/CDatabaseLoginSettingsEditorComp.h>


namespace imtdbgui
{


// public methods

CDatabaseLoginSettingsEditorComp::CDatabaseLoginSettingsEditorComp()
{
}


// protected methods

// reimplemented (imod::IModelEditor)

void CDatabaseLoginSettingsEditorComp::UpdateModel() const
{
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CDatabaseLoginSettingsEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtdb::IDatabaseLoginSettings* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);
}


void CDatabaseLoginSettingsEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CDatabaseLoginSettingsEditorComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CDatabaseLoginSettingsEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CDatabaseLoginSettingsEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


} // namespace imtdbgui


