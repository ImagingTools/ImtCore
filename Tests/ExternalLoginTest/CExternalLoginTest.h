#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QTcpServer>
#include <QtTest/QtTest>

// ACF includes
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IExternalLoginController.h>
#include <imtauth/IExternalIdentityManager.h>
#include <imtauth/IExternalOidcProvider.h>


/**
	Identity provider answering the token and user info requests of the external login controller.
	It checks the client credentials, the redirect URI and the PKCE verifier of every code exchange.
*/
class CMockIdentityProvider
{
public:
	struct Grant
	{
		QByteArray codeChallenge;
		QJsonObject idTokenClaims;
		QJsonObject userInfo;
	};

	bool Start(quint16 port);
	void AddGrant(const QByteArray& code, const Grant& grant);
	int GetRejectedTokenRequestCount() const;

private:
	void OnNewConnection();
	void OnReadyRead(QTcpSocket* socketPtr);
	QByteArray CreateTokenResponse(const QByteArray& body, int& statusCode);
	QByteArray CreateUserInfoResponse(const QByteArray& authorization, int& statusCode) const;

	QTcpServer m_server;
	QMap<QTcpSocket*, QByteArray> m_buffers;
	QMap<QByteArray, Grant> m_grants;
	QMap<QByteArray, QJsonObject> m_userInfos;
	int m_rejectedTokenRequestCount = 0;
};


class CExternalLoginTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

	void testOnlyConfiguredProvidersAreOffered();
	void testAuthorizationRequest();
	void testUnknownProviderIsRejected();
	void testResultIsPendingUntilCallback();
	void testFirstSignInLinksUserWithVerifiedEmail();
	void testSignInWithLinkedAccount();
	void testStateIsSingleUse();
	void testNonceMismatchIsRejected();
	void testForeignAudienceIsRejected();
	void testForeignIssuerIsRejected();
	void testExpiredIdTokenIsRejected();
	void testUserInfoOfOtherAccountIsRejected();
	void testUnverifiedEmailIsNotLinked();
	void testUnknownAccountWithoutRegistration();
	void testProviderErrorFailsLogin();
	void testDisabledUserCannotSignIn();
	void testLinkAccountToSignedInUser();
	void testAccountLinkedToOtherUserCannotBeLinked();
	void testSecondAccountOfSameProviderCannotBeLinked();
	void testLastSignInMethodCannotBeUnlinked();
	void testUnlinkAccount();
	void testDeletedUserReleasesAccount();
	void testRegistrationCreatesUserWithoutPassword();
	void testRegistrationNeedsVerifiedEmail();
	void testRegistrationDoesNotTakeUsedLogin();
	void testSettingsOverrideProviderAttributes();

private:
	struct StartedLogin
	{
		QByteArray loginId;
		QUrlQuery query;
	};

	StartedLogin StartLogin(const QByteArray& linkUserId = QByteArray(), imtauth::IExternalLoginController* controllerPtr = nullptr);
	QJsonObject CreateClaims(const StartedLogin& login, const QString& subject, const QString& email, bool isEmailVerified) const;
	QByteArray GrantCode(const StartedLogin& login, const QJsonObject& claims, const QJsonObject& userInfo = QJsonObject());
	imtauth::IExternalLoginController::LoginResult FinishLogin(const StartedLogin& login, const QJsonObject& claims, const QJsonObject& userInfo = QJsonObject(), imtauth::IExternalLoginController* controllerPtr = nullptr);
	void AddUser(const QByteArray& userId, const QString& email, bool hasPassword, bool isEnabled);
	bool IsLinked(const QByteArray& userId, const QString& subject) const;

	QString m_registryFile;
	QString m_configFile;
	istd::TDelPtr<ipackage::CComponentAccessor> m_compositePtr;
	imtauth::IExternalLoginController* m_controllerPtr = nullptr;
	imtauth::IExternalLoginController* m_registeringControllerPtr = nullptr;
	imtauth::IExternalIdentityManager* m_identityManagerPtr = nullptr;
	imtbase::IObjectCollection* m_userCollectionPtr = nullptr;
	CMockIdentityProvider m_identityProvider;
	int m_codeCounter = 0;
};


