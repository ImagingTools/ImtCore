#include <imtauthgql/CPersonalAccessTokenCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenCollectionControllerCompBase)

bool CPersonalAccessTokenCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokensListGqlRequest& listRequest,
			sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenItemData::V1_0& representationObject,
			QString& errorMessage) const
{
	const imtauth::IPersonalAccessToken* tokenPtr = 
		dynamic_cast<const imtauth::IPersonalAccessToken*>(objectCollectionIterator.GetObjectData().GetPtr());
	
	if (!tokenPtr)
	{
		errorMessage = "Invalid token object";
		return false;
	}
	
	representationObject.id = tokenPtr->GetId();
	representationObject.typeId = "PersonalAccessToken";
	representationObject.userId = tokenPtr->GetUserId();
	representationObject.name = tokenPtr->GetName();
	representationObject.description = tokenPtr->GetDescription();
	representationObject.scopes = tokenPtr->GetScopes();
	representationObject.createdAt = tokenPtr->GetCreatedAt().toString(Qt::ISODate);
	
	if (tokenPtr->GetLastUsedAt().isValid())
	{
		representationObject.lastUsedAt = tokenPtr->GetLastUsedAt().toString(Qt::ISODate);
	}
	
	if (tokenPtr->GetExpiresAt().isValid())
	{
		representationObject.expiresAt = tokenPtr->GetExpiresAt().toString(Qt::ISODate);
	}
	
	representationObject.revoked = tokenPtr->IsRevoked();
	
	return true;
}


istd::IChangeableUniquePtr CPersonalAccessTokenCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenData::V1_0& dataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_tokenFactoryCompPtr.IsValid())
	{
		errorMessage = "Token factory not configured";
		return nullptr;
	}
	
	imtauth::IPersonalAccessTokenUniquePtr tokenPtr = m_tokenFactoryCompPtr->Create();
	if (!tokenPtr)
	{
		errorMessage = "Failed to create token object";
		return nullptr;
	}
	
	if (!FillObjectFromRepresentation(dataRepresentation, *tokenPtr, newObjectId, errorMessage))
	{
		return nullptr;
	}
	
	return istd::IChangeableUniquePtr(tokenPtr.Release());
}


bool CPersonalAccessTokenCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenItemGqlRequest& itemRequest,
			sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenData::V1_0& representationPayload,
			QString& errorMessage) const
{
	const imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<const imtauth::IPersonalAccessToken*>(&data);
	
	if (!tokenPtr)
	{
		errorMessage = "Invalid token object";
		return false;
	}
	
	representationPayload.id = tokenPtr->GetId();
	representationPayload.userId = tokenPtr->GetUserId();
	representationPayload.name = tokenPtr->GetName();
	representationPayload.description = tokenPtr->GetDescription();
	representationPayload.scopes = tokenPtr->GetScopes();
	representationPayload.createdAt = tokenPtr->GetCreatedAt().toString(Qt::ISODate);
	
	if (tokenPtr->GetLastUsedAt().isValid())
	{
		representationPayload.lastUsedAt = tokenPtr->GetLastUsedAt().toString(Qt::ISODate);
	}
	
	if (tokenPtr->GetExpiresAt().isValid())
	{
		representationPayload.expiresAt = tokenPtr->GetExpiresAt().toString(Qt::ISODate);
	}
	
	representationPayload.revoked = tokenPtr->IsRevoked();
	
	return true;
}


sdl::imtauth::PersonalAccessTokens::CCreatePersonalAccessTokenPayload 
CPersonalAccessTokenCollectionControllerComp::OnCreatePersonalAccessToken(
			const sdl::imtauth::PersonalAccessTokens::CCreatePersonalAccessTokenGqlRequest& request,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CCreatePersonalAccessTokenPayload payload;
	payload.success = false;
	
	if (!m_tokenManagerCompPtr.IsValid())
	{
		errorMessage = "Token manager not configured";
		payload.message = errorMessage;
		return payload;
	}
	
	QDateTime expiresAt;
	if (!request.input.expiresAt.isEmpty())
	{
		expiresAt = QDateTime::fromString(request.input.expiresAt, Qt::ISODate);
		if (!expiresAt.isValid())
		{
			errorMessage = "Invalid expiration date format";
			payload.message = errorMessage;
			return payload;
		}
	}
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result = 
		m_tokenManagerCompPtr->CreateToken(
			request.input.userId,
			request.input.name,
			request.input.description,
			request.input.scopes,
			expiresAt);
	
	if (!result.success)
	{
		errorMessage = "Failed to create token";
		payload.message = errorMessage;
		return payload;
	}
	
	payload.id = result.tokenId;
	payload.token = result.rawToken;
	payload.success = true;
	payload.message = "Token created successfully. Save this token - it will not be shown again.";
	
	return payload;
}


sdl::imtauth::PersonalAccessTokens::CRevokePersonalAccessTokenPayload 
CPersonalAccessTokenCollectionControllerComp::OnRevokePersonalAccessToken(
			const sdl::imtauth::PersonalAccessTokens::CRevokePersonalAccessTokenGqlRequest& request,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CRevokePersonalAccessTokenPayload payload;
	payload.success = false;
	
	if (!m_tokenManagerCompPtr.IsValid())
	{
		errorMessage = "Token manager not configured";
		payload.message = errorMessage;
		return payload;
	}
	
	if (!m_tokenManagerCompPtr->RevokeToken(request.input.id))
	{
		errorMessage = "Failed to revoke token";
		payload.message = errorMessage;
		return payload;
	}
	
	payload.success = true;
	payload.message = "Token revoked successfully";
	
	return payload;
}


sdl::imtauth::PersonalAccessTokens::CValidatePersonalAccessTokenPayload 
CPersonalAccessTokenCollectionControllerComp::OnValidatePersonalAccessToken(
			const sdl::imtauth::PersonalAccessTokens::CValidatePersonalAccessTokenGqlRequest& request,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::PersonalAccessTokens::CValidatePersonalAccessTokenPayload payload;
	payload.valid = false;
	
	if (!m_tokenManagerCompPtr.IsValid())
	{
		errorMessage = "Token manager not configured";
		payload.message = errorMessage;
		return payload;
	}
	
	QByteArray userId;
	QByteArrayList scopes;
	
	if (m_tokenManagerCompPtr->ValidateToken(request.input.token, userId, scopes))
	{
		payload.valid = true;
		payload.userId = userId;
		payload.scopes = scopes;
		payload.message = "Token is valid";
		
		// Update last used timestamp
		// Extract token ID from hash - for now we just validate
		// In production, you might want to update last used time here
	}
	else
	{
		payload.message = "Token is invalid, expired, or revoked";
	}
	
	return payload;
}


// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus 
CPersonalAccessTokenCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CVisualStatus retVal;
	retVal.visible = true;
	retVal.enabled = true;
	return retVal;
}


sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload 
CPersonalAccessTokenCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return BaseClass::OnGetElementMetaInfo(getElementMetaInfoRequest, gqlRequest, errorMessage);
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CPersonalAccessTokenCollectionControllerComp::CheckPermissions(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	// For now, allow all authenticated users to manage their own tokens
	// In production, you should check that users can only manage their own tokens
	return true;
}


// private methods

bool CPersonalAccessTokenCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenData::V1_0& representation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<imtauth::IPersonalAccessToken*>(&object);
	
	if (!tokenPtr)
	{
		errorMessage = "Invalid token object";
		return false;
	}
	
	tokenPtr->SetId(representation.id);
	tokenPtr->SetUserId(representation.userId);
	tokenPtr->SetName(representation.name);
	tokenPtr->SetDescription(representation.description);
	tokenPtr->SetScopes(representation.scopes);
	
	if (!representation.createdAt.isEmpty())
	{
		QDateTime createdAt = QDateTime::fromString(representation.createdAt, Qt::ISODate);
		if (createdAt.isValid())
		{
			tokenPtr->SetCreatedAt(createdAt);
		}
	}
	
	if (!representation.lastUsedAt.isEmpty())
	{
		QDateTime lastUsedAt = QDateTime::fromString(representation.lastUsedAt, Qt::ISODate);
		if (lastUsedAt.isValid())
		{
			tokenPtr->SetLastUsedAt(lastUsedAt);
		}
	}
	
	if (!representation.expiresAt.isEmpty())
	{
		QDateTime expiresAt = QDateTime::fromString(representation.expiresAt, Qt::ISODate);
		if (expiresAt.isValid())
		{
			tokenPtr->SetExpiresAt(expiresAt);
		}
	}
	
	tokenPtr->SetRevoked(representation.revoked);
	
	objectId = representation.id;
	
	return true;
}


} // namespace imtauthgql
