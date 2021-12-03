#pragma once

// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtguigql
{


class CGetStyleDataControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGetStyleDataControllerComp);
	I_END_COMPONENT;


// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtguigql
