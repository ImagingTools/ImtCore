#include "CPersonalAccessTokenTest.h"


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QFile>
#include <QtCore/QUuid>

// ACF includes
#include <itest/CStandardTestExecutor.h>
#include <icomp/TSimComponentWrap.h>

#include <imtauthdb/CPersonalAccessTokenSqlPersistence.h>
#include <imtauthdb/CPersonalAccessTokenDatabaseDelegateComp.h>


namespace
{


class CTokenDatabaseEngine: public imtdb::IDatabaseEngine
{
public:
	explicit CTokenDatabaseEngine(const QString& driver = QStringLiteral("QSQLITE"))
		:m_database(QSqlDatabase::addDatabase(driver, QUuid::createUuid().toString()))
	{
		if (driver == "QPSQL"){
			m_database.setHostName("127.0.0.1");
			m_database.setPort(qEnvironmentVariableIntValue("IMT_TEST_POSTGRES_PORT"));
			m_database.setUserName("postgres");
			m_database.setDatabaseName("postgres");
		}
		else{
			m_database.setDatabaseName(":memory:");
		}
		m_database.open();
		if (driver == "QPSQL" && m_database.isOpen()){
			m_schema = "token_test_" + QUuid::createUuid().toString(QUuid::Id128).toUtf8();
			ExecSqlQuery("CREATE SCHEMA " + m_schema);
			ExecSqlQuery("SET search_path TO " + m_schema);
		}
	}

	~CTokenDatabaseEngine()
	{
		if (!m_schema.isEmpty()){
			ExecSqlQuery("DROP SCHEMA " + m_schema + " CASCADE");
		}
		const QString name = m_database.connectionName();
		m_database.close();
		m_database = QSqlDatabase();
		QSqlDatabase::removeDatabase(name);
	}

	bool BeginTransaction() const override { return m_database.transaction(); }
	bool FinishTransaction() const override { return m_database.commit(); }
	bool CancelTransaction() const override { return m_database.rollback(); }
	QByteArray GetDatabaseDriverId() const override { return m_database.driverName().toUtf8(); }

	QSqlQuery ExecSqlQuery(const QByteArray& sql, QSqlError* error = nullptr, bool forward = false) const override
	{
		QSqlQuery query(m_database);
		query.setForwardOnly(forward);
		query.exec(QString::fromUtf8(sql));
		if (error != nullptr){
			*error = query.lastError();
		}
		return query;
	}

	QSqlQuery ExecSqlQuery(const QByteArray& sql, const QVariantMap& values, QSqlError* error = nullptr, bool forward = false) const override
	{
		QSqlQuery query(m_database);
		query.setForwardOnly(forward);
		if (query.prepare(QString::fromUtf8(sql))){
			for (auto it = values.cbegin(); it != values.cend(); ++it){
				query.bindValue(it.key(), it.value());
			}
			query.exec();
		}
		if (error != nullptr){
			*error = query.lastError();
		}
		return query;
	}

	QSqlQuery ExecSqlQueryFromFile(const QString& path, QSqlError* error = nullptr, bool forward = false) const override
	{
		return ExecSqlQueryFromFile(path, QVariantMap(), error, forward);
	}

	QSqlQuery ExecSqlQueryFromFile(const QString& path, const QVariantMap& values, QSqlError* error = nullptr, bool forward = false) const override
	{
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly)){
			if (error != nullptr){
				*error = QSqlError(file.errorString(), QString(), QSqlError::StatementError);
			}
			return QSqlQuery(m_database);
		}
		return ExecSqlQuery(file.readAll(), values, error, forward);
	}

private:
	mutable QSqlDatabase m_database;
	QByteArray m_schema;
};


const QByteArray s_legacyTokenSchema =
		"CREATE TABLE \"PersonalAccessTokens\" (\"Id\" TEXT PRIMARY KEY, \"UserId\" TEXT NOT NULL, "
		"\"TokenHash\" TEXT, \"Scopes\" TEXT, \"LastUsedAt\" TEXT, \"Revoked\" BOOLEAN DEFAULT FALSE)";


class CTokenDatabaseEngineComp: public icomp::CComponentBase, public CTokenDatabaseEngine
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CTokenDatabaseEngineComp)
		I_REGISTER_INTERFACE(imtdb::IDatabaseEngine);
	I_END_COMPONENT
};


} // namespace


