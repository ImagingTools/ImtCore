// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossOrgGrantManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtauth
{


QByteArray CCrossOrgGrantManagerComp::CreateGrant(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId,
		const QByteArrayList& roleIds,
		const QString& description,
		const QString& expiresAt)
{
	if (!m_grantCollectionCompPtr.IsValid() || !m_grantFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Grant collection or factory not configured", "CCrossOrgGrantManagerComp");
		return QByteArray();
	}

	if (sourceTenantId.isEmpty() || targetTenantId.isEmpty()){
		SendErrorMessage(0, "Source tenant and target tenant are required", "CCrossOrgGrantManagerComp");
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
	info.roleIds = roleIds;
	info.description = description;
	info.createdAt = now;
	info.expiresAt = expiresAt;
	info.isActive = true;

	// Provide a deterministic fallback name so that repeated saves update the same
	// record instead of creating duplicates. A friendlier name based on tenant
	// display names is computed on the service layer when available.
	if (info.name.isEmpty()){
		QString targetName = QString::fromUtf8(targetTenantId);
		QStringList roleNames;
		for (const QByteArray& roleId : roleIds){
			if (!roleId.isEmpty()){
				roleNames << QString::fromUtf8(roleId);
			}
		}
		info.name = roleNames.isEmpty()
				? targetName
				: QStringLiteral("%1 (%2)").arg(targetName, roleNames.join(QStringLiteral(", ")));
	}

	grantPtr->SetGrantInfo(info);

	QByteArray storedId = m_grantCollectionCompPtr->InsertNewObject("CrossOrgGrant", info.name, QString(), grantPtr.GetPtr(), grantId);
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


bool CCrossOrgGrantManagerComp::RemoveGrants(const QByteArrayList& grantIds)
{
	if (!m_grantCollectionCompPtr.IsValid() || grantIds.isEmpty()){
		return false;
	}

	QByteArrayList filteredIds;
	for (const QByteArray& grantId : grantIds){
		if (!grantId.isEmpty()){
			filteredIds.push_back(grantId);
		}
	}

	if (filteredIds.isEmpty()){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);
	return m_grantCollectionCompPtr->RemoveElements(filteredIds);
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
		const QByteArray& roleId) const
{
	if (roleId.isEmpty()){
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
		if (info.roleIds.contains(roleId)){
			return true;
		}
	}

	return false;
}


QByteArrayList CCrossOrgGrantManagerComp::GetDelegatedSourceTenants(const QByteArray& targetTenantId) const
{
	QByteArrayList result;

	const CrossOrgGrants grants = GetGrantsByTargetTenant(targetTenantId);
	for (const CrossOrgGrantInfo& info : grants){
		if (!IsGrantEffective(info)){
			continue;
		}
		if (!result.contains(info.sourceTenantId)){
			result.append(info.sourceTenantId);
		}
	}

	return result;
}


QByteArrayList CCrossOrgGrantManagerComp::GetGrantedRoles(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId) const
{
	QByteArrayList result;

	const CrossOrgGrants grants = GetGrantsByTargetTenant(targetTenantId);
	for (const CrossOrgGrantInfo& info : grants){
		if (info.sourceTenantId != sourceTenantId){
			continue;
		}
		if (!IsGrantEffective(info)){
			continue;
		}
		for (const QByteArray& roleId : info.roleIds){
			if (!result.contains(roleId)){
				result.append(roleId);
			}
		}
	}

	return result;
}


bool CCrossOrgGrantManagerComp::IsDelegatedAccess(
		const QByteArray& sourceTenantId,
		const QByteArray& targetTenantId) const
{
	const CrossOrgGrants grants = GetGrantsByTargetTenant(targetTenantId);
	for (const CrossOrgGrantInfo& info : grants){
		if (info.sourceTenantId != sourceTenantId){
			continue;
		}
		if (IsGrantEffective(info)){
			return true;
		}
	}

	return false;
}


} // namespace imtauth
