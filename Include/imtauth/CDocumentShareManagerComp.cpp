// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CDocumentShareManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/imtbase.h>


namespace imtauth
{


// reimplemented (imtauth::IDocumentShareManager)

QByteArray CDocumentShareManagerComp::ShareDocument(const IDocumentShareInfo& shareInfo, QString* errorMessagePtr)
{
	QString errorMessage;
	QString ownerTenantName;
	QString targetTenantName;
	QDateTime relatedSince;

	const QByteArray ownerTenantId = shareInfo.GetOwnerTenantId();
	const QByteArray targetTenantId = shareInfo.GetTargetTenantId();
	const IDocumentShareInfo::AccessLevel accessLevel = shareInfo.GetAccessLevel();

	if (shareInfo.GetEntityId().isEmpty() || ownerTenantId.isEmpty() || targetTenantId.isEmpty()){
		errorMessage = QStringLiteral("Document, owner organization and target organization are required");
	}
	else if (ownerTenantId == targetTenantId){
		errorMessage = QStringLiteral("A document cannot be shared with its own organization");
	}
	else if (accessLevel != IDocumentShareInfo::DAL_VIEW && accessLevel != IDocumentShareInfo::DAL_EDIT){
		errorMessage = QStringLiteral("Invalid access level");
	}
	else{
		// An incomplete history gives a start too late, and the existing share would be taken for one of an earlier relationship
		bool isRelationshipRead = false;
		const RelatedTenantInfos relatedTenants = m_relationshipProviderCompPtr->GetRelatedTenants(ownerTenantId, &ownerTenantName, &isRelationshipRead);
		bool isRelated = false;
		bool isEffective = false;
		for (const RelatedTenantInfo& relatedTenant : relatedTenants){
			if (relatedTenant.tenantId == targetTenantId){
				targetTenantName = relatedTenant.tenantName;
				relatedSince = relatedTenant.relatedSince;
				isRelated = true;
				isEffective = relatedTenant.isEffective;
				break;
			}
		}

		if (!isRelationshipRead){
			errorMessage = QStringLiteral("The relationships of the organization could not be read. Try again.");
		}
		else if (!isRelated){
			errorMessage = QStringLiteral("The organizations have no active relationship");
		}
		else if (!isEffective){
			errorMessage = QStringLiteral("The relationship of the organizations is not valid at this time");
		}
	}

	if (!errorMessage.isEmpty()){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = errorMessage;
		}

		return QByteArray();
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	const IDocumentShareInfo::ShareStatus initialStatus = *m_autoAcceptAttrPtr ? IDocumentShareInfo::DSS_ACTIVE : IDocumentShareInfo::DSS_PENDING;

	// A share that cannot be read may be the existing one: a second share beside it is not created
	bool isShareRead = false;
	IDocumentShareInfoSharedPtr storedPtr;
	for (const IDocumentShareInfoSharedPtr& existingPtr : FindShares("EntityId", shareInfo.GetEntityId(), &isShareRead)){
		if (existingPtr->GetTargetTenantId() == targetTenantId){
			storedPtr = existingPtr;
			break;
		}
	}

	if (!isShareRead){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QStringLiteral("The shares of the document could not be read. Try again.");
		}

		return QByteArray();
	}

	// A share left from an earlier relationship ends; sharing again starts a new one
	if (storedPtr.IsValid() && storedPtr->GetOwnerTenantId() == ownerTenantId && IsOfEarlierRelationship(*storedPtr, relatedSince)){
		if (!RemoveShares({storedPtr->GetShareId()})){
			if (errorMessagePtr != nullptr){
				*errorMessagePtr = QStringLiteral("The earlier share of the document could not be ended. Try again.");
			}

			return QByteArray();
		}

		storedPtr.Reset();
	}

	const bool isNew = !storedPtr.IsValid();
	if (isNew){
		IDocumentShareInfoUniquePtr newSharePtr = m_shareFactoryCompPtr.CreateInstance();
		if (!newSharePtr.IsValid() || !newSharePtr->CopyFrom(shareInfo)){
			if (errorMessagePtr != nullptr){
				*errorMessagePtr = QStringLiteral("The document share could not be created");
			}

			SendErrorMessage(0, QStringLiteral("Document share for '%1' could not be created from the given data").arg(QString::fromUtf8(shareInfo.GetEntityId())), "CDocumentShareManagerComp");

			return QByteArray();
		}

		newSharePtr->SetShareId(QUuid::createUuid().toByteArray(QUuid::WithoutBraces));
		newSharePtr->SetTargetGroupIds(QByteArrayList());
		newSharePtr->SetStatus(initialStatus);
		newSharePtr->SetCreatedAt(now);

		storedPtr = IDocumentShareInfoSharedPtr::CreateFromUnique(std::move(newSharePtr));
	}
	else{
		if (storedPtr->GetOwnerTenantId() != ownerTenantId){
			if (errorMessagePtr != nullptr){
				*errorMessagePtr = QStringLiteral("The document belongs to another organization");
			}

			return QByteArray();
		}

		storedPtr->SetCollectionId(shareInfo.GetCollectionId());
		storedPtr->SetEntityName(shareInfo.GetEntityName());
		storedPtr->SetAccessLevel(accessLevel);
		storedPtr->SetValidUntil(shareInfo.GetValidUntil());
		storedPtr->SetMessage(shareInfo.GetMessage());
		if (storedPtr->GetStatus() == IDocumentShareInfo::DSS_DECLINED){
			storedPtr->SetStatus(initialStatus);
		}
	}

	storedPtr->SetOwnerTenantName(ownerTenantName);
	storedPtr->SetTargetTenantName(targetTenantName);
	storedPtr->SetUpdatedAt(now);

	if (!StoreShare(*storedPtr, isNew)){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QStringLiteral("The document share could not be stored");
		}

		return QByteArray();
	}

	return storedPtr->GetShareId();
}


bool CDocumentShareManagerComp::RemoveShares(const QByteArrayList& shareIds)
{
	if (shareIds.isEmpty()){
		return true;
	}

	istd::CChangeNotifier changeNotifier(this);

	const bool retVal = m_shareCollectionCompPtr->RemoveElements(shareIds);
	++m_sharesRevision;

	return retVal;
}


bool CDocumentShareManagerComp::SetShareStatus(const QByteArrayList& shareIds, IDocumentShareInfo::ShareStatus status)
{
	for (const QByteArray& shareId : shareIds){
		IDocumentShareInfoUniquePtr sharePtr = GetShare(shareId);
		if (!sharePtr.IsValid()){
			return false;
		}

		if (sharePtr->GetStatus() != status){
			sharePtr->SetStatus(status);
			sharePtr->SetUpdatedAt(QDateTime::currentDateTimeUtc());

			if (!StoreShare(*sharePtr, false)){
				return false;
			}
		}
	}

	return true;
}


bool CDocumentShareManagerComp::SetShareTargetGroups(const QByteArray& shareId, const QByteArrayList& groupIds)
{
	IDocumentShareInfoUniquePtr sharePtr = GetShare(shareId);
	if (!sharePtr.IsValid()){
		return false;
	}

	sharePtr->SetTargetGroupIds(groupIds);
	sharePtr->SetUpdatedAt(QDateTime::currentDateTimeUtc());

	return StoreShare(*sharePtr, false);
}


IDocumentShareInfoUniquePtr CDocumentShareManagerComp::GetShare(const QByteArray& shareId) const
{
	if (shareId.isEmpty()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_shareCollectionCompPtr->GetObjectData(shareId, dataPtr)){
		return nullptr;
	}

	const IDocumentShareInfo* storedSharePtr = dynamic_cast<const IDocumentShareInfo*>(dataPtr.GetPtr());
	if (storedSharePtr == nullptr){
		return nullptr;
	}

	IDocumentShareInfoUniquePtr sharePtr = m_shareFactoryCompPtr.CreateInstance();
	if (!sharePtr.IsValid() || !sharePtr->CopyFrom(*storedSharePtr)){
		return nullptr;
	}

	return sharePtr;
}


DocumentShares CDocumentShareManagerComp::GetDocumentShares(const QByteArray& entityId) const
{
	return FindShares("EntityId", entityId);
}


DocumentShares CDocumentShareManagerComp::GetIncomingShares(const QByteArray& targetTenantId) const
{
	return FindShares("TargetTenantId", targetTenantId);
}


DocumentShares CDocumentShareManagerComp::GetOutgoingShares(const QByteArray& ownerTenantId) const
{
	return FindShares("OwnerTenantId", ownerTenantId);
}


QByteArrayList CDocumentShareManagerComp::GetShareOwnerTenantIds(const QByteArray& targetTenantId) const
{
	QByteArrayList retVal;
	ReadRelatedTenants(targetTenantId, retVal);

	return retVal;
}


int CDocumentShareManagerComp::RemoveUnrelatedShares(const QByteArray& tenantId)
{
	QByteArrayList effectiveTenantIds;

	return ReadRelatedTenants(tenantId, effectiveTenantIds);
}


int CDocumentShareManagerComp::GetSharesRevision() const
{
	return m_sharesRevision;
}


// private methods

int CDocumentShareManagerComp::ReadRelatedTenants(const QByteArray& tenantId, QByteArrayList& effectiveTenantIds) const
{
	effectiveTenantIds.clear();
	if (tenantId.isEmpty()){
		return 0;
	}

	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	{
		QMutexLocker locker(&m_relatedTenantsCacheMutex);
		auto cacheIter = m_relatedTenantsCache.constFind(tenantId);
		if (cacheIter != m_relatedTenantsCache.constEnd() && cacheIter->expiresAt > now){
			effectiveTenantIds = cacheIter->tenantIds;

			return 0;
		}
	}

	bool isRead = false;
	const RelatedTenantInfos relatedTenants = m_relationshipProviderCompPtr->GetRelatedTenants(tenantId, nullptr, &isRead);

	// An unreadable answer is not an ended relationship; the requester gets no shares, without affecting the others
	if (!isRead){
		return 0;
	}

	// The shares of ended relationships go before the partners are trusted, so a re-created relationship revives none.
	// A share that cannot be read may be such a share: no partner is trusted then, the next request tries again.
	bool isOutgoingRead = false;
	bool isIncomingRead = false;
	const DocumentShares shares = FindShares("OwnerTenantId", tenantId, &isOutgoingRead) + FindShares("TargetTenantId", tenantId, &isIncomingRead);
	if (!isOutgoingRead || !isIncomingRead){
		SendErrorMessage(0, QStringLiteral("The document shares of tenant '%1' could not be read completely").arg(QString::fromUtf8(tenantId)), "CDocumentShareManagerComp");

		return 0;
	}

	const QByteArrayList endedShareIds = FindSharesOfEndedRelationships(tenantId, relatedTenants, shares);
	int removedCount = 0;
	if (!endedShareIds.isEmpty()){
		// The old shares are still stored: no partner is trusted until they go, the next request tries again
		if (!const_cast<CDocumentShareManagerComp*>(this)->RemoveShares(endedShareIds)){
			SendErrorMessage(0, QStringLiteral("Unable to remove %1 document shares of tenant '%2' of ended relationships").arg(QString::number(endedShareIds.size()), QString::fromUtf8(tenantId)), "CDocumentShareManagerComp");

			return 0;
		}

		removedCount = imtbase::narrow_cast<int>(endedShareIds.size());

		SendInfoMessage(0, QStringLiteral("Removed %1 document shares of tenant '%2' of ended relationships").arg(QString::number(removedCount), QString::fromUtf8(tenantId)), "CDocumentShareManagerComp");
	}

	RelatedTenantsCacheItem cacheItem;
	cacheItem.expiresAt = now + qint64(qMax(0, *m_relationshipCacheTimeoutAttrPtr)) * 1000;
	for (const RelatedTenantInfo& relatedTenant : relatedTenants){
		if (relatedTenant.isEffective){
			cacheItem.tenantIds << relatedTenant.tenantId;
		}
	}

	QMutexLocker locker(&m_relatedTenantsCacheMutex);
	m_relatedTenantsCache.insert(tenantId, cacheItem);
	effectiveTenantIds = cacheItem.tenantIds;

	return removedCount;
}


