// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IOidcScopeMapper.h>
#include <imtauth/IUserInfoProvider.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/IPermissionsProvider.h>
#include <imtbase/IObjectCollection.h>


namespace imtoidc
{


/**
	OIDC scope-to-claims mapper component.

	Maps standard OIDC scopes to user claims:
	- openid → sub
	- profile → name, family_name, given_name
	- email → email, email_verified
	- roles → role IDs from IUserRoleInfo
	- permissions → feature IDs from IPermissionsProvider/IRole

	\ingroup OIDC
*/
class COidcScopeMapperComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IOidcScopeMapper
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(COidcScopeMapperComp);
		I_REGISTER_INTERFACE(imtauth::IOidcScopeMapper);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection for user data lookup", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::IOidcScopeMapper)
	virtual QJsonObject MapScopesToClaims(const QByteArray& userId, const QByteArrayList& scopes) const override;
	virtual QByteArrayList GetSupportedScopes() const override;
	virtual QByteArrayList GetSupportedClaims() const override;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtoidc


