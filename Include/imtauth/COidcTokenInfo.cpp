// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/COidcTokenInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IOidcTokenInfo)

QByteArray COidcTokenInfo::GetTokenId() const
{
	return m_tokenId;
}


void COidcTokenInfo::SetTokenId(const QByteArray& tokenId)
{
	if (m_tokenId != tokenId){
		istd::CChangeNotifier changeNotifier(this);

		m_tokenId = tokenId;
	}
}


QByteArray COidcTokenInfo::GetClientId() const
{
	return m_clientId;
}


void COidcTokenInfo::SetClientId(const QByteArray& clientId)
{
	if (m_clientId != clientId){
		istd::CChangeNotifier changeNotifier(this);

		m_clientId = clientId;
	}
}


QByteArray COidcTokenInfo::GetUserId() const
{
	return m_userId;
}


void COidcTokenInfo::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QString COidcTokenInfo::GetScope() const
{
	return m_scope;
}


void COidcTokenInfo::SetScope(const QString& scope)
{
	if (m_scope != scope){
		istd::CChangeNotifier changeNotifier(this);

		m_scope = scope;
	}
}


IOidcTokenInfo::OidcTokenType COidcTokenInfo::GetTokenType() const
{
	return m_tokenType;
}


void COidcTokenInfo::SetTokenType(OidcTokenType tokenType)
{
	if (m_tokenType != tokenType){
		istd::CChangeNotifier changeNotifier(this);

		m_tokenType = tokenType;
	}
}


QDateTime COidcTokenInfo::GetExpirationDate() const
{
	return m_expirationDate;
}


void COidcTokenInfo::SetExpirationDate(const QDateTime& expirationDate)
{
	if (m_expirationDate != expirationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_expirationDate = expirationDate;
	}
}


bool COidcTokenInfo::IsRevoked() const
{
	return m_revoked;
}


void COidcTokenInfo::SetRevoked(bool revoked)
{
	if (m_revoked != revoked){
		istd::CChangeNotifier changeNotifier(this);

		m_revoked = revoked;
	}
}


QDateTime COidcTokenInfo::GetCreatedAt() const
{
	return m_createdAt;
}


void COidcTokenInfo::SetCreatedAt(const QDateTime& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier changeNotifier(this);

		m_createdAt = createdAt;
	}
}


// reimplemented (iser::ISerializable)

bool COidcTokenInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag tokenIdTag("TokenId", "Token identifier");
	retVal = retVal && archive.BeginTag(tokenIdTag);
	retVal = retVal && archive.Process(m_tokenId);
	retVal = retVal && archive.EndTag(tokenIdTag);

	static iser::CArchiveTag clientIdTag("ClientId", "Client identifier");
	retVal = retVal && archive.BeginTag(clientIdTag);
	retVal = retVal && archive.Process(m_clientId);
	retVal = retVal && archive.EndTag(clientIdTag);

	static iser::CArchiveTag userIdTag("UserId", "User identifier");
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	static iser::CArchiveTag scopeTag("Scope", "Token scope");
	retVal = retVal && archive.BeginTag(scopeTag);
	retVal = retVal && archive.Process(m_scope);
	retVal = retVal && archive.EndTag(scopeTag);

	static iser::CArchiveTag tokenTypeTag("TokenType", "Token type");
	retVal = retVal && archive.BeginTag(tokenTypeTag);
	I_SERIALIZE_ENUM(OidcTokenType, archive, m_tokenType);
	retVal = retVal && archive.EndTag(tokenTypeTag);

	static iser::CArchiveTag expirationTag("ExpirationDate", "Expiration timestamp");
	retVal = retVal && archive.BeginTag(expirationTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expirationDate);
	retVal = retVal && archive.EndTag(expirationTag);

	static iser::CArchiveTag revokedTag("Revoked", "Revocation status");
	retVal = retVal && archive.BeginTag(revokedTag);
	retVal = retVal && archive.Process(m_revoked);
	retVal = retVal && archive.EndTag(revokedTag);

	static iser::CArchiveTag createdAtTag("CreatedAt", "Creation timestamp");
	retVal = retVal && archive.BeginTag(createdAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool COidcTokenInfo::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const COidcTokenInfo* tokenPtr = dynamic_cast<const COidcTokenInfo*>(&object);
	if (tokenPtr == nullptr){
		return false;
	}

	m_tokenId = tokenPtr->m_tokenId;
	m_clientId = tokenPtr->m_clientId;
	m_userId = tokenPtr->m_userId;
	m_scope = tokenPtr->m_scope;
	m_tokenType = tokenPtr->m_tokenType;
	m_expirationDate = tokenPtr->m_expirationDate;
	m_revoked = tokenPtr->m_revoked;
	m_createdAt = tokenPtr->m_createdAt;

	return true;
}


bool COidcTokenInfo::IsEqual(const IChangeable& object) const
{
	const COidcTokenInfo* sourcePtr = dynamic_cast<const COidcTokenInfo*>(&object);
	if (sourcePtr != nullptr){
		bool retVal = true;

		retVal = retVal && (m_tokenId == sourcePtr->m_tokenId);
		retVal = retVal && (m_clientId == sourcePtr->m_clientId);
		retVal = retVal && (m_userId == sourcePtr->m_userId);
		retVal = retVal && (m_scope == sourcePtr->m_scope);
		retVal = retVal && (m_tokenType == sourcePtr->m_tokenType);
		retVal = retVal && (m_expirationDate == sourcePtr->m_expirationDate);
		retVal = retVal && (m_revoked == sourcePtr->m_revoked);
		retVal = retVal && (m_createdAt == sourcePtr->m_createdAt);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr COidcTokenInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new COidcTokenInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool COidcTokenInfo::ResetData(CompatibilityMode /*mode*/)
{
	m_tokenId.clear();
	m_clientId.clear();
	m_userId.clear();
	m_scope.clear();
	m_tokenType = OTT_ACCESS;
	m_expirationDate = QDateTime();
	m_revoked = false;
	m_createdAt = QDateTime();

	return true;
}


} // namespace imtauth


