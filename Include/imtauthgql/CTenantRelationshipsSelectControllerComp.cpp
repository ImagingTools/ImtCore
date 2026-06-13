 // SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantRelationshipsSelectControllerComp.h>

// Qt includes
#include <QtCore/QJsonObject.h>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtbase/ICollectionInfo.h>
#include "imtbasesdl/SDL/1.0/CPP/FilterableSelect.h"
#include "imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h"


namespace imtauthgql
{


// reimplemented (imtgql::IGqlRequestHandler)


sdl::V1_0::imtbase::CGetSelectableItemsPayload CTenantRelationshipsSelectControllerComp::OnGetSelectableItems(
			const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetSelectableItemsPayload payload;

	if (!m_tenantRelationshipCollectionCompPtr.IsValid()){
		return payload;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr || gqlContextPtr->GetTenantId().isEmpty()){
		return payload;
	}

	const QByteArray currentTenantId = gqlContextPtr->GetTenantId();

	// Extract filter text from request arguments
	QString filterText;
	sdl::V1_0::imtbase::GetSelectableItemsRequestArguments arguments = getSelectableItemsRequest.GetRequestedArguments();
	if (arguments.input && arguments.input->viewParams && arguments.input->viewParams->filterModel){
		auto& filterModel = *arguments.input->viewParams->filterModel;
		if (filterModel.textFilter && filterModel.textFilter->text){
			filterText = *filterModel.textFilter->text;
		}
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::CSelectableItemData> itemsList;

	for (const QByteArray& objectId : m_tenantRelationshipCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tenantRelationshipCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			continue;
		}

		const imtauth::ITenantRelationshipInfo* relationshipInfoPtr = dynamic_cast<const imtauth::ITenantRelationshipInfo*>(dataPtr.GetPtr());
		if (relationshipInfoPtr == nullptr){
			continue;
		}

		if (relationshipInfoPtr->GetStatus() != imtauth::ITenantRelationshipInfo::TRS_ACTIVE){
			continue;
		}

		QByteArray partnerTenantId;
		if (relationshipInfoPtr->GetSourceTenantId() == currentTenantId){
			partnerTenantId = relationshipInfoPtr->GetTargetTenantId();
		}
		else if (relationshipInfoPtr->GetTargetTenantId() == currentTenantId){
			partnerTenantId = relationshipInfoPtr->GetSourceTenantId();
		}
		else{
			continue;
		}

		sdl::V1_0::imtbase::CSelectableItemData item;
		item.id = objectId;

		QString name = m_tenantRelationshipCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		if (name.isEmpty()){
			if (m_tenantCollectionCompPtr.IsValid()){
				name = m_tenantCollectionCompPtr->GetElementInfo(partnerTenantId, imtbase::ICollectionInfo::EIT_NAME).toString();
			}
			if (name.isEmpty()){
				name = QString::fromUtf8(partnerTenantId);
			}
		}
		item.name = name;

		QString description = relationshipInfoPtr->GetDescription();
		if (!description.isEmpty()){
			item.description = description;
		}

		// Fill additional parameters via IObjectParamsFiller
		if (m_objectParamsFillerCompPtr.IsValid() && m_paramSetRepresentationControllerCompPtr.IsValid()){
			iprm::CParamsSet itemParamsSet;
			if (m_objectParamsFillerCompPtr->FillParams(objectId, itemParamsSet, currentTenantId)){
				QJsonObject paramsJsonObject;
				if (m_paramSetRepresentationControllerCompPtr->GetRepresentationFromDataModel(itemParamsSet, paramsJsonObject)){
					sdl::V1_0::imtbase::CParamsSet sdlParamsSet;
					if (sdlParamsSet.ReadFromJsonObject(paramsJsonObject)){
						item.params = sdlParamsSet;
					}
				}
			}
		}

		// Apply text filter
		if (!filterText.isEmpty()){
			bool matchesFilter = name.contains(filterText, Qt::CaseInsensitive)
				|| description.contains(filterText, Qt::CaseInsensitive);
			if (!matchesFilter){
				continue;
			}
		}

		itemsList << item;
	}

	payload.items = itemsList;

	sdl::V1_0::imtbase::CNotificationItem notification;
	notification.pagesCount = 1;
	notification.totalCount = itemsList.size();
	payload.notification = notification;

	return payload;
}


} // imtauthgql
