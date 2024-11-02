#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/CUserInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{


class CAuthorizationControllerComp: public sdl::imtauth::Authorization::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Authorization::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationControllerComp);
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection", true, "SessionCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN_MULTI_0(m_credentialControllersCompPtr, "CredentialControllers", "Credential Controllers", true);
		I_ASSIGN_MULTI_0(m_systemIdsAttrPtr, "SystemIds", "System-IDs", true);
	I_END_COMPONENT;

protected:
	bool ParseDataFromGqlRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& login, QByteArray& password, QByteArray& productId) const;
	QByteArray GetUserObjectId(const QByteArray& login) const;
	bool CheckCredential(const QByteArray& systemId, const QByteArray& login, const QByteArray& password) const;
	sdl::imtauth::Authorization::V1_0::CAuthorizationPayload CreateInvalidLoginOrPasswordResponse(const QByteArray& login, QString& errorMessage) const;
	sdl::imtauth::Authorization::V1_0::CAuthorizationPayload CreateAuthorizationSuccessfulResponse(
				imtauth::CUserInfo& userInfo,
				const QByteArray& systemId,
				const QByteArray& productId,
				QString& errorMessage) const;

	// reimplemented (sdl::imtauth::Authorization::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Authorization::V1_0::CAuthorizationPayload OnAuthorization(
				const sdl::imtauth::Authorization::V1_0::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
    virtual sdl::imtauth::Authorization::V1_0::CAuthorizationPayload OnUserToken(
                const sdl::imtauth::Authorization::V1_0::CUserTokenGqlRequest& userTokenRequest,
                const ::imtgql::CGqlRequest& gqlRequest,
                QString& errorMessage) const override;
	virtual sdl::imtauth::Authorization::V1_0::CUserManagementPayload OnGetUserMode(
				const sdl::imtauth::Authorization::V1_0::CGetUserModeGqlRequest& getUserModeRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
protected:
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_MULTIREF(imtauth::ICredentialController, m_credentialControllersCompPtr);
	I_MULTIATTR(QByteArray, m_systemIdsAttrPtr);
};


} // namespace imtauthgql


