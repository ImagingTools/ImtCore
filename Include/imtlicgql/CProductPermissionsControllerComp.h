#pragma once


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtgql/IItemBasedRepresentationDataProvider.h>


namespace imtlicgql
{


class CProductPermissionsControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductPermissionsControllerComp);
		I_ASSIGN(m_productProviderCompPtr, "ProductProvider", "Product provider", true, "ProductProvider");
	I_END_COMPONENT;

	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IItemBasedRepresentationDataProvider, m_productProviderCompPtr);
};


} // namespace imtlicgql


