// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttaggql/CTagSelectControllerComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imttag/ITag.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace imttaggql
{


const QByteArray CTagSelectControllerComp::s_isSystemParamId = QByteArrayLiteral("IsSystem");
const QByteArray CTagSelectControllerComp::s_usageCountParamId = QByteArrayLiteral("UsageCount");


// protected methods

// reimplemented (imtservergql::CFilterableSelectControllerComp)

sdl::V1_0::imtbase::CGetSelectableItemsPayload CTagSelectControllerComp::OnGetSelectableItems(
			const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_systemTagSeederCompPtr.IsValid()){
		m_systemTagSeederCompPtr->EnsureSystemTags();
	}

	sdl::V1_0::imtbase::CGetSelectableItemsPayload response = BaseClass::OnGetSelectableItems(getSelectableItemsRequest, gqlRequest, errorMessage);
	if (!m_assignmentManagerCompPtr.IsValid() || !response.items){
		return response;
	}

	QByteArrayList tagIds;
	for (const istd::TNullableValue<sdl::V1_0::imtbase::CSelectableItemData>& item: *response.items){
		if (item && item->id){
			tagIds << *item->id;
		}
	}

	const QMap<QByteArray, int> usageCounts = m_assignmentManagerCompPtr->GetUsageCounts(tagIds);
	for (istd::TNullableValue<sdl::V1_0::imtbase::CSelectableItemData>& item: *response.items){
		if (item && item->id){
			AddTextParameter(*item, s_usageCountParamId, QString::number(usageCounts.value(*item->id, 0)));
		}
	}

	return response;
}


void CTagSelectControllerComp::OnSelectableItemCreated(
			const imtbase::IObjectCollectionIterator& iterator,
			sdl::V1_0::imtbase::CSelectableItemData& itemRepresentation,
			const ::imtgql::CGqlRequest& /*gqlRequest*/) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!iterator.GetObjectData(dataPtr)){
		return;
	}

	const imttag::ITag* tagPtr = dynamic_cast<const imttag::ITag*>(dataPtr.GetPtr());
	if (tagPtr == nullptr){
		return;
	}

	itemRepresentation.color = tagPtr->GetColor();

	AddTextParameter(itemRepresentation, s_isSystemParamId, tagPtr->IsSystem() ? QStringLiteral("true") : QStringLiteral("false"));
}


// private static methods

void CTagSelectControllerComp::AddTextParameter(sdl::V1_0::imtbase::CSelectableItemData& itemRepresentation, const QByteArray& parameterId, const QString& text)
{
	QJsonObject textParamObject;
	textParamObject[QStringLiteral("text")] = text;

	sdl::V1_0::imtbase::CParameter parameter;
	parameter.id = parameterId;
	parameter.typeId = QByteArrayLiteral("TextParam");
	parameter.name = QString::fromUtf8(parameterId);
	parameter.data = QString::fromUtf8(QJsonDocument(textParamObject).toJson(QJsonDocument::Compact));

	sdl::V1_0::imtbase::CParamsSet paramsSet;
	if (itemRepresentation.params){
		paramsSet = *itemRepresentation.params;
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::CParameter> parameters;
	if (paramsSet.parameters){
		parameters = *paramsSet.parameters;
	}

	parameters.append(istd::TNullableValue<sdl::V1_0::imtbase::CParameter>(parameter));
	paramsSet.parameters = parameters;
	itemRepresentation.params = paramsSet;
}


} // namespace imttaggql


