// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtserverapp/CComplexCollectionFilterRepresentationController.h>
#include <imtservergql/IParamsSetJoiner.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect>


namespace imtservergql
{


class CFilterableSelectControllerComp:
		public sdl::V1_0::imtbase::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFilterableSelectControllerComp);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID to match incoming requests", true, "");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection providing selectable items", true, "ObjectCollection");
		I_ASSIGN(m_tenantFilterEnabledAttrPtr, "TenantFilterEnabled", "Apply tenant filter from request input or GQL context", false, true);
		I_ASSIGN_MULTI_0(m_filterFillersCompPtr, "ParamsSetJoiners", "Optional params set joiners for injecting additional filters", false);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	// reimplemented (sdl::V1_0::imtbase::CGraphQlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	imtserverapp::CComplexCollectionFilterRepresentationController m_complexCollectionFilterRepresentationController;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_ATTR(bool, m_tenantFilterEnabledAttrPtr);
	I_MULTIREF(IParamsSetJoiner, m_filterFillersCompPtr);
};


} // namespace imtservergql
