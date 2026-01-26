#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtgql/IGqlRequestProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens.h>


namespace imtauthgql
{


class CPersonalAccessTokenCollectionControllerComp: 
			public sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPersonalAccessTokenCollectionControllerComp);
		I_ASSIGN(m_gqlRequestProviderCompPtr, "GqlRequestProvider", "GraphQL request provider", false, "GqlRequestProvider");
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Personal access token manager", true, "PersonalAccessTokenManager");
		I_ASSIGN(m_tokenFactoryCompPtr, "TokenFactory", "Factory for creating token instances", true, "PersonalAccessTokenFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokensListGqlRequest& listRequest,
				sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenData::V1_0& dataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenItemGqlRequest& itemRequest,
				sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenData::V1_0& representationPayload,
				QString& errorMessage) const override;
	
	virtual sdl::imtauth::PersonalAccessTokens::CCreatePersonalAccessTokenPayload OnCreatePersonalAccessToken(
				const sdl::imtauth::PersonalAccessTokens::CCreatePersonalAccessTokenGqlRequest& request,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	
	virtual sdl::imtauth::PersonalAccessTokens::CRevokePersonalAccessTokenPayload OnRevokePersonalAccessToken(
				const sdl::imtauth::PersonalAccessTokens::CRevokePersonalAccessTokenGqlRequest& request,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	
	virtual sdl::imtauth::PersonalAccessTokens::CValidatePersonalAccessTokenPayload OnValidatePersonalAccessToken(
				const sdl::imtauth::PersonalAccessTokens::CValidatePersonalAccessTokenGqlRequest& request,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	
	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	
	virtual sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	
	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::imtauth::PersonalAccessTokens::CPersonalAccessTokenData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

private:
	I_REF(imtgql::IGqlRequestProvider, m_gqlRequestProviderCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_FACT(imtauth::IPersonalAccessToken, m_tokenFactoryCompPtr);
};


} // namespace imtauthgql


