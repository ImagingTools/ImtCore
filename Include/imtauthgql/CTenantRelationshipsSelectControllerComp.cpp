 // SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantRelationshipsSelectControllerComp.h>


// Qt includes
#include <QSet>

// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>
#include "imtbasesdl/SDL/1.0/CPP/FilterableSelect.h"


namespace imtauthgql
{


// reimplemented (imtgql::IGqlRequestHandler)


sdl::V1_0::imtbase::CGetSelectableItemsPayload CTenantRelationshipsSelectControllerComp::OnGetSelectableItems(
			const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_tenantRelationshipCollectionCompPtr.IsValid()){
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

	QSet<QByteArray> relatedTenantIds;
	for (const QByteArray& relationshipObjectId : m_tenantRelationshipCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tenantRelationshipCollectionCompPtr->GetObjectData(relationshipObjectId, dataPtr)){
			continue;
		}

		const imtauth::ITenantRelationshipInfo* relationshipInfoPtr = dynamic_cast<const imtauth::ITenantRelationshipInfo*>(dataPtr.GetPtr());
		if (relationshipInfoPtr == nullptr){
			continue;
		}

		if (relationshipInfoPtr->GetSourceTenantId() == currentTenantId && !relationshipInfoPtr->GetTargetTenantId().isEmpty()){
			relatedTenantIds.insert(relationshipInfoPtr->GetTargetTenantId());
		}
		else if (relationshipInfoPtr->GetTargetTenantId() == currentTenantId && !relationshipInfoPtr->GetSourceTenantId().isEmpty()){
			relatedTenantIds.insert(relationshipInfoPtr->GetSourceTenantId());
		}
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::CSelectableItemData> filteredItems;
	for (const auto& item : payload.items->ToList()){
		if (item.id && relatedTenantIds.contains(*item.id)){
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
