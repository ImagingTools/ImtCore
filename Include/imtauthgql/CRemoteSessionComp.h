// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtauth/IAccessTokenProvider.h>
#include <imtbase/IUrlParam.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IRemoteSession.h>


namespace imtauthgql
{


/**
	Session of this application with a remote server under an account of its own, signed in by its login and password.

	It signs in for the first request and again after the remote server refused the token; parallel requests wait
	for one sign-in. After a refused sign-in it does not try again until the login or the password changes: a server
	may lock an account after a few failed attempts. A failed connection is no refusal: the next request tries again.

	The token is issued for the product of the remote server ('ProductId'), and the requests name that product,
	not the product of this application. A new token is announced to the model observers (subscription managers)
	in the thread of the component; register the component with icomp::TModelCompWrap.
*/
class CRemoteSessionComp:
			public ilog::CLoggerComponentBase,
			virtual public imtclientgql::IRemoteSession,
			virtual public imtauth::IAccessTokenProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteSessionComp);
		I_REGISTER_INTERFACE(imtclientgql::IRemoteSession);
		I_REGISTER_INTERFACE(imtauth::IAccessTokenProvider);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "API client connected to the remote server", true, "ApiClient");
		I_ASSIGN(m_urlParamCompPtr, "UrlParam", "Optional URL override of the remote server, the same as for its requests", false, "UrlParam");
		I_ASSIGN(m_loginParamCompPtr, "LoginParam", "Login of the account of this application on the remote server", true, "LoginParam");
		I_ASSIGN(m_passwordParamCompPtr, "PasswordParam", "Password of the account of this application on the remote server", true, "PasswordParam");
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "ID of the product of the remote server: the token is issued for it", true, "");
	I_END_COMPONENT;

	// reimplemented (imtclientgql::IRemoteSession)
	virtual imtgql::IGqlContextSharedPtr CreateRequestContext(QString& error) override;
	virtual void OnTokenRejected(const QByteArray& token) override;

	// reimplemented (imtauth::IAccessTokenProvider)
	/**
		The current token; empty before the first sign-in. It never signs in itself.
	*/
	virtual QByteArray GetToken(const QByteArray& userId) const override;

private:
	/**
		Signs in with the current login and password.
		\return the new token, empty on failure
	*/
	QByteArray SignIn(QString& error);
	void AnnounceTokenChange();

private:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
	I_REF(iprm::ITextParam, m_loginParamCompPtr);
	I_REF(iprm::ITextParam, m_passwordParamCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);

	// Held for the whole sign-in, so that the requests waiting for a token share one sign-in
	QMutex m_signInMutex;
	mutable QMutex m_stateMutex;
	QByteArray m_token;
	// The login and the password the remote server refused (hashed), and why; they are not sent again
	QByteArray m_refusedCredentialsHash;
	QString m_refusalMessage;
	// Lives in the thread of the component: the observers get the token change there
	QObject m_announcementContext;
};


} // namespace imtauthgql
