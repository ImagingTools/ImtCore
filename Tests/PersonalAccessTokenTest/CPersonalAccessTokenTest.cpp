#include "CPersonalAccessTokenTest.h"


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QThread>

// ACF includes
#include <itest/CStandardTestExecutor.h>


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
	QDateTime expiresAt = QDateTime::currentDateTimeUtc().addDays(30);
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, description, scopes, expiresAt);
	
	QVERIFY2(result.success, "Failed to create token");
	QVERIFY2(!result.tokenId.isEmpty(), "Token ID is empty");
	QVERIFY2(!result.rawToken.isEmpty(), "Raw token is empty");
	QVERIFY2(result.rawToken.startsWith("imt_pat_"), "Token doesn't have expected prefix");
	
	// Verify we can retrieve the token
	imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerPtr->GetToken(result.tokenId);
	QVERIFY2(tokenPtr.IsValid(), "Failed to retrieve created token");
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
	QByteArray validatedTokenId;
	QByteArrayList validatedScopes;
	
	bool isValid = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedTokenId, validatedScopes);
	QVERIFY2(isValid, "Token validation failed");
	QCOMPARE(validatedUserId, userId);
	QCOMPARE(validatedScopes, scopes);
	
	// Test with invalid token
	QByteArray invalidToken = "imt_pat_invalid_token_12345";
	bool shouldBeFalse = m_tokenManagerPtr->ValidateToken(invalidToken, validatedUserId, validatedTokenId, validatedScopes);
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
	QByteArray validatedTokenId;
	QByteArrayList validatedScopes;
	
	bool isValid = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedTokenId, validatedScopes);
	QVERIFY2(isValid, "Token should be valid before revocation");
	
	// Revoke the token
	bool revoked = m_tokenManagerPtr->RevokeToken(result.tokenId);
	QVERIFY2(revoked, "Failed to revoke token");
	
	// Verify token is no longer valid
	bool shouldBeInvalid = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedTokenId, validatedScopes);
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
	QDateTime expiresAt = QDateTime::currentDateTimeUtc().addSecs(-3600); // Expired 1 hour ago
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, "", scopes, expiresAt);
	
	QVERIFY2(result.success, "Failed to create token for expiration test");
	
	// Verify token is not valid (expired)
	QByteArray validatedUserId;
	QByteArray validatedTokenId;
	QByteArrayList validatedScopes;
	
	bool shouldBeExpired = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedTokenId, validatedScopes);
	QVERIFY2(!shouldBeExpired, "Expired token should not be valid");
	
	// Create a token that hasn't expired yet
	QDateTime futureExpiry = QDateTime::currentDateTimeUtc().addDays(7);
	imtauth::IPersonalAccessTokenManager::TokenCreationResult validResult = 
		m_tokenManagerPtr->CreateToken(userId, "Valid Token", "", scopes, futureExpiry);
	
	QVERIFY2(validResult.success, "Failed to create non-expired token");
	
	// Verify this token is valid
	bool isValid = m_tokenManagerPtr->ValidateToken(validResult.rawToken, validatedUserId, validatedTokenId, validatedScopes);
	QVERIFY2(isValid, "Non-expired token should be valid");
}


void CPersonalAccessTokenTest::testGetTokenIds()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create multiple tokens for the same user
	QByteArray userId = "test_user_multi";
	QString name1 = "Token 1";
	QString name2 = "Token 2";
	QString name3 = "Token 3";
	QByteArrayList scopes;
	scopes << "read:api";
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result1 = 
		m_tokenManagerPtr->CreateToken(userId, name1, "", scopes, QDateTime());
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result2 = 
		m_tokenManagerPtr->CreateToken(userId, name2, "", scopes, QDateTime());
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result3 = 
		m_tokenManagerPtr->CreateToken(userId, name3, "", scopes, QDateTime());
	
	QVERIFY2(result1.success, "Failed to create token 1");
	QVERIFY2(result2.success, "Failed to create token 2");
	QVERIFY2(result3.success, "Failed to create token 3");
	
	// Get all token IDs for this user
	QByteArrayList tokenIds = m_tokenManagerPtr->GetTokenIds(userId);
	
	// Should have at least 3 tokens (may have more from previous tests)
	QVERIFY2(tokenIds.size() >= 3, "Should have at least 3 tokens for user");
	
	// Verify that all three token IDs are in the list
	QVERIFY2(tokenIds.contains(result1.tokenId), "Token 1 ID should be in the list");
	QVERIFY2(tokenIds.contains(result2.tokenId), "Token 2 ID should be in the list");
	QVERIFY2(tokenIds.contains(result3.tokenId), "Token 3 ID should be in the list");
	
	// Test with a user that has no tokens
	QByteArrayList emptyList = m_tokenManagerPtr->GetTokenIds("nonexistent_user");
	QVERIFY2(emptyList.isEmpty(), "Nonexistent user should have no tokens");
}


