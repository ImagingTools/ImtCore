// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect>


namespace imtclientgql
{


class CRemoteFilterableSelectControllerComp:
		public TClientRequestManagerCompWrap<
						sdl::V1_0::imtbase::CFilterableSelectGqlHandlerCompBase>
{
public:
	typedef TClientRequestManagerCompWrap<sdl::V1_0::imtbase::CFilterableSelectGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteFilterableSelectControllerComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collection ID to match incoming requests", true);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	// reimplemented (sdl::V1_0::imtbase::CFilterableSelectGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtclientgql
