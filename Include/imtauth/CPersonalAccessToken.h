#pragma once


// Qt includes
#include <QDateTime>

// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>


namespace imtauth
{


class CPersonalAccessToken: virtual public imtauth::IPersonalAccessToken
{
public:
	// reimplemented (imtauth::IPersonalAccessToken)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual QByteArray GetTokenHash() const override;
	virtual void SetTokenHash(const QByteArray& tokenHash) override;
	virtual QByteArrayList GetScopes() const override;
	virtual void SetScopes(const QByteArrayList& scopes) override;
	virtual QDateTime GetCreatedAt() const override;
	virtual void SetCreatedAt(const QDateTime& createdAt) override;
	virtual QDateTime GetLastUsedAt() const override;
	virtual void SetLastUsedAt(const QDateTime& lastUsedAt) override;
	virtual QDateTime GetExpiresAt() const override;
	virtual void SetExpiresAt(const QDateTime& expiresAt) override;
	virtual bool IsRevoked() const override;
	virtual void SetRevoked(bool revoked) override;
	virtual bool IsExpired() const override;
	virtual bool IsValid() const override;

private:
	QByteArray m_id;
	QByteArray m_userId;
	QString m_name;
	QString m_description;
	QByteArray m_tokenHash;
	QByteArrayList m_scopes;
	QDateTime m_createdAt;
	QDateTime m_lastUsedAt;
	QDateTime m_expiresAt;
	bool m_revoked;
};


} // namespace imtauth


