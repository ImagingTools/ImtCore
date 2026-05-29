// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtservergql/CGqlRepresentationDataControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/AuthorizationOptions>


namespace imtauthgql
{


class CAuthorizationOptionsControllerComp: public sdl::V1_0::imtauth::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CGraphQlHandlerCompBase BaseClass;

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

	// reimplemented (sdl::V1_0::imtauth::CGraphQlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CUserManagementPayload OnGetUserMode(
				const sdl::V1_0::imtauth::CGetUserModeGqlRequest& getUserModeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(iprm::ISelectionParam, m_selectionParamCompPtr);
};


} // namespace imtauthgql


