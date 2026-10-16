// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CExternalOidcAuthControllerComp.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QUuid>
#include <QtCore/QDateTime>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>

// ImtCore includes
#include <imtauth/IExternalIdentity.h>


namespace imtauth
{


// reimplemented (imtauth::IExternalOidcAuthController)

QByteArrayList CExternalOidcAuthControllerComp::GetAvailableProviders() const
{
	QByteArrayList providerIds;

	int count = m_providersCompPtr.GetSize();
	for (int i = 0; i < count; ++i){
		const IExternalOidcProvider* providerPtr = m_providersCompPtr.GetAt(i);
		if (providerPtr != nullptr){
			providerIds.append(providerPtr->GetProviderId());
		}
	}

	return providerIds;
}


QString CExternalOidcAuthControllerComp::GetAuthorizationUrl(
			const QByteArray& providerId,
			const QByteArray& state,
			const QByteArray& nonce) const
{
	const IExternalOidcProvider* providerPtr = FindProvider(providerId);
	if (providerPtr == nullptr){
		SendWarningMessage(0, QObject::tr("External OIDC provider '%1' not found").arg(QString::fromUtf8(providerId)));
		return QString();
	}

	QUrl authUrl(providerPtr->GetAuthorizationEndpoint());
	QUrlQuery query;

	query.addQueryItem(QStringLiteral("response_type"), QStringLiteral("code"));
	query.addQueryItem(QStringLiteral("client_id"), QString::fromUtf8(providerPtr->GetClientId()));
	query.addQueryItem(QStringLiteral("redirect_uri"), providerPtr->GetRedirectUri());
	query.addQueryItem(QStringLiteral("scope"), providerPtr->GetScopes());
	query.addQueryItem(QStringLiteral("state"), QString::fromUtf8(state));

	if (!nonce.isEmpty()){
		query.addQueryItem(QStringLiteral("nonce"), QString::fromUtf8(nonce));
	}

	authUrl.setQuery(query);

	return authUrl.toString();
}


IExternalOidcAuthController::AuthResult CExternalOidcAuthControllerComp::HandleCallback(
			const QByteArray& providerId,
			const QByteArray& authCode,
			const QByteArray& state) const
{
	AuthResult result;
	result.success = false;

	const IExternalOidcProvider* providerPtr = FindProvider(providerId);
	if (providerPtr == nullptr){
		result.errorMessage = QObject::tr("External OIDC provider '%1' not found").arg(QString::fromUtf8(providerId));
		return result;
	}

	if (authCode.isEmpty()){
		result.errorMessage = QObject::tr("Authorization code is empty");
		return result;
	}

	// Exchange authorization code for tokens
	QNetworkAccessManager networkManager;
	QNetworkRequest tokenRequest(QUrl(providerPtr->GetTokenEndpoint()));
	tokenRequest.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/x-www-form-urlencoded"));

	QUrlQuery postData;
	postData.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("authorization_code"));
	postData.addQueryItem(QStringLiteral("code"), QString::fromUtf8(authCode));
	postData.addQueryItem(QStringLiteral("redirect_uri"), providerPtr->GetRedirectUri());
	postData.addQueryItem(QStringLiteral("client_id"), QString::fromUtf8(providerPtr->GetClientId()));
	postData.addQueryItem(QStringLiteral("client_secret"), QString::fromUtf8(providerPtr->GetClientSecret()));

