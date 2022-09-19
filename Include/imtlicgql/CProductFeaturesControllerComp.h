#pragma once


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>



namespace imtlicgql
{


class CProductFeaturesControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductFeaturesControllerComp);
//		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
	I_END_COMPONENT;

	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
//	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtlicgql


