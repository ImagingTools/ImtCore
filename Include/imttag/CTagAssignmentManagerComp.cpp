// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CTagAssignmentManagerComp.h>


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IOperationContext.h>
#include <imtcol/CDocumentIdFilter.h>


namespace imttag
{


static QByteArrayList UniqueIds(const QByteArrayList& ids)
{
	QByteArrayList retVal;
	QSet<QByteArray> seen;
	for (const QByteArray& id : ids){
		if (!id.isEmpty() && !seen.contains(id)){
			seen.insert(id);
			retVal << id;
		}
	}

	return retVal;
}


// reimplemented (imttag::ITagAssignmentManager)

bool CTagAssignmentManagerComp::AddTags(
			const QByteArray& entityType,
			const QByteArrayList& entityIds,
			const QByteArrayList& tagIds,
			const imtbase::IOperationContext* operationContextPtr,
			EntityTagChanges* changesPtr,
			QString* errorMessagePtr)
{
	return ChangeTags(CM_ADD, entityType, entityIds, tagIds, operationContextPtr, changesPtr, errorMessagePtr);
}


bool CTagAssignmentManagerComp::RemoveTags(
			const QByteArray& entityType,
			const QByteArrayList& entityIds,
			const QByteArrayList& tagIds,
			const imtbase::IOperationContext* operationContextPtr,
			EntityTagChanges* changesPtr,
			QString* errorMessagePtr)
{
	return ChangeTags(CM_REMOVE, entityType, entityIds, tagIds, operationContextPtr, changesPtr, errorMessagePtr);
}


bool CTagAssignmentManagerComp::SetTags(
			const QByteArray& entityType,
			const QByteArray& entityId,
			const QByteArrayList& tagIds,
			const imtbase::IOperationContext* operationContextPtr,
			EntityTagChanges* changesPtr,
			QString* errorMessagePtr)
{
	return ChangeTags(CM_REPLACE, entityType, QByteArrayList() << entityId, tagIds, operationContextPtr, changesPtr, errorMessagePtr);
}


bool CTagAssignmentManagerComp::ClearTags(
			const QByteArray& entityType,
			const QByteArrayList& entityIds,
			const imtbase::IOperationContext* operationContextPtr,
			EntityTagChanges* changesPtr,
			QString* errorMessagePtr)
{
	return ChangeTags(CM_REPLACE, entityType, entityIds, QByteArrayList(), operationContextPtr, changesPtr, errorMessagePtr);
}


ITagAssignmentManager::EntityTagIds CTagAssignmentManagerComp::GetTagIds(
			const QByteArray& entityType,
			const QByteArrayList& entityIds,
			const imtbase::IOperationContext* operationContextPtr) const
{
	EntityTagIds retVal;

	const imtbase::IObjectCollection* entityCollectionPtr = GetEntityCollection(entityType);
	if (entityCollectionPtr == nullptr){
		return retVal;
	}

	const QByteArrayList visibleEntityIds = GetVisibleIds(*entityCollectionPtr, UniqueIds(entityIds), operationContextPtr);
	const QMap<QByteArray, QMap<QByteArray, QByteArray>> assignments = LoadAssignments(entityType, visibleEntityIds);

	QByteArrayList assignedTagIds;
	for (const QMap<QByteArray, QByteArray>& entityAssignments : assignments){
		assignedTagIds << entityAssignments.keys();
	}

	const TagSnapshots visibleTags = LoadVisibleTags(UniqueIds(assignedTagIds), operationContextPtr);

	for (const QByteArray& entityId : visibleEntityIds){
		QByteArrayList entityTagIds;
		const QMap<QByteArray, QByteArray> entityAssignments = assignments.value(entityId);
		for (const QByteArray& tagId : entityAssignments.keys()){
			if (visibleTags.contains(tagId)){
				entityTagIds << tagId;
			}
		}

		retVal[entityId] = entityTagIds;
	}

	return retVal;
}


QMap<QByteArray, int> CTagAssignmentManagerComp::GetUsageCounts(const QByteArrayList& tagIds) const
{
	QMap<QByteArray, int> retVal;

	if (!m_assignmentCollectionCompPtr.IsValid()){
		return retVal;
	}

	for (const QByteArray& tagId : UniqueIds(tagIds)){
		imtbase::IComplexCollectionFilter::FilterExpression filterExpression;
		filterExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("TagId", QString::fromUtf8(tagId));

		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.SetFilterExpression(filterExpression);

		iprm::CParamsSet filterParams;
		filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

		retVal[tagId] = m_assignmentCollectionCompPtr->GetElementsCount(&filterParams);
	}

	return retVal;
}


bool CTagAssignmentManagerComp::IsTaggableEntityType(const QByteArray& entityType) const
{
	return GetEntityCollection(entityType) != nullptr;
}


const imtbase::IObjectCollection* CTagAssignmentManagerComp::GetEntityCollection(const QByteArray& entityType) const
{
	if (entityType.isEmpty()){
		return nullptr;
	}

	const int count = qMin(m_entityTypesCompPtr.GetCount(), m_entityCollectionsCompPtr.GetCount());
	for (int i = 0; i < count; ++i){
		const imtbase::IEntityTypeProvider* entityTypePtr = m_entityTypesCompPtr[i];
		const imtbase::IObjectCollectionProvider* collectionProviderPtr = m_entityCollectionsCompPtr[i];
		if ((entityTypePtr != nullptr) && (collectionProviderPtr != nullptr) && (entityTypePtr->GetEntityTypeId().toUtf8() == entityType)){
			return collectionProviderPtr->GetObjectCollection();
		}
	}

	return nullptr;
}


// reimplemented (imtbase::IEntityTagInfoProvider)

imtbase::IEntityTagInfoProvider::EntityTagInfos CTagAssignmentManagerComp::GetEntityTagInfos(
			const QByteArray& entityType,
			const QByteArrayList& entityIds,
			const imtbase::IOperationContext* operationContextPtr) const
{
	EntityTagInfos retVal;

	const imtbase::IObjectCollection* entityCollectionPtr = GetEntityCollection(entityType);
	if (entityCollectionPtr == nullptr || entityIds.isEmpty()){
		return retVal;
	}

	const QByteArrayList visibleEntityIds = GetVisibleIds(*entityCollectionPtr, UniqueIds(entityIds), operationContextPtr);
	const QMap<QByteArray, QMap<QByteArray, QByteArray>> assignments = LoadAssignments(entityType, visibleEntityIds);

	QByteArrayList assignedTagIds;
	for (const QMap<QByteArray, QByteArray>& entityAssignments : assignments){
		assignedTagIds << entityAssignments.keys();
	}

	const TagSnapshots visibleTags = LoadVisibleTags(UniqueIds(assignedTagIds), operationContextPtr);

	for (auto assignmentIter = assignments.cbegin(); assignmentIter != assignments.cend(); ++assignmentIter){
		TagInfoList tagInfos;
		for (const QByteArray& tagId : assignmentIter.value().keys()){
			auto tagIter = visibleTags.constFind(tagId);
			if (tagIter == visibleTags.cend()){
				continue;
			}

			TagInfo tagInfo;
			tagInfo.id = tagId;
			tagInfo.name = tagIter->name;
			tagInfo.color = tagIter->color;
			tagInfo.isSystem = tagIter->isSystem;

			tagInfos << tagInfo;
		}

		if (!tagInfos.isEmpty()){
			retVal[assignmentIter.key()] = tagInfos;
		}
	}

	return retVal;
}


// private methods

bool CTagAssignmentManagerComp::ChangeTags(
			ChangeMode mode,
			const QByteArray& entityType,
			const QByteArrayList& entityIds,
			const QByteArrayList& tagIds,
			const imtbase::IOperationContext* operationContextPtr,
			EntityTagChanges* changesPtr,
			QString* errorMessagePtr)
{
	if (!m_tagCollectionCompPtr.IsValid() || !m_assignmentCollectionCompPtr.IsValid() || !m_assignmentFactoryCompPtr.IsValid()){
		return SetError(errorMessagePtr, QStringLiteral("Tag assignment manager is not configured"));
	}

	const imtbase::IObjectCollection* entityCollectionPtr = GetEntityCollection(entityType);
	if (entityCollectionPtr == nullptr){
		return SetError(errorMessagePtr, QStringLiteral("Entity type '%1' is not taggable").arg(QString::fromUtf8(entityType)));
	}

	const QByteArrayList uniqueEntityIds = UniqueIds(entityIds);
	if (uniqueEntityIds.isEmpty()){
		return SetError(errorMessagePtr, QStringLiteral("No entities given"));
	}

	const QByteArrayList visibleEntityIds = GetVisibleIds(*entityCollectionPtr, uniqueEntityIds, operationContextPtr);
	if (visibleEntityIds.count() != uniqueEntityIds.count()){
		return SetError(errorMessagePtr, QStringLiteral("One or more entities do not exist"));
	}

	const QByteArrayList requestedTagIds = UniqueIds(tagIds);
	if ((mode != CM_REPLACE) && requestedTagIds.isEmpty()){
		return SetError(errorMessagePtr, QStringLiteral("No tags given"));
	}

	const QMap<QByteArray, QMap<QByteArray, QByteArray>> assignments = LoadAssignments(entityType, uniqueEntityIds);

	QByteArrayList involvedTagIds = requestedTagIds;
	for (const QMap<QByteArray, QByteArray>& entityAssignments : assignments){
		involvedTagIds << entityAssignments.keys();
	}

	// Assignments of deleted or foreign tags are invisible and stay untouched.
	const TagSnapshots visibleTags = LoadVisibleTags(UniqueIds(involvedTagIds), operationContextPtr);
	for (const QByteArray& tagId : requestedTagIds){
		if (!visibleTags.contains(tagId)){
			return SetError(errorMessagePtr, QStringLiteral("Tag '%1' does not exist").arg(QString::fromUtf8(tagId)));
		}
	}

	EntityTagChanges changes;
	for (const QByteArray& entityId : uniqueEntityIds){
		const QMap<QByteArray, QByteArray> entityAssignments = assignments.value(entityId);

		EntityTagChange change;
		change.entityId = entityId;

		for (const QByteArray& tagId : requestedTagIds){
			const bool isAssigned = entityAssignments.contains(tagId);
			if ((mode == CM_ADD || mode == CM_REPLACE) && !isAssigned){
				change.addedTagIds << tagId;
			}
			else if ((mode == CM_REMOVE) && isAssigned){
				change.removedTagIds << tagId;
			}
		}

		if (mode == CM_REPLACE){
			for (const QByteArray& tagId : entityAssignments.keys()){
				if (visibleTags.contains(tagId) && !requestedTagIds.contains(tagId)){
					change.removedTagIds << tagId;
				}
			}
		}

		if (!change.addedTagIds.isEmpty() || !change.removedTagIds.isEmpty()){
			changes << change;
		}
	}

	if (changes.isEmpty()){
		if (changesPtr != nullptr){
			changesPtr->clear();
		}

		return true;
	}

	const bool useTransaction = m_transactionManagerCompPtr.IsValid() && m_transactionManagerCompPtr->StartTransaction();

	bool retVal = ApplyChanges(entityType, changes, assignments, operationContextPtr);

	if (useTransaction){
		if (retVal){
			retVal = m_transactionManagerCompPtr->EndTransaction();
		}
		else{
			m_transactionManagerCompPtr->CancelTransaction();
		}
	}

	if (!retVal){
		return SetError(errorMessagePtr, QStringLiteral("Tag assignments could not be stored"));
	}

	if (changesPtr != nullptr){
		*changesPtr = changes;
	}

	QVariantList changeList;
	for (const EntityTagChange& change : changes){
		QVariantMap changeMap;
		changeMap["EntityId"] = change.entityId;
		changeMap["AddedTagIds"] = QVariant::fromValue(change.addedTagIds);
		changeMap["RemovedTagIds"] = QVariant::fromValue(change.removedTagIds);
		changeList << changeMap;
	}

	istd::IChangeable::ChangeSet changeSet(CF_ASSIGNMENTS_CHANGED);
	changeSet.SetChangeInfo("EntityType", entityType);
	changeSet.SetChangeInfo("TenantId", (operationContextPtr != nullptr) ? operationContextPtr->GetTenantId() : QByteArray());
	changeSet.SetChangeInfo("Changes", changeList);
	istd::CChangeNotifier notifier(this, &changeSet);

	return true;
}


bool CTagAssignmentManagerComp::ApplyChanges(
			const QByteArray& entityType,
			const EntityTagChanges& changes,
			const QMap<QByteArray, QMap<QByteArray, QByteArray>>& assignmentIds,
			const imtbase::IOperationContext* operationContextPtr)
{
	const QByteArray userId = (operationContextPtr != nullptr) ? operationContextPtr->GetOperationOwnerId().id : QByteArray();
	const QDateTime now = QDateTime::currentDateTimeUtc();

	imtbase::ICollectionInfo::Ids idsToRemove;
	for (const EntityTagChange& change : changes){
		for (const QByteArray& tagId : change.removedTagIds){
			const QByteArray assignmentId = assignmentIds.value(change.entityId).value(tagId);
			if (!assignmentId.isEmpty()){
				idsToRemove << assignmentId;
			}
		}
	}

	if (!idsToRemove.isEmpty() && !m_assignmentCollectionCompPtr->RemoveElements(idsToRemove, operationContextPtr)){
		SendErrorMessage(0, QStringLiteral("Tag assignments could not be removed"));

		return false;
	}

	for (const EntityTagChange& change : changes){
		for (const QByteArray& tagId : change.addedTagIds){
			ITagAssignmentUniquePtr assignmentPtr = m_assignmentFactoryCompPtr.CreateInstance();
			if (!assignmentPtr.IsValid()){
				return false;
			}

			assignmentPtr->SetTagId(tagId);
			assignmentPtr->SetEntityType(entityType);
			assignmentPtr->SetEntityId(change.entityId);
			assignmentPtr->SetCreatedAt(now);
			assignmentPtr->SetCreatedByUserId(userId);

			const QByteArray newId = m_assignmentCollectionCompPtr->InsertNewObject(
						"TagAssignment",
						QString(),
						QString(),
						assignmentPtr.GetPtr(),
						QByteArray(),
						nullptr,
						nullptr,
						operationContextPtr);
			if (newId.isEmpty()){
				SendErrorMessage(0, QStringLiteral("Tag '%1' could not be assigned to '%2'").arg(QString::fromUtf8(tagId), QString::fromUtf8(change.entityId)));

				return false;
			}
		}
	}

	return true;
}


QMap<QByteArray, QMap<QByteArray, QByteArray>> CTagAssignmentManagerComp::LoadAssignments(
			const QByteArray& entityType,
			const QByteArrayList& entityIds) const
{
	QMap<QByteArray, QMap<QByteArray, QByteArray>> retVal;

	if (!m_assignmentCollectionCompPtr.IsValid() || entityIds.isEmpty()){
		return retVal;
	}

	imtbase::IComplexCollectionFilter::FilterExpression entityExpression;
	entityExpression.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
	for (const QByteArray& entityId : entityIds){
		entityExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityId", QString::fromUtf8(entityId));
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpression;
	filterExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityType", QString::fromUtf8(entityType));
	filterExpression.filterExpressions << entityExpression;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpression);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	const imtbase::ICollectionInfo::Ids assignmentIds = m_assignmentCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	for (const QByteArray& assignmentId : assignmentIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_assignmentCollectionCompPtr->GetObjectData(assignmentId, dataPtr)){
			continue;
		}

		const ITagAssignment* assignmentPtr = dynamic_cast<const ITagAssignment*>(dataPtr.GetPtr());
		if (assignmentPtr != nullptr){
			retVal[assignmentPtr->GetEntityId()][assignmentPtr->GetTagId()] = assignmentId;
		}
	}

