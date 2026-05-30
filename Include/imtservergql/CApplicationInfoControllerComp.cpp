// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CApplicationInfoControllerComp.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::V1_0::imtapp::CApplicationGqlHandlerCompBase)

CApplicationInfoControllerComp::ApplicationInfo CApplicationInfoControllerComp::OnGetApplicationInfo(
			const sdl::V1_0::imtapp::CGetApplicationInfoGqlRequest& /*getApplicationInfoRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to get an application info. Error: Component reference 'ApplicationInfo' was not set");
		SendErrorMessage(0, errorMessage, __func__);

		return {};
	}

	ApplicationInfo applicationInfo;

	const iser::IVersionInfo& versionInfo = m_applicationInfoCompPtr->GetVersionInfo();
	int mainVersion = m_applicationInfoCompPtr->GetMainVersionId();

	quint32 versionNumber;
	if (versionInfo.GetVersionNumber(mainVersion, versionNumber)){
		QString version = versionInfo.GetEncodedVersionName(mainVersion, versionNumber);
		applicationInfo.version = std::move(version);
	}

	using AppAttribute = ibase::IApplicationInfo::ApplicationAttribute;
	using AppInfoMember = istd::TSharedNullable<QString> ApplicationInfo::*;

	const QHash<AppAttribute, QPair<QByteArray, AppInfoMember>> attributeHash = {
		{ AppAttribute::AA_APPLICATION_ID,	 { QByteArrayLiteral("ApplicationInfo/ApplicationId"),	 &ApplicationInfo::applicationId	 } },
		{ AppAttribute::AA_APPLICATION_NAME, { QByteArrayLiteral("ApplicationInfo/ApplicationName"), &ApplicationInfo::applicationName } },
		{ AppAttribute::AA_APPLICATION_TYPE, { QByteArrayLiteral("ApplicationInfo/ApplicationType"), &ApplicationInfo::applicationType } },
		{ AppAttribute::AA_COMPANY_NAME,	 { QByteArrayLiteral("ApplicationInfo/CompanyName"),	 &ApplicationInfo::companyName	 } },
		{ AppAttribute::AA_PRODUCT_NAME,	 { QByteArrayLiteral("ApplicationInfo/ProductName"),	 &ApplicationInfo::productName	 } },
	};

	for (auto it = attributeHash.constBegin(); it != attributeHash.constEnd(); ++it) {
		const QByteArray& paramName	= it.value().first;
		AppInfoMember member		= it.value().second;

		/**
			Set value from settings XML file if settings component is assigned and parameter is set in the file.
			This allows to overwrite default application info attributes with values from settings file.
		*/
		if(m_applicationPreferencesCompPtr.IsValid()){
			iprm::TParamsPtr<iprm::ITextParam> paramPtr(m_applicationPreferencesCompPtr.GetPtr(), paramName, false);
			if (paramPtr.IsValid()){
				applicationInfo.*member = paramPtr->GetText();

				continue;
			}
		}

		applicationInfo.*member = m_applicationInfoCompPtr->GetApplicationAttribute(it.key());
	}

	/**
		Custom parameters may be added to application info as well.
		For example, logo icon name parameter is used to display a different logo per client installation.
	*/
	iprm::TParamsPtr<iprm::ITextParam> logoIconNameParamPtr(m_applicationPreferencesCompPtr.GetPtr(), QByteArrayLiteral("Style/LogoIconName"), false);
	if(logoIconNameParamPtr.IsValid()){
		applicationInfo.logoIconName = logoIconNameParamPtr->GetText();
	}

	return applicationInfo;
}


} // namespace imtservergql


