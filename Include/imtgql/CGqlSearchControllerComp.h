#pragma once


// ImtCore includes
#include <imtbase/ISearchController.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CGqlSearchControllerComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlSearchControllerComp);
		I_ASSIGN_MULTI_0(m_searchControllersCompPtr, "SearchControllers", "Search controllers", false);
		I_ASSIGN(m_optionListCompPtr, "SearchControllersInfos", "Search controllers infos", false, "SearchControllersInfos");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_MULTIREF(imtbase::ISearchController, m_searchControllersCompPtr);
	I_REF(iprm::IOptionsList, m_optionListCompPtr);
};


} // namespace imtgql


