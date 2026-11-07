// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtauth/IConnectedProduct.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtbase/IObjectCollection.h>
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
		I_ASSIGN(m_snapshotCollectionCompPtr, "SnapshotCollection", "Published permission trees, they give the version of a product", false, "SnapshotCollection");
		I_ASSIGN(m_ownProductIdAttrPtr, "OwnProductId", "This server in its own catalog: the entry is created with the first request and cannot be removed", false, "");
		I_ASSIGN(m_ownProductNameAttrPtr, "OwnProductName", "Name of this server in its own catalog", false, "");
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
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	void EnsureOwnProduct() const;
	QString GetPublishedVersion(const QByteArray& productId) const;
	void FillServerState(
				const QByteArray& productId,
				const sdl::V1_0::imtauth::CConnectedProductsListGqlRequest& listRequest,
				sdl::V1_0::imtauth::CConnectedProductItemData& representationObject) const;

private:
	I_REF(imtbase::IObjectCollection, m_snapshotCollectionCompPtr);
	I_ATTR(QByteArray, m_ownProductIdAttrPtr);
	I_ATTR(QString, m_ownProductNameAttrPtr);

	mutable QMutex m_ownProductMutex;
	mutable bool m_isOwnProductEnsured = false;

	struct PublishedVersion
	{
		QString version;
		qint64 expiresAt = 0;
	};
	mutable QHash<QByteArray, PublishedVersion> m_publishedVersions;
	mutable QMutex m_publishedVersionsMutex;

	I_FACT(imtauth::IConnectedProduct, m_productFactCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
};


} // namespace imtauthgql