	QEventLoop eventLoop;
	QNetworkReply* replyPtr = networkManager.post(tokenRequest, postData.toString(QUrl::FullyEncoded).toUtf8());
	QObject::connect(replyPtr, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec();

	if (replyPtr->error() != QNetworkReply::NoError){
		result.errorMessage = QObject::tr("Token exchange failed: %1").arg(replyPtr->errorString());
		replyPtr->deleteLater();
		return result;
	}

	QByteArray tokenResponseData = replyPtr->readAll();
	replyPtr->deleteLater();

	QJsonDocument tokenDoc = QJsonDocument::fromJson(tokenResponseData);
	if (!tokenDoc.isObject()){
		result.errorMessage = QObject::tr("Invalid token response from provider");
		return result;
	}

	QJsonObject tokenObj = tokenDoc.object();
	QString accessToken = tokenObj.value(QStringLiteral("access_token")).toString();

	if (accessToken.isEmpty()){
		result.errorMessage = QObject::tr("No access token in provider response");
		return result;
	}

	// Extract user info — either from id_token or userinfo endpoint
	QString externalSubject;
	QString externalEmail;

	QString idToken = tokenObj.value(QStringLiteral("id_token")).toString();
	if (!idToken.isEmpty()){
		// Decode JWT payload (base64url-encoded middle part)
		QStringList parts = idToken.split('.');
		if (parts.size() >= 2){
			QByteArray payload = QByteArray::fromBase64(parts[1].toUtf8(), QByteArray::Base64UrlEncoding);
			QJsonDocument payloadDoc = QJsonDocument::fromJson(payload);
			if (payloadDoc.isObject()){
				QJsonObject claims = payloadDoc.object();
				externalSubject = claims.value(QStringLiteral("sub")).toString();
				externalEmail = claims.value(QStringLiteral("email")).toString();
			}
		}
	}

	// If no id_token or no sub extracted, try userinfo endpoint
	if (externalSubject.isEmpty() && !providerPtr->GetUserInfoEndpoint().isEmpty()){
		QNetworkRequest userInfoRequest(QUrl(providerPtr->GetUserInfoEndpoint()));
		userInfoRequest.setRawHeader(QByteArrayLiteral("Authorization"),
			QByteArrayLiteral("Bearer ") + accessToken.toUtf8());

		QNetworkReply* userInfoReplyPtr = networkManager.get(userInfoRequest);
		QEventLoop userInfoLoop;
		QObject::connect(userInfoReplyPtr, &QNetworkReply::finished, &userInfoLoop, &QEventLoop::quit);
		userInfoLoop.exec();

		if (userInfoReplyPtr->error() == QNetworkReply::NoError){
			QJsonDocument userInfoDoc = QJsonDocument::fromJson(userInfoReplyPtr->readAll());
			if (userInfoDoc.isObject()){
				QJsonObject userInfoObj = userInfoDoc.object();
				externalSubject = userInfoObj.value(QStringLiteral("sub")).toString();
				if (externalSubject.isEmpty()){
					// Facebook uses "id" instead of "sub"
					externalSubject = userInfoObj.value(QStringLiteral("id")).toString();
				}
				if (externalEmail.isEmpty()){
					externalEmail = userInfoObj.value(QStringLiteral("email")).toString();
				}
			}
		}
		userInfoReplyPtr->deleteLater();
	}

	if (externalSubject.isEmpty()){
		result.errorMessage = QObject::tr("Could not extract subject from provider response");
		return result;
	}

	// Look up existing external identity or create a new one
	QByteArray localUserId;
	bool identityFound = false;

	if (m_identityCollectionCompPtr.IsValid()){
		QByteArrayList objectIds = m_identityCollectionCompPtr->GetObjectIds();
		for (const QByteArray& objId : ::std::as_const(objectIds)){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_identityCollectionCompPtr->GetObjectData(objId, dataPtr)){
				const IExternalIdentity* identityPtr = dynamic_cast<const IExternalIdentity*>(dataPtr.GetPtr());
				if (identityPtr != nullptr
					&& identityPtr->GetProvider() == providerId
					&& identityPtr->GetExternalSubject() == externalSubject)
				{
					localUserId = identityPtr->GetUserId();
					identityFound = true;

					// Update last auth timestamp
					IExternalIdentity* mutableIdentityPtr = const_cast<IExternalIdentity*>(identityPtr);
					mutableIdentityPtr->SetLastAuthAt(QDateTime::currentDateTimeUtc());
					break;
				}
			}
		}
	}

	// If no linked identity found, find or create a user by email
	if (!identityFound){
		if (!externalEmail.isEmpty() && m_userManagerCompPtr.IsValid()){
			// Try to find existing user by email
			QList<IUserManager::User> users = m_userManagerCompPtr->GetUserList();
			for (const IUserManager::User& user : ::std::as_const(users)){
				if (user.email.compare(externalEmail, Qt::CaseInsensitive) == 0){
					localUserId = user.uuid;
					break;
				}
			}

			// Create new user if not found
			if (localUserId.isEmpty()){
				QString userName = externalEmail.split('@').first();
				QByteArray login = externalEmail.toUtf8();
				// Generate a random password for externally-authenticated users
				QByteArray randomPassword = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
				localUserId = m_userManagerCompPtr->CreateUser(userName, login, randomPassword, externalEmail);
			}
		}

		if (localUserId.isEmpty()){
			result.errorMessage = QObject::tr("Could not find or create local user for external identity");
			return result;
		}

		// Create external identity record
		if (m_identityCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::IDataFactory* factoryPtr = m_identityCollectionCompPtr->GetDataFactory();
			if (factoryPtr != nullptr){
				istd::IChangeableUniquePtr newIdentityPtr = factoryPtr->CreateInstance();
				IExternalIdentity* identityPtr = dynamic_cast<IExternalIdentity*>(newIdentityPtr.GetPtr());
				if (identityPtr != nullptr){
					QByteArray identityId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
					identityPtr->SetId(identityId);
					identityPtr->SetUserId(localUserId);
					identityPtr->SetProvider(providerId);
					identityPtr->SetExternalSubject(externalSubject);
					identityPtr->SetExternalEmail(externalEmail);
					identityPtr->SetLinkedAt(QDateTime::currentDateTimeUtc());
					identityPtr->SetLastAuthAt(QDateTime::currentDateTimeUtc());

					imtbase::IObjectCollection::ICollectionDataController* dataControllerPtr =
						m_identityCollectionCompPtr->GetDataController();
					if (dataControllerPtr != nullptr){
						dataControllerPtr->InsertObject(
							QByteArrayLiteral("ExternalIdentity"),
							identityId,
							QString(),
							QString(),
							identityPtr);
					}
				}
			}
		}
	}

	// Create internal session
	if (m_sessionControllerCompPtr.IsValid()){
		IJwtSessionController::UserSession session;
		if (m_sessionControllerCompPtr->CreateNewSession(localUserId, QByteArray(), session)){
			result.success = true;
			result.session = session;
			result.userId = localUserId;
		}
		else{
			result.errorMessage = QObject::tr("Failed to create internal session");
		}
	}
	else{
		result.errorMessage = QObject::tr("Session controller is not available");
	}

	return result;
}


bool CExternalOidcAuthControllerComp::UnlinkProvider(
			const QByteArray& userId,
			const QByteArray& providerId) const
{
	if (!m_identityCollectionCompPtr.IsValid()){
		return false;
	}

	QByteArrayList objectIds = m_identityCollectionCompPtr->GetObjectIds();
	for (const QByteArray& objId : ::std::as_const(objectIds)){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_identityCollectionCompPtr->GetObjectData(objId, dataPtr)){
			const IExternalIdentity* identityPtr = dynamic_cast<const IExternalIdentity*>(dataPtr.GetPtr());
			if (identityPtr != nullptr
				&& identityPtr->GetUserId() == userId
				&& identityPtr->GetProvider() == providerId)
			{
				imtbase::IObjectCollection::ICollectionDataController* dataControllerPtr =
					m_identityCollectionCompPtr->GetDataController();
				if (dataControllerPtr != nullptr){
					QByteArrayList idsToDelete;
					idsToDelete.append(objId);
					return dataControllerPtr->DeleteObjects(idsToDelete);
				}
			}
		}
	}

	return false;
}


QList<IExternalOidcAuthController::LinkedProvider> CExternalOidcAuthControllerComp::GetLinkedProviders(
			const QByteArray& userId) const
{
	QList<LinkedProvider> providers;

	if (!m_identityCollectionCompPtr.IsValid()){
		return providers;
	}

	QByteArrayList objectIds = m_identityCollectionCompPtr->GetObjectIds();
	for (const QByteArray& objId : ::std::as_const(objectIds)){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_identityCollectionCompPtr->GetObjectData(objId, dataPtr)){
			const IExternalIdentity* identityPtr = dynamic_cast<const IExternalIdentity*>(dataPtr.GetPtr());
			if (identityPtr != nullptr && identityPtr->GetUserId() == userId){
				LinkedProvider linked;
				linked.provider = identityPtr->GetProvider();
				linked.externalSubject = identityPtr->GetExternalSubject();
				linked.externalEmail = identityPtr->GetExternalEmail();
				linked.linkedAt = identityPtr->GetLinkedAt();
				linked.lastAuthAt = identityPtr->GetLastAuthAt();
				providers.append(linked);
			}
		}
	}

	return providers;
}


// private methods

const IExternalOidcProvider* CExternalOidcAuthControllerComp::FindProvider(const QByteArray& providerId) const
{
	int count = m_providersCompPtr.GetSize();
	for (int i = 0; i < count; ++i){
		const IExternalOidcProvider* providerPtr = m_providersCompPtr.GetAt(i);
		if (providerPtr != nullptr && providerPtr->GetProviderId() == providerId){
			return providerPtr;
		}
	}

	return nullptr;
}


} // namespace imtauth
