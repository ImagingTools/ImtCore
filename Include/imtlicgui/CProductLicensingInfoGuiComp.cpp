#include <imtlicgui/CProductLicensingInfoGuiComp.h>


// Acf includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>


namespace imtlicgui
{


// public methods

CProductLicensingInfoGuiComp::CProductLicensingInfoGuiComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CProductLicensingInfoGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	//imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


void CProductLicensingInfoGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CProductLicensingInfoGuiComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


void CProductLicensingInfoGuiComp::UpdateModel() const
{
	//imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


// private slots

} // namespace imtlicgui


