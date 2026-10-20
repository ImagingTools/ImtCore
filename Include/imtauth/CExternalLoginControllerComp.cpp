// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CExternalLoginControllerComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QEventLoop>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QMutexLocker>
#include <QtCore/QRandomGenerator>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IIdentifiable.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/IRole.h>


namespace imtauth
{


static const int s_maxPendingLogins = 10000;
static const qint64 s_clockSkewSecs = 120;


// public methods

// reimplemented (imtauth::IExternalLoginController)

IExternalLoginController::Providers CExternalLoginControllerComp::GetProviders() const
{
	Providers retVal;

	for (int i = 0; i < m_providersCompPtr.GetCount(); ++i){
		const IExternalOidcProvider* providerPtr = m_providersCompPtr[i];
		if (providerPtr != nullptr && !providerPtr->GetClientId().isEmpty()){
			ProviderInfo providerInfo;
			providerInfo.id = providerPtr->GetProviderId();
			providerInfo.name = providerPtr->GetProviderName();
			providerInfo.type = providerPtr->GetProviderType();

			retVal.append(providerInfo);
		}
	}

	return retVal;
}


bool CExternalLoginControllerComp::BeginLogin(
			const QByteArray& providerId,
			const QByteArray& productId,
			const QByteArray& linkUserId,
			QByteArray& loginId,
			QString& authorizationUrl,
			QString& errorMessage) const
{
	const IExternalOidcProvider* providerPtr = FindProvider(providerId);
	if (providerPtr == nullptr){
		errorMessage = QStringLiteral("Unknown sign-in provider '%1'").arg(QString::fromUtf8(providerId));

		return false;
	}

	const QString callbackUrl = GetCallbackUrl();
	if (callbackUrl.isEmpty()){
		errorMessage = QStringLiteral("Sign-in with external accounts is not configured");
		SendErrorMessage(0, QStringLiteral("The callback URL is not set, the external sign-in cannot start"));

		return false;
	}

	PendingLogin login;
	login.providerId = providerId;
	login.callbackUrl = callbackUrl;
	login.state = CreateRandomString();
	login.nonce = CreateRandomString();
	login.productId = productId;
	login.linkUserId = linkUserId;
	login.expiresAt = QDateTime::currentDateTimeUtc().addSecs(*m_loginTimeoutAttrPtr);
	login.result.status = LS_PENDING;
	login.result.productId = productId;

	QUrlQuery query;
	query.addQueryItem(QStringLiteral("response_type"), QStringLiteral("code"));
	query.addQueryItem(QStringLiteral("client_id"), QString::fromUtf8(providerPtr->GetClientId()));
	query.addQueryItem(QStringLiteral("redirect_uri"), callbackUrl);
	query.addQueryItem(QStringLiteral("scope"), providerPtr->GetScopes());
	query.addQueryItem(QStringLiteral("state"), QString::fromLatin1(login.state));

	if (providerPtr->IsOpenIdConnect()){
		query.addQueryItem(QStringLiteral("nonce"), QString::fromLatin1(login.nonce));
	}

	if (providerPtr->IsPkceSupported()){
		login.codeVerifier = CreateRandomString();

		const QByteArray codeChallenge = QCryptographicHash::hash(login.codeVerifier, QCryptographicHash::Sha256)
					.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);

		query.addQueryItem(QStringLiteral("code_challenge"), QString::fromLatin1(codeChallenge));
		query.addQueryItem(QStringLiteral("code_challenge_method"), QStringLiteral("S256"));
	}

	const IExternalOidcProvider::QueryItems additionalParams = providerPtr->GetAuthorizationParams();
	for (const QPair<QString, QString>& param : additionalParams){
		query.addQueryItem(param.first, param.second);
	}

	QUrl url(providerPtr->GetAuthorizationEndpoint());
	url.setQuery(query);

