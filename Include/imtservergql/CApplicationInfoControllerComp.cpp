#include <imtservergql/CApplicationInfoControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtapp::Application::V1_0::CGraphQlHandlerCompBase)

sdl::imtapp::Application::V1_0::CApplicationInfo CApplicationInfoControllerComp::OnGetApplicationInfo(
			const sdl::imtapp::Application::V1_0::CGetApplicationInfoGqlRequest& /*getApplicationInfoRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		errorMessage = QString("Unable to get an application info. Error: Component attribute 'm_applicationInfoCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CApplicationInfoControllerComp");

		return sdl::imtapp::Application::V1_0::CApplicationInfo();
	}

	sdl::imtapp::Application::V1_0::CApplicationInfo applicationInfo;

	const iser::IVersionInfo& versionInfo = m_applicationInfoCompPtr->GetVersionInfo();
	int mainVersion = m_applicationInfoCompPtr->GetMainVersionId();

	quint32 versionNumber;
	if (versionInfo.GetVersionNumber(mainVersion, versionNumber)){
		QString version = versionInfo.GetEncodedVersionName(mainVersion, versionNumber);
		applicationInfo.SetVersion(version);
	}

	QString appId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
	applicationInfo.SetApplicationId(appId);

	QString appName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_NAME);
	applicationInfo.SetApplicationName(appName);

	QString productName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_PRODUCT_NAME);
	applicationInfo.SetProductName(productName);

	QString companyName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_COMPANY_NAME);
	applicationInfo.SetCompanyName(companyName);

	QString type = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_TYPE);
	applicationInfo.SetApplicationType(type);

	return applicationInfo;
}


} // namespace imtservergql


