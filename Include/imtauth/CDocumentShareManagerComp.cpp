// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CDocumentShareManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


static bool IsExpired(const QString& expiresAt)
{
	if (expiresAt.isEmpty()){
		return false;
	}

	QDateTime expires = QDateTime::fromString(expiresAt, Qt::ISODateWithMs);
	if (!expires.isValid()){
		return false;
	}

	return expires < QDateTime::currentDateTimeUtc();
}


bool CDocumentShareManagerComp::ValidateRelationship(const QByteArray& sourceTenantId, const QByteArray& relationshipId) const
{
	if (relationshipId.isEmpty() || !m_tenantManagerCompPtr.IsValid()){
		// Shares without a relationship reference are allowed, as are reduced
		// compositions without a tenant manager.
		return true;
	}

	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(sourceTenantId);
	if (!tenantPtr.IsValid()){
		return false;
	}

	return tenantPtr->GetRelationshipIds().contains(relationshipId);
}


bool CDocumentShareManagerComp::StoreShareInfo(const QByteArray& shareId, const DocumentShareInfo& info)
{
	if (!m_shareCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_shareCollectionCompPtr->GetObjectData(shareId, dataPtr)){
		return false;
	}

	IDocumentShareData* sharePtr = dynamic_cast<IDocumentShareData*>(dataPtr.GetPtr());
	if (sharePtr == nullptr){
		return false;
	}

	sharePtr->SetShareInfo(info);

	return m_shareCollectionCompPtr->SetObjectData(shareId, *sharePtr);
}


QByteArray CDocumentShareManagerComp::CreateShare(const DocumentShareInfo& shareInfo)
{
	if (!m_shareCollectionCompPtr.IsValid() || !m_shareFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Document share collection or factory not configured", "CDocumentShareManagerComp");
		return QByteArray();
	}

	if (shareInfo.sourceTenantId.isEmpty() || shareInfo.targetTenantId.isEmpty() || shareInfo.documentId.isEmpty()){
		SendErrorMessage(0, "Source tenant, target tenant and document are required", "CDocumentShareManagerComp");
		return QByteArray();
	}

	if (shareInfo.sourceTenantId == shareInfo.targetTenantId){
		SendErrorMessage(0, "A document cannot be shared with the owning tenant", "CDocumentShareManagerComp");
		return QByteArray();
	}

	if (!ValidateRelationship(shareInfo.sourceTenantId, shareInfo.relationshipId)){
		SendErrorMessage(0, "No relationship of the source tenant authorizes this document share", "CDocumentShareManagerComp");
		return QByteArray();
	}

	IDocumentShareDataUniquePtr sharePtr = m_shareFactoryCompPtr.CreateInstance();
	if (!sharePtr.IsValid()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray shareId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	DocumentShareInfo info = shareInfo;
	info.shareId = shareId;
	info.status = DSS_ACTIVE;
	info.createdAt = now;
	info.updatedAt = now;
	info.revokedAt.clear();

	sharePtr->SetShareInfo(info);

	QByteArray storedId = m_shareCollectionCompPtr->InsertNewObject("DocumentShare", info.documentName, QString(), sharePtr.GetPtr(), shareId);
	if (storedId.isEmpty()){
		SendErrorMessage(0, "Document share could not be stored", "CDocumentShareManagerComp");
		return QByteArray();
	}

	return shareId;
}


DocumentShareInfo CDocumentShareManagerComp::GetShare(const QByteArray& shareId) const
{
	if (!m_shareCollectionCompPtr.IsValid()){
		return DocumentShareInfo();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_shareCollectionCompPtr->GetObjectData(shareId, dataPtr)){
		return DocumentShareInfo();
	}

	const IDocumentShareData* sharePtr = dynamic_cast<const IDocumentShareData*>(dataPtr.GetPtr());
	if (sharePtr == nullptr){
		return DocumentShareInfo();
	}

	return sharePtr->GetShareInfo();
}


DocumentShares CDocumentShareManagerComp::CollectShares(const std::function<bool(const DocumentShareInfo&)>& predicate) const
{
	DocumentShares result;
	if (!m_shareCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_shareCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_shareCollectionCompPtr->GetObjectData(id, dataPtr)){
			const IDocumentShareData* sharePtr = dynamic_cast<const IDocumentShareData*>(dataPtr.GetPtr());
			if (sharePtr != nullptr){
				DocumentShareInfo info = sharePtr->GetShareInfo();
				if (predicate(info)){
					result.append(info);
				}
			}
		}
	}

	return result;
}


DocumentShares CDocumentShareManagerComp::GetIncomingShares(const QByteArray& targetTenantId) const
{
	return CollectShares([&targetTenantId](const DocumentShareInfo& info){
		return info.targetTenantId == targetTenantId;
	});
}


DocumentShares CDocumentShareManagerComp::GetOutgoingShares(const QByteArray& sourceTenantId) const
{
	return CollectShares([&sourceTenantId](const DocumentShareInfo& info){
		return info.sourceTenantId == sourceTenantId;
	});
}


DocumentShares CDocumentShareManagerComp::GetSharesOfDocument(const QByteArray& documentId) const
{
	return CollectShares([&documentId](const DocumentShareInfo& info){
		return info.documentId == documentId;
	});
}


bool CDocumentShareManagerComp::SetAccessMode(const QByteArray& shareId, DocumentShareAccessMode accessMode)
{
	DocumentShareInfo info = GetShare(shareId);
	if (info.shareId.isEmpty()){
		return false;
	}

	if (info.status != DSS_ACTIVE){
		SendErrorMessage(0, "Access mode of an inactive document share cannot be changed", "CDocumentShareManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.accessMode = accessMode;
	info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	return StoreShareInfo(shareId, info);
}


bool CDocumentShareManagerComp::RevokeShare(const QByteArray& shareId)
{
	DocumentShareInfo info = GetShare(shareId);
	if (info.shareId.isEmpty()){
		return false;
	}

	if (info.status == DSS_REVOKED){
		return true;
	}

	istd::CChangeNotifier changeNotifier(this);

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	info.status = DSS_REVOKED;
	info.revokedAt = now;
	info.updatedAt = now;

	return StoreShareInfo(shareId, info);
}


bool CDocumentShareManagerComp::GetDocumentAccess(
		const QByteArray& tenantId,
		const QByteArray& documentId,
		DocumentShareAccessMode* accessModePtr) const
{
	if (tenantId.isEmpty() || documentId.isEmpty()){
		return false;
	}

	bool hasAccess = false;
	DocumentShareAccessMode accessMode = DSAM_READ_ONLY;

	DocumentShares shares = CollectShares([&tenantId, &documentId](const DocumentShareInfo& info){
		return info.targetTenantId == tenantId && info.documentId == documentId;
	});

	for (const DocumentShareInfo& info : shares){
		if (info.status != DSS_ACTIVE || IsExpired(info.expiresAt)){
			continue;
		}

		hasAccess = true;
		if (info.accessMode == DSAM_FULL_ACCESS){
			accessMode = DSAM_FULL_ACCESS;
		}
	}

	if (hasAccess && accessModePtr != nullptr){
		*accessModePtr = accessMode;
	}

	return hasAccess;
}


} // namespace imtauth
