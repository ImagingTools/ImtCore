#include <imtauthdb/CSqlitePersonalAccessTokenDatabaseDelegateComp.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlitePersonalAccessTokenDatabaseDelegateComp::CreateTableQuery(const QByteArray& typeId) const
{
	QByteArray retVal = BaseClass::CreateTableQuery(typeId);
	
	retVal += 
		"CREATE TABLE IF NOT EXISTS PersonalAccessTokens ("
		"Id TEXT PRIMARY KEY NOT NULL, "
		"TypeId TEXT NOT NULL, "
		"Name TEXT, "
		"Description TEXT, "
		"UserId TEXT NOT NULL, "
		"TokenHash TEXT NOT NULL, "
		"Scopes TEXT, "
		"CreatedAt TEXT NOT NULL, "
		"LastUsedAt TEXT, "
		"ExpiresAt TEXT, "
		"Revoked INTEGER DEFAULT 0, "
		"FOREIGN KEY(UserId) REFERENCES Users(Id) ON DELETE CASCADE"
		"); "
		"CREATE INDEX IF NOT EXISTS idx_pat_user_id ON PersonalAccessTokens(UserId); "
		"CREATE INDEX IF NOT EXISTS idx_pat_token_hash ON PersonalAccessTokens(TokenHash); "
		"CREATE INDEX IF NOT EXISTS idx_pat_created_at ON PersonalAccessTokens(CreatedAt); ";
	
	return retVal;
}


} // namespace imtauthdb
