// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <imtauth/IPersonalAccessTokenPersistence.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtauthdb
{


class CPersonalAccessTokenSqlPersistence: public imtauth::IPersonalAccessTokenPersistence
{
public:
	explicit CPersonalAccessTokenSqlPersistence(imtdb::IDatabaseEngine& engine);

	bool EnsureSchema();
	QString GetErrorMessage() const;

	virtual bool RevokeToken(const QByteArray& tokenId) override;
	virtual bool SetTokenScopes(const QByteArray& tokenId, const QByteArrayList& scopes) override;
	virtual bool UpdateLastUsedAt(const QByteArray& tokenId, const QDateTime& usedAt) override;

private:
	bool Execute(const QByteArray& sql, const QVariantMap& values = QVariantMap());
	bool Update(const QByteArray& sql, const QVariantMap& values);
	bool MigrateSqlite(bool addClientProduct, bool removeOwnerNotNull);

	imtdb::IDatabaseEngine& m_engine;
	QString m_errorMessage;
};


} // namespace imtauthdb