void CPersonalAccessTokenTest::testSqlSchemaMigration()
{
	CTokenDatabaseEngine engine;
	QVERIFY(engine.ExecSqlQuery(s_legacyTokenSchema).isActive());
	QVERIFY(engine.ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" VALUES ('one','owner','hash','read',NULL,FALSE)").isActive());
	QVERIFY(engine.ExecSqlQuery("CREATE INDEX \"CustomOwnerIndex\" ON \"PersonalAccessTokens\" (\"UserId\")").isActive());
	QVERIFY(engine.ExecSqlQuery("CREATE TABLE \"Audit\" (\"Id\" TEXT)").isActive());
	QVERIFY(engine.ExecSqlQuery("CREATE TRIGGER \"TokenAudit\" AFTER UPDATE ON \"PersonalAccessTokens\" "
				"BEGIN INSERT INTO \"Audit\" VALUES (new.\"Id\"); END").isActive());
	imtauthdb::CPersonalAccessTokenSqlPersistence persistence(engine);
	QVERIFY2(persistence.EnsureSchema(), qPrintable(persistence.GetErrorMessage()));
	QVERIFY2(persistence.EnsureSchema(), qPrintable(persistence.GetErrorMessage()));
	QVERIFY(engine.ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" (\"Id\",\"TokenHash\",\"ClientProductId\") "
				"VALUES ('server','server-hash','client')").isActive());
	QSqlQuery row = engine.ExecSqlQuery("SELECT \"UserId\",\"Scopes\",\"TokenHash\" FROM \"PersonalAccessTokens\" WHERE \"Id\"='one'");
	QVERIFY(row.next());
	QCOMPARE(row.value(0).toString(), QString("owner"));
	QCOMPARE(row.value(1).toString(), QString("read"));
	QCOMPARE(row.value(2).toString(), QString("hash"));
	row.finish();
	QVERIFY(!engine.ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" (\"Id\",\"TokenHash\") VALUES ('duplicate','hash')").isActive());
	QVERIFY(persistence.RevokeToken("one"));
	row = engine.ExecSqlQuery("SELECT count(*) FROM \"Audit\"");
	QVERIFY(row.next());
	QCOMPARE(row.value(0).toInt(), 1);
	row.finish();
	row = engine.ExecSqlQuery("SELECT count(*) FROM sqlite_master WHERE name='CustomOwnerIndex'");
	QVERIFY(row.next());
	QCOMPARE(row.value(0).toInt(), 1);
}


void CPersonalAccessTokenTest::testSqlMigrationRollback()
{
	CTokenDatabaseEngine engine;
	QVERIFY(engine.ExecSqlQuery(s_legacyTokenSchema).isActive());
	QVERIFY(engine.ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" VALUES ('one','owner','same','read',NULL,FALSE)").isActive());
	QVERIFY(engine.ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" VALUES ('two','owner','same','write',NULL,FALSE)").isActive());
	imtauthdb::CPersonalAccessTokenSqlPersistence persistence(engine);
	QVERIFY(!persistence.EnsureSchema());
	QVERIFY(!persistence.GetErrorMessage().isEmpty());
	QSqlQuery row = engine.ExecSqlQuery("SELECT count(*) FROM \"PersonalAccessTokens\"");
	QVERIFY(row.next());
	QCOMPARE(row.value(0).toInt(), 2);
	row.finish();
	row = engine.ExecSqlQuery("SELECT count(*) FROM pragma_table_info('PersonalAccessTokens') WHERE name='ClientProductId'");
	QVERIFY(row.next());
	QCOMPARE(row.value(0).toInt(), 0);
	row.finish();
	QVERIFY(engine.ExecSqlQuery("DELETE FROM \"PersonalAccessTokens\" WHERE \"Id\"='two'").isActive());
	QVERIFY(engine.ExecSqlQuery("PRAGMA foreign_keys=ON").isActive());
	QVERIFY(engine.ExecSqlQuery("CREATE TABLE \"Child\" (\"TokenId\" TEXT REFERENCES \"PersonalAccessTokens\" (\"Id\") ON DELETE CASCADE)").isActive());
	QVERIFY(engine.ExecSqlQuery("INSERT INTO \"Child\" VALUES ('one')").isActive());
	QVERIFY(!persistence.EnsureSchema());
	QVERIFY(persistence.GetErrorMessage().contains("foreign keys"));
	row = engine.ExecSqlQuery("SELECT count(*) FROM \"Child\"");
	QVERIFY(row.next());
	QCOMPARE(row.value(0).toInt(), 1);
}


