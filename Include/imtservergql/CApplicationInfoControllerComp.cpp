// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CApplicationInfoControllerComp.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>


namespace imtservergql
{


// Bounds the dependency tree, e.g. when two services depend on each other
static const int s_maxDependencyDepth = 8;


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

	// version kind (e.g. "Beta Version") is selected via build pipeline, see ibase::IApplicationInfo::VersionKind
	QString versionKindText = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_VERSION_KIND);
	if (!versionKindText.isEmpty()){
		applicationInfo.versionKind = versionKindText;
	}

	if (!m_buildTime.isEmpty()){
		applicationInfo.buildTime = m_buildTime;
	}

	using AppAttribute = ibase::IApplicationInfo::ApplicationAttribute;
	using AppInfoMember = istd::TNullableValue<QString> ApplicationInfo::*;

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

	FillWebSocketUrl(applicationInfo);
	FillUserMode(applicationInfo);
	FillSuperuserStatus(applicationInfo);
	FillServiceDependencies(applicationInfo);

	applicationInfo.tenantsEnabled = m_tenantsEnabledAttrPtr.IsValid() ? *m_tenantsEnabledAttrPtr : false;
	applicationInfo.externalLoginEnabled = m_externalLoginEnabledAttrPtr.IsValid() ? *m_externalLoginEnabledAttrPtr : false;
	applicationInfo.documentSharingEnabled = m_documentSharingEnabledAttrPtr.IsValid() ? *m_documentSharingEnabledAttrPtr : false;

	return applicationInfo;
}


// reimplemented (icomp::CComponentBase)

void CApplicationInfoControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// Read once: a replaced executable is not the one that is running
	QFileInfo executableInfo(QCoreApplication::applicationFilePath());
	if (executableInfo.exists()){
		m_buildTime = executableInfo.lastModified().toUTC().toString(Qt::ISODate);
	}
}


// private methods

void CApplicationInfoControllerComp::FillWebSocketUrl(ApplicationInfo& applicationInfo) const
{
	if (!m_webSocketUrlProviderCompPtr.IsValid()){
		return;
	}

	QUrl url;
	if (!m_webSocketUrlProviderCompPtr->GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url)){
		return;
	}

	sdl::V1_0::imtbase::CUrlParam webSocketUrl;
	webSocketUrl.host = url.host();
	webSocketUrl.port = url.port();
	webSocketUrl.scheme = url.scheme();

	applicationInfo.webSocketUrl = std::move(webSocketUrl);
}


void CApplicationInfoControllerComp::FillUserMode(ApplicationInfo& applicationInfo) const
{
	if (!m_userModeSelectionParamCompPtr.IsValid()){
		return;
	}

	// Index order matches Sdl/imtapp/1.0/Application.sdl's UserManagementMode enum and
	// the UserModeOptions SelectionConstraints in Partitura/ImtAuthVoce.arp/AuthorizationOptions.acc.
	enum UserModeIndex
	{
		UMI_NO_USER_MANAGEMENT,
		UMI_OPTIONAL_USER_MANAGEMENT,
		UMI_STRONG_USER_MANAGEMENT
	};

	int index = m_userModeSelectionParamCompPtr->GetSelectedOptionIndex();

	sdl::V1_0::imtapp::UserManagementMode userMode = sdl::V1_0::imtapp::UserManagementMode::STRONG_USER_MANAGEMENT;
	switch (index){
	case UMI_NO_USER_MANAGEMENT:
		userMode = sdl::V1_0::imtapp::UserManagementMode::NO_USER_MANAGEMENT;
		break;

	case UMI_OPTIONAL_USER_MANAGEMENT:
		userMode = sdl::V1_0::imtapp::UserManagementMode::OPTIONAL_USER_MANAGEMENT;
		break;

	case UMI_STRONG_USER_MANAGEMENT:
		userMode = sdl::V1_0::imtapp::UserManagementMode::STRONG_USER_MANAGEMENT;
		break;

	default:
		break;
	}

	applicationInfo.userMode = userMode;
}


