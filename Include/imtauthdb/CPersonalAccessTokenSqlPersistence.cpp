// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CPersonalAccessTokenSqlPersistence.h>


#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtSql/QSqlRecord>


namespace imtauthdb
{


CPersonalAccessTokenSqlPersistence::CPersonalAccessTokenSqlPersistence(imtdb::IDatabaseEngine& engine)
	:m_engine(engine)
{
}


QString CPersonalAccessTokenSqlPersistence::GetErrorMessage() const
{
	return m_errorMessage;
}


bool CPersonalAccessTokenSqlPersistence::Execute(const QByteArray& sql, const QVariantMap& values)
{
	QSqlError error;
	const QSqlQuery query = values.isEmpty() ? m_engine.ExecSqlQuery(sql, &error) : m_engine.ExecSqlQuery(sql, values, &error);
	if (error.isValid() || !query.isActive()){
		m_errorMessage = error.isValid() ? error.text() : QStringLiteral("Token SQL operation failed");
		return false;
	}

	return true;
}


bool CPersonalAccessTokenSqlPersistence::Update(const QByteArray& sql, const QVariantMap& values)
{
	QSqlError error;
	QSqlQuery query = m_engine.ExecSqlQuery(sql, values, &error);
	if (error.isValid() || !query.isActive()){
		m_errorMessage = error.isValid() ? error.text() : QStringLiteral("Token update failed");
		return false;
	}
	if (!query.next()){
		m_errorMessage = QStringLiteral("The token does not exist or cannot be changed");
		return false;
	}

	return true;
}


bool CPersonalAccessTokenSqlPersistence::RevokeToken(const QByteArray& tokenId)
{
	return Update("UPDATE \"PersonalAccessTokens\" SET \"Revoked\"=TRUE WHERE \"Id\"=:id RETURNING \"Id\"",
				{{":id", QString::fromUtf8(tokenId)}});
}


bool CPersonalAccessTokenSqlPersistence::SetTokenScopes(const QByteArray& tokenId, const QByteArrayList& scopes)
{
	return Update("UPDATE \"PersonalAccessTokens\" SET \"Scopes\"=:scopes WHERE \"Id\"=:id AND NOT \"Revoked\" RETURNING \"Id\"",
				{{":id", QString::fromUtf8(tokenId)}, {":scopes", QString::fromUtf8(scopes.join(','))}});
}


bool CPersonalAccessTokenSqlPersistence::UpdateLastUsedAt(const QByteArray& tokenId, const QDateTime& usedAt)
{
	if (!usedAt.isValid()){
		m_errorMessage = QStringLiteral("The token usage date is invalid");
		return false;
	}

	return Update("UPDATE \"PersonalAccessTokens\" SET \"LastUsedAt\"=CASE WHEN \"LastUsedAt\" IS NULL OR \"LastUsedAt\"<:used "
				"THEN :used ELSE \"LastUsedAt\" END WHERE \"Id\"=:id RETURNING \"Id\"",
				{{":id", QString::fromUtf8(tokenId)}, {":used", usedAt.toUTC().toString(Qt::ISODate)}});
}


bool CPersonalAccessTokenSqlPersistence::MigrateSqlite(bool addClientProduct, bool removeOwnerNotNull)
{
	if (addClientProduct && !Execute("ALTER TABLE \"PersonalAccessTokens\" ADD COLUMN \"ClientProductId\" TEXT")){
		return false;
	}
	if (!removeOwnerNotNull){
		return true;
	}

	QSqlError error;
	QSqlQuery references = m_engine.ExecSqlQuery(
				"SELECT t.name FROM sqlite_master AS t JOIN pragma_foreign_key_list(t.name) AS f "
				"WHERE t.type='table' AND f.\"table\"='PersonalAccessTokens'", &error);
	if (error.isValid() || !references.isActive()){
		m_errorMessage = QStringLiteral("Unable to inspect token foreign keys: %1").arg(error.text());
		return false;
	}
	if (references.next()){
		m_errorMessage = QStringLiteral("Token table has incoming foreign keys; an explicit database migration is required");
		return false;
	}
	references.finish();

	QSqlQuery schema = m_engine.ExecSqlQuery("SELECT sql FROM sqlite_master WHERE type='table' AND name='PersonalAccessTokens'", &error);
	if (error.isValid() || !schema.next()){
		m_errorMessage = QStringLiteral("Unable to read the token table schema: %1").arg(error.text());
		return false;
	}
	QString createSql = schema.value(0).toString();
	schema.finish();
	const QRegularExpression ownerConstraint(QStringLiteral("(\"UserId\"\\s+\\w+)\\s+NOT\\s+NULL"), QRegularExpression::CaseInsensitiveOption);
	if (!createSql.contains(ownerConstraint)){
		m_errorMessage = QStringLiteral("Unsupported token owner constraint; the schema was not changed");
		return false;
	}
	createSql.replace(ownerConstraint, QStringLiteral("\\1"));
	const qsizetype opening = createSql.indexOf('(');
	if (opening < 0){
		m_errorMessage = QStringLiteral("Invalid token table schema");
		return false;
	}
	createSql = QStringLiteral("CREATE TABLE \"PersonalAccessTokens_Migration\" ") + createSql.mid(opening);

	QStringList definitions;
	QSqlQuery objects = m_engine.ExecSqlQuery("SELECT sql FROM sqlite_master WHERE tbl_name='PersonalAccessTokens' "
				"AND type IN ('index','trigger') AND sql IS NOT NULL", &error);
	if (error.isValid() || !objects.isActive()){
		m_errorMessage = QStringLiteral("Unable to read token indexes and triggers: %1").arg(error.text());
		return false;
	}
	while (objects.next()){
		definitions.append(objects.value(0).toString());
	}
	objects.finish();

	if (!Execute(createSql.toUtf8())
				|| !Execute("INSERT INTO \"PersonalAccessTokens_Migration\" SELECT * FROM \"PersonalAccessTokens\"")
				|| !Execute("DROP TABLE \"PersonalAccessTokens\"")
				|| !Execute("ALTER TABLE \"PersonalAccessTokens_Migration\" RENAME TO \"PersonalAccessTokens\"")){
		return false;
	}
	for (const QString& definition : definitions){
		if (!Execute(definition.toUtf8())){
			return false;
		}
	}

	return true;
}


bool CPersonalAccessTokenSqlPersistence::EnsureSchema()
{
	m_errorMessage.clear();
	const QByteArray driver = m_engine.GetDatabaseDriverId().toUpper();
	const bool isSqlite = driver == "QSQLITE";
	if (!isSqlite && driver != "QPSQL" && !driver.contains("DUCK")){
		m_errorMessage = QStringLiteral("Unsupported token database driver: %1").arg(QString::fromUtf8(driver));
		return false;
	}
	if (!m_engine.BeginTransaction()){
		m_errorMessage = QStringLiteral("Unable to start token schema migration");
		return false;
	}
	auto rollback = qScopeGuard([this](){
		if (!m_engine.CancelTransaction()){
			m_errorMessage += QStringLiteral("; unable to roll back token schema migration");
		}
	});

	bool hasClientProduct = false;
	bool removeOwnerNotNull = false;
	QSqlError error;
	QSqlQuery columns = m_engine.ExecSqlQuery(isSqlite
				? QByteArray("PRAGMA table_info('PersonalAccessTokens')")
				: QByteArray("SELECT column_name, is_nullable FROM information_schema.columns "
							"WHERE table_schema=current_schema() AND table_name='PersonalAccessTokens'"), &error);
	if (error.isValid() || !columns.isActive()){
		m_errorMessage = QStringLiteral("Unable to inspect token schema: %1").arg(error.text());
		return false;
	}
	bool hasOwner = false;
	while (columns.next()){
		const QString name = columns.value(isSqlite ? "name" : "column_name").toString();
		hasClientProduct = hasClientProduct || name == "ClientProductId";
		if (name == "UserId"){
			hasOwner = true;
			removeOwnerNotNull = isSqlite ? columns.value("notnull").toBool() : columns.value("is_nullable").toString() == "NO";
		}
	}
	columns.finish();
	if (!hasOwner){
		m_errorMessage = QStringLiteral("The token table is missing its owner column");
		return false;
	}

	if (isSqlite){
		if (!MigrateSqlite(!hasClientProduct, removeOwnerNotNull)){
			return false;
		}
	}
	else{
		if ((!hasClientProduct && !Execute("ALTER TABLE \"PersonalAccessTokens\" ADD COLUMN \"ClientProductId\" TEXT"))
					|| (removeOwnerNotNull && !Execute("ALTER TABLE \"PersonalAccessTokens\" ALTER COLUMN \"UserId\" DROP NOT NULL"))){
			return false;
		}
	}
	if (!Execute("CREATE UNIQUE INDEX IF NOT EXISTS \"IX_PersonalAccessTokens_TokenHash\" ON \"PersonalAccessTokens\" (\"TokenHash\")")
				|| !Execute("CREATE INDEX IF NOT EXISTS \"IX_PersonalAccessTokens_ClientProductId\" ON \"PersonalAccessTokens\" (\"ClientProductId\")")){
		return false;
	}
	if (!m_engine.FinishTransaction()){
		m_errorMessage = QStringLiteral("Unable to commit token schema migration");
		return false;
	}
	rollback.dismiss();

	return true;
}


} // namespace imtauthdb