	{
		QMutexLocker locker(&m_loginsMutex);

		RemoveExpiredLogins();

		if (m_logins.count() >= s_maxPendingLogins){
			errorMessage = QStringLiteral("Too many sign-in requests. Please try again later.");
			SendWarningMessage(0, errorMessage);

			return false;
		}

		loginId = CreateRandomString();

		m_loginIdsByState.insert(login.state, loginId);
		m_logins.insert(loginId, login);
	}

	authorizationUrl = url.toString(QUrl::FullyEncoded);

	return true;
}


IExternalLoginController::LoginResult CExternalLoginControllerComp::TakeLoginResult(const QByteArray& loginId) const
{
	QMutexLocker locker(&m_loginsMutex);

	RemoveExpiredLogins();

	auto loginIter = m_logins.find(loginId);
	if (loginIter == m_logins.end()){
		return LoginResult();
	}

	LoginResult retVal = loginIter->result;
	if (retVal.status != LS_PENDING){
		m_loginIdsByState.remove(loginIter->state);
		m_logins.erase(loginIter);
	}

	return retVal;
}


IExternalLoginController::LinkedAccounts CExternalLoginControllerComp::GetLinkedAccounts(const QByteArray& userId) const
{
	LinkedAccounts retVal;

	const IExternalIdentityManager::Identities identities = m_identityManagerCompPtr->GetUserIdentities(userId);
	for (const IExternalIdentitySharedPtr& identityPtr : identities){
		LinkedAccount account;
		account.providerId = identityPtr->GetProviderId();
		account.email = identityPtr->GetEmail();
		account.linkedAt = identityPtr->GetLinkedAt();
		account.lastSignInAt = identityPtr->GetLastSignInAt();

		retVal.append(account);
	}

	return retVal;
}


bool CExternalLoginControllerComp::UnlinkAccount(const QByteArray& userId, const QByteArray& providerId, QString& errorMessage) const
{
	const IExternalIdentityManager::Identities identities = m_identityManagerCompPtr->GetUserIdentities(userId);

	IExternalIdentitySharedPtr unlinkedIdentityPtr;
	for (const IExternalIdentitySharedPtr& identityPtr : identities){
		if (identityPtr->GetProviderId() == providerId){
			unlinkedIdentityPtr = identityPtr;
		}
	}

	if (!unlinkedIdentityPtr.IsValid()){
		errorMessage = QStringLiteral("The account is not linked.");

		return false;
	}

	IUserInfoSharedPtr userPtr = GetUser(userId);
	const bool hasPassword = userPtr.IsValid() && !userPtr->GetSystemInfos().isEmpty();
	if (!hasPassword && identities.count() < 2){
		errorMessage = QStringLiteral("This account is the only way to sign in, it cannot be unlinked.");

		return false;
	}

	if (!m_identityManagerCompPtr->RemoveIdentity(unlinkedIdentityPtr->GetId())){
		errorMessage = QStringLiteral("The account could not be unlinked.");

		return false;
	}

	SendInfoMessage(0, QStringLiteral("The '%1' account was unlinked from the user '%2'").arg(QString::fromUtf8(providerId), QString::fromUtf8(userId)));

	return true;
}


// reimplemented (imtauth::IExternalLoginCallbackHandler)

