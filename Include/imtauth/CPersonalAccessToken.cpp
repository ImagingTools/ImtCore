// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPersonalAccessToken.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


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
	if (!m_expiresAt.isValid()){
		return false;  // No expiration set means never expires
	}
	
	return QDateTime::currentDateTimeUtc() > m_expiresAt;
}


bool CPersonalAccessToken::IsValid() const
{
	return !IsRevoked() && !IsExpired();
}


// reimplemented (iser::ISerializable)

bool CPersonalAccessToken::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Token identifier");
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag userIdTag("UserId", "User identifier");
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag nameTag("Name", "Token name");
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag descriptionTag("Description", "Token description");
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static iser::CArchiveTag tokenHashTag("TokenHash", "Token hash value");
	retVal = retVal && archive.BeginTag(tokenHashTag);
	retVal = retVal && archive.Process(m_tokenHash);
	retVal = retVal && archive.EndTag(tokenHashTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_scopes, "Scopes", "Permission scopes");

	static iser::CArchiveTag createdAtTag("CreatedAt", "Creation timestamp");
	retVal = retVal && archive.BeginTag(createdAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	static iser::CArchiveTag lastUsedAtTag("LastUsedAt", "Last used timestamp");
	retVal = retVal && archive.BeginTag(lastUsedAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_lastUsedAt);
	retVal = retVal && archive.EndTag(lastUsedAtTag);

	static iser::CArchiveTag expiresAtTag("ExpiresAt", "Expiration timestamp");
	retVal = retVal && archive.BeginTag(expiresAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expiresAt);
	retVal = retVal && archive.EndTag(expiresAtTag);

	static iser::CArchiveTag revokedTag("Revoked", "Revocation status");
	retVal = retVal && archive.BeginTag(revokedTag);
	retVal = retVal && archive.Process(m_revoked);
	retVal = retVal && archive.EndTag(revokedTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CPersonalAccessToken::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CPersonalAccessToken* tokenPtr = dynamic_cast<const CPersonalAccessToken*>(&object);
	if (tokenPtr == nullptr){
		return false;
	}

	m_id = tokenPtr->m_id;
	m_userId = tokenPtr->m_userId;
	m_name = tokenPtr->m_name;
	m_description = tokenPtr->m_description;
	m_tokenHash = tokenPtr->m_tokenHash;
	m_scopes = tokenPtr->m_scopes;
	m_createdAt = tokenPtr->m_createdAt;
	m_lastUsedAt = tokenPtr->m_lastUsedAt;
	m_expiresAt = tokenPtr->m_expiresAt;
	m_revoked = tokenPtr->m_revoked;

	return true;
}


bool CPersonalAccessToken::IsEqual(const IChangeable& object) const
{
	const CPersonalAccessToken* sourcePtr = dynamic_cast<const CPersonalAccessToken*>(&object);
	if (sourcePtr != nullptr) {
		bool retVal = true;

		retVal = retVal && (m_id == sourcePtr->m_id);
		retVal = retVal && (m_userId == sourcePtr->m_userId);
		retVal = retVal && (m_name == sourcePtr->m_name);
		retVal = retVal && (m_description == sourcePtr->m_description);
		retVal = retVal && (m_tokenHash == sourcePtr->m_tokenHash);
		retVal = retVal && (m_scopes == sourcePtr->m_scopes);
		retVal = retVal && (m_createdAt == sourcePtr->m_createdAt);
		retVal = retVal && (m_lastUsedAt == sourcePtr->m_lastUsedAt);
		retVal = retVal && (m_expiresAt == sourcePtr->m_expiresAt);
		retVal = retVal && (m_revoked == sourcePtr->m_revoked);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CPersonalAccessToken::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CPersonalAccessToken);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CPersonalAccessToken::ResetData(CompatibilityMode /*mode*/)
{
	m_id.clear();
	m_userId.clear();
	m_name.clear();
	m_description.clear();
	m_tokenHash.clear();
	m_scopes.clear();
	m_createdAt = QDateTime();
	m_lastUsedAt = QDateTime();
	m_expiresAt = QDateTime();
	m_revoked = false;

	return true;
}


} // namespace imtauth


