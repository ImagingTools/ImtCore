#include "CExternalLoginTest.h"


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>
#include <QtNetwork/QTcpSocket>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


static const quint16 s_providerPort = 18931;
static const QByteArray s_providerId = QByteArrayLiteral("testidp");
static const QString s_issuer = QStringLiteral("http://127.0.0.1:18931");
static const QString s_clientId = QStringLiteral("test-client");
static const QString s_clientSecret = QStringLiteral("test-secret");
static const QString s_callbackUrl = QStringLiteral("https://app.example.com/ExternalLogin/Callback");


static QByteArray ToBase64Url(const QByteArray& data)
{
	return data.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


// CMockIdentityProvider

bool CMockIdentityProvider::Start(quint16 port)
{
	QObject::connect(&m_server, &QTcpServer::newConnection, [this](){ OnNewConnection(); });

	return m_server.listen(QHostAddress::LocalHost, port);
}


void CMockIdentityProvider::AddGrant(const QByteArray& code, const Grant& grant)
{
	m_grants.insert(code, grant);
}


int CMockIdentityProvider::GetRejectedTokenRequestCount() const
{
	return m_rejectedTokenRequestCount;
}


void CMockIdentityProvider::OnNewConnection()
{
	while (m_server.hasPendingConnections()){
		QTcpSocket* socketPtr = m_server.nextPendingConnection();
		QObject::connect(socketPtr, &QTcpSocket::readyRead, [this, socketPtr](){ OnReadyRead(socketPtr); });
		QObject::connect(socketPtr, &QTcpSocket::disconnected, socketPtr, &QObject::deleteLater);
		QObject::connect(socketPtr, &QObject::destroyed, [this, socketPtr](){ m_buffers.remove(socketPtr); });
	}
}


void CMockIdentityProvider::OnReadyRead(QTcpSocket* socketPtr)
{
	QByteArray& buffer = m_buffers[socketPtr];
	buffer.append(socketPtr->readAll());

	const int headerEnd = buffer.indexOf("\r\n\r\n");
	if (headerEnd < 0){
		return;
	}

	const QList<QByteArray> headerLines = buffer.left(headerEnd).split('\n');
	const QList<QByteArray> requestLine = headerLines.first().trimmed().split(' ');

	int contentLength = 0;
	QByteArray authorization;
	for (const QByteArray& line : headerLines){
		const int separator = line.indexOf(':');
		if (separator > 0){
			const QByteArray name = line.left(separator).trimmed().toLower();
			const QByteArray value = line.mid(separator + 1).trimmed();
			if (name == "content-length"){
				contentLength = value.toInt();
			}
			else if (name == "authorization"){
				authorization = value;
			}
		}
	}

	const QByteArray body = buffer.mid(headerEnd + 4);
	if (body.size() < contentLength){
		return;
	}

	const QByteArray path = requestLine.value(1);

	int statusCode = 404;
	QByteArray response = QByteArrayLiteral("{}");
	if (requestLine.value(0) == "POST" && path == "/token"){
		response = CreateTokenResponse(body.left(contentLength), statusCode);
	}
	else if (requestLine.value(0) == "GET" && path == "/userinfo"){
		response = CreateUserInfoResponse(authorization, statusCode);
	}

	QByteArray reply = QByteArrayLiteral("HTTP/1.1 ") + QByteArray::number(statusCode) + (statusCode == 200 ? " OK" : " Error") + "\r\n";
	reply += "Content-Type: application/json\r\n";
	reply += "Content-Length: " + QByteArray::number(response.size()) + "\r\n";
	reply += "Connection: close\r\n\r\n";
	reply += response;

	socketPtr->write(reply);
	socketPtr->disconnectFromHost();
}


QByteArray CMockIdentityProvider::CreateTokenResponse(const QByteArray& body, int& statusCode)
{
	const QUrlQuery form(QString::fromUtf8(body));
	auto value = [&form](const QString& key){ return form.queryItemValue(key, QUrl::FullyDecoded); };

	const QByteArray code = value(QStringLiteral("code")).toUtf8();
	const QByteArray codeVerifier = value(QStringLiteral("code_verifier")).toUtf8();

	// A code is single-use, as at a real provider.
	const Grant grant = m_grants.take(code);

	const bool isValid =
				value(QStringLiteral("grant_type")) == QStringLiteral("authorization_code") &&
				value(QStringLiteral("client_id")) == s_clientId &&
				value(QStringLiteral("client_secret")) == s_clientSecret &&
				value(QStringLiteral("redirect_uri")) == s_callbackUrl &&
				!grant.codeChallenge.isEmpty() &&
				ToBase64Url(QCryptographicHash::hash(codeVerifier, QCryptographicHash::Sha256)) == grant.codeChallenge;
	if (!isValid){
		++m_rejectedTokenRequestCount;
		statusCode = 400;

		return QByteArrayLiteral(R"({"error":"invalid_grant","error_description":"The code, the client or the verifier is invalid"})");
	}

	const QByteArray header = ToBase64Url(QByteArrayLiteral(R"({"alg":"RS256","typ":"JWT"})"));
	const QByteArray payload = ToBase64Url(QJsonDocument(grant.idTokenClaims).toJson(QJsonDocument::Compact));
	const QByteArray accessToken = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	m_userInfos.insert(accessToken, grant.userInfo);

	QJsonObject response;
	response.insert(QStringLiteral("access_token"), QString::fromLatin1(accessToken));
	response.insert(QStringLiteral("token_type"), QStringLiteral("Bearer"));
	response.insert(QStringLiteral("expires_in"), 3600);
	response.insert(QStringLiteral("id_token"), QString::fromLatin1(header + '.' + payload + '.' + ToBase64Url("signature")));

	statusCode = 200;

	return QJsonDocument(response).toJson(QJsonDocument::Compact);
}


QByteArray CMockIdentityProvider::CreateUserInfoResponse(const QByteArray& authorization, int& statusCode) const
{
	const QByteArray accessToken = authorization.startsWith("Bearer ") ? authorization.mid(7) : QByteArray();
	if (!m_userInfos.contains(accessToken)){
		statusCode = 401;

		return QByteArrayLiteral(R"({"error":"invalid_token"})");
	}

	statusCode = 200;

	return QJsonDocument(m_userInfos.value(accessToken)).toJson(QJsonDocument::Compact);
}


// CExternalLoginTest

void CExternalLoginTest::initTestCase()
{
	const QString imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	m_registryFile = imtCorePath + QStringLiteral("/Tests/ExternalLoginTest/Partitura/ExternalLoginTest.acc");
	m_configFile = imtCorePath + QStringLiteral("/Config/ImtCoreTest.awc");

	m_compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	QVERIFY2(m_compositePtr.IsValid(), "Failed to create component accessor");

	m_controllerPtr = m_compositePtr->GetComponentInterface<imtauth::IExternalLoginController>();
	QVERIFY2(m_controllerPtr != nullptr, "ExternalLoginController is not available");

	m_registeringControllerPtr = m_compositePtr->GetComponentInterface<imtauth::IExternalLoginController>("RegisteringLoginController");
	QVERIFY2(m_registeringControllerPtr != nullptr, "RegisteringLoginController is not available");

	m_identityManagerPtr = m_compositePtr->GetComponentInterface<imtauth::IExternalIdentityManager>("ExternalIdentityManager");
	QVERIFY2(m_identityManagerPtr != nullptr, "ExternalIdentityManager is not available");

	m_userCollectionPtr = m_compositePtr->GetComponentInterface<imtbase::IObjectCollection>("UserCollection");
	QVERIFY2(m_userCollectionPtr != nullptr, "UserCollection is not available");

	QVERIFY2(m_identityProvider.Start(s_providerPort), "The mock identity provider cannot listen on its port");

	AddUser("alice", QStringLiteral("Alice@Example.com"), true, true);
	AddUser("bob", QStringLiteral("bob@example.com"), false, true);
	AddUser("carol", QStringLiteral("carol@example.com"), true, false);
}


void CExternalLoginTest::cleanupTestCase()
{
	// Every code exchange of the tests must carry valid client credentials, redirect URI and PKCE verifier.
	QCOMPARE(m_identityProvider.GetRejectedTokenRequestCount(), 0);

	m_registeringControllerPtr = nullptr;
	m_controllerPtr = nullptr;
	m_identityManagerPtr = nullptr;
	m_userCollectionPtr = nullptr;
	m_compositePtr.SetPtr(nullptr);
}


void CExternalLoginTest::testOnlyConfiguredProvidersAreOffered()
{
	const imtauth::IExternalLoginController::Providers providers = m_controllerPtr->GetProviders();

	QCOMPARE(providers.count(), 1);
	QCOMPARE(providers.first().id, s_providerId);
	QCOMPARE(providers.first().name, QStringLiteral("Test IdP"));
	QCOMPARE(providers.first().type, QByteArrayLiteral("Generic"));
}


void CExternalLoginTest::testAuthorizationRequest()
{
	QByteArray loginId;
	QString authorizationUrl;
	QString errorMessage;
	QVERIFY(m_controllerPtr->BeginLogin(s_providerId, "product", QByteArray(), loginId, authorizationUrl, errorMessage));

	const QUrl url(authorizationUrl);
	QCOMPARE(url.toString(QUrl::RemoveQuery), QStringLiteral("http://127.0.0.1:18931/authorize"));

	const QUrlQuery query(url);
	QCOMPARE(query.queryItemValue(QStringLiteral("response_type")), QStringLiteral("code"));
	QCOMPARE(query.queryItemValue(QStringLiteral("client_id")), s_clientId);
	QCOMPARE(query.queryItemValue(QStringLiteral("redirect_uri"), QUrl::FullyDecoded), s_callbackUrl);
	QCOMPARE(query.queryItemValue(QStringLiteral("scope"), QUrl::FullyDecoded), QStringLiteral("openid email profile"));
	QCOMPARE(query.queryItemValue(QStringLiteral("code_challenge_method")), QStringLiteral("S256"));
	QCOMPARE(query.queryItemValue(QStringLiteral("code_challenge")).size(), 43);
	QVERIFY(query.queryItemValue(QStringLiteral("state")).size() >= 43);
	QVERIFY(query.queryItemValue(QStringLiteral("nonce")).size() >= 43);

	// The login ID is the client's secret and never goes to the provider.
	QVERIFY(loginId.size() >= 43);
	QVERIFY(!authorizationUrl.contains(QString::fromLatin1(loginId)));
}


void CExternalLoginTest::testUnknownProviderIsRejected()
{
	QByteArray loginId;
	QString authorizationUrl;
	QString errorMessage;

	QVERIFY(!m_controllerPtr->BeginLogin("unknown", QByteArray(), QByteArray(), loginId, authorizationUrl, errorMessage));
	QVERIFY(!errorMessage.isEmpty());

	// A provider without client ID is not usable either.
	errorMessage.clear();
	QVERIFY(!m_controllerPtr->BeginLogin("google", QByteArray(), QByteArray(), loginId, authorizationUrl, errorMessage));
	QVERIFY(!errorMessage.isEmpty());
}


void CExternalLoginTest::testResultIsPendingUntilCallback()
{
	const StartedLogin login = StartLogin();

	QCOMPARE(m_controllerPtr->TakeLoginResult(login.loginId).status, imtauth::IExternalLoginController::LS_PENDING);
	QCOMPARE(m_controllerPtr->TakeLoginResult(login.loginId).status, imtauth::IExternalLoginController::LS_PENDING);
	QCOMPARE(m_controllerPtr->TakeLoginResult("unknown-login").status, imtauth::IExternalLoginController::LS_UNKNOWN);

	// The state is not a login ID.
	QCOMPARE(m_controllerPtr->TakeLoginResult(login.query.queryItemValue(QStringLiteral("state")).toLatin1()).status, imtauth::IExternalLoginController::LS_UNKNOWN);
}


void CExternalLoginTest::testFirstSignInLinksUserWithVerifiedEmail()
{
	const StartedLogin login = StartLogin();
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "alice-sub", QStringLiteral("alice@example.com"), true));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_SIGNED_IN);
	QCOMPARE(result.userId, QByteArrayLiteral("alice"));
	QCOMPARE(result.productId, QByteArrayLiteral("product"));
	QVERIFY(IsLinked("alice", "alice-sub"));

	// A finished result is handed out once.
	QCOMPARE(m_controllerPtr->TakeLoginResult(login.loginId).status, imtauth::IExternalLoginController::LS_UNKNOWN);
}