bool CExternalLoginControllerComp::HandleCallback(
			const QByteArray& state,
			const QByteArray& code,
			const QString& providerError,
			QString& message) const
{
	QByteArray loginId;
	PendingLogin login;
	{
		QMutexLocker locker(&m_loginsMutex);

		RemoveExpiredLogins();

		// The state is single-use: a replayed redirect finds nothing.
		loginId = m_loginIdsByState.take(state);
		if (loginId.isEmpty() || !m_logins.contains(loginId)){
			message = QStringLiteral("The sign-in request is unknown or has expired. Please start the sign-in again.");

			return false;
		}

		login = m_logins.value(loginId);
	}

	LoginResult result;

	const IExternalOidcProvider* providerPtr = FindProvider(login.providerId);
	if (providerPtr == nullptr){
		result = CreateFailedResult(QStringLiteral("The sign-in provider is no longer available."));
	}
	else if (!providerError.isEmpty()){
		if (providerError == QStringLiteral("access_denied") || providerError == QStringLiteral("user_cancelled_authorize")){
			result = CreateFailedResult(QStringLiteral("The sign-in with %1 was cancelled.").arg(providerPtr->GetProviderName()));
		}
		else{
			result = CreateFailedResult(QStringLiteral("%1 rejected the sign-in (%2).").arg(providerPtr->GetProviderName(), providerError));
		}
	}
	else if (code.isEmpty()){
		result = CreateFailedResult(QStringLiteral("%1 returned no authorization code.").arg(providerPtr->GetProviderName()));
	}
	else{
		ExternalAccount account;
		QString errorMessage;
		if (RequestAccount(*providerPtr, login, code, account, errorMessage)){
			result = ResolveUser(*providerPtr, login, account);
		}
		else{
			SendErrorMessage(0, QStringLiteral("Sign-in with '%1' failed: %2").arg(QString::fromUtf8(login.providerId), errorMessage));

			result = CreateFailedResult(QStringLiteral("The sign-in with %1 could not be completed. Please try again.").arg(providerPtr->GetProviderName()));
		}
	}

	result.productId = login.productId;

	{
		QMutexLocker locker(&m_loginsMutex);

		auto loginIter = m_logins.find(loginId);
		if (loginIter != m_logins.end()){
			loginIter->result = result;
		}
	}

	switch (result.status){
	case LS_SIGNED_IN:
		message = QStringLiteral("You are signed in. Return to the application to continue.");
		return true;

	case LS_LINKED:
		message = QStringLiteral("The %1 account is linked. Return to the application to continue.").arg(providerPtr->GetProviderName());
		return true;

	default:
		message = result.message;
		return false;
	}
}


// private methods

const IExternalOidcProvider* CExternalLoginControllerComp::FindProvider(const QByteArray& providerId) const
{
	if (providerId.isEmpty()){
		return nullptr;
	}

	for (int i = 0; i < m_providersCompPtr.GetCount(); ++i){
		const IExternalOidcProvider* providerPtr = m_providersCompPtr[i];
		if (providerPtr != nullptr && providerPtr->GetProviderId() == providerId && !providerPtr->GetClientId().isEmpty()){
			return providerPtr;
		}
	}

	return nullptr;
}


QString CExternalLoginControllerComp::GetCallbackUrl() const
{
	if (m_callbackUrlParamCompPtr.IsValid()){
		const QString callbackUrl = m_callbackUrlParamCompPtr->GetText().trimmed();
		if (!callbackUrl.isEmpty()){
			return callbackUrl;
		}
	}

	return *m_callbackUrlAttrPtr;
}


