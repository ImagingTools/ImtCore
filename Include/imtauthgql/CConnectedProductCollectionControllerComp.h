// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CConnectedProduct.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ConnectedProducts_fwd.h>


namespace imtauthgql
{


/**
	The catalog of the products connected to the authorization server. Available to a superuser only.
*/
class CConnectedProductCollectionControllerComp: public sdl::V1_0::imtauth::CConnectedProductCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CConnectedProductCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CConnectedProductCollectionControllerComp);
		I_ASSIGN(m_productFactCompPtr, "ProductFactory", "Factory of the catalog entries", true, "ProductFactory");
		I_ASSIGN(m_tokenManagerCompPtr, "PersonalAccessTokenManager", "Revokes the tokens of a removed product", true, "PersonalAccessTokenManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CConnectedProductCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtauth::CConnectedProductsListGqlRequest& listRequest,
				sdl::V1_0::imtauth::CConnectedProductItemData& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imtauth::CConnectedProductData& representation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::imtauth::CConnectedProductItemGqlRequest& itemRequest,
				sdl::V1_0::imtauth::CConnectedProductData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imtauth::CConnectedProductUpdateGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual bool OnBeforeRemoveElements(const QByteArrayList& elementIds, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_FACT(imtauth::IConnectedProduct, m_productFactCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
};


} // namespace imtauthgql
