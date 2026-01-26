#include <imtauth/CPersonalAccessToken.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IPersonalAccessToken)

QByteArray CPersonalAccessToken::GetId() const
{
	return m_id;
}


void CPersonalAccessToken::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier changeNotifier(this);

		m_id = id;
	}
}


QByteArray CPersonalAccessToken::GetUserId() const
{
	return m_userId;
}


void CPersonalAccessToken::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QString CPersonalAccessToken::GetName() const
{
	return m_name;
}


void CPersonalAccessToken::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QString CPersonalAccessToken::GetDescription() const
{
	return m_description;
}


void CPersonalAccessToken::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


QByteArray CPersonalAccessToken::GetTokenHash() const
{
	return m_tokenHash;
}


void CPersonalAccessToken::SetTokenHash(const QByteArray& tokenHash)
{
	if (m_tokenHash != tokenHash){
		istd::CChangeNotifier changeNotifier(this);

		m_tokenHash = tokenHash;
	}
}


QByteArrayList CPersonalAccessToken::GetScopes() const
{
	return m_scopes;
}


void CPersonalAccessToken::SetScopes(const QByteArrayList& scopes)
{
	if (m_scopes != scopes){
		istd::CChangeNotifier changeNotifier(this);

		m_scopes = scopes;
	}
}


QDateTime CPersonalAccessToken::GetCreatedAt() const
{
	return m_createdAt;
}


void CPersonalAccessToken::SetCreatedAt(const QDateTime& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier changeNotifier(this);

		m_createdAt = createdAt;
	}
}


QDateTime CPersonalAccessToken::GetLastUsedAt() const
{
	return m_lastUsedAt;
}


void CPersonalAccessToken::SetLastUsedAt(const QDateTime& lastUsedAt)
{
	if (m_lastUsedAt != lastUsedAt){
		istd::CChangeNotifier changeNotifier(this);

		m_lastUsedAt = lastUsedAt;
	}
}


QDateTime CPersonalAccessToken::GetExpiresAt() const
{
	return m_expiresAt;
}


void CPersonalAccessToken::SetExpiresAt(const QDateTime& expiresAt)
{
	if (m_expiresAt != expiresAt){
		istd::CChangeNotifier changeNotifier(this);

		m_expiresAt = expiresAt;
	}
}


bool CPersonalAccessToken::IsRevoked() const
{
	return m_revoked;
}


void CPersonalAccessToken::SetRevoked(bool revoked)
{
	if (m_revoked != revoked){
		istd::CChangeNotifier changeNotifier(this);

		m_revoked = revoked;
	}
}


bool CPersonalAccessToken::IsExpired() const
{
	if (!m_expiresAt.isValid())
	{
		return false;  // No expiration set means never expires
	}
	
	return QDateTime::currentDateTime() > m_expiresAt;
}


bool CPersonalAccessToken::IsValid() const
{
	return !IsRevoked() && !IsExpired();
}


} // namespace imtauth