bool CExternalLoginControllerComp::RequestAccount(
			const IExternalOidcProvider& provider,
			const PendingLogin& login,
			const QByteArray& code,
			ExternalAccount& account,
			QString& errorMessage) const
{
	QUrlQuery tokenQuery;
	tokenQuery.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("authorization_code"));
	tokenQuery.addQueryItem(QStringLiteral("code"), QString::fromUtf8(code));
	tokenQuery.addQueryItem(QStringLiteral("redirect_uri"), login.callbackUrl);
	tokenQuery.addQueryItem(QStringLiteral("client_id"), QString::fromUtf8(provider.GetClientId()));
	tokenQuery.addQueryItem(QStringLiteral("client_secret"), QString::fromUtf8(provider.GetClientSecret()));
	if (!login.codeVerifier.isEmpty()){
		tokenQuery.addQueryItem(QStringLiteral("code_verifier"), QString::fromLatin1(login.codeVerifier));
	}

	// QUrlQuery leaves '+' as is, in a form body it would be read as a space.
	QByteArray postData = tokenQuery.toString(QUrl::FullyEncoded).toUtf8();
	postData.replace('+', "%2B");

	QJsonObject tokenResponse;
	if (!SendRequest(provider.GetTokenEndpoint(), postData, QByteArray(), tokenResponse, errorMessage)){
		return false;
	}

	const QByteArray accessToken = tokenResponse.value(QStringLiteral("access_token")).toString().toUtf8();

	QJsonObject claims;
	if (provider.IsOpenIdConnect()){
		const QString idToken = tokenResponse.value(QStringLiteral("id_token")).toString();
		if (!ReadIdTokenClaims(provider, login, idToken, claims, errorMessage)){
			return false;
		}
	}

	const QString userInfoEndpoint = provider.GetUserInfoEndpoint();
	if (!userInfoEndpoint.isEmpty()){
		if (accessToken.isEmpty()){
			errorMessage = QStringLiteral("The token response contains no access token");

			return false;
		}

		QJsonObject userInfo;
		if (!SendRequest(userInfoEndpoint, QByteArray(), accessToken, userInfo, errorMessage)){
			return false;
		}

		const QString subjectClaim = provider.GetSubjectClaim();
		if (claims.contains(subjectClaim) && claims.value(subjectClaim) != userInfo.value(subjectClaim)){
			errorMessage = QStringLiteral("The user info belongs to another account than the ID token");

			return false;
		}

		for (auto claimIter = userInfo.constBegin(); claimIter != userInfo.constEnd(); ++claimIter){
			if (!claims.contains(claimIter.key())){
				claims.insert(claimIter.key(), claimIter.value());
			}
		}
	}

	account.subject = claims.value(provider.GetSubjectClaim()).toVariant().toString();
	if (account.subject.isEmpty()){
		errorMessage = QStringLiteral("The provider returned no account identifier ('%1')").arg(provider.GetSubjectClaim());

		return false;
	}

	account.email = claims.value(QStringLiteral("email")).toString().trimmed();
	account.name = claims.value(QStringLiteral("name")).toString().trimmed();

	// Apple sends the flag as a string.
	const QJsonValue emailVerified = claims.value(QStringLiteral("email_verified"));
	account.isEmailVerified =
				!account.email.isEmpty() &&
				(provider.IsEmailVerifiedByProvider() || emailVerified.toBool() || emailVerified.toString() == QStringLiteral("true"));

	return true;
}


bool CExternalLoginControllerComp::ReadIdTokenClaims(
			const IExternalOidcProvider& provider,
			const PendingLogin& login,
			const QString& idToken,
			QJsonObject& claims,
			QString& errorMessage) const
{
	// The token comes straight from the token endpoint over TLS, which authenticates the issuer
	// (OpenID Connect Core 3.1.3.7), so the signature is not verified; the claims are.
	const QStringList parts = idToken.split(QLatin1Char('.'));
	if (parts.count() != 3){
		errorMessage = QStringLiteral("The token response contains no valid ID token");

		return false;
	}

	const QByteArray payload = QByteArray::fromBase64(parts[1].toLatin1(), QByteArray::Base64UrlEncoding);
	const QJsonDocument payloadDocument = QJsonDocument::fromJson(payload);
	if (!payloadDocument.isObject()){
		errorMessage = QStringLiteral("The ID token payload is not a JSON object");

		return false;
	}

	claims = payloadDocument.object();

	const QStringList issuers = provider.GetIssuers();
	const QString issuer = claims.value(QStringLiteral("iss")).toString();
	if (!issuers.isEmpty() && !issuers.contains(issuer)){
		errorMessage = QStringLiteral("The ID token was issued by an unexpected issuer '%1'").arg(issuer);

		return false;
	}

	const QString clientId = QString::fromUtf8(provider.GetClientId());
	const QJsonValue audience = claims.value(QStringLiteral("aud"));
	const bool isAudienceValid = audience.isArray() ? audience.toArray().contains(clientId) : (audience.toString() == clientId);
	if (!isAudienceValid){
		errorMessage = QStringLiteral("The ID token was issued for another client");

		return false;
	}

	const qint64 expiresAt = claims.value(QStringLiteral("exp")).toVariant().toLongLong();
	if (expiresAt + s_clockSkewSecs < QDateTime::currentSecsSinceEpoch()){
		errorMessage = QStringLiteral("The ID token has expired");

		return false;
	}

	if (claims.value(QStringLiteral("nonce")).toString().toLatin1() != login.nonce){
		errorMessage = QStringLiteral("The ID token belongs to another sign-in request (nonce mismatch)");

		return false;
	}

	return true;
}


