#include <imtlicgui/CLicenseInfoEditorGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureDependenciesProvider.h>

namespace imtlicgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CLicenseInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	IdEdit->setText(licenseInfoPtr->GetLicenseId());
	NameEdit->setText(licenseInfoPtr->GetLicenseName());
}


void CLicenseInfoEditorGuiComp::UpdateModel() const
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(licenseInfoPtr);

	licenseInfoPtr->SetLicenseId(IdEdit->text().toUtf8());
	licenseInfoPtr->SetLicenseName(NameEdit->text());
}


// private slots

void CLicenseInfoEditorGuiComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}




} // namespace imtlicgui


