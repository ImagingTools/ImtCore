// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantConnectionsSelectControllerComp.h>


// ImtCore includes
#include <imtauth/ITenantConnectionInfo.h>
#include "imtbasesdl/SDL/1.0/CPP/FilterableSelect.h"
#include <imtgql/IGqlContext.h>

// Qt includes
#include <QSet>


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

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr || gqlContextPtr->GetTenantId().isEmpty()){
		return sdl::V1_0::imtbase::CGetSelectableItemsPayload();
	}

	const QByteArray currentTenantId = gqlContextPtr->GetTenantId();

	sdl::V1_0::imtbase::CGetSelectableItemsPayload payload = BaseClass::OnGetSelectableItems(getSelectableItemsRequest, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return sdl::V1_0::imtbase::CGetSelectableItemsPayload();
	}

	if (!payload.items.HasValue()){
		return payload;
	}

	QSet<QByteArray> connectedTenantIds;
	for (const QByteArray& connectionId : m_tenantConnectionCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tenantConnectionCollectionCompPtr->GetObjectData(connectionId, dataPtr)){
			continue;
		}

		const imtauth::ITenantConnectionInfo* connectionInfoPtr = dynamic_cast<const imtauth::ITenantConnectionInfo*>(dataPtr.GetPtr());
		if (connectionInfoPtr == nullptr || connectionInfoPtr->GetStatus() != imtauth::ITenantConnectionInfo::CS_ACTIVE){
			continue;
		}

		if (connectionInfoPtr->GetTenantAId() == currentTenantId){
			connectedTenantIds.insert(connectionInfoPtr->GetTenantBId());
		}
		else if (connectionInfoPtr->GetTenantBId() == currentTenantId){
			connectedTenantIds.insert(connectionInfoPtr->GetTenantAId());
		}
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::CSelectableItemData> filteredItems;
	for (const auto& item : payload.items->ToList()){
		if (item.id && connectedTenantIds.contains(*item.id)){
			filteredItems << item;
		}
	}

	payload.items = filteredItems;
	if (payload.notification.HasValue()){
		payload.notification->totalCount = filteredItems.size();
		payload.notification->pagesCount = 1;
	}

	return payload;
}


} // imtauthgql

