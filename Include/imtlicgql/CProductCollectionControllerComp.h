#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/CPP/Products/Products.h>


namespace imtlicgql
{


class CProductCollectionControllerComp: public sdl::imtlic::Products::CProductCollectionControllerCompBase
{
public:
	typedef sdl::imtlic::Products::CProductCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductCollectionControllerComp);
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_productInfoFactCompPtr, "ProductFactory", "Factory used for creation of the new product instance", true, "ProductFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtlic::Products::CProductCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtlic::Products::CProductsListGqlRequest& productsListRequest,
				sdl::imtlic::Products::CProductItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Products::CProductData& productDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Products::CProductItemGqlRequest& productItemRequest,
				sdl::imtlic::Products::CProductDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest,
								 const imtbase::CTreeItemModel& objectFilterModel,
								 iprm::CParamsSet& filterParams) const override;
	virtual imtbase::CTreeItemModel* RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_FACT(imtlic::IProductInfo, m_productInfoFactCompPtr);
};


} // namespace imtlicgql


