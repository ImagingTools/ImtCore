// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CDocumentShareManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>


namespace imtauth
{


// reimplemented (imtauth::IDocumentShareManager)

QByteArray CDocumentShareManagerComp::ShareDocument(const IDocumentShareInfo& shareInfo, QString* errorMessagePtr)
{
	QString errorMessage;
	QString ownerTenantName;
	QString targetTenantName;

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
		bool isRelated = false;
		for (const RelatedTenantInfo& relatedTenant : m_relationshipProviderCompPtr->GetRelatedTenants(ownerTenantId, &ownerTenantName)){
			if (relatedTenant.tenantId == targetTenantId){
				targetTenantName = relatedTenant.tenantName;
				isRelated = true;
				break;
			}
		}

		if (!isRelated){
			errorMessage = QStringLiteral("The organizations have no active relationship");
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

	IDocumentShareInfoSharedPtr storedPtr;
	for (const IDocumentShareInfoSharedPtr& existingPtr : GetDocumentShares(shareInfo.GetEntityId())){
		if (existingPtr->GetTargetTenantId() == targetTenantId){
			storedPtr = existingPtr;
			break;
		}
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
	if (targetTenantId.isEmpty()){
		return QByteArrayList();
	}

	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	{
		QMutexLocker locker(&m_relatedTenantsCacheMutex);
		auto cacheIter = m_relatedTenantsCache.constFind(targetTenantId);
		if (cacheIter != m_relatedTenantsCache.constEnd() && cacheIter->expiresAt > now){
			return cacheIter->tenantIds;
		}
	}

	RelatedTenantsCacheItem cacheItem;
	cacheItem.expiresAt = now + qint64(qMax(0, *m_relationshipCacheTimeoutAttrPtr)) * 1000;
	bool isRead = false;
	for (const RelatedTenantInfo& relatedTenant : m_relationshipProviderCompPtr->GetRelatedTenants(targetTenantId, nullptr, &isRead)){
		cacheItem.tenantIds << relatedTenant.tenantId;
	}

	// A requester who may not read the relationships gets no shares, without affecting the others
	if (!isRead){
		return QByteArrayList();
	}

	QMutexLocker locker(&m_relatedTenantsCacheMutex);
	m_relatedTenantsCache.insert(targetTenantId, cacheItem);

	return cacheItem.tenantIds;
}


int CDocumentShareManagerComp::GetSharesRevision() const
{
	return m_sharesRevision;
}


// private methods

DocumentShares CDocumentShareManagerComp::FindShares(const QByteArray& fieldId, const QByteArray& value) const
{
	DocumentShares retVal;

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

	while (iteratorPtr->Next()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!iteratorPtr->GetObjectData(dataPtr)){
			continue;
		}

		const IDocumentShareInfo* storedSharePtr = dynamic_cast<const IDocumentShareInfo*>(dataPtr.GetPtr());
		if (storedSharePtr == nullptr){
			continue;
		}

		IDocumentShareInfoUniquePtr sharePtr = m_shareFactoryCompPtr.CreateInstance();
		if (sharePtr.IsValid() && sharePtr->CopyFrom(*storedSharePtr)){
			retVal.append(IDocumentShareInfoSharedPtr::CreateFromUnique(std::move(sharePtr)));
		}
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