	return retVal;
}


CTagAssignmentManagerComp::TagSnapshots CTagAssignmentManagerComp::LoadVisibleTags(
			const QByteArrayList& tagIds,
			const imtbase::IOperationContext* operationContextPtr) const
{
	TagSnapshots retVal;

	if (!m_tagCollectionCompPtr.IsValid() || tagIds.isEmpty()){
		return retVal;
	}

	const QByteArrayList visibleTagIds = GetVisibleIds(*m_tagCollectionCompPtr, tagIds, operationContextPtr);
	for (const QByteArray& tagId : visibleTagIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_tagCollectionCompPtr->GetObjectData(tagId, dataPtr)){
			continue;
		}

		const ITag* tagPtr = dynamic_cast<const ITag*>(dataPtr.GetPtr());
		if (tagPtr != nullptr){
			TagSnapshot snapshot;
			snapshot.name = tagPtr->GetName();
			snapshot.color = tagPtr->GetColor();
			snapshot.isSystem = tagPtr->IsSystem();

			retVal[tagId] = snapshot;
		}
	}

	return retVal;
}


QByteArrayList CTagAssignmentManagerComp::GetVisibleIds(
			const imtbase::IObjectCollection& collection,
			const QByteArrayList& ids,
			const imtbase::IOperationContext* operationContextPtr) const
{
	if (ids.isEmpty()){
		return QByteArrayList();
	}

	iprm::CParamsSet scopeParams;
	InitScopeParams(scopeParams, ids, operationContextPtr);

	// Collections that ignore DocumentIdFilter return everything they see.
	const imtbase::ICollectionInfo::Ids foundIds = collection.GetElementIds(0, -1, &scopeParams);
	const QSet<QByteArray> foundIdSet(foundIds.cbegin(), foundIds.cend());

	QByteArrayList retVal;
	for (const QByteArray& id : ids){
		if (foundIdSet.contains(id)){
			retVal << id;
		}
	}

	return retVal;
}


void CTagAssignmentManagerComp::InitScopeParams(
			iprm::CParamsSet& paramsSet,
			const QByteArrayList& ids,
			const imtbase::IOperationContext* operationContextPtr) const
{
	imtcol::CDocumentIdFilter* documentIdFilterPtr = new imtcol::CDocumentIdFilter;
	documentIdFilterPtr->SetDocumentIds(ids);
	documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_IN);
	paramsSet.SetEditableParameter("DocumentIdFilter", documentIdFilterPtr, true);

	if (operationContextPtr != nullptr){
		imtauth::CTenantFilterParam* tenantFilterPtr = new imtauth::CTenantFilterParam;
		tenantFilterPtr->SetTenantId(operationContextPtr->GetTenantId());
		paramsSet.SetEditableParameter("TenantFilter", tenantFilterPtr, true);
	}
}


bool CTagAssignmentManagerComp::SetError(QString* errorMessagePtr, const QString& message) const
{
	if (errorMessagePtr != nullptr){
		*errorMessagePtr = message;
	}

	return false;
}


} // namespace imttag


