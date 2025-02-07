#pragma once


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtservergql/CGqlRepresentationDataControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


class CAuthorizationOptionsControllerComp: public sdl::imtauth::Authorization::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Authorization::CGraphQlHandlerCompBase BaseClass;

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

	// reimplemented (sdl::imtauth::Authorization::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Authorization::CAuthorizationPayload OnAuthorization(
				const sdl::imtauth::Authorization::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CAuthorizationPayload OnUserToken(
				const sdl::imtauth::Authorization::CUserTokenGqlRequest& userTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CUserManagementPayload OnGetUserMode(
				const sdl::imtauth::Authorization::CGetUserModeGqlRequest& getUserModeRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_REF(iprm::ISelectionParam, m_selectionParamCompPtr);
};


} // namespace imtauthgql


