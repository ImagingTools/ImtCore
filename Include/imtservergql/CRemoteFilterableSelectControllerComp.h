// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect.h>


namespace imtservergql
{


/*!
 * \brief Remote variant of CFilterableSelectControllerComp that delegates
 * filterable select requests to a remote GQL service via TClientRequestManagerCompWrap.
 *
 * Follows the same pattern as CRemoteTenantControllerComp: inherits from
 * TClientRequestManagerCompWrap<CGraphQlHandlerCompBase> and uses SendModelRequest
 * to forward requests through ApiClient (IGqlClient).
 *
 * Does NOT implement any interface — acts purely as a GQL request handler.
 */
class CRemoteFilterableSelectControllerComp:
		public imtclientgql::TClientRequestManagerCompWrap<
						sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteFilterableSelectControllerComp);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID to match incoming requests", true, "");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	// reimplemented (sdl::imtbase::FilterableSelect::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::imtbase::FilterableSelect::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql
