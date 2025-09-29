#pragma once


// ImtCore includes
#include <imtbase/ISearchController.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Search.h>


namespace imtservergql
{


class CGqlSearchControllerComp: public sdl::imtbase::Search::CGraphQlHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlSearchControllerComp);
		I_ASSIGN_MULTI_0(m_searchControllersCompPtr, "SearchControllers", "Search controllers", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::Search::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::Search::CSearchResults OnSearch(
				const sdl::imtbase::Search::CSearchGqlRequest& searchRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_MULTIREF(imtbase::ISearchController, m_searchControllersCompPtr);
};


} // namespace imtservergql


