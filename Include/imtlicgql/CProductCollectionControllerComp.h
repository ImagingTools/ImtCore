// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products_fwd.h>


namespace imtlicgql
{


class CProductCollectionControllerComp: public sdl::V1_0::imtlic::CProductCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtlic::CProductCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductCollectionControllerComp);
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
		I_ASSIGN(m_productInfoFactCompPtr, "ProductFactory", "Factory used for creation of the new product instance", true, "ProductFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CImportObjectPayload OnImportObject(
				const sdl::V1_0::imtbase::CImportObjectGqlRequest& importObjectRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual void OnAfterSetObjectName(
				const QByteArray& objectId,
				const QString& oldName,
				const QString& newName,
				const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (sdl::V1_0::imtlic::CProductCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtlic::CProductsListGqlRequest& productsListRequest,
				sdl::V1_0::imtlic::CProductItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imtlic::CProductData& productDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::imtlic::CProductItemGqlRequest& productItemRequest,
				sdl::V1_0::imtlic::CProductData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imtlic::CProductUpdateGqlRequest& productUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	virtual QString GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const override;
	virtual QString GetExportFileName(const QByteArray& objectId) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::imtlic::CProductData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
protected:
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_FACT(imtlic::IProductInfo, m_productInfoFactCompPtr);
};


} // namespace imtlicgql