bool CDocumentShareManagerComp::IsOfEarlierRelationship(const IDocumentShareInfo& share, const QDateTime& relatedSince) const
{
	// The clocks of the servers may differ a little, an unknown time decides nothing
	const QDateTime createdAt = share.GetCreatedAt();

	return relatedSince.isValid() && createdAt.isValid() && createdAt.addSecs(s_clockToleranceSecs) < relatedSince;
}


QByteArrayList CDocumentShareManagerComp::FindSharesOfEndedRelationships(
			const QByteArray& tenantId,
			const RelatedTenantInfos& relatedTenants,
			const DocumentShares& shares) const
{
	// A relationship outside its validity period still exists: its shares grant nothing until it is valid again
	QHash<QByteArray, QDateTime> relatedSinceByTenant;
	for (const RelatedTenantInfo& relatedTenant : relatedTenants){
		relatedSinceByTenant.insert(relatedTenant.tenantId, relatedTenant.relatedSince);
	}

	QByteArrayList retVal;
	for (const IDocumentShareInfoSharedPtr& sharePtr : shares){
		const bool isOwner = sharePtr->GetOwnerTenantId() == tenantId;
		if (!isOwner && sharePtr->GetTargetTenantId() != tenantId){
			continue;
		}

		const QByteArray partnerTenantId = isOwner ? sharePtr->GetTargetTenantId() : sharePtr->GetOwnerTenantId();
		const auto relatedIter = relatedSinceByTenant.constFind(partnerTenantId);
		const bool isEnded = relatedIter == relatedSinceByTenant.constEnd() || IsOfEarlierRelationship(*sharePtr, relatedIter.value());
		if (isEnded && !retVal.contains(sharePtr->GetShareId())){
			retVal.append(sharePtr->GetShareId());
		}
	}

	return retVal;
}


DocumentShares CDocumentShareManagerComp::FindShares(const QByteArray& fieldId, const QByteArray& value, bool* isReadPtr) const
{
	DocumentShares retVal;
	if (isReadPtr != nullptr){
		*isReadPtr = value.isEmpty();
	}

	if (value.isEmpty()){
		return retVal;
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(fieldId, QString::fromUtf8(value));

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	istd::TDelPtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_shareCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &filterParams));
	if (!iteratorPtr.IsValid()){
		return retVal;
	}

	bool isRead = true;
	while (iteratorPtr->Next()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const IDocumentShareInfo* storedSharePtr = iteratorPtr->GetObjectData(dataPtr) ? dynamic_cast<const IDocumentShareInfo*>(dataPtr.GetPtr()) : nullptr;
		if (storedSharePtr == nullptr){
			isRead = false;
			continue;
		}

		IDocumentShareInfoUniquePtr sharePtr = m_shareFactoryCompPtr.CreateInstance();
		if (!sharePtr.IsValid() || !sharePtr->CopyFrom(*storedSharePtr)){
			isRead = false;
			continue;
		}

		retVal.append(IDocumentShareInfoSharedPtr::CreateFromUnique(std::move(sharePtr)));
	}

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


bool CDocumentShareManagerComp::StoreShare(const IDocumentShareInfo& shareInfo, bool isNew)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = false;
	if (!isNew){
		retVal = m_shareCollectionCompPtr->SetObjectData(shareInfo.GetShareId(), shareInfo);
	}
	else{
		const QByteArray storedId = m_shareCollectionCompPtr->InsertNewObject(
					"DocumentShare",
					shareInfo.GetEntityName(),
					QString(),
					&shareInfo,
					shareInfo.GetShareId());
		retVal = !storedId.isEmpty();
	}

	++m_sharesRevision;

	return retVal;
}


} // namespace imtauth


