// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantConnectionsSelectControllerComp.h>


// ImtCore includes
#include "imtbasesdl/SDL/1.0/CPP/FilterableSelect.h"


namespace imtauthgql
{


// reimplemented (imtgql::IGqlRequestHandler)


sdl::V1_0::imtbase::CGetSelectableItemsPayload CTenantConnectionsSelectControllerComp::OnGetSelectableItems(
			const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_tenantConnectionCollectionCompPtr.IsValid()){
		return sdl::V1_0::imtbase::CGetSelectableItemsPayload();
	}

	sdl::V1_0::imtbase::CGetSelectableItemsPayload payload = BaseClass::OnGetSelectableItems(getSelectableItemsRequest, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return sdl::V1_0::imtbase::CGetSelectableItemsPayload();
	}

	return payload;
}


} // imtauthgql


