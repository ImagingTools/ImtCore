// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/IJwtSessionController.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


class CAuthorizationControllerComp: public sdl::imtauth::Authorization::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Authorization::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationControllerComp);
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection", true, "SessionCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_userConnectionCollectionCompPtr, "UserConnectionCollection", "User connection collection", false, "UserConnectionCollection");
		I_ASSIGN_MULTI_0(m_credentialControllersCompPtr, "CredentialControllers", "Credential Controllers", true);
		I_ASSIGN_MULTI_0(m_systemIdsAttrPtr, "SystemIds", "System-IDs", true);
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "JWT session controller", false, "JwtSessionController");
	I_END_COMPONENT;

protected:
	bool ParseDataFromGqlRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& login, QByteArray& password, QByteArray& productId) const;
	QByteArray GetUserObjectId(const QByteArray& login) const;
	bool CheckCredential(const QByteArray& systemId, const QByteArray& login, const QByteArray& password) const;
	sdl::imtauth::Authorization::CAuthorizationPayload CreateInvalidLoginOrPasswordResponse(const QByteArray& login, QString& errorMessage) const;
	sdl::imtauth::Authorization::CAuthorizationPayload CreateAuthorizationSuccessfulResponse(
				imtauth::CUserInfo& userInfo,
				const QByteArray& systemId,
				const QByteArray& productId,
				QString& errorMessage) const;

	// reimplemented (sdl::imtauth::Authorization::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Authorization::CAuthorizationPayload OnAuthorization(
				const sdl::imtauth::Authorization::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CAuthorizationPayload OnUserToken(
				const sdl::imtauth::Authorization::CUserTokenGqlRequest& userTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CLogoutPayload OnLogout(
				const sdl::imtauth::Authorization::CLogoutGqlRequest& logoutRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::CPermissionList OnGetPermissions(
				const sdl::imtauth::Authorization::CGetPermissionsGqlRequest& getPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userConnectionCollectionCompPtr);
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_MULTIREF(imtauth::ICredentialController, m_credentialControllersCompPtr);
	I_MULTIATTR(QByteArray, m_systemIdsAttrPtr);
};


} // namespace imtauthgql


