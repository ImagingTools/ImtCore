#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtguigql
{


class CPagesDataControllerComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef imtgql::CGqlRepresentationDataControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPagesDataControllerComp);
		I_ASSIGN(m_pagesDataProviderCompPtr, "PagesDataProvider", "List of pages providers for joining", true, "PagesDataProvider");
	I_END_COMPONENT;

	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlRequestHandler, m_pagesDataProviderCompPtr);
};


} // namespace imtguigql


