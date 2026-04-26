// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CFilterableSelectControllerComp.h>


// STL includes
#include <cmath>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/ICollectionInfo.h>
#include <imtgql/CGqlRequest.h>


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


sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload CFilterableSelectControllerComp::OnGetSelectableItems(
			const sdl::imtbase::FilterableSelect::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::FilterableSelect::CGetSelectableItemsPayload response;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Object collection is not set");
		SendErrorMessage(0, errorMessage, "CFilterableSelectControllerComp");
		return response;
	}

	sdl::imtbase::FilterableSelect::GetSelectableItemsRequestArguments arguments = getSelectableItemsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = QStringLiteral("Invalid request arguments");
		SendErrorMessage(0, errorMessage, "CFilterableSelectControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	imtsdl::TElementList<sdl::imtbase::FilterableSelect::CSelectableItemData::V1_0> itemsList;

	// ID-based fetch mode: when ids are provided, return only those specific items
	if (arguments.input.Version_1_0->ids && !arguments.input.Version_1_0->ids->empty()){
		const auto& requestedIds = *arguments.input.Version_1_0->ids;
		for (size_t i = 0; i < requestedIds.size(); ++i){
			QByteArray objectId = *requestedIds[i];
			if (objectId.isEmpty()){
				continue;
			}

			QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
			if (name.isEmpty()){
				continue;
			}

			sdl::imtbase::FilterableSelect::CSelectableItemData::V1_0 itemRepresentation;
			itemRepresentation.id = objectId;
			itemRepresentation.name = name;
			itemsList << itemRepresentation;
		}

		response.Version_1_0->items = itemsList;
		
		sdl::imtbase::ImtCollection::CNotificationItem::V1_0 notification;
		notification.pagesCount = 1;
		notification.totalCount = static_cast<int>(itemsList.size());
		response.Version_1_0->notification = notification;

		return response;
	}

	// Normal paginated fetch mode
	int offset = 0;
	int count = -1;
	iprm::CParamsSet filterParams;

	if (arguments.input.Version_1_0->viewParams){
		auto& viewParams = *arguments.input.Version_1_0->viewParams;
		if (viewParams.offset){
			offset = *viewParams.offset;
		}
		if (viewParams.count){
			count = *viewParams.count;
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
		
		sdl::imtbase::FilterableSelect::CSelectableItemData::V1_0 itemRepresentation;
		itemRepresentation.id = objectId;
		
		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		itemRepresentation.name = name.isEmpty() ? QString::fromUtf8(objectId) : name;
		
		itemsList << itemRepresentation;
	}

	response.Version_1_0->items = itemsList;

	int elementsCount = iteratorPtr->GetElementsCount();
	int pagesCount = (count > 0) ? static_cast<int>(std::ceil(elementsCount / static_cast<double>(count))) : 1;
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	sdl::imtbase::ImtCollection::CNotificationItem::V1_0 notification;
	notification.pagesCount = pagesCount;
	notification.totalCount = elementsCount;
	response.Version_1_0->notification = notification;

	return response;
}


}