void CPersonalAccessTokenTest::testSqlAtomicMutations()
{
	CTokenDatabaseEngine engine;
	QVERIFY(engine.ExecSqlQuery(s_legacyTokenSchema).isActive());
	imtauthdb::CPersonalAccessTokenSqlPersistence persistence(engine);
	QVERIFY2(persistence.EnsureSchema(), qPrintable(persistence.GetErrorMessage()));
	QVERIFY(engine.ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" (\"Id\",\"UserId\",\"TokenHash\",\"Scopes\") "
				"VALUES ('one','owner','hash','read,write')").isActive());
	const QDateTime now = QDateTime::currentDateTimeUtc();
	QVERIFY(persistence.SetTokenScopes("one", {"read"}));
	QVERIFY(persistence.UpdateLastUsedAt("one", now));
	QVERIFY(persistence.RevokeToken("one"));
	QVERIFY(persistence.UpdateLastUsedAt("one", now.addSecs(-60)));
	QVERIFY(!persistence.SetTokenScopes("one", {"write"}));
	QVERIFY(!persistence.RevokeToken("missing"));
	QVERIFY(!persistence.UpdateLastUsedAt("one", QDateTime()));
	QSqlQuery row = engine.ExecSqlQuery("SELECT \"Scopes\",\"Revoked\",\"LastUsedAt\" FROM \"PersonalAccessTokens\" WHERE \"Id\"='one'");
	QVERIFY(row.next());
	QCOMPARE(row.value(0).toString(), QString("read"));
	QVERIFY(row.value(1).toBool());
	QCOMPARE(row.value(2).toString(), now.toString(Qt::ISODate));
}


void CPersonalAccessTokenTest::testPostgresAtomicMigration()
{
	if (qEnvironmentVariableIntValue("IMT_TEST_POSTGRES_PORT") <= 0){
		QSKIP("Set IMT_TEST_POSTGRES_PORT to an isolated local test cluster");
	}
	CTokenDatabaseEngine engine(QStringLiteral("QPSQL"));
	QSqlQuery query = engine.ExecSqlQuery("CREATE TABLE \"PersonalAccessTokens\" (\"Id\" UUID PRIMARY KEY, \"UserId\" UUID NOT NULL, "
				"\"TokenHash\" TEXT, \"Scopes\" TEXT, \"LastUsedAt\" TIMESTAMP WITH TIME ZONE, \"Revoked\" BOOLEAN DEFAULT FALSE)");
	QVERIFY2(query.isActive(), qPrintable(query.lastError().text()));
	query.finish();
	imtauthdb::CPersonalAccessTokenSqlPersistence persistence(engine);
	QVERIFY2(persistence.EnsureSchema(), qPrintable(persistence.GetErrorMessage()));
	QVERIFY2(persistence.EnsureSchema(), qPrintable(persistence.GetErrorMessage()));
	const QByteArray id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	query = engine.ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" (\"Id\",\"TokenHash\",\"Scopes\",\"ClientProductId\") "
				"VALUES (:id,'hash','read,write','client')", {{":id", QString::fromUtf8(id)}});
	QVERIFY2(query.isActive(), qPrintable(query.lastError().text()));
	query.finish();
	const QDateTime now = QDateTime::fromSecsSinceEpoch(QDateTime::currentSecsSinceEpoch(), Qt::UTC);
	QVERIFY2(persistence.SetTokenScopes(id, {"read"}), qPrintable(persistence.GetErrorMessage()));
	QVERIFY2(persistence.UpdateLastUsedAt(id, now), qPrintable(persistence.GetErrorMessage()));
	QVERIFY2(persistence.RevokeToken(id), qPrintable(persistence.GetErrorMessage()));
	QVERIFY2(persistence.UpdateLastUsedAt(id, now.addSecs(-60)), qPrintable(persistence.GetErrorMessage()));
	QVERIFY(!persistence.SetTokenScopes(id, {"write"}));
	query = engine.ExecSqlQuery("SELECT \"Scopes\",\"Revoked\",EXTRACT(EPOCH FROM \"LastUsedAt\"),\"UserId\","
				"\"LastUsedAt\"::text FROM \"PersonalAccessTokens\"");
	QVERIFY(query.next());
	QCOMPARE(query.value(0).toString(), QString("read"));
	QVERIFY(query.value(1).toBool());
	QVERIFY2(!query.value(2).isNull(), qPrintable(QStringLiteral("Timestamp: %1").arg(query.value(4).toString())));
	QCOMPARE(query.value(2).toLongLong(), now.toSecsSinceEpoch());
	QVERIFY(query.value(3).isNull());
}


