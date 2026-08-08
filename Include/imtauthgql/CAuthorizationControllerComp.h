// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/IJwtSessionController.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/IRoleInfoProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization_fwd.h>


namespace imtauthgql
{


class CAuthorizationControllerComp: public sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_userConnectionCollectionCompPtr, "UserConnectionCollection", "User connection collection", false, "UserConnectionCollection");
		I_ASSIGN_MULTI_0(m_credentialControllersCompPtr, "CredentialControllers", "Credential Controllers", true);
		I_ASSIGN_MULTI_0(m_systemIdsAttrPtr, "SystemIds", "System-IDs", true);
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "JWT session controller", false, "JwtSessionController");
		I_ASSIGN(m_personalAccessTokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", false, "PersonalAccessTokenManager");
		I_ASSIGN(m_patPrefixAttrPtr, "PatPrefix", "Personal access token prefix", false, "imt_pat_");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager", false, "TenantManager");
		I_ASSIGN(m_tenantMembershipManagerCompPtr, "TenantMembershipManager", "Tenant membership manager", false, "TenantMembershipManager");
		I_ASSIGN(m_bindingManagerCompPtr, "BindingManager", "Tenant entity binding manager for tenant-scoped adaptation of user roles/groups/permissions", false, "TenantEntityBindingManager");
		I_ASSIGN(m_delegatedAccessCompPtr, "DelegatedAccess", "Delegated access resolver for cross-org grants", false, "DelegatedAccessResolver");
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider (used for delegated role product validation)", false, "RoleInfoProvider");
	I_END_COMPONENT;

protected:
	bool ParseDataFromGqlRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& login, QByteArray& password, QByteArray& productId) const;
	QByteArray GetUserObjectId(const QByteArray& login) const;
	bool CheckCredential(const QByteArray& systemId, const QByteArray& login, const QByteArray& password) const;
	QByteArrayList CalculateGlobalPermissions(const imtauth::IUserInfo& userInfo, const QByteArray& userId, const QByteArray& productId) const;
	sdl::V1_0::imtauth::CAuthorizationPayload CreateInvalidLoginOrPasswordResponse(const QByteArray& login, QString& errorMessage) const;
	sdl::V1_0::imtauth::CAuthorizationPayload CreateAuthorizationSuccessfulResponse(
				imtauth::CUserInfo& userInfo,
				const QByteArray& systemId,
				const QByteArray& productId,
				QString& errorMessage) const;

	// reimplemented (sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnAuthorization(
				const sdl::V1_0::imtauth::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAuthorizationPayload OnUserToken(
				const sdl::V1_0::imtauth::CUserTokenGqlRequest& userTokenRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CLogoutPayload OnLogout(
				const sdl::V1_0::imtauth::CLogoutGqlRequest& logoutRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CPermissionList OnGetPermissions(
				const sdl::V1_0::imtauth::CGetPermissionsGqlRequest& getPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userConnectionCollectionCompPtr);
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_personalAccessTokenManagerCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_tenantMembershipManagerCompPtr);
	I_REF(imtauth::ITenantEntityBindingManager, m_bindingManagerCompPtr);
	I_REF(imtauth::IDelegatedAccess, m_delegatedAccessCompPtr);
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_MULTIREF(imtauth::ICredentialController, m_credentialControllersCompPtr);
	I_MULTIATTR(QByteArray, m_systemIdsAttrPtr);
	I_ATTR(QByteArray, m_patPrefixAttrPtr);
};


} // namespace imtauthgql

