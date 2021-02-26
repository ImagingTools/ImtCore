#include <imtlicgui/CFeatureInfoProviderGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

#include <imtbase/CCollectionInfo.h>
#include <imtlic/CFeatureInfoProvider.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>

namespace imtlicgui
{


// public methods

CFeatureInfoProviderGuiComp::CFeatureInfoProviderGuiComp()
{
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CFeatureInfoProviderGuiComp::GetCommands() const
{
	//ibase::ICommandsProvider* commandsProviderPtr = dynamic_cast<ibase::ICommandsProvider*>(m_licenseCollectionGuiCompPtr.GetPtr());
	//if (commandsProviderPtr != nullptr){
	//	commandsProviderPtr->GetCommands();
	//}
	
	return nullptr;
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFeatureInfoProviderGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	//imtlic::IProductLicensingInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


void CFeatureInfoProviderGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	//if (m_licenseCollectionGuiCompPtr.IsValid() && m_licenseCollectionObserverCompPtr.IsValid()){
	//	imod::IModel* licensingInfoModelPtr = GetObservedModel();
	//	Q_ASSERT(licensingInfoModelPtr != nullptr);

	//	if (licensingInfoModelPtr->AttachObserver(m_licenseCollectionObserverCompPtr.GetPtr())){
	//		m_licenseCollectionGuiCompPtr->CreateGui(LicenseCollectionFrame);
	//	}
	//}
}


void CFeatureInfoProviderGuiComp::OnGuiModelDetached()
{
	//if (m_licenseCollectionGuiCompPtr.IsValid() && m_licenseCollectionObserverCompPtr.IsValid()){
	//	imod::IModel* licensingInfoModelPtr = GetObservedModel();
	//	Q_ASSERT(licensingInfoModelPtr != nullptr);

	//	if (licensingInfoModelPtr->IsAttached(m_licenseCollectionObserverCompPtr.GetPtr())){
	//		licensingInfoModelPtr->DetachObserver(m_licenseCollectionObserverCompPtr.GetPtr());

	//		m_licenseCollectionGuiCompPtr->DestroyGui();
	//	}
	//}

	BaseClass::OnGuiModelDetached();
}


void CFeatureInfoProviderGuiComp::UpdateModel() const
{
	//imtlic::IProductLicensingInfo* licenseInfoPtr = GetObservedObject();
	//Q_ASSERT(licenseInfoPtr != nullptr);
}


} // namespace imtlicgui


