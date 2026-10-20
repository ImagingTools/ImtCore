// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/IExternalOidcProvider.h>
#include <imtauth/IExternalIdentityManager.h>
#include <imtauth/IExternalLoginController.h>


namespace imtauth
{


/**
	Sign-in with external identity providers as a confidential OAuth 2.0 / OpenID Connect client.

	The pending logins are kept in memory for LoginTimeout seconds. The provider redirects to
	CallbackUrl, the callback servlet passes the code to HandleCallback, which exchanges it with the
	provider and resolves the local user:
	- the user the external account is linked to;
	- otherwise, if LinkByVerifiedEmail is set, the only user with the verified e-mail of the account;
	- otherwise, if AllowRegistration is set, a new user without password.
*/
class CExternalLoginControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public IExternalLoginController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CExternalLoginControllerComp);
		I_REGISTER_INTERFACE(IExternalLoginController);
		I_REGISTER_INTERFACE(IExternalLoginCallbackHandler);
		I_ASSIGN_MULTI_0(m_providersCompPtr, "Providers", "External identity providers, the ones without client ID are not offered", true);
		I_ASSIGN(m_identityManagerCompPtr, "IdentityManager", "Links between external accounts and users", true, "ExternalIdentityManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_userFactoryCompPtr, "UserFactory", "Factory of the users created with AllowRegistration", false, "UserFactory");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection, the default role of the product is given to registered users", false, "RoleCollection");
		I_ASSIGN(m_callbackUrlAttrPtr, "CallbackUrl", "Public URL of the callback servlet, registered at the providers as redirect URI", true, "");
		I_ASSIGN(m_callbackUrlParamCompPtr, "CallbackUrlParam", "Callback URL from the settings, used instead of CallbackUrl if not empty", false, "CallbackUrlParam");
		I_ASSIGN(m_linkByVerifiedEmailAttrPtr, "LinkByVerifiedEmail", "Link an external account at its first sign-in to the user with the same verified e-mail", true, true);
		I_ASSIGN(m_allowRegistrationAttrPtr, "AllowRegistration", "Create a user for an external account with a verified e-mail that matches no user", true, false);
		I_ASSIGN(m_loginTimeoutAttrPtr, "LoginTimeout", "Seconds a started login waits for the provider", true, 600);
		I_ASSIGN(m_requestTimeoutAttrPtr, "RequestTimeout", "Timeout of the requests to the provider in milliseconds", true, 15000);
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalLoginController)
	virtual Providers GetProviders() const override;
	virtual bool BeginLogin(
				const QByteArray& providerId,
				const QByteArray& productId,
				const QByteArray& linkUserId,
				QByteArray& loginId,
				QString& authorizationUrl,
				QString& errorMessage) const override;
	virtual LoginResult TakeLoginResult(const QByteArray& loginId) const override;
	virtual LinkedAccounts GetLinkedAccounts(const QByteArray& userId) const override;
	virtual bool UnlinkAccount(const QByteArray& userId, const QByteArray& providerId, QString& errorMessage) const override;

	// reimplemented (imtauth::IExternalLoginCallbackHandler)
	virtual bool HandleCallback(
				const QByteArray& state,
				const QByteArray& code,
				const QString& providerError,
				QString& message) const override;

private:
	struct PendingLogin
	{
		QByteArray providerId;
		QString callbackUrl;
		QByteArray state;
		QByteArray nonce;
		QByteArray codeVerifier;
		QByteArray productId;
		QByteArray linkUserId;
		QDateTime expiresAt;
		LoginResult result;
	};

	struct ExternalAccount
	{
		QString subject;
		QString email;
		bool isEmailVerified = false;
		QString name;
	};

	const IExternalOidcProvider* FindProvider(const QByteArray& providerId) const;
	QString GetCallbackUrl() const;
	bool RequestAccount(const IExternalOidcProvider& provider, const PendingLogin& login, const QByteArray& code, ExternalAccount& account, QString& errorMessage) const;
	bool ReadIdTokenClaims(const IExternalOidcProvider& provider, const PendingLogin& login, const QString& idToken, QJsonObject& claims, QString& errorMessage) const;
	bool SendRequest(const QString& url, const QByteArray& postData, const QByteArray& bearerToken, QJsonObject& response, QString& errorMessage) const;
	LoginResult ResolveUser(const IExternalOidcProvider& provider, const PendingLogin& login, const ExternalAccount& account) const;
	LoginResult LinkAccount(const IExternalOidcProvider& provider, const QByteArray& userId, const ExternalAccount& account) const;
	LoginResult RegisterUser(const IExternalOidcProvider& provider, const PendingLogin& login, const ExternalAccount& account) const;
	IUserInfoSharedPtr GetUser(const QByteArray& userId) const;
	bool IsLoginUsed(const QByteArray& login) const;
	QByteArrayList FindUsersByEmail(const QString& email) const;
	void RemoveExpiredLogins() const;

	static QByteArray CreateRandomString();
	static LoginResult CreateFailedResult(const QString& message);

private:
	I_MULTIREF(IExternalOidcProvider, m_providersCompPtr);
	I_REF(IExternalIdentityManager, m_identityManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_FACT(IUserInfo, m_userFactoryCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_ATTR(QString, m_callbackUrlAttrPtr);
	I_REF(iprm::ITextParam, m_callbackUrlParamCompPtr);
	I_ATTR(bool, m_linkByVerifiedEmailAttrPtr);
	I_ATTR(bool, m_allowRegistrationAttrPtr);
	I_ATTR(int, m_loginTimeoutAttrPtr);
	I_ATTR(int, m_requestTimeoutAttrPtr);

	mutable QMutex m_loginsMutex;
	mutable QMap<QByteArray, PendingLogin> m_logins;
	mutable QMap<QByteArray, QByteArray> m_loginIdsByState;
};


} // namespace imtauth