void CExternalLoginTest::testSignInWithLinkedAccount()
{
	const StartedLogin login = StartLogin();
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "alice-sub", QStringLiteral("alice.new@example.org"), false));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_SIGNED_IN);
	QCOMPARE(result.userId, QByteArrayLiteral("alice"));

	imtauth::IExternalIdentitySharedPtr identityPtr = m_identityManagerPtr->FindIdentity(s_providerId, "alice-sub");
	QVERIFY(identityPtr.IsValid());
	QCOMPARE(identityPtr->GetEmail(), QStringLiteral("alice.new@example.org"));
}


void CExternalLoginTest::testStateIsSingleUse()
{
	const StartedLogin login = StartLogin();
	const QJsonObject claims = CreateClaims(login, "alice-sub", QStringLiteral("alice@example.com"), true);
	QCOMPARE(FinishLogin(login, claims).status, imtauth::IExternalLoginController::LS_SIGNED_IN);

	QString message;
	const QByteArray replayedCode = GrantCode(login, claims);
	QVERIFY(!m_controllerPtr->HandleCallback(login.query.queryItemValue(QStringLiteral("state")).toLatin1(), replayedCode, QString(), message));
	QVERIFY(!message.isEmpty());
}


void CExternalLoginTest::testNonceMismatchIsRejected()
{
	const StartedLogin login = StartLogin();
	QJsonObject claims = CreateClaims(login, "alice-sub", QStringLiteral("alice@example.com"), true);
	claims.insert(QStringLiteral("nonce"), QStringLiteral("nonce-of-another-login"));

	QCOMPARE(FinishLogin(login, claims).status, imtauth::IExternalLoginController::LS_FAILED);
}