void CApplicationInfoControllerComp::FillSuperuserStatus(ApplicationInfo& applicationInfo) const
{
	if (!m_superuserProviderCompPtr.IsValid()){
		return;
	}

	sdl::V1_0::imtapp::CSuperuserCheckResult superuser;

	if (m_databaseConnectionCheckerCompPtr.IsValid()){
		QString connectionMessage;
		if (!m_databaseConnectionCheckerCompPtr->CheckDatabaseConnection(connectionMessage)){
			superuser.status = sdl::V1_0::imtapp::SuperuserExistsStatus::UNKNOWN;
			superuser.message = connectionMessage;

			applicationInfo.superuser = std::move(superuser);

			return;
		}
	}

	QString errorMessage;
	imtauth::ISuperuserProvider::ExistsStatus status = m_superuserProviderCompPtr->SuperuserExists(errorMessage);

	sdl::V1_0::imtapp::SuperuserExistsStatus sdlStatus = sdl::V1_0::imtapp::SuperuserExistsStatus::UNKNOWN;
	switch (status){
	case imtauth::ISuperuserProvider::ES_EXISTS:
		sdlStatus = sdl::V1_0::imtapp::SuperuserExistsStatus::EXISTS;
		break;

	case imtauth::ISuperuserProvider::ES_NOT_EXISTS:
		sdlStatus = sdl::V1_0::imtapp::SuperuserExistsStatus::NOT_EXISTS;
		break;

	default:
		break;
	}

	superuser.status = sdlStatus;
	if (!errorMessage.isEmpty()){
		superuser.message = errorMessage;
	}

	applicationInfo.superuser = std::move(superuser);
}


void CApplicationInfoControllerComp::FillServiceDependencies(ApplicationInfo& applicationInfo) const
{
	if (!m_serviceDependenciesCompPtr.IsValid()){
		return;
	}

	QList<imtcom::IServiceInfoProvider::ServiceInfo> dependencies;
	const int count = m_serviceDependenciesCompPtr.GetCount();
	for (int i = 0; i < count; ++i){
		const imtcom::IServiceInfoProvider* serviceInfoProviderPtr = m_serviceDependenciesCompPtr[i];
		if (serviceInfoProviderPtr != nullptr){
			dependencies << serviceInfoProviderPtr->GetServiceInfo();
		}
	}

	if (dependencies.isEmpty()){
		return;
	}

	applicationInfo.dependencies = CreateDependencyList(dependencies, 1);
}


imtsdl::TElementList<sdl::V1_0::imtapp::CServiceInfo> CApplicationInfoControllerComp::CreateDependencyList(
			const QList<imtcom::IServiceInfoProvider::ServiceInfo>& dependencies,
			int depth)
{
	imtsdl::TElementList<sdl::V1_0::imtapp::CServiceInfo> dependencyList;
	if (depth > s_maxDependencyDepth){
		return dependencyList;
	}

	for (const imtcom::IServiceInfoProvider::ServiceInfo& dependency : dependencies){
		sdl::V1_0::imtapp::CServiceInfo dependencyData;
		dependencyData.serviceId = dependency.serviceId;

		switch (dependency.status){
		case imtcom::IConnectionStatusProvider::CS_CONNECTED:
			dependencyData.status = sdl::V1_0::imtapp::ServiceStatus::CONNECTED;
			break;
		case imtcom::IConnectionStatusProvider::CS_DISCONNECTED:
			dependencyData.status = sdl::V1_0::imtapp::ServiceStatus::DISCONNECTED;
			break;
		default:
			dependencyData.status = sdl::V1_0::imtapp::ServiceStatus::UNKNOWN;
			break;
		}

		if (!dependency.applicationName.isEmpty()){
			dependencyData.applicationName = dependency.applicationName;
		}
		if (!dependency.version.isEmpty()){
			dependencyData.version = dependency.version;
		}
		if (!dependency.versionKind.isEmpty()){
			dependencyData.versionKind = dependency.versionKind;
		}
		if (dependency.buildTime.isValid()){
			dependencyData.buildTime = dependency.buildTime.toUTC().toString(Qt::ISODate);
		}
		if (!dependency.dependencies.isEmpty()){
			dependencyData.dependencies = CreateDependencyList(dependency.dependencies, depth + 1);
		}

		dependencyList << dependencyData;
	}

	return dependencyList;
}


} // namespace imtservergql


