// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		applicationInfo.version = std::move(version);
	}

	QString appId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
	applicationInfo.applicationId = std::move(appId);

	QString appName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_NAME);
	applicationInfo.applicationName = std::move(appName);

	QString productName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_PRODUCT_NAME);
	applicationInfo.productName = std::move(productName);

	QString companyName = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_COMPANY_NAME);
	applicationInfo.companyName = std::move(companyName);

	QString type = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_TYPE);
	applicationInfo.applicationType = std::move(type);

	sdl::imtapp::Application::CApplicationInfo retVal;
	retVal.Version_1_0 = std::move(applicationInfo);

	return retVal;
}


} // namespace imtservergql


