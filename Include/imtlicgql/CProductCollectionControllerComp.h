#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products.h>


namespace imtlicgql
{


class CProductCollectionControllerComp: public sdl::imtlic::Products::V1_0::CProductCollectionControllerCompBase
{
public:
	typedef sdl::imtlic::Products::V1_0::CProductCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductCollectionControllerComp);
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
		I_ASSIGN(m_productInfoFactCompPtr, "ProductFactory", "Factory used for creation of the new product instance", true, "ProductFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtlic::Products::V1_0::CProductCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtlic::Products::V1_0::CProductsListGqlRequest& productsListRequest,
				sdl::imtlic::Products::V1_0::CProductItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Products::V1_0::CProductData& productDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Products::V1_0::CProductItemGqlRequest& productItemRequest,
				sdl::imtlic::Products::V1_0::CProductDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest,
								 const imtbase::CTreeItemModel& objectFilterModel,
								 iprm::CParamsSet& filterParams) const override;
	virtual imtbase::CTreeItemModel* RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ImportObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QString GetExtensionFromMimeType(const imtbase::CMimeType& mimeType) const override;
	virtual QString GetExportFileName(const QByteArray& objectId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_FACT(imtlic::IProductInfo, m_productInfoFactCompPtr);
};


} // namespace imtlicgql


