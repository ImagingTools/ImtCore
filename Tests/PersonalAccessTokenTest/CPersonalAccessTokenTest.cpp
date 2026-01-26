#include "CPersonalAccessTokenTest.h"


// Qt includes
#include <QtCore/QDebug>


namespace imtauthtest
{


void CPersonalAccessTokenTest::initTestCase()
{
	// Paths to test component accessor and config
	m_imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	m_registryFile = m_imtCorePath + QString("/Tests/PersonalAccessTokenTest/Partitura/PersonalAccessTokenTest.acc");
	m_configFile = m_imtCorePath + "/Config/ImtCoreTest.awc";
	
	// Initialize component accessor
	m_compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	QVERIFY2(m_compositePtr.IsValid(), "Failed to create component accessor");
	
	// Get token manager component
	m_tokenManagerPtr = m_compositePtr->GetComponentInterface<imtauth::IPersonalAccessTokenManager>();
	QVERIFY2(m_tokenManagerPtr != nullptr, "Failed to get PersonalAccessTokenManager component");
}


void CPersonalAccessTokenTest::cleanupTestCase()
{
	m_tokenManagerPtr = nullptr;
	m_compositePtr.SetPtr(nullptr);
}


void CPersonalAccessTokenTest::testTokenCreation()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create a token
	QByteArray userId = "test_user_123";
	QString name = "Test Token";
	QString description = "A test token for unit testing";
	QByteArrayList scopes;
	scopes << "read:users" << "write:data";
	QDateTime expiresAt = QDateTime::currentDateTime().addDays(30);
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, description, scopes, expiresAt);
	
	QVERIFY2(result.success, "Failed to create token");
	QVERIFY2(!result.tokenId.isEmpty(), "Token ID is empty");
	QVERIFY2(!result.rawToken.isEmpty(), "Raw token is empty");
	QVERIFY2(result.rawToken.startsWith("imt_pat_"), "Token doesn't have expected prefix");
	
	// Verify we can retrieve the token
	imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerPtr->GetToken(result.tokenId);
	QVERIFY2(tokenPtr != nullptr, "Failed to retrieve created token");
	QCOMPARE(tokenPtr->GetUserId(), userId);
	QCOMPARE(tokenPtr->GetName(), name);
}


void CPersonalAccessTokenTest::testTokenValidation()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create a token
	QByteArray userId = "test_user_456";
	QString name = "Validation Test Token";
	QByteArrayList scopes;
	scopes << "read:api";
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, "", scopes, QDateTime());
	
	QVERIFY2(result.success, "Failed to create token for validation test");
	
	// Validate the token
	QByteArray validatedUserId;
	QByteArrayList validatedScopes;
	
	bool isValid = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedScopes);
	QVERIFY2(isValid, "Token validation failed");
	QCOMPARE(validatedUserId, userId);
	QCOMPARE(validatedScopes, scopes);
	
	// Test with invalid token
	QByteArray invalidToken = "imt_pat_invalid_token_12345";
	bool shouldBeFalse = m_tokenManagerPtr->ValidateToken(invalidToken, validatedUserId, validatedScopes);
	QVERIFY2(!shouldBeFalse, "Invalid token was validated as valid");
}


void CPersonalAccessTokenTest::testTokenRevocation()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create a token
	QByteArray userId = "test_user_789";
	QString name = "Revocation Test Token";
	QByteArrayList scopes;
	scopes << "write:api";
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, "", scopes, QDateTime());
	
	QVERIFY2(result.success, "Failed to create token for revocation test");
	
	// Verify token is valid
	QByteArray validatedUserId;
	QByteArrayList validatedScopes;
	
	bool isValid = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedScopes);
	QVERIFY2(isValid, "Token should be valid before revocation");
	
	// Revoke the token
	bool revoked = m_tokenManagerPtr->RevokeToken(result.tokenId);
	QVERIFY2(revoked, "Failed to revoke token");
	
	// Verify token is no longer valid
	bool shouldBeInvalid = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedScopes);
	QVERIFY2(!shouldBeInvalid, "Revoked token should not be valid");
}


void CPersonalAccessTokenTest::testTokenExpiration()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create an expired token
	QByteArray userId = "test_user_expired";
	QString name = "Expired Token";
	QByteArrayList scopes;
	scopes << "read:api";
	QDateTime expiresAt = QDateTime::currentDateTime().addSecs(-3600); // Expired 1 hour ago
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, "", scopes, expiresAt);
	
	QVERIFY2(result.success, "Failed to create token for expiration test");
	
	// Verify token is not valid (expired)
	QByteArray validatedUserId;
	QByteArrayList validatedScopes;
	
	bool shouldBeExpired = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedScopes);
	QVERIFY2(!shouldBeExpired, "Expired token should not be valid");
	
	// Create a token that hasn't expired yet
	QDateTime futureExpiry = QDateTime::currentDateTime().addDays(7);
	imtauth::IPersonalAccessTokenManager::TokenCreationResult validResult = 
		m_tokenManagerPtr->CreateToken(userId, "Valid Token", "", scopes, futureExpiry);
	
	QVERIFY2(validResult.success, "Failed to create non-expired token");
	
	// Verify this token is valid
	bool isValid = m_tokenManagerPtr->ValidateToken(validResult.rawToken, validatedUserId, validatedScopes);
	QVERIFY2(isValid, "Non-expired token should be valid");
}


} // namespace imtauthtest


QTEST_MAIN(imtauthtest::CPersonalAccessTokenTest)
