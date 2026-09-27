// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IExternalOidcAuthController.h>
#include <imtauth/IExternalOidcProvider.h>
#include <imtauth/IUserManager.h>
#include <imtauth/IJwtSessionController.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Component implementing external OIDC authentication flows.

	Orchestrates login via external providers (Google, Apple, Facebook):
	1. GetAuthorizationUrl — builds redirect URL to the external provider
	2. HandleCallback — exchanges authorization code for tokens,
	   validates the ID token, looks up or creates a local user,
	   and creates an internal session
	3. UnlinkProvider — removes an external identity link
	4. GetLinkedProviders — lists linked providers for a user

	Dependencies:
	- Multiple IExternalOidcProvider instances (one per configured provider)
	- IUserManager for user lookup/creation
	- IJwtSessionController for internal session creation
	- IObjectCollection for ExternalIdentities persistence

	\ingroup Authorization
*/
class CExternalOidcAuthControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IExternalOidcAuthController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CExternalOidcAuthControllerComp);
		I_REGISTER_INTERFACE(imtauth::IExternalOidcAuthController);
		I_ASSIGN_MULTI_0(m_providersCompPtr, "Providers", "External OIDC providers (Google, Apple, Facebook)", true);
		I_ASSIGN(m_userManagerCompPtr, "UserManager", "User manager for user lookup and creation", true, "UserManager");
		I_ASSIGN(m_sessionControllerCompPtr, "SessionController", "JWT session controller for creating internal sessions", true, "JwtSessionController");
		I_ASSIGN(m_identityCollectionCompPtr, "IdentityCollection", "Collection of external identities", true, "ExternalIdentityCollection");
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalOidcAuthController)
	virtual QByteArrayList GetAvailableProviders() const override;
	virtual QString GetAuthorizationUrl(const QByteArray& providerId, const QByteArray& state, const QByteArray& nonce) const override;
	virtual AuthResult HandleCallback(const QByteArray& providerId, const QByteArray& authCode, const QByteArray& state) const override;
	virtual bool UnlinkProvider(const QByteArray& userId, const QByteArray& providerId) const override;
	virtual QList<LinkedProvider> GetLinkedProviders(const QByteArray& userId) const override;

private:
	const IExternalOidcProvider* FindProvider(const QByteArray& providerId) const;

private:
	I_MULTIREF(imtauth::IExternalOidcProvider, m_providersCompPtr);
	I_REF(imtauth::IUserManager, m_userManagerCompPtr);
	I_REF(imtauth::IJwtSessionController, m_sessionControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_identityCollectionCompPtr);
};


} // namespace imtauth
