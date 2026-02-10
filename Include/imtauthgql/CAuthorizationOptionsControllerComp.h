// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtservergql/CGqlRepresentationDataControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/AuthorizationOptions.h>


namespace imtauthgql
{


class CAuthorizationOptionsControllerComp: public sdl::imtauth::AuthorizationOptions::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::AuthorizationOptions::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationOptionsControllerComp);
		I_ASSIGN(m_selectionParamCompPtr, "SelectionParam", "Selection param", true, "SelectionParam");
	I_END_COMPONENT;

protected:
	enum UserMode
	{
		UM_NO_USER_MANAGEMENT,
		UM_OPTIONAL_USER_MANAGEMENT,
		UM_STRONG_USER_MANAGEMENT
	};

	// reimplemented (sdl::imtauth::AuthorizationOptions::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::AuthorizationOptions::CUserManagementPayload OnGetUserMode(
				const sdl::imtauth::AuthorizationOptions::CGetUserModeGqlRequest& getUserModeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(iprm::ISelectionParam, m_selectionParamCompPtr);
};


} // namespace imtauthgql