bool CExternalLoginControllerComp::SendRequest(
			const QString& url,
			const QByteArray& postData,
			const QByteArray& bearerToken,
			QJsonObject& response,
			QString& errorMessage) const
{
	QNetworkRequest request{QUrl(url)};
	request.setTransferTimeout(*m_requestTimeoutAttrPtr);
	request.setRawHeader(QByteArrayLiteral("Accept"), QByteArrayLiteral("application/json"));
	if (!bearerToken.isEmpty()){
		request.setRawHeader(QByteArrayLiteral("Authorization"), QByteArrayLiteral("Bearer ") + bearerToken);
	}

	// Owns the reply, both live only for this request in the calling thread.
	QNetworkAccessManager networkManager;

	QNetworkReply* replyPtr = nullptr;
	if (postData.isEmpty()){
		replyPtr = networkManager.get(request);
	}
	else{
		request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));
		replyPtr = networkManager.post(request, postData);
	}

	if (!replyPtr->isFinished()){
		QEventLoop eventLoop;
		QObject::connect(replyPtr, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
		eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
	}

	const QByteArray body = replyPtr->readAll();
	const QJsonDocument document = QJsonDocument::fromJson(body);
	response = document.object();

	const QString host = QUrl(url).host();

	if (replyPtr->error() != QNetworkReply::NoError){
		const QString providerError = response.value(QStringLiteral("error_description")).toString();
		if (!providerError.isEmpty()){
			errorMessage = QStringLiteral("Request to '%1' failed: %2").arg(host, providerError);
		}
		else{
			errorMessage = QStringLiteral("Request to '%1' failed: %2").arg(host, replyPtr->errorString());
		}

		return false;
	}

	if (!document.isObject()){
		errorMessage = QStringLiteral("Request to '%1' returned no JSON object").arg(host);

		return false;
	}

	return true;
}


IExternalLoginController::LoginResult CExternalLoginControllerComp::ResolveUser(
			const IExternalOidcProvider& provider,
			const PendingLogin& login,
			const ExternalAccount& account) const
{
	const QByteArray providerId = provider.GetProviderId();

	IExternalIdentitySharedPtr identityPtr = m_identityManagerCompPtr->FindIdentity(providerId, account.subject);
	if (identityPtr.IsValid() && !GetUser(identityPtr->GetUserId()).IsValid()){
		// The user was deleted, the account is free again.
		m_identityManagerCompPtr->RemoveIdentity(identityPtr->GetId());
		identityPtr.Reset();
	}

	if (!login.linkUserId.isEmpty()){
		if (identityPtr.IsValid()){
			if (identityPtr->GetUserId() != login.linkUserId){
				return CreateFailedResult(QStringLiteral("This %1 account is already linked to another user.").arg(provider.GetProviderName()));
			}

			LoginResult result;
			result.status = LS_LINKED;
			result.userId = login.linkUserId;

			return result;
		}

		return LinkAccount(provider, login.linkUserId, account);
	}

	if (identityPtr.IsValid()){
		const QByteArray userId = identityPtr->GetUserId();

		IUserInfoSharedPtr userPtr = GetUser(userId);
		if (!userPtr->IsEnabled()){
			return CreateFailedResult(QStringLiteral("This account has been deactivated. Please contact your administrator."));
		}

		m_identityManagerCompPtr->RegisterSignIn(identityPtr->GetId(), account.email);

		LoginResult result;
		result.status = LS_SIGNED_IN;
		result.userId = userId;

		return result;
	}

	if (*m_linkByVerifiedEmailAttrPtr && account.isEmailVerified){
		const QByteArrayList userIds = FindUsersByEmail(account.email);
		if (userIds.count() == 1){
			IUserInfoSharedPtr userPtr = GetUser(userIds.first());
			if (userPtr.IsValid() && !userPtr->IsEnabled()){
				return CreateFailedResult(QStringLiteral("This account has been deactivated. Please contact your administrator."));
			}

			LoginResult result = LinkAccount(provider, userIds.first(), account);
			if (result.status == LS_LINKED){
				result.status = LS_SIGNED_IN;
			}

			return result;
		}

		if (userIds.count() > 1){
			SendWarningMessage(0, QStringLiteral("The %1 account '%2' is not linked: %3 users have this e-mail").arg(provider.GetProviderName(), account.email).arg(userIds.count()));
		}
	}

	if (*m_allowRegistrationAttrPtr && account.isEmailVerified){
		return RegisterUser(provider, login, account);
	}

	return CreateFailedResult(
				QStringLiteral("No user is linked to this %1 account. Sign in with your password and link the account in your profile, or contact your administrator.")
				.arg(provider.GetProviderName()));
}


