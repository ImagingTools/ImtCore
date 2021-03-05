#include <imtlicgui/CProductInstanceInfoEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoEditorComp::CProductInstanceInfoEditorComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CProductInstanceInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);
}


void CProductInstanceInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);
}


void CProductInstanceInfoEditorComp::OnGuiModelDetached()
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	BaseClass::OnGuiModelDetached();
}


void CProductInstanceInfoEditorComp::UpdateModel() const
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(productInstanceInfoPtr);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductInstanceInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CProductInstanceInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CProductInstanceInfoEditorComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}


} // namespace imtlicgui


