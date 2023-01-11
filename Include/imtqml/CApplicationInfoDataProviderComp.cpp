#include <imtqml/CApplicationInfoDataProviderComp.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

bool CApplicationInfoDataProviderComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	return true;
}


imtbase::CTreeItemModel* CApplicationInfoDataProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_applicationInfoCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
		rootModelPtr->SetData("Name", *m_paramNameAttrPtr);
		rootModelPtr->SetData("Source", *m_sourceAttrPtr);

		const iser::IVersionInfo& versionInfo =  m_applicationInfoCompPtr->GetVersionInfo();

		quint32 versionNumber;
		versionInfo.GetVersionNumber(1983, versionNumber);
		QString version = versionInfo.GetEncodedVersionName(1983, versionNumber);

		rootModelPtr->SetData("Value", version);

		return rootModelPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtqml


