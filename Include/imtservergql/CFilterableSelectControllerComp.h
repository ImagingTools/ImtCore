// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect.h>


namespace imtservergql
{


class CFilterableSelectControllerComp:
		public sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFilterableSelectControllerComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection providing selectable items", true, "ObjectCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::imtbase::FilterableSelect::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtservergql
