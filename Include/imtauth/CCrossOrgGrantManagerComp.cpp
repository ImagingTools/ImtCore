// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossOrgGrantManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


QByteArray CCrossOrgGrantManagerComp::CreateGrant(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArray& relationshipId,
		CrossOrgAccessLevel accessLevel,
		const QString& resourceScope,
		const QByteArray& targetTeamId,
		const QString& description,
		const QString& expiresAt,
		const QByteArray& contractId)
{
	if (!m_grantCollectionCompPtr.IsValid() || !m_grantFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Grant collection or factory not configured", "CCrossOrgGrantManagerComp");
		return QByteArray();
	}

	if (sourceTenantId.isEmpty() || targetTenantId.isEmpty() || relationshipId.isEmpty()){
		SendErrorMessage(0, "Source tenant, target tenant and relationship are required", "CCrossOrgGrantManagerComp");
		return QByteArray();
	}

	if (sourceTenantId == targetTenantId){
		SendErrorMessage(0, "Cross-org grant cannot reference the same tenant as source and target", "CCrossOrgGrantManagerComp");
		return QByteArray();
	}

	ICrossOrgGrantDataUniquePtr grantPtr = m_grantFactoryCompPtr.CreateInstance();
	if (!grantPtr.IsValid()){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray grantId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	CrossOrgGrantInfo info;
	info.grantId = grantId;
	info.sourceTenantId = sourceTenantId;
	info.targetTenantId = targetTenantId;
	info.relationshipId = relationshipId;
	info.contractId = contractId;
	info.targetTeamId = targetTeamId;
	info.accessLevel = accessLevel;
	info.resourceScope = resourceScope;
	info.description = description;
	info.createdAt = now;
	info.expiresAt = expiresAt;
	info.isActive = true;

	grantPtr->SetGrantInfo(info);

	QByteArray storedId = m_grantCollectionCompPtr->InsertNewObject("CrossOrgGrant", QString(), QString(), grantPtr.GetPtr(), grantId);
	return storedId.isEmpty() ? QByteArray() : grantId;
}


bool CCrossOrgGrantManagerComp::RevokeGrant(const QByteArray& grantId)
{
	if (!m_grantCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_grantCollectionCompPtr->GetObjectData(grantId, dataPtr)){
		return false;
	}

	ICrossOrgGrantData* grantPtr = dynamic_cast<ICrossOrgGrantData*>(dataPtr.GetPtr());
	if (grantPtr == nullptr){
		return false;
	}

	CrossOrgGrantInfo info = grantPtr->GetGrantInfo();
	if (!info.isActive){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	info.isActive = false;
	grantPtr->SetGrantInfo(info);

	return m_grantCollectionCompPtr->SetObjectData(grantId, *grantPtr);
}


CrossOrgGrantInfo CCrossOrgGrantManagerComp::GetGrant(const QByteArray& grantId) const
{
	if (!m_grantCollectionCompPtr.IsValid()){
		return CrossOrgGrantInfo();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_grantCollectionCompPtr->GetObjectData(grantId, dataPtr)){
		return CrossOrgGrantInfo();
	}

	const ICrossOrgGrantData* grantPtr = dynamic_cast<const ICrossOrgGrantData*>(dataPtr.GetPtr());
	if (grantPtr == nullptr){
		return CrossOrgGrantInfo();
	}

	return grantPtr->GetGrantInfo();
}


CrossOrgGrants CCrossOrgGrantManagerComp::CollectGrants(const std::function<bool(const CrossOrgGrantInfo&)>& predicate) const
{
	CrossOrgGrants result;
	if (!m_grantCollectionCompPtr.IsValid()){
		return result;
	}

	for (const QByteArray& id : m_grantCollectionCompPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_grantCollectionCompPtr->GetObjectData(id, dataPtr)){
			const ICrossOrgGrantData* grantPtr = dynamic_cast<const ICrossOrgGrantData*>(dataPtr.GetPtr());
			if (grantPtr != nullptr){
				CrossOrgGrantInfo info = grantPtr->GetGrantInfo();
				if (predicate(info)){
					result.append(info);
				}
			}
		}
	}

	return result;
}


CrossOrgGrants CCrossOrgGrantManagerComp::GetGrantsBySourceTenant(const QByteArray& sourceTenantId) const
{
	return CollectGrants([&sourceTenantId](const CrossOrgGrantInfo& info){
		return info.sourceTenantId == sourceTenantId;
	});
}


CrossOrgGrants CCrossOrgGrantManagerComp::GetGrantsByTargetTenant(const QByteArray& targetTenantId) const
{
	return CollectGrants([&targetTenantId](const CrossOrgGrantInfo& info){
		return info.targetTenantId == targetTenantId;
	});
}


CrossOrgGrants CCrossOrgGrantManagerComp::GetGrantsByRelationship(const QByteArray& relationshipId) const
{
	return CollectGrants([&relationshipId](const CrossOrgGrantInfo& info){
		return info.relationshipId == relationshipId;
	});
}


bool CCrossOrgGrantManagerComp::IsGrantEffective(const CrossOrgGrantInfo& grantInfo) const
{
	if (!grantInfo.isActive){
		return false;
	}

	if (!grantInfo.expiresAt.isEmpty()){
		QDateTime expiresAt = QDateTime::fromString(grantInfo.expiresAt, Qt::ISODateWithMs);
		if (expiresAt.isValid() && expiresAt < QDateTime::currentDateTimeUtc()){
			return false;
		}
	}

	return true;
}


bool CCrossOrgGrantManagerComp::HasAccess(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QString& resourceScope,
		CrossOrgAccessLevel requiredLevel) const
{
	if (requiredLevel == COAL_NONE){
		return true;
	}

	const CrossOrgGrants grants = GetGrantsByTargetTenant(targetTenantId);
	for (const CrossOrgGrantInfo& info : grants){
		if (info.sourceTenantId != sourceTenantId){
			continue;
		}
		if (!IsGrantEffective(info)){
			continue;
		}
		// An empty resource scope on the grant applies to all resources.
		if (!info.resourceScope.isEmpty() && info.resourceScope != resourceScope){
			continue;
		}
		if (info.accessLevel >= requiredLevel){
			return true;
		}
	}

	return false;
}


} // namespace imtauth
