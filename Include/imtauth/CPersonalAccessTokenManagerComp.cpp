// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CPersonalAccessTokenManagerComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QRandomGenerator>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IPersonalAccessTokenManager)

IPersonalAccessTokenManager::TokenCreationResult CPersonalAccessTokenManagerComp::CreateToken(
			const QByteArray& userId,
			const QString& name,
			const QString& description,
			const QByteArrayList& scopes,
			const QDateTime& expiresAt)
{
	TokenCreationResult result;
	result.success = false;

	if (!m_tokenCollectionCompPtr.IsValid() || !m_tokenFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Token collection or factory not configured", "CPersonalAccessTokenManagerComp");
		return result;
	}

	// Generate unique token ID
	QByteArray tokenId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	// Generate random token value
	QByteArray rawToken = GenerateRandomToken();

	// Compute token hash
	QByteArray tokenHash = HashToken(rawToken);

	// Create token object
	IPersonalAccessTokenUniquePtr tokenPtr = m_tokenFactoryCompPtr.CreateInstance();
	if (!tokenPtr.IsValid()){
		SendErrorMessage(0, "Failed to create token object", "CPersonalAccessTokenManagerComp");
		return result;
	}

	tokenPtr->SetId(tokenId);
	tokenPtr->SetUserId(userId);
	tokenPtr->SetName(name);
	tokenPtr->SetDescription(description);
	tokenPtr->SetTokenHash(tokenHash);
	tokenPtr->SetScopes(scopes);
	tokenPtr->SetCreatedAt(QDateTime::currentDateTimeUtc());
	tokenPtr->SetExpiresAt(expiresAt);
	tokenPtr->SetRevoked(false);

	// Store in collection
	QByteArray retVal = m_tokenCollectionCompPtr->InsertNewObject("PersonalAccessToken", name, description, tokenPtr.GetPtr(), tokenId);
	if (retVal.isEmpty()){
		SendErrorMessage(0, "Failed to store token in collection", "CPersonalAccessTokenManagerComp");
		return result;
	}

	// Return the raw token (only time it's shown)
	result.tokenId = tokenId;
	result.rawToken = rawToken;
	result.success = true;

	SendInfoMessage(0, QString("Created personal access token '%1' for user '%2'").arg(name).arg(QString::fromUtf8(userId)), "CPersonalAccessTokenManagerComp");

	return result;
}


bool CPersonalAccessTokenManagerComp::ValidateToken(
			const QByteArray& rawToken,
			QByteArray& userId,
			QByteArray& tokenId,
			QByteArrayList& scopes) const
{
	if (!m_tokenCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}

	// Hash the provided token
	QByteArray tokenHash = HashToken(rawToken);

	// Find token by hash
	imtbase::ICollectionInfo::Ids tokenIds = m_tokenCollectionCompPtr->GetElementIds();
	for (const QByteArray& currentTokenId : std::as_const(tokenIds)){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tokenCollectionCompPtr->GetObjectData(currentTokenId, dataPtr)){
			continue;
		}

		IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
		if (!tokenPtr->IsValid()){
			continue;
		}

		// Check if hash matches (constant-time comparison to prevent timing attacks)
		QByteArray storedHash = tokenPtr->GetTokenHash();
		if (storedHash.size() != tokenHash.size()){
			continue;
		}

		// Constant-time comparison
		int diff = 0;
		for (int i = 0; i < storedHash.size(); ++i){
			diff |= (storedHash[i] ^ tokenHash[i]);
		}

		if (diff != 0){
			continue;
		}

		// Check if token is valid
		if (!tokenPtr->IsValid()){
			SendWarningMessage(0, QString("Token '%1' is not valid (revoked or expired)").arg(QString::fromUtf8(currentTokenId)), "CPersonalAccessTokenManagerComp");
			return false;
		}

		// Token is valid
		userId = tokenPtr->GetUserId();
		tokenId = tokenPtr->GetId();
		scopes = tokenPtr->GetScopes();

		return true;
	}

	SendWarningMessage(0, "Token validation failed: token not found", "CPersonalAccessTokenManagerComp");

	return false;
}


