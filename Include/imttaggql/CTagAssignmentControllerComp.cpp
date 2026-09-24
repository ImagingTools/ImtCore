// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttaggql/CTagAssignmentControllerComp.h>


// ImtCore includes
#include <imtbase/COperationContext.h>
#include <imttag/ITag.h>
#include <imttaggql/imttaggql.h>
#include <GeneratedFiles/imttagsdl/SDL/1.0/CPP/Tags.h>


namespace imttaggql
{


// protected methods

// reimplemented (sdl::V1_0::imttag::CTagsGqlHandlerCompBase)

sdl::V1_0::imttag::CEntityTagsPayload CTagAssignmentControllerComp::OnEntityTagsGet(
			const sdl::V1_0::imttag::CEntityTagsGetGqlRequest& entityTagsGetRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imttag::CEntityTagsPayload response;

	const sdl::V1_0::imttag::EntityTagsGetRequestArguments arguments = entityTagsGetRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->entityType){
		errorMessage = QStringLiteral("Entity type is missing");

		return response;
	}

	const QString entityType = *arguments.input->entityType;
	const QByteArrayList entityIds = arguments.input->entityIds ? arguments.input->entityIds->ToList() : QByteArrayList();

	if (!m_assignmentManagerCompPtr->IsTaggableEntityType(entityType.toUtf8())){
		errorMessage = QStringLiteral("Entity type '%1' is not taggable").arg(entityType);

		return response;
	}

	imtbase::COperationContext operationContext;
	InitOperationContext(gqlRequest, QString(), operationContext);

	const imttag::ITagAssignmentManager::EntityTagIds entityTagIds = m_assignmentManagerCompPtr->GetTagIds(entityType.toUtf8(), entityIds, &operationContext);

	QMap<QByteArray, sdl::V1_0::imttag::CTagData> tagCache;
	QList<sdl::V1_0::imttag::CEntityTags> items;
	for (auto entityIter = entityTagIds.cbegin(); entityIter != entityTagIds.cend(); ++entityIter){
		QList<sdl::V1_0::imttag::CTagData> tags;
		for (const QByteArray& tagId : entityIter.value()){
			if (!tagCache.contains(tagId)){
				imtbase::IObjectCollection::DataPtr dataPtr;
				const imttag::ITag* tagPtr = nullptr;
				if (m_tagCollectionCompPtr->GetObjectData(tagId, dataPtr)){
					tagPtr = dynamic_cast<const imttag::ITag*>(dataPtr.GetPtr());
				}

				if (tagPtr == nullptr){
					continue;
				}

				sdl::V1_0::imttag::CTagData tagData;
				tagData.id = tagId;
				tagData.name = tagPtr->GetName();
				tagData.color = tagPtr->GetColor();
				tagData.description = tagPtr->GetDescription();
				tagData.isSystem = tagPtr->IsSystem();

				tagCache[tagId] = tagData;
			}

			tags << tagCache[tagId];
		}

		sdl::V1_0::imttag::CEntityTags entityTags;
		entityTags.entityId = entityIter.key();
		entityTags.tags.Emplace().FromList(tags);

		items << entityTags;
	}

	response.entityType = entityType;
	response.items.Emplace().FromList(items);

	return response;
}


sdl::V1_0::imttag::CTagsUsagePayload CTagAssignmentControllerComp::OnTagsUsage(
			const sdl::V1_0::imttag::CTagsUsageGqlRequest& tagsUsageRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imttag::CTagsUsagePayload response;

	const sdl::V1_0::imttag::TagsUsageRequestArguments arguments = tagsUsageRequest.GetRequestedArguments();
	const QByteArrayList tagIds = (arguments.input && arguments.input->tagIds) ? arguments.input->tagIds->ToList() : QByteArrayList();

	const QMap<QByteArray, int> usageCounts = m_assignmentManagerCompPtr->GetUsageCounts(tagIds);

	QList<sdl::V1_0::imttag::CTagUsage> items;
	for (auto usageIter = usageCounts.cbegin(); usageIter != usageCounts.cend(); ++usageIter){
		sdl::V1_0::imttag::CTagUsage usage;
		usage.tagId = usageIter.key();
		usage.count = usageIter.value();

		items << usage;
	}

	response.items.Emplace().FromList(items);

	return response;
}


sdl::V1_0::imttag::CEntityTagsChangedPayload CTagAssignmentControllerComp::OnEntityTagsAdd(
			const sdl::V1_0::imttag::CEntityTagsAddGqlRequest& entityTagsAddRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const sdl::V1_0::imttag::EntityTagsAddRequestArguments arguments = entityTagsAddRequest.GetRequestedArguments();
	if (!arguments.input){
		errorMessage = QStringLiteral("Input is missing");

		return sdl::V1_0::imttag::CEntityTagsChangedPayload();
	}

	return ChangeTags(CM_ADD, *arguments.input, gqlRequest, errorMessage);
}