void CExternalLoginTest::testForeignAudienceIsRejected()
{
	const StartedLogin login = StartLogin();
	QJsonObject claims = CreateClaims(login, "alice-sub", QStringLiteral("alice@example.com"), true);
	claims.insert(QStringLiteral("aud"), QStringLiteral("another-client"));

	QCOMPARE(FinishLogin(login, claims).status, imtauth::IExternalLoginController::LS_FAILED);
}


void CExternalLoginTest::testForeignIssuerIsRejected()
{
	const StartedLogin login = StartLogin();
	QJsonObject claims = CreateClaims(login, "alice-sub", QStringLiteral("alice@example.com"), true);
	claims.insert(QStringLiteral("iss"), QStringLiteral("https://issuer.example.net"));

	QCOMPARE(FinishLogin(login, claims).status, imtauth::IExternalLoginController::LS_FAILED);
}


void CExternalLoginTest::testExpiredIdTokenIsRejected()
{
	const StartedLogin login = StartLogin();
	QJsonObject claims = CreateClaims(login, "alice-sub", QStringLiteral("alice@example.com"), true);
	claims.insert(QStringLiteral("exp"), QDateTime::currentSecsSinceEpoch() - 3600);

	QCOMPARE(FinishLogin(login, claims).status, imtauth::IExternalLoginController::LS_FAILED);
}


void CExternalLoginTest::testUserInfoOfOtherAccountIsRejected()
{
	const StartedLogin login = StartLogin();
	const QJsonObject claims = CreateClaims(login, "alice-sub", QStringLiteral("alice@example.com"), true);

	QJsonObject userInfo;
	userInfo.insert(QStringLiteral("sub"), QStringLiteral("someone-else"));

	QCOMPARE(FinishLogin(login, claims, userInfo).status, imtauth::IExternalLoginController::LS_FAILED);
}


void CExternalLoginTest::testUnverifiedEmailIsNotLinked()
{
	const StartedLogin login = StartLogin();
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "unverified-sub", QStringLiteral("bob@example.com"), false));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_FAILED);
	QVERIFY(!m_identityManagerPtr->FindIdentity(s_providerId, "unverified-sub").IsValid());
}


void CExternalLoginTest::testUnknownAccountWithoutRegistration()
{
	const int userCount = m_userCollectionPtr->GetElementIds().count();

	const StartedLogin login = StartLogin();
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "stranger-sub", QStringLiteral("stranger@example.com"), true));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_FAILED);
	QVERIFY(result.message.contains(QStringLiteral("No user is linked")));
	QCOMPARE(m_userCollectionPtr->GetElementIds().count(), userCount);
}