IExternalLoginController::LoginResult CExternalLoginControllerComp::LinkAccount(
			const IExternalOidcProvider& provider,
			const QByteArray& userId,
			const ExternalAccount& account) const
{
	const QByteArray providerId = provider.GetProviderId();

	if (!GetUser(userId).IsValid()){
		return CreateFailedResult(QStringLiteral("The user does not exist."));
	}

	// One account per provider and user keeps unlinking by provider unambiguous.
	const IExternalIdentityManager::Identities identities = m_identityManagerCompPtr->GetUserIdentities(userId);
	for (const IExternalIdentitySharedPtr& identityPtr : identities){
		if (identityPtr->GetProviderId() == providerId){
			return CreateFailedResult(QStringLiteral("Another %1 account is already linked to this user. Unlink it in the profile first.").arg(provider.GetProviderName()));
		}
	}

	if (m_identityManagerCompPtr->LinkIdentity(userId, providerId, account.subject, account.email).isEmpty()){
		return CreateFailedResult(QStringLiteral("The %1 account could not be linked.").arg(provider.GetProviderName()));
	}

	SendInfoMessage(0, QStringLiteral("The %1 account '%2' was linked to the user '%3'").arg(provider.GetProviderName(), account.email, QString::fromUtf8(userId)));

	LoginResult result;
	result.status = LS_LINKED;
	result.userId = userId;

	return result;
}