void CPersonalAccessTokenTest::testDelegateMigratesOnInitialization()
{
	for (const bool duplicate : {false, true}){
		icomp::TSimSharedComponentPtr<CTokenDatabaseEngineComp> engine;
		engine->InitComponent();
		QVERIFY(engine->ExecSqlQuery(s_legacyTokenSchema).isActive());
		QVERIFY(engine->ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" VALUES ('one','owner','hash','read',NULL,FALSE)").isActive());
		if (duplicate){
			QVERIFY(engine->ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" VALUES ('two','owner','hash','read',NULL,FALSE)").isActive());
		}
		icomp::TSimSharedComponentPtr<imtauthdb::CPersonalAccessTokenDatabaseDelegateComp> delegate;
		delegate->SetRef("DatabaseEngine", engine);
		delegate->SetIdAttr("TableName", "PersonalAccessTokens");
		delegate->SetBoolAttr("AutoCreateTable", true);
		delegate->InitComponent();
		if (duplicate){
			QVERIFY(!delegate->RevokeToken("one"));
			QVERIFY(delegate->GetSelectionQuery().isEmpty());
		}
		else{
			QVERIFY(delegate->RevokeToken("one"));
			QVERIFY(engine->ExecSqlQuery("INSERT INTO \"PersonalAccessTokens\" (\"Id\",\"TokenHash\",\"ClientProductId\") "
						"VALUES ('server','server-hash','client')").isActive());
		}
	}
}


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
	QByteArray productId = "test_product_abc";
	QString name = "Test Token";
	QString description = "A test token for unit testing";
	QByteArrayList scopes;
	scopes << "read:users" << "write:data";
	QDateTime expiresAt = QDateTime::currentDateTimeUtc().addDays(30);
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, productId, name, description, scopes, expiresAt);
	
	QVERIFY2(result.success, "Failed to create token");
	QVERIFY2(!result.tokenId.isEmpty(), "Token ID is empty");
	QVERIFY2(!result.rawToken.isEmpty(), "Raw token is empty");
	QVERIFY2(result.rawToken.startsWith("imt_pat_"), "Token doesn't have expected prefix");

	// Verify we can retrieve the token
	imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_tokenManagerPtr->GetToken(result.tokenId);
	QVERIFY2(tokenPtr.IsValid(), "Failed to retrieve created token");
	QCOMPARE(tokenPtr->GetUserId(), userId);
	QCOMPARE(tokenPtr->GetProductId(), productId);
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
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name, "", scopes, QDateTime());
	
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
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name, "", scopes, QDateTime());
	
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
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name, "", scopes, expiresAt);
	
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
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), "Valid Token", "", scopes, futureExpiry);
	
	QVERIFY2(validResult.success, "Failed to create non-expired token");
	
	// Verify this token is valid
	bool isValid = m_tokenManagerPtr->ValidateToken(validResult.rawToken, validatedUserId, validatedTokenId, validatedScopes);
	QVERIFY2(isValid, "Non-expired token should be valid");
}


void CPersonalAccessTokenTest::testGetTokenIds()
{
	QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
	
	// Create multiple tokens for the same user
	// The token collection selects by the user column, which holds UUIDs
	QByteArray userId = "7f3c2a10-5d1e-4c7b-9a42-1b6e8d0f3a11";
	QString name1 = "Token 1";
	QString name2 = "Token 2";
	QString name3 = "Token 3";
	QByteArrayList scopes;
	scopes << "read:api";
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result1 = 
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name1, "", scopes, QDateTime());
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result2 = 
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name2, "", scopes, QDateTime());
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result3 = 
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name3, "", scopes, QDateTime());
	
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
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name, "", scopes, QDateTime());
	
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
	// The token collection selects by the user column, which holds UUIDs
	QByteArray userId = "2b9e4d77-8c03-4f16-a5d1-6c0f7e3b9d22";
	QString name = "Delete Test Token";
	QByteArrayList scopes;
	scopes << "read:api";
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerPtr->CreateToken(userId, QByteArray(), name, "", scopes, QDateTime());
	
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
//	bool shouldFail = m_tokenManagerPtr->DeleteToken("nonexistent_token_id");
//	QVERIFY2(!shouldFail, "Deleting nonexistent token should fail");
}


I_ADD_TEST(CPersonalAccessTokenTest);