void CPersonalAccessTokenTest::testUpdateLastUsedAt()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create a token
	QByteArray userId = "test_user_lastused";
	QString name = "Last Used Test Token";
	QByteArrayList scopes;
	scopes << "read:api";
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, "", scopes, QDateTime());
	
	QVERIFY2(result.success, "Failed to create token for last used test");
	
	// Get the initial token
	imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerPtr->GetToken(result.tokenId);
	QVERIFY2(tokenPtr.IsValid(), "Failed to retrieve token");
	
	QDateTime initialLastUsed = tokenPtr->GetLastUsedAt();
	
	// Wait a bit to ensure timestamp will be different
	QThread::msleep(100);
	
	// Update the last used timestamp
	bool updated = m_tokenManagerPtr->UpdateLastUsedAt(result.tokenId);
	QVERIFY2(updated, "Failed to update last used timestamp");
	
	// Get the token again and verify timestamp changed
	tokenPtr = m_tokenManagerPtr->GetToken(result.tokenId);
	QVERIFY2(tokenPtr.IsValid(), "Failed to retrieve token after update");
	
	QDateTime newLastUsed = tokenPtr->GetLastUsedAt();
	QVERIFY2(newLastUsed.isValid(), "Last used timestamp should be valid");
	
	if (initialLastUsed.isValid()) {
		QVERIFY2(newLastUsed > initialLastUsed, "Last used timestamp should be updated");
	}
	
	// Test updating nonexistent token
	bool shouldFail = m_tokenManagerPtr->UpdateLastUsedAt("nonexistent_token_id");
	QVERIFY2(!shouldFail, "Updating nonexistent token should fail");
}


void CPersonalAccessTokenTest::testDeleteToken()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create a token
	QByteArray userId = "test_user_delete";
	QString name = "Delete Test Token";
	QByteArrayList scopes;
	scopes << "read:api";
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, name, "", scopes, QDateTime());
	
	QVERIFY2(result.success, "Failed to create token for delete test");
	
	// Verify token exists
	imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerPtr->GetToken(result.tokenId);
	QVERIFY2(tokenPtr.IsValid(), "Token should exist before deletion");
	
	// Verify token is in user's token list
	QByteArrayList tokenIds = m_tokenManagerPtr->GetTokenIds(userId);
	QVERIFY2(tokenIds.contains(result.tokenId), "Token should be in user's token list");
	
	// Delete the token
	bool deleted = m_tokenManagerPtr->DeleteToken(result.tokenId);
	QVERIFY2(deleted, "Failed to delete token");
	
	// Verify token no longer exists
	tokenPtr = m_tokenManagerPtr->GetToken(result.tokenId);
	QVERIFY2(!tokenPtr.IsValid(), "Token should not exist after deletion");
	
	// Verify token is no longer in user's token list
	tokenIds = m_tokenManagerPtr->GetTokenIds(userId);
	QVERIFY2(!tokenIds.contains(result.tokenId), "Token should not be in user's token list after deletion");
	
	// Verify token cannot be validated
	QByteArray validatedUserId;
	QByteArray validatedTokenId;
	QByteArrayList validatedScopes;
	bool shouldBeInvalid = m_tokenManagerPtr->ValidateToken(result.rawToken, validatedUserId, validatedTokenId, validatedScopes);
	QVERIFY2(!shouldBeInvalid, "Deleted token should not be valid");
	
	// Test deleting nonexistent token
	bool shouldFail = m_tokenManagerPtr->DeleteToken("nonexistent_token_id");
	QVERIFY2(!shouldFail, "Deleting nonexistent token should fail");
}


I_ADD_TEST(CPersonalAccessTokenTest);


