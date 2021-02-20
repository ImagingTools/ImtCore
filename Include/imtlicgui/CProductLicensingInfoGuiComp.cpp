#include <imtlicgui/CProductLicensingInfoGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtlicgui
{


// public methods

CProductLicensingInfoGuiComp::CProductLicensingInfoGuiComp()
{
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CProductLicensingInfoGuiComp::GetCommands() const
{
	ibase::ICommandsProvider* commandsProviderPtr = dynamic_cast<ibase::ICommandsProvider*>(m_licenseCollectionGuiCompPtr.GetPtr());
	if (commandsProviderPtr != nullptr){
		commandsProviderPtr->GetCommands();
	}
	
	return nullptr;
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CProductLicensingInfoGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	//imtlic::IProductLicensingInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


void CProductLicensingInfoGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_licenseCollectionGuiCompPtr.IsValid() && m_licenseCollectionObserverCompPtr.IsValid()){
		imod::IModel* licensingInfoModelPtr = GetObservedModel();
		Q_ASSERT(licensingInfoModelPtr != nullptr);

		if (licensingInfoModelPtr->AttachObserver(m_licenseCollectionObserverCompPtr.GetPtr())){
			m_licenseCollectionGuiCompPtr->CreateGui(LicenseCollectionFrame);
		}
	}
}


void CProductLicensingInfoGuiComp::OnGuiModelDetached()
{
	if (m_licenseCollectionGuiCompPtr.IsValid() && m_licenseCollectionObserverCompPtr.IsValid()){
		imod::IModel* licensingInfoModelPtr = GetObservedModel();
		Q_ASSERT(licensingInfoModelPtr != nullptr);

		if (licensingInfoModelPtr->IsAttached(m_licenseCollectionObserverCompPtr.GetPtr())){
			licensingInfoModelPtr->DetachObserver(m_licenseCollectionObserverCompPtr.GetPtr());

			m_licenseCollectionGuiCompPtr->DestroyGui();
		}
	}

	BaseClass::OnGuiModelDetached();
}


void CProductLicensingInfoGuiComp::UpdateModel() const
{
	//imtlic::IProductLicensingInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


} // namespace imtlicgui