IExternalLoginController::LoginResult CExternalLoginControllerComp::RegisterUser(
			const IExternalOidcProvider& provider,
			const PendingLogin& login,
			const ExternalAccount& account) const
{
	if (!m_userFactoryCompPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("AllowRegistration is set, but the user factory is not configured"));

		return CreateFailedResult(QStringLiteral("The user could not be created."));
	}

	const QByteArray userLogin = account.email.toUtf8();
	if (IsLoginUsed(userLogin)){
		return CreateFailedResult(QStringLiteral("A user with the login '%1' already exists. Please contact your administrator.").arg(account.email));
	}

	IUserInfoUniquePtr userPtr = m_userFactoryCompPtr.CreateInstance();
	imtbase::IIdentifiable* identifiablePtr = dynamic_cast<imtbase::IIdentifiable*>(userPtr.GetPtr());
	if (!userPtr.IsValid() || identifiablePtr == nullptr){
		return CreateFailedResult(QStringLiteral("The user could not be created."));
	}

	const QByteArray userId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	const QString userName = account.name.isEmpty() ? account.email : account.name;

	// No system info: the user has no password and signs in with the external account only.
	identifiablePtr->SetObjectUuid(userId);
	userPtr->SetId(userLogin);
	userPtr->SetName(userName);
	userPtr->SetMail(account.email);
	userPtr->SetEnabled(true);

	if (m_roleCollectionCompPtr.IsValid() && !login.productId.isEmpty()){
		const imtbase::ICollectionInfo::Ids roleIds = m_roleCollectionCompPtr->GetElementIds();
		for (const QByteArray& roleId : roleIds){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (m_roleCollectionCompPtr->GetObjectData(roleId, roleDataPtr)){
				const IRole* rolePtr = dynamic_cast<const IRole*>(roleDataPtr.GetPtr());
				if (rolePtr != nullptr && rolePtr->GetProductId() == login.productId && rolePtr->IsDefault()){
					userPtr->AddRole(login.productId, roleId);
					break;
				}
			}
		}
	}

	if (m_userCollectionCompPtr->InsertNewObject("User", userName, QString(), userPtr.GetPtr(), userId).isEmpty()){
		SendErrorMessage(0, QStringLiteral("Unable to store the user registered with the %1 account '%2'").arg(provider.GetProviderName(), account.email));

		return CreateFailedResult(QStringLiteral("The user could not be created."));
	}

	SendInfoMessage(0, QStringLiteral("The user '%1' was registered with the %2 account").arg(account.email, provider.GetProviderName()));

	LoginResult result = LinkAccount(provider, userId, account);
	if (result.status == LS_LINKED){
		result.status = LS_SIGNED_IN;
	}

	return result;
}


IUserInfoSharedPtr CExternalLoginControllerComp::GetUser(const QByteArray& userId) const
{
	if (userId.isEmpty()){
		return IUserInfoSharedPtr();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		return IUserInfoSharedPtr();
	}

	return dataPtr.dynamicCast<IUserInfo>();
}


bool CExternalLoginControllerComp::IsLoginUsed(const QByteArray& login) const
{
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter(QByteArrayLiteral("Id"), QString::fromUtf8(login)));

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	const imtbase::ICollectionInfo::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	for (const QByteArray& userId : userIds){
		IUserInfoSharedPtr userPtr = GetUser(userId);
		if (userPtr.IsValid() && userPtr->GetId() == login){
			return true;
		}
	}

	return false;
}


QByteArrayList CExternalLoginControllerComp::FindUsersByEmail(const QString& email) const
{
	// The pattern match ignores the case; '_' and '%' in an address are wildcards, so every hit is compared.
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter(QByteArrayLiteral("Mail"), email, imtbase::IComplexCollectionFilter::FO_CONTAINS));

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	QByteArrayList retVal;

	const imtbase::ICollectionInfo::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	for (const QByteArray& userId : userIds){
		IUserInfoSharedPtr userPtr = GetUser(userId);
		if (userPtr.IsValid() && userPtr->GetMail().trimmed().compare(email, Qt::CaseInsensitive) == 0){
			retVal.append(userId);
		}
	}

	return retVal;
}


void CExternalLoginControllerComp::RemoveExpiredLogins() const
{
	const QDateTime now = QDateTime::currentDateTimeUtc();

	for (auto loginIter = m_logins.begin(); loginIter != m_logins.end();){
		if (loginIter->expiresAt < now){
			m_loginIdsByState.remove(loginIter->state);
			loginIter = m_logins.erase(loginIter);
		}
		else{
			++loginIter;
		}
	}
}


QByteArray CExternalLoginControllerComp::CreateRandomString()
{
	quint32 randomData[8];
	QRandomGenerator::system()->fillRange(randomData);

	return QByteArray(reinterpret_cast<const char*>(randomData), sizeof(randomData))
				.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


IExternalLoginController::LoginResult CExternalLoginControllerComp::CreateFailedResult(const QString& message)
{
	LoginResult result;
	result.status = LS_FAILED;
	result.message = message;

	return result;
}


} // namespace imtauth


