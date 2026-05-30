// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ISearchController.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Search>


namespace imtservergql
{


class CGqlSearchControllerComp: public sdl::V1_0::imtbase::CSearchGqlHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlSearchControllerComp);
		I_ASSIGN_MULTI_0(m_searchControllersCompPtr, "SearchControllers", "Search controllers", false);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CSearchGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CSearchResults OnSearch(
				const sdl::V1_0::imtbase::CSearchGqlRequest& searchRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_MULTIREF(imtbase::ISearchController, m_searchControllersCompPtr);
};


} // namespace imtservergql


