// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QWaitCondition>

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

	It signs in for the first request and again after the remote server refused the token; the requests of other threads
	wait for that sign-in and take its outcome. No lock is held across the sign-in: a synchronous request waits in a nested
	event loop of its thread, and a request that comes from that loop fails at once instead of waiting for ever.
	After a refused sign-in it does not try again until the login or the password changes: a server may lock an account
	after a few failed attempts. A failed connection is no refusal: the next request tries again.

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
		The current token, or the token of a new sign-in.
		\return empty on failure
	*/
	QByteArray ObtainToken(QString& error);
	/**
		Signs in with the current login and password and keeps the outcome. It is called with stateLocker locked
		and unlocks it for the time of the request.
		\return the new token, empty on failure
	*/
	QByteArray SignIn(QMutexLocker<QMutex>& stateLocker, QString& error);
	/**
		Sends the sign-in request; it touches no state of the session.
		\param isRefused [OUTPUT] - the remote server refused the login and the password (not a failed connection)
		\return the token, empty on failure
	*/
	QByteArray RequestToken(const QString& login, const QString& password, bool& isRefused, QString& error) const;
	void AnnounceTokenChange();

private:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
	I_REF(iprm::ITextParam, m_loginParamCompPtr);
	I_REF(iprm::ITextParam, m_passwordParamCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);

	mutable QMutex m_stateMutex;
	QByteArray m_token;
	// The thread of the sign-in in progress, if there is one; the other threads wait for its end
	QThread* m_signInThreadPtr = nullptr;
	QWaitCondition m_signInFinished;
	quint64 m_finishedSignInCount = 0;
	// Why the last sign-in failed; empty after a successful one
	QString m_lastSignInError;
	// The login and the password the remote server refused (hashed), and why; they are not sent again
	QByteArray m_refusedCredentialsHash;
	QString m_refusalMessage;
	// Lives in the thread of the component: the observers get the token change there
	QObject m_announcementContext;
};


} // namespace imtauthgql
