// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtserverapp/CComplexCollectionFilterRepresentationController.h>
#include <imtservergql/IParamsSetJoiner.h>
#include <imtservergql/CFilterableSelectControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect.h>


namespace imtservergql
{


/*!
 * \brief Remote variant of CFilterableSelectControllerComp that delegates
 * filterable select requests to a slave IGqlRequestHandler (e.g., a remote service).
 *
 * Use this component when the selectable items are provided by another GQL handler
 * (possibly on a different server node) rather than a local IObjectCollection.
 * If a local ObjectCollection is also provided, it serves as a fallback.
 */
class CRemoteFilterableSelectControllerComp:
		public sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteFilterableSelectControllerComp);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID to match incoming requests", true, "");
		I_ASSIGN(m_slaveRequestHandlerCompPtr, "SlaveRequestHandler", "Remote GQL request handler to delegate filterable select requests to", false, "SlaveRequestHandler");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Optional local object collection (fallback if slave handler is not available)", false, "ObjectCollection");
		I_ASSIGN_MULTI_0(m_filterFillersCompPtr, "ParamsSetJoiners", "Optional params set joiners for injecting additional filters", false);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual QJsonObject CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// reimplemented (sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::imtbase::FilterableSelect::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload GetItemsFromLocalCollection(
				const sdl::imtbase::FilterableSelect::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const;

private:
	imtserverapp::CComplexCollectionFilterRepresentationController m_complexCollectionFilterRepresentationController;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_REF(imtgql::IGqlRequestHandler, m_slaveRequestHandlerCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_MULTIREF(IParamsSetJoiner, m_filterFillersCompPtr);
};


} // namespace imtservergql
