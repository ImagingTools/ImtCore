// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtauth/ISuperuserProvider.h>
#include <imtcom/IServerConnectionInterface.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application_fwd.h>


namespace imtservergql
{


/**
	Serves the GetApplicationInfo query. Besides the basic application info, this
	controller also folds in the startup bootstrap data that used to require three
	extra round-trips (GetWebSocketUrl, GetUserMode, CheckSuperuserExists), so a
	client can obtain everything it needs at launch with a single request.

	The WebSocketUrlProvider / UserModeSelectionParam / SuperuserProvider refs are
	optional: when a reference is not wired, the corresponding response field is
	simply left unset (existing behaviour for apps that have not been migrated yet).
*/
class CApplicationInfoControllerComp: public sdl::V1_0::imtapp::CApplicationGqlHandlerCompBase
{
public:
	using BaseClass = sdl::V1_0::imtapp::CApplicationGqlHandlerCompBase;
	using ApplicationInfo = sdl::V1_0::imtapp::CApplicationInfo;

	I_BEGIN_COMPONENT(CApplicationInfoControllerComp);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application data info", true, "ApplicationInfo");
		I_ASSIGN(m_applicationPreferencesCompPtr, "ApplicationPreferences", "Optional reference to ParamSet written to XML file containing application preferences. May overwrite default ApplicationInfo attrubutes if configured", false, "IParamsSet");
		I_ASSIGN(m_webSocketUrlProviderCompPtr, "WebSocketUrlProvider", "Optional reference used to fold the WebSocket connection URL into the response (replaces a separate GetWebSocketUrl call)", false, "ServerConnectionInterface");
		I_ASSIGN(m_userModeSelectionParamCompPtr, "UserModeSelectionParam", "Optional reference used to fold the user-management mode into the response (replaces a separate GetUserMode call)", false, "SelectionParam");
		I_ASSIGN(m_superuserProviderCompPtr, "SuperuserProvider", "Optional reference used to fold the superuser-exists status into the response (replaces a separate CheckSuperuserExists call)", false, "SuperuserProvider");
		I_ASSIGN(m_databaseConnectionCheckerCompPtr, "DatabaseConnectionChecker", "Optional database connectivity check performed before the superuser lookup", false, "DatabaseConnectionChecker");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtapp::CApplicationGqlHandlerCompBase)
	virtual ApplicationInfo OnGetApplicationInfo(
				const sdl::V1_0::imtapp::CGetApplicationInfoGqlRequest& getApplicationInfoRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	void FillWebSocketUrl(ApplicationInfo& applicationInfo) const;
	void FillUserMode(ApplicationInfo& applicationInfo) const;
	void FillSuperuserStatus(ApplicationInfo& applicationInfo) const;

protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_REF(iprm::IParamsSet, m_applicationPreferencesCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_webSocketUrlProviderCompPtr);
	I_REF(iprm::ISelectionParam, m_userModeSelectionParamCompPtr);
	I_REF(imtauth::ISuperuserProvider, m_superuserProviderCompPtr);
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_databaseConnectionCheckerCompPtr);
};


} // namespace imtservergql