QByteArrayList CPersonalAccessTokenManagerComp::GetTokenIds(const QByteArray& userId) const
{
	QByteArrayList result;

	if (!m_tokenCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return result;
	}

	imtbase::ICollectionInfo::Ids tokenIds = m_tokenCollectionCompPtr->GetElementIds();
	for (const QByteArray& tokenId : tokenIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr)){
			continue;
		}

		IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
		if (tokenPtr && tokenPtr->GetUserId() == userId){
			result.append(tokenId);
		}
	}

	return result;
}


IPersonalAccessTokenSharedPtr CPersonalAccessTokenManagerComp::GetToken(const QByteArray& tokenId) const
{
	if (!m_tokenCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr)){
		return nullptr;
	}

	const IPersonalAccessToken* tokenPtr = dynamic_cast<const IPersonalAccessToken*>(dataPtr.GetPtr());
	if (tokenPtr == nullptr){
		return nullptr;
	}

	// Clone via IChangeable interface (preferred, keeps logic in the token implementation).
	istd::TUniqueInterfacePtr<IPersonalAccessToken> clonedToken = m_tokenFactoryCompPtr.CreateInstance();
	if (!clonedToken.IsValid()){
		return nullptr;
	}

	if (!clonedToken->CopyFrom(*tokenPtr)){
		return nullptr;
	}

	return IPersonalAccessTokenSharedPtr(clonedToken.PopInterfacePtr());
}


bool CPersonalAccessTokenManagerComp::RevokeToken(const QByteArray& tokenId)
{
	if (!m_tokenCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr)){
		SendErrorMessage(0, QString("Token '%1' not found").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");
		return false;
	}

	IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
	if (!tokenPtr->IsValid()){
		SendErrorMessage(0, "Invalid token object", "CPersonalAccessTokenManagerComp");
		return false;
	}

	tokenPtr->SetRevoked(true);

	if (!m_tokenCollectionCompPtr->SetObjectData(tokenId, *tokenPtr)){
		SendErrorMessage(0, QString("Failed to update token '%1'").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Revoked token '%1'").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");

	return true;
}


bool CPersonalAccessTokenManagerComp::UpdateLastUsedAt(const QByteArray& tokenId)
{
	if (!m_tokenCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr)){
		return false;
	}

	IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
	if (!tokenPtr->IsValid()){
		return false;
	}

	tokenPtr->SetLastUsedAt(QDateTime::currentDateTimeUtc());

	return m_tokenCollectionCompPtr->SetObjectData(tokenId, *tokenPtr);
}


bool CPersonalAccessTokenManagerComp::DeleteToken(const QByteArray& tokenId)
{
	if (!m_tokenCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}

	if (!m_tokenCollectionCompPtr->RemoveElements({tokenId})){
		SendErrorMessage(0, QString("Failed to delete token '%1'").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Deleted token '%1'").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");

	return true;
}


// private methods

QByteArray CPersonalAccessTokenManagerComp::GenerateRandomToken() const
{
	// Generate a cryptographically secure random token
	// Format: pat_<base64url_encoded_random_data>
	// The token should be long enough to be secure (32 bytes = 256 bits of entropy)

	QByteArray randomData;
	randomData.resize(32);

	// Use cryptographically secure random number generator
	// Note: QRandomGenerator::system() is cryptographically secure on most platforms
	// For critical applications, consider using platform-specific secure RNG
	QRandomGenerator* rng = QRandomGenerator::system();

	for (int i = 0; i < randomData.size(); ++i){
		randomData[i] = static_cast<char>(rng->generate() & 0xFF);
	}

	// Convert to base64url for safe transmission
	QByteArray token = "imt_pat_" + randomData.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);

	return token;
}


QByteArray CPersonalAccessTokenManagerComp::HashToken(const QByteArray& rawToken) const
{
	// Use SHA-256 to hash the token
	return QCryptographicHash::hash(rawToken, QCryptographicHash::Sha256).toHex();
}


QByteArray CPersonalAccessTokenManagerComp::ExtractTokenId(const QByteArray& rawToken) const
{
	// For now, we don't embed the ID in the token
	// We search by hash instead
	return QByteArray();
}


} // namespace imtauth