void CExternalLoginTest::testProviderErrorFailsLogin()
{
	const StartedLogin login = StartLogin();

	QString message;
	QVERIFY(!m_controllerPtr->HandleCallback(login.query.queryItemValue(QStringLiteral("state")).toLatin1(), QByteArray(), QStringLiteral("access_denied"), message));
	QVERIFY(message.contains(QStringLiteral("cancelled")));

	const imtauth::IExternalLoginController::LoginResult result = m_controllerPtr->TakeLoginResult(login.loginId);
	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_FAILED);
	QCOMPARE(result.message, message);
}


void CExternalLoginTest::testDisabledUserCannotSignIn()
{
	const StartedLogin login = StartLogin();
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "carol-sub", QStringLiteral("carol@example.com"), true));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_FAILED);
	QVERIFY(result.message.contains(QStringLiteral("deactivated")));
	QVERIFY(result.userId.isEmpty());
}


void CExternalLoginTest::testLinkAccountToSignedInUser()
{
	const StartedLogin login = StartLogin("bob");
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "bob-sub", QStringLiteral("bob.private@example.net"), false));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_LINKED);
	QCOMPARE(result.userId, QByteArrayLiteral("bob"));
	QVERIFY(IsLinked("bob", "bob-sub"));

	const imtauth::IExternalLoginController::LinkedAccounts accounts = m_controllerPtr->GetLinkedAccounts("bob");
	QCOMPARE(accounts.count(), 1);
	QCOMPARE(accounts.first().providerId, s_providerId);
	QCOMPARE(accounts.first().email, QStringLiteral("bob.private@example.net"));
	QVERIFY(accounts.first().linkedAt.isValid());
}


void CExternalLoginTest::testAccountLinkedToOtherUserCannotBeLinked()
{
	const StartedLogin login = StartLogin("alice");
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "bob-sub", QStringLiteral("bob.private@example.net"), false));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_FAILED);
	QVERIFY(IsLinked("bob", "bob-sub"));
}


void CExternalLoginTest::testSecondAccountOfSameProviderCannotBeLinked()
{
	const StartedLogin login = StartLogin("bob");
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "bob-second-sub", QStringLiteral("bob.second@example.net"), true));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_FAILED);
	QVERIFY(!m_identityManagerPtr->FindIdentity(s_providerId, "bob-second-sub").IsValid());
	QCOMPARE(m_controllerPtr->GetLinkedAccounts("bob").count(), 1);
}


void CExternalLoginTest::testLastSignInMethodCannotBeUnlinked()
{
	// Bob has no password, the linked account is his only way to sign in.
	QString errorMessage;
	QVERIFY(!m_controllerPtr->UnlinkAccount("bob", s_providerId, errorMessage));
	QVERIFY(!errorMessage.isEmpty());
	QVERIFY(IsLinked("bob", "bob-sub"));
}


void CExternalLoginTest::testUnlinkAccount()
{
	QString errorMessage;
	QVERIFY(m_controllerPtr->UnlinkAccount("alice", s_providerId, errorMessage));
	QVERIFY(!m_identityManagerPtr->FindIdentity(s_providerId, "alice-sub").IsValid());
	QVERIFY(m_controllerPtr->GetLinkedAccounts("alice").isEmpty());

	QVERIFY(!m_controllerPtr->UnlinkAccount("alice", s_providerId, errorMessage));
}


void CExternalLoginTest::testDeletedUserReleasesAccount()
{
	AddUser("dave", QStringLiteral("dave@example.com"), true, true);

	StartedLogin login = StartLogin();
	QCOMPARE(FinishLogin(login, CreateClaims(login, "dave-sub", QStringLiteral("dave@example.com"), true)).userId, QByteArrayLiteral("dave"));
	QVERIFY(IsLinked("dave", "dave-sub"));

	QVERIFY(m_userCollectionPtr->RemoveElements(imtbase::ICollectionInfo::Ids() << QByteArrayLiteral("dave")));

	login = StartLogin("alice");
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, CreateClaims(login, "dave-sub", QStringLiteral("dave@example.com"), true));

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_LINKED);
	QVERIFY(IsLinked("alice", "dave-sub"));
}


