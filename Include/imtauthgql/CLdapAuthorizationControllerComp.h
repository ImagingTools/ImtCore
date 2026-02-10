// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtauthgql/CAuthorizationControllerComp.h>


namespace imtauthgql
{


class CLdapAuthorizationControllerComp: public CAuthorizationControllerComp
{
public:
	typedef CAuthorizationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CLdapAuthorizationControllerComp);
		I_ASSIGN(m_systemIdAttrPtr, "SystemId", "System-ID", false, "Ldap");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", false, "RoleCollection");
		I_ASSIGN(m_enableableParamCompPtr, "LdapEnableableParam", "Ldap enableable param", false, "LdapEnableableParam");
	I_END_COMPONENT;

protected:
	enum RoleType
	{
		RT_DEFAULT,
		RT_GUEST
	};

	virtual QByteArray CheckExistsRole(const QByteArray& productId, RoleType roleType) const;
	virtual QByteArray InsertNewIdentifiableRoleInfo(
				const QByteArray& roleId,
				const QString& roleName,
				const QString& description,
				const QByteArray& productId,
				bool isDefault,
				bool isGuest) const;
	virtual istd::TUniqueInterfacePtr<imtauth::IUserInfo> CreateUserInfoFromLdapUser(const QByteArray& ldapUserId) const;

	// reimplemented (CAuthorizationControllerComp)
	virtual sdl::imtauth::Authorization::CAuthorizationPayload OnAuthorization(
				const sdl::imtauth::Authorization::CAuthorizationGqlRequest& authorizationRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(iprm::IEnableableParam, m_enableableParamCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_ATTR(QByteArray, m_systemIdAttrPtr);
};


} // namespace imtauthgql


