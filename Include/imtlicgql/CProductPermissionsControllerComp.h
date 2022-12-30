#pragma once


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtlicgql
{


class CProductPermissionsControllerComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef imtgql::CGqlRepresentationDataControllerComp BaseClass;

	I_BEGIN_COMPONENT(CProductPermissionsControllerComp);
		I_ASSIGN(m_productProviderCompPtr, "ProductProvider", "Product provider", true, "ProductProvider");
	I_END_COMPONENT;

	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlRequestHandler, m_productProviderCompPtr);
};


} // namespace imtlicgql