void CExternalLoginTest::testRegistrationCreatesUserWithoutPassword()
{
	const StartedLogin login = StartLogin(QByteArray(), m_registeringControllerPtr);
	QJsonObject claims = CreateClaims(login, "newcomer-sub", QStringLiteral("newcomer@example.com"), true);
	claims.insert(QStringLiteral("name"), QStringLiteral("New Comer"));

	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, claims, QJsonObject(), m_registeringControllerPtr);

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_SIGNED_IN);
	QVERIFY(!result.userId.isEmpty());
	QVERIFY(IsLinked(result.userId, "newcomer-sub"));

	imtbase::IObjectCollection::DataPtr dataPtr;
	QVERIFY(m_userCollectionPtr->GetObjectData(result.userId, dataPtr));
	const imtauth::IUserInfo* userPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	QVERIFY(userPtr != nullptr);
	QCOMPARE(userPtr->GetId(), QByteArrayLiteral("newcomer@example.com"));
	QCOMPARE(userPtr->GetName(), QStringLiteral("New Comer"));
	QCOMPARE(userPtr->GetMail(), QStringLiteral("newcomer@example.com"));
	QVERIFY(userPtr->IsEnabled());

	// No password: the user can sign in with the external account only.
	QVERIFY(userPtr->GetSystemInfos().isEmpty());
}


void CExternalLoginTest::testRegistrationNeedsVerifiedEmail()
{
	const int userCount = m_userCollectionPtr->GetElementIds().count();

	const StartedLogin login = StartLogin(QByteArray(), m_registeringControllerPtr);
	const QJsonObject claims = CreateClaims(login, "unverified-newcomer-sub", QStringLiteral("unverified.newcomer@example.com"), false);

	QCOMPARE(FinishLogin(login, claims, QJsonObject(), m_registeringControllerPtr).status, imtauth::IExternalLoginController::LS_FAILED);
	QCOMPARE(m_userCollectionPtr->GetElementIds().count(), userCount);
}


void CExternalLoginTest::testRegistrationDoesNotTakeUsedLogin()
{
	// The login equals the e-mail of the account, but the user has no e-mail to match.
	AddUser("taken@example.com", QString(), true, true);
	const int userCount = m_userCollectionPtr->GetElementIds().count();

	const StartedLogin login = StartLogin(QByteArray(), m_registeringControllerPtr);
	const QJsonObject claims = CreateClaims(login, "taken-sub", QStringLiteral("taken@example.com"), true);
	const imtauth::IExternalLoginController::LoginResult result = FinishLogin(login, claims, QJsonObject(), m_registeringControllerPtr);

	QCOMPARE(result.status, imtauth::IExternalLoginController::LS_FAILED);
	QVERIFY(result.message.contains(QStringLiteral("already exists")));
	QCOMPARE(m_userCollectionPtr->GetElementIds().count(), userCount);
	QVERIFY(!m_identityManagerPtr->FindIdentity(s_providerId, "taken-sub").IsValid());
}


void CExternalLoginTest::testSettingsOverrideProviderAttributes()
{
	imtauth::IExternalOidcProvider* providerPtr = m_compositePtr->GetComponentInterface<imtauth::IExternalOidcProvider>("SettingsProvider");
	QVERIFY2(providerPtr != nullptr, "SettingsProvider is not available");

	// A non-empty setting wins over the attribute, trimmed.
	QCOMPARE(providerPtr->GetProviderName(), QStringLiteral("Company Login"));
	QCOMPARE(providerPtr->GetAuthorizationEndpoint(), QStringLiteral("https://idp.example.org/authorize"));
	QCOMPARE(providerPtr->GetClientId(), QByteArrayLiteral("settings-client"));
	QCOMPARE(providerPtr->GetIssuers(), QStringList() << QStringLiteral("https://idp.example.org"));

	// An empty setting leaves the attribute in effect.
	QCOMPARE(providerPtr->GetTokenEndpoint(), QStringLiteral("https://attribute.example.org/token"));
	QCOMPARE(providerPtr->GetClientSecret(), QByteArrayLiteral("attribute-secret"));
	QCOMPARE(providerPtr->GetUserInfoEndpoint(), QString());
	QCOMPARE(providerPtr->GetProviderType(), QByteArrayLiteral("Generic"));
}


