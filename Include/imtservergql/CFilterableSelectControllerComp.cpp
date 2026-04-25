// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CFilterableSelectControllerComp.h>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/ICollectionInfo.h>


namespace imtservergql
{


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
				m_objectCollectionCompPtr->CreateObjectCollectionIterator(offset, count, &filterParams));

	if (!iteratorPtr.IsValid()){
		errorMessage = QStringLiteral("Failed to create collection iterator");
		SendErrorMessage(0, errorMessage, "CFilterableSelectControllerComp");
		return response;
	}

	imtsdl::TElementList<sdl::imtbase::FilterableSelect::CSelectableItemData::V1_0> itemsList;

	do {
		const QByteArray objectId = iteratorPtr->GetObjectId();
		if (objectId.isEmpty()){
			continue;
		}

		sdl::imtbase::FilterableSelect::CSelectableItemData::V1_0 itemRepresentation;
		itemRepresentation.id = objectId;

		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		itemRepresentation.name = name.isEmpty() ? QString::fromUtf8(objectId) : name;

		itemsList << itemRepresentation;
	} while (iteratorPtr->Next());

	response.Version_1_0->items = itemsList;

	sdl::imtbase::ImtCollection::CNotificationItem::V1_0 notification;
	notification.pagesCount = iteratorPtr->GetElementsCount();
	response.Version_1_0->notification = notification;

	return response;
}
