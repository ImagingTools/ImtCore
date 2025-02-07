#include <imtservergql/CApplicationInfoControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtapp::Application::CGraphQlHandlerCompBase)

sdl::imtapp::Application::CApplicationInfo CApplicationInfoControllerComp::OnGetApplicationInfo(
			const sdl::imtapp::Application::CGetApplicationInfoGqlRequest& /*getApplicationInfoRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		errorMessage = QString("Unable to get an application info. Error: Component attribute 'm_applicationInfoCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CApplicationInfoControllerComp");

		return sdl::imtapp::Application::CApplicationInfo();
	}

	sdl::imtapp::Application::CApplicationInfo::V1_0 applicationInfo;

	const iser::IVersionInfo& versionInfo = m_applicationInfoCompPtr->GetVersionInfo();
	int mainVersion = m_applicationInfoCompPtr->GetMainVersionId();

	quint32 versionNumber;
	if (versionInfo.GetVersionNumber(mainVersion, versionNumber)){
		QString version = versionInfo.GetEncodedVersionName(mainVersion, versionNumber);
		applicationInfo.Version = std::make_optional<QString>(version);
	}

	QString appId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
	applicationInfo.ApplicationId = std::make_optional<QString>(appId);

	QString appName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_NAME);
	applicationInfo.ApplicationName = std::make_optional<QString>(appName);

	QString productName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_PRODUCT_NAME);
	applicationInfo.ProductName = std::make_optional<QString>(productName);

	QString companyName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_COMPANY_NAME);
	applicationInfo.CompanyName = std::make_optional<QString>(companyName);

	QString type = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_TYPE);
	applicationInfo.ApplicationType = std::make_optional<QString>(type);

	sdl::imtapp::Application::CApplicationInfo retVal;
	retVal.Version_1_0 = std::make_optional(applicationInfo);

	return retVal;
}


} // namespace imtservergql


