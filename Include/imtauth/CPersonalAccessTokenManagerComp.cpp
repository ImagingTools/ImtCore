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
	
	if (!m_tokenCollectionCompPtr.IsValid() || !m_tokenFactoryCompPtr.IsValid())
	{
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
	IPersonalAccessTokenUniquePtr tokenPtr = m_tokenFactoryCompPtr->CreateInstance();
	if (!tokenPtr)
	{
		SendErrorMessage(0, "Failed to create token object", "CPersonalAccessTokenManagerComp");
		return result;
	}
	
	tokenPtr->SetId(tokenId);
	tokenPtr->SetUserId(userId);
	tokenPtr->SetName(name);
	tokenPtr->SetDescription(description);
	tokenPtr->SetTokenHash(tokenHash);
	tokenPtr->SetScopes(scopes);
	tokenPtr->SetCreatedAt(QDateTime::currentDateTime());
	tokenPtr->SetExpiresAt(expiresAt);
	tokenPtr->SetRevoked(false);
	
	// Store in collection
	if (!m_tokenCollectionCompPtr->InsertNewObject(tokenId, name, description, tokenPtr.GetPtr()))
	{
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
	QByteArrayList& scopes) const
{
	if (!m_tokenCollectionCompPtr.IsValid())
	{
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}
	
	// Hash the provided token
	QByteArray tokenHash = HashToken(rawToken);
	
	// Find token by hash
	imtbase::ICollectionInfo::Ids tokenIds = m_tokenCollectionCompPtr->GetObjectIds();
	for (const QByteArray& tokenId : tokenIds)
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr))
		{
			continue;
		}
		
		IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
		if (!tokenPtr)
		{
			continue;
		}
		
		// Check if hash matches (constant-time comparison to prevent timing attacks)
		QByteArray storedHash = tokenPtr->GetTokenHash();
		if (storedHash.size() != tokenHash.size())
		{
			continue;
		}
		
		// Constant-time comparison
		int diff = 0;
		for (int i = 0; i < storedHash.size(); ++i)
		{
			diff |= (storedHash[i] ^ tokenHash[i]);
		}
		
		if (diff != 0)
		{
			continue;
		}
		
		// Check if token is valid
		if (!tokenPtr->IsValid())
		{
			SendWarningMessage(0, QString("Token '%1' is not valid (revoked or expired)").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");
			return false;
		}
		
		// Token is valid
		userId = tokenPtr->GetUserId();
		scopes = tokenPtr->GetScopes();
		
		return true;
	}
	
	SendWarningMessage(0, "Token validation failed: token not found", "CPersonalAccessTokenManagerComp");
	return false;
}


QByteArrayList CPersonalAccessTokenManagerComp::GetTokenIds(const QByteArray& userId) const
{
	QByteArrayList result;
	
	if (!m_tokenCollectionCompPtr.IsValid())
	{
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return result;
	}
	
	imtbase::ICollectionInfo::Ids tokenIds = m_tokenCollectionCompPtr->GetObjectIds();
	for (const QByteArray& tokenId : tokenIds)
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr))
		{
			continue;
		}
		
		IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
		if (tokenPtr && tokenPtr->GetUserId() == userId)
		{
			result.append(tokenId);
		}
	}
	
	return result;
}


IPersonalAccessTokenSharedPtr CPersonalAccessTokenManagerComp::GetToken(const QByteArray& tokenId) const
{
	if (!m_tokenCollectionCompPtr.IsValid())
	{
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return nullptr;
	}
	
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr))
	{
		return nullptr;
	}
	
	IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
	if (!tokenPtr)
	{
		return nullptr;
	}
	
	// Clone the token
	IPersonalAccessTokenUniquePtr clonedToken = m_tokenFactoryCompPtr->CreateInstance();
	if (clonedToken)
	{
		clonedToken->SetId(tokenPtr->GetId());
		clonedToken->SetUserId(tokenPtr->GetUserId());
		clonedToken->SetName(tokenPtr->GetName());
		clonedToken->SetDescription(tokenPtr->GetDescription());
		clonedToken->SetTokenHash(tokenPtr->GetTokenHash());
		clonedToken->SetScopes(tokenPtr->GetScopes());
		clonedToken->SetCreatedAt(tokenPtr->GetCreatedAt());
		clonedToken->SetLastUsedAt(tokenPtr->GetLastUsedAt());
		clonedToken->SetExpiresAt(tokenPtr->GetExpiresAt());
		clonedToken->SetRevoked(tokenPtr->IsRevoked());
	}
	
	return IPersonalAccessTokenSharedPtr(clonedToken.Release());
}


bool CPersonalAccessTokenManagerComp::RevokeToken(const QByteArray& tokenId)
{
	if (!m_tokenCollectionCompPtr.IsValid())
	{
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}
	
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr))
	{
		SendErrorMessage(0, QString("Token '%1' not found").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");
		return false;
	}
	
	IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
	if (!tokenPtr)
	{
		SendErrorMessage(0, "Invalid token object", "CPersonalAccessTokenManagerComp");
		return false;
	}
	
	tokenPtr->SetRevoked(true);
	
	if (!m_tokenCollectionCompPtr->UpdateObject(tokenId, tokenPtr))
	{
		SendErrorMessage(0, QString("Failed to update token '%1'").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");
		return false;
	}
	
	SendInfoMessage(0, QString("Revoked token '%1'").arg(QString::fromUtf8(tokenId)), "CPersonalAccessTokenManagerComp");
	return true;
}


bool CPersonalAccessTokenManagerComp::UpdateLastUsedAt(const QByteArray& tokenId)
{
	if (!m_tokenCollectionCompPtr.IsValid())
	{
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}
	
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tokenCollectionCompPtr->GetObjectData(tokenId, dataPtr))
	{
		return false;
	}
	
	IPersonalAccessToken* tokenPtr = dynamic_cast<IPersonalAccessToken*>(dataPtr.GetPtr());
	if (!tokenPtr)
	{
		return false;
	}
	
	tokenPtr->SetLastUsedAt(QDateTime::currentDateTime());
	
	return m_tokenCollectionCompPtr->UpdateObject(tokenId, tokenPtr);
}


bool CPersonalAccessTokenManagerComp::DeleteToken(const QByteArray& tokenId)
{
	if (!m_tokenCollectionCompPtr.IsValid())
	{
		SendErrorMessage(0, "Token collection not configured", "CPersonalAccessTokenManagerComp");
		return false;
	}
	
	if (!m_tokenCollectionCompPtr->RemoveObject(tokenId))
	{
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
	// Format: imt_pat_<base64url_encoded_random_data>
	// The token should be long enough to be secure (32 bytes = 256 bits of entropy)
	
	QByteArray randomData;
	randomData.resize(32);
	
	// Use cryptographically secure random number generator
	// Note: QRandomGenerator::system() is cryptographically secure on most platforms
	// For critical applications, consider using platform-specific secure RNG
	QRandomGenerator* rng = QRandomGenerator::system();
	
	for (int i = 0; i < randomData.size(); ++i)
	{
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
