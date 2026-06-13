// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CFilterableSelectControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>

// Qt includes
#include <QtCore/QJsonObject.h>

// STL includes
#include <cmath>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/ICollectionInfo.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtcol/CDocumentIdFilter.h>
#include <imtgql/CGqlRequest.h>
#include <imtauthgql/imtauthgql.h>


namespace imtservergql
{


// reimplemented (imtgql::IGqlRequestHandler)

bool CFilterableSelectControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}
		
		if (m_collectionIdAttrPtr.IsValid() && *m_collectionIdAttrPtr != ""){
			QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();
			return *m_collectionIdAttrPtr == collectionId;
		}
	}
	
	return false;
}


sdl::V1_0::imtbase::CGetSelectableItemsPayload CFilterableSelectControllerComp::OnGetSelectableItems(
			const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetSelectableItemsPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Object collection is not set");
		SendErrorMessage(0, errorMessage, "CFilterableSelectControllerComp");
		return response;
	}

	sdl::V1_0::imtbase::GetSelectableItemsRequestArguments arguments = getSelectableItemsRequest.GetRequestedArguments();
	imtsdl::TElementList<sdl::V1_0::imtbase::CSelectableItemData> itemsList;

	// Normal paginated fetch mode
	int offset = 0;
	int count = -1;
	iprm::CParamsSet filterParams;

	if (!m_tenantFilterEnabledAttrPtr.IsValid() || *m_tenantFilterEnabledAttrPtr){
		imtauth::CTenantFilterParam* tenantFilterPtr = imtauthgql::CreateTenantFilterParam(gqlRequest);
		if (tenantFilterPtr != nullptr){
			filterParams.SetEditableParameter("TenantFilter", tenantFilterPtr, true);
		}
	}

	// Exclude selected IDs: create CDocumentIdFilter with CT_NOT_IN
	if (arguments.input && arguments.input->excludeIds && !arguments.input->excludeIds->empty()){
		const auto& excludeIdsList = *arguments.input->excludeIds;
		QByteArrayList documentIds;
		for (size_t i = 0; i < excludeIdsList.size(); ++i){
			QByteArray docId = *excludeIdsList[i];
			if (!docId.isEmpty()){
				documentIds.append(docId);
			}
		}
		if (!documentIds.isEmpty()){
			imtcol::CDocumentIdFilter* documentIdFilterPtr = new imtcol::CDocumentIdFilter();
			documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_NOT_IN);
			documentIdFilterPtr->SetDocumentIds(documentIds);
			filterParams.SetEditableParameter("DocumentIdFilter", documentIdFilterPtr, true);
		}
	}

	if (arguments.input && arguments.input->viewParams){
		auto& viewParams = *arguments.input->viewParams;
		if (viewParams.offset){
			offset = *viewParams.offset;
		}
		if (viewParams.count){
			count = *viewParams.count;
		}

		if (viewParams.filterModel){
			imtbase::CComplexCollectionFilter* complexFilterPtr = new imtbase::CComplexCollectionFilter();
			if (m_complexCollectionFilterRepresentationController.GetDataModelFromSdlRepresentation(*complexFilterPtr, *viewParams.filterModel)){
				filterParams.SetEditableParameter("ComplexFilter", complexFilterPtr, true);
			}
		}
	}

	// Apply additional filters from params set joiners
	for (int i = 0; i < m_filterFillersCompPtr.GetCount(); ++i){
		const IParamsSetJoiner* filterFillerPtr = m_filterFillersCompPtr[i];
		if (filterFillerPtr != nullptr){
			filterFillerPtr->JoinParamsSet(gqlRequest, filterParams);
		}
	}

	istd::TDelPtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, &filterParams));
	if (!iteratorPtr.IsValid()){
		errorMessage = QStringLiteral("Failed to create collection iterator");
		SendErrorMessage(0, errorMessage, "CFilterableSelectControllerComp");
		return response;
	}

	while (iteratorPtr->Next()){
		const QByteArray objectId = iteratorPtr->GetObjectId();
		if (objectId.isEmpty()){
			continue;
		}

		sdl::V1_0::imtbase::CSelectableItemData itemRepresentation;
		itemRepresentation.id = objectId;

		QByteArray objectTypeId = iteratorPtr->GetObjectTypeId();
		if (!objectTypeId.isEmpty()){
			itemRepresentation.typeId = objectTypeId;
		}

		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		itemRepresentation.name = name.isEmpty() ? QString::fromUtf8(objectId) : name;

		QString description = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
		itemRepresentation.description = description;

		// Fill additional parameters from info provider
		iprm::CParamsSet itemParamsSet;
		if (m_objectParamsFillerCompPtr.IsValid()){
			QByteArray contextTenantId;
			const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
			if (gqlContextPtr != nullptr){
				contextTenantId = gqlContextPtr->GetTenantId();
			}
			m_objectParamsFillerCompPtr->FillParams(objectId, itemParamsSet, contextTenantId);
		}

		// Convert iprm::IParamsSet to SDL CParamsSet via representation controller
		if (m_paramSetRepresentationControllerCompPtr.IsValid()){
			QJsonObject paramsJsonObject;
			if (m_paramSetRepresentationControllerCompPtr->GetRepresentationFromDataModel(itemParamsSet, paramsJsonObject)){
				sdl::V1_0::imtbase::CParamsSet sdlParamsSet;
				if (sdlParamsSet.ReadFromJsonObject(paramsJsonObject)){
					itemRepresentation.params = sdlParamsSet;
				}
			}
		}

		itemsList << itemRepresentation;
	}

	response.items = itemsList;

	int elementsCount = iteratorPtr->GetElementsCount();
	int pagesCount = (count > 0) ? static_cast<int>(std::ceil(elementsCount / static_cast<double>(count))) : 1;
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	sdl::V1_0::imtbase::CNotificationItem notification;
	notification.pagesCount = pagesCount;
	notification.totalCount = elementsCount;
	response.notification = notification;

	return response;
}


}

