#include <imtqml/CApplicationInfoDataProviderComp.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CApplicationInfoDataProviderComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_applicationInfoCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
		rootModelPtr->SetData("Name", *m_paramNameAttrPtr);
		rootModelPtr->SetData("Source", "SettingsTextLabel.qml");

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