sdl::V1_0::imttag::CEntityTagsChangedPayload CTagAssignmentControllerComp::OnEntityTagsSet(
			const sdl::V1_0::imttag::CEntityTagsSetGqlRequest& entityTagsSetRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const sdl::V1_0::imttag::EntityTagsSetRequestArguments arguments = entityTagsSetRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->entityType || !arguments.input->entityId){
		errorMessage = QStringLiteral("Entity is missing");

		return sdl::V1_0::imttag::CEntityTagsChangedPayload();
	}

	const QString entityType = *arguments.input->entityType;
	const QByteArrayList tagIds = arguments.input->tagIds ? arguments.input->tagIds->ToList() : QByteArrayList();

	imtbase::COperationContext operationContext;
	InitOperationContext(gqlRequest, QStringLiteral("SetTags"), operationContext);

	imttag::ITagAssignmentManager::EntityTagChanges changes;
	if (!m_assignmentManagerCompPtr->SetTags(entityType.toUtf8(), *arguments.input->entityId, tagIds, &operationContext, &changes, &errorMessage)){
		return sdl::V1_0::imttag::CEntityTagsChangedPayload();
	}

	return CreateChangedPayload(entityType, changes);
}


sdl::V1_0::imttag::CEntityTagsChangedPayload CTagAssignmentControllerComp::OnEntityTagRemove(
			const sdl::V1_0::imttag::CEntityTagRemoveGqlRequest& entityTagRemoveRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const sdl::V1_0::imttag::EntityTagRemoveRequestArguments arguments = entityTagRemoveRequest.GetRequestedArguments();
	if (!arguments.input){
		errorMessage = QStringLiteral("Input is missing");

		return sdl::V1_0::imttag::CEntityTagsChangedPayload();
	}

	return ChangeTags(CM_REMOVE, *arguments.input, gqlRequest, errorMessage);
}


sdl::V1_0::imttag::CEntityTagsChangedPayload CTagAssignmentControllerComp::OnEntityTagsClear(
			const sdl::V1_0::imttag::CEntityTagsClearGqlRequest& entityTagsClearRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const sdl::V1_0::imttag::EntityTagsClearRequestArguments arguments = entityTagsClearRequest.GetRequestedArguments();
	if (!arguments.input){
		errorMessage = QStringLiteral("Input is missing");

		return sdl::V1_0::imttag::CEntityTagsChangedPayload();
	}

	return ChangeTags(CM_CLEAR, *arguments.input, gqlRequest, errorMessage);
}


// private methods

sdl::V1_0::imttag::CEntityTagsChangedPayload CTagAssignmentControllerComp::ChangeTags(
			ChangeMode mode,
			const sdl::V1_0::imttag::CEntityTagsChangeInput& input,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!input.entityType){
		errorMessage = QStringLiteral("Entity type is missing");

		return sdl::V1_0::imttag::CEntityTagsChangedPayload();
	}

	const QString entityType = *input.entityType;
	const QByteArrayList entityIds = input.entityIds ? input.entityIds->ToList() : QByteArrayList();
	const QByteArrayList tagIds = input.tagIds ? input.tagIds->ToList() : QByteArrayList();

	imtbase::COperationContext operationContext;
	InitOperationContext(gqlRequest, QStringLiteral("ChangeTags"), operationContext);

	imttag::ITagAssignmentManager::EntityTagChanges changes;
	bool isDone = false;
	switch (mode){
	case CM_ADD:
		isDone = m_assignmentManagerCompPtr->AddTags(entityType.toUtf8(), entityIds, tagIds, &operationContext, &changes, &errorMessage);
		break;

	case CM_REMOVE:
		isDone = m_assignmentManagerCompPtr->RemoveTags(entityType.toUtf8(), entityIds, tagIds, &operationContext, &changes, &errorMessage);
		break;

	case CM_CLEAR:
		isDone = m_assignmentManagerCompPtr->ClearTags(entityType.toUtf8(), entityIds, &operationContext, &changes, &errorMessage);
		break;
	}

	if (!isDone){
		return sdl::V1_0::imttag::CEntityTagsChangedPayload();
	}

	return CreateChangedPayload(entityType, changes);
}


sdl::V1_0::imttag::CEntityTagsChangedPayload CTagAssignmentControllerComp::CreateChangedPayload(
			const QString& entityType,
			const imttag::ITagAssignmentManager::EntityTagChanges& changes)
{
	sdl::V1_0::imttag::CEntityTagsChangedPayload response;
	response.entityType = entityType;

	QList<sdl::V1_0::imttag::CEntityTagChange> changeList;
	for (const imttag::ITagAssignmentManager::EntityTagChange& change : changes){
		sdl::V1_0::imttag::CEntityTagChange changeRepresentation;
		changeRepresentation.entityId = change.entityId;
		changeRepresentation.addedTagIds.Emplace().FromList(change.addedTagIds);
		changeRepresentation.removedTagIds.Emplace().FromList(change.removedTagIds);

		changeList << changeRepresentation;
	}

	response.changes.Emplace().FromList(changeList);

	return response;
}


} // namespace imttaggql