// private methods

CExternalLoginTest::StartedLogin CExternalLoginTest::StartLogin(const QByteArray& linkUserId, imtauth::IExternalLoginController* controllerPtr)
{
	if (controllerPtr == nullptr){
		controllerPtr = m_controllerPtr;
	}

	StartedLogin retVal;

	QString authorizationUrl;
	QString errorMessage;
	if (!controllerPtr->BeginLogin(s_providerId, "product", linkUserId, retVal.loginId, authorizationUrl, errorMessage)){
		qWarning() << "BeginLogin failed:" << errorMessage;
	}

	retVal.query = QUrlQuery(QUrl(authorizationUrl));

	return retVal;
}


QJsonObject CExternalLoginTest::CreateClaims(const StartedLogin& login, const QString& subject, const QString& email, bool isEmailVerified) const
{
	QJsonObject claims;
	claims.insert(QStringLiteral("iss"), s_issuer);
	claims.insert(QStringLiteral("aud"), s_clientId);
	claims.insert(QStringLiteral("sub"), subject);
	claims.insert(QStringLiteral("email"), email);
	claims.insert(QStringLiteral("email_verified"), isEmailVerified);
	claims.insert(QStringLiteral("iat"), QDateTime::currentSecsSinceEpoch());
	claims.insert(QStringLiteral("exp"), QDateTime::currentSecsSinceEpoch() + 600);
	claims.insert(QStringLiteral("nonce"), login.query.queryItemValue(QStringLiteral("nonce")));

	return claims;
}


QByteArray CExternalLoginTest::GrantCode(const StartedLogin& login, const QJsonObject& claims, const QJsonObject& userInfo)
{
	CMockIdentityProvider::Grant grant;
	grant.codeChallenge = login.query.queryItemValue(QStringLiteral("code_challenge")).toLatin1();
	grant.idTokenClaims = claims;
	grant.userInfo = userInfo;
	if (grant.userInfo.isEmpty()){
		grant.userInfo.insert(QStringLiteral("sub"), claims.value(QStringLiteral("sub")));
	}

	// '/' and '+' are in real codes and must survive the form encoding.
	const QByteArray code = QByteArrayLiteral("4/code+") + QByteArray::number(++m_codeCounter);
	m_identityProvider.AddGrant(code, grant);

	return code;
}


imtauth::IExternalLoginController::LoginResult CExternalLoginTest::FinishLogin(const StartedLogin& login, const QJsonObject& claims, const QJsonObject& userInfo, imtauth::IExternalLoginController* controllerPtr)
{
	if (controllerPtr == nullptr){
		controllerPtr = m_controllerPtr;
	}

	const QByteArray code = GrantCode(login, claims, userInfo);

	QString message;
	controllerPtr->HandleCallback(login.query.queryItemValue(QStringLiteral("state")).toLatin1(), code, QString(), message);

	return controllerPtr->TakeLoginResult(login.loginId);
}


void CExternalLoginTest::AddUser(const QByteArray& userId, const QString& email, bool hasPassword, bool isEnabled)
{
	imtauth::CIdentifiableUserInfo user;
	user.SetObjectUuid(userId);
	user.SetId(userId);
	user.SetName(QString::fromUtf8(userId));
	user.SetMail(email);
	user.SetEnabled(isEnabled);
	if (hasPassword){
		user.AddToSystem(imtauth::IUserInfo::SystemInfo());
	}

	QCOMPARE(m_userCollectionPtr->InsertNewObject("User", QString::fromUtf8(userId), QString(), &user, userId), userId);
}


bool CExternalLoginTest::IsLinked(const QByteArray& userId, const QString& subject) const
{
	imtauth::IExternalIdentitySharedPtr identityPtr = m_identityManagerPtr->FindIdentity(s_providerId, subject);

	return identityPtr.IsValid() && identityPtr->GetUserId() == userId;
}


I_ADD_TEST(CExternalLoginTest);


