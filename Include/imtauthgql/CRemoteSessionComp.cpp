// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteSessionComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QThread>

// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace imtauthgql
{
namespace
{


// The login and the password as one value to compare with, so that no copy of the password is kept
QByteArray HashCredentials(const QString& login, const QString& password)
{
	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(login.toUtf8());
	hash.addData(QByteArrayLiteral("\n"));
	hash.addData(password.toUtf8());

	return hash.result();
}


} // namespace


// public methods

// reimplemented (imtclientgql::IRemoteSession)

imtgql::IGqlContextSharedPtr CRemoteSessionComp::CreateRequestContext(QString& error)
{
	const QByteArray token = ObtainToken(error);
	if (token.isEmpty()){
		return imtgql::IGqlContextSharedPtr();
	}

	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetToken(token);
	contextPtr->SetProductId(*m_productIdAttrPtr);

	return imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr));
}


void CRemoteSessionComp::OnTokenRejected(const QByteArray& token)
{
	QMutexLocker stateLocker(&m_stateMutex);

	// A request may report a token that another sign-in has replaced already
	if (!token.isEmpty() && token == m_token){
		m_token.clear();
	}
}


// reimplemented (imtauth::IAccessTokenProvider)

QByteArray CRemoteSessionComp::GetToken(const QByteArray& /*userId*/) const
{
	QMutexLocker stateLocker(&m_stateMutex);

	return m_token;
}


// private methods

QByteArray CRemoteSessionComp::ObtainToken(QString& error)
{
	QMutexLocker stateLocker(&m_stateMutex);

	while (m_token.isEmpty()){
		if (m_signInThreadPtr == nullptr){
			return SignIn(stateLocker, error);
		}

		// A synchronous request waits for its answer in a nested event loop that runs the queued calls of its thread,
		// so this call may come from inside that sign-in: waiting here would block the sign-in for ever
		if (m_signInThreadPtr == QThread::currentThread()){
			error = QStringLiteral("The sign-in to the remote server is in progress");

			return QByteArray();
		}

		// Another thread signs in: this request takes the outcome of that sign-in
		const quint64 awaitedSignIn = m_finishedSignInCount;
		while (m_finishedSignInCount == awaitedSignIn){
			m_signInFinished.wait(&m_stateMutex);
		}

		if (m_token.isEmpty() && !m_lastSignInError.isEmpty()){
			error = m_lastSignInError;

			return QByteArray();
		}
	}

	return m_token;
}


QByteArray CRemoteSessionComp::SignIn(QMutexLocker<QMutex>& stateLocker, QString& error)
{
	const QString login = m_loginParamCompPtr->GetText();
	const QString password = m_passwordParamCompPtr->GetText();
	const QByteArray credentialsHash = HashCredentials(login, password);
	if (credentialsHash == m_refusedCredentialsHash){
		error = m_refusalMessage;

		return QByteArray();
	}

	m_signInThreadPtr = QThread::currentThread();

	// No lock is held across the request (see ObtainToken)
	stateLocker.unlock();
	bool isRefused = false;
	QByteArray token = RequestToken(login, password, isRefused, error);
	stateLocker.relock();

	m_signInThreadPtr = nullptr;
	++m_finishedSignInCount;
	m_lastSignInError = token.isEmpty() ? error : QString();
	m_signInFinished.wakeAll();

	if (token.isEmpty()){
		if (isRefused){
			m_refusedCredentialsHash = credentialsHash;
			m_refusalMessage = error;
		}

		return QByteArray();
	}

	m_token = token;
	m_refusedCredentialsHash.clear();
	m_refusalMessage.clear();
	AnnounceTokenChange();

	return token;
}


QByteArray CRemoteSessionComp::RequestToken(const QString& login, const QString& password, bool& isRefused, QString& error) const
{
	isRefused = false;

	namespace authsdl = sdl::V1_0::imtauth;

	authsdl::AuthorizationRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->login = login;
	arguments.input->password = password;
	arguments.input->productId = *m_productIdAttrPtr;

	imtgql::CGqlRequest gqlRequest;
	if (!authsdl::CAuthorizationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		error = QStringLiteral("Unable to build the sign-in request");
		SendErrorMessage(0, error, "CRemoteSessionComp");

		return QByteArray();
	}

	using PayloadFields = authsdl::CAuthorizationPayload::AuthorizationPayloadFields;
	gqlRequest.AddSimpleField(PayloadFields::Token.toUtf8());
	gqlRequest.AddSimpleField(PayloadFields::PasswordExpired.toUtf8());
	gqlRequest.AddSimpleField(PayloadFields::AccountDisabled.toUtf8());

	// The sign-in goes on behalf of no one, not with the context of the request being handled
	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetProductId(*m_productIdAttrPtr);
	gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (!requestPtr.IsValid()){
		error = QStringLiteral("Unable to clone the sign-in request");
		SendErrorMessage(0, error, "CRemoteSessionComp");

		return QByteArray();
	}

	imtbase::IUrlParam* urlParamPtr = nullptr;
	if (m_urlParamCompPtr.IsValid()){
		urlParamPtr = m_urlParamCompPtr.GetPtr();
	}

	const imtclientgql::IGqlClient::GqlModelResult<authsdl::CAuthorizationPayload> result =
				imtclientgql::CClientRequestModelHelpers::ParseModelResult<authsdl::CAuthorizationPayload>(
							m_apiClientCompPtr->SendRequest(requestPtr, urlParamPtr),
							gqlRequest.GetCommandId());

	QString refusalReason;
	if (result){
		const authsdl::CAuthorizationPayload& payload = result.model;
		if (payload.token && !payload.token->isEmpty()){
			return *payload.token;
		}

		// An answer without a token: the account cannot be used as it is
		if (payload.passwordExpired && *payload.passwordExpired){
			refusalReason = QStringLiteral("the password has expired");
		}
		else if (payload.accountDisabled && *payload.accountDisabled){
			refusalReason = QStringLiteral("the account is disabled");
		}
		else{
			refusalReason = QStringLiteral("no token was issued");
		}
	}
	else if (result.errorCategory == imtclientgql::IGqlClient::EC_SERVER_ERROR
				|| result.errorCategory == imtclientgql::IGqlClient::EC_ACCESS_DENIED){
		refusalReason = result.errorMessage;
	}
	else{
		// No answer is no refusal: the next request signs in again
		error = QStringLiteral("Unable to sign in to the remote server: %1").arg(result.errorMessage);
		SendErrorMessage(0, error, "CRemoteSessionComp");

		return QByteArray();
	}

	error = QStringLiteral("The remote server refused the sign-in of '%1': %2. "
				"It is not tried again until the login or the password changes").arg(login, refusalReason);
	SendErrorMessage(0, error, "CRemoteSessionComp");
	isRefused = true;

	return QByteArray();
}


void CRemoteSessionComp::AnnounceTokenChange()
{
	// The observers work in the thread of the component; the sign-in may run in the thread of a request
	QMetaObject::invokeMethod(&m_announcementContext, [this](){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);
	}, Qt::QueuedConnection);
}


} // namespace imtauthgql
