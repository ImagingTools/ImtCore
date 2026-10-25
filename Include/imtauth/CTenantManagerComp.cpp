// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/imtauth.h>
#include <imtauth/TenantEvents.h>
#include <imtbase/imtbase.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/CollectionSelection.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::ITenantManager)

QByteArrayList CTenantManagerComp::GetTenantIds() const
{
	QByteArrayList result;

	if (!m_tenantCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection not configured", "CTenantManagerComp");
		return result;
	}

	return m_tenantCollectionCompPtr->GetElementIds();
}


ITenantInfoUniquePtr CTenantManagerComp::GetTenant(const QByteArray& tenantId) const
{
	if (!m_tenantCollectionCompPtr.IsValid() || !m_tenantFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection or factory not configured", "CTenantManagerComp");
		return nullptr;
	}

	if (!imtbase::isValidUuid(tenantId)){
		SendErrorMessage(0, QStringLiteral("Invalid tenant id '%1'").arg(tenantId), QStringLiteral("CTenantManagerComp"));
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tenantCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		return nullptr;
	}

	const ITenantInfo* tenantPtr = dynamic_cast<const ITenantInfo*>(dataPtr.GetPtr());
	if (tenantPtr == nullptr){
		return nullptr;
	}

	ITenantInfoUniquePtr clonedTenant = m_tenantFactoryCompPtr.CreateInstance();
	if (!clonedTenant.IsValid()){
		return nullptr;
	}

	if (!clonedTenant->CopyFrom(*tenantPtr)){
		return nullptr;
	}

	return clonedTenant;
}


QByteArrayList CTenantManagerComp::GetOwnedOrCreatedTenantIds(const QByteArray& userId) const
{
	QByteArrayList retVal;
	if (!m_tenantCollectionCompPtr.IsValid()){
		return retVal;
	}

	// A collection that does not know the filter returns every organization
	for (const QByteArray& candidateId : CollectionSelection::SelectIds(*m_tenantCollectionCompPtr, CollectionSelection::AnyColumnEquals("OwnerId", "CreatorId", userId))){
		ITenantInfoUniquePtr candidatePtr = GetTenant(candidateId);
		if (candidatePtr.IsValid() && (candidatePtr->GetOwnerId() == userId || candidatePtr->GetCreatorId() == userId)){
			retVal.append(candidateId);
		}
	}

	return retVal;
}


QByteArrayList CTenantManagerComp::GetChildTenantIds(const QByteArray& tenantId) const
{
	QByteArrayList retVal;
	if (!m_tenantCollectionCompPtr.IsValid() || tenantId.isEmpty()){
		return retVal;
	}

	iprm::CIdParam parentTenantParam;
	parentTenantParam.SetId(tenantId);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ParentTenantId", &parentTenantParam);

	// A collection that does not know the filter returns every organization
	for (const QByteArray& candidateId : m_tenantCollectionCompPtr->GetElementIds(0, -1, &filterParams)){
		ITenantInfoUniquePtr candidatePtr = GetTenant(candidateId);
		if (candidatePtr.IsValid() && candidatePtr->GetParentTenantId() == tenantId){
			retVal.append(candidateId);
		}
	}

	return retVal;
}


QByteArrayList CTenantManagerComp::FindTenantIdsByName(const QString& tenantName) const
{
	QByteArrayList retVal;
	const QString name = tenantName.trimmed();
	if (!m_tenantCollectionCompPtr.IsValid() || name.isEmpty()){
		return retVal;
	}

	// The case-insensitive match of the database selects the candidates, the comparison below decides; SQLite ignores the case of ASCII letters only
	imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("Name", name, imtbase::IComplexCollectionFilter::FO_CONTAINS);

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	for (const QByteArray& candidateId : m_tenantCollectionCompPtr->GetElementIds(0, -1, &filterParams)){
		ITenantInfoUniquePtr candidatePtr = GetTenant(candidateId);
		if (candidatePtr.IsValid() && candidatePtr->GetTenantName().trimmed().compare(name, Qt::CaseInsensitive) == 0){
			retVal.append(candidateId);
		}
	}

	return retVal;
}


QByteArrayList CTenantManagerComp::GetTenantPermissions(const QByteArray& tenantId) const
{
	ITenantInfoUniquePtr tenantPtr = GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		return QByteArrayList();
	}

	return tenantPtr->GetTenantPermissions();
}


QByteArray CTenantManagerComp::CreateTenant(
			const QString& tenantName,
			const QString& description,
			const QByteArray& ownerId,
			const QByteArray& creatorId,
			const QByteArray& parentTenantId)
{
	if (!m_tenantCollectionCompPtr.IsValid() || !m_tenantFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection or factory not configured", "CTenantManagerComp");
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	QByteArray tenantId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	ITenantInfoUniquePtr tenantPtr = m_tenantFactoryCompPtr.CreateInstance();
	if (!tenantPtr.IsValid()){
		SendErrorMessage(0, "Failed to create tenant object", "CTenantManagerComp");
		return QByteArray();
	}

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	tenantPtr->SetTenantId(tenantId);
	tenantPtr->SetTenantName(tenantName);
	tenantPtr->SetTenantDescription(description);
	tenantPtr->SetOwnerId(ownerId);
	tenantPtr->SetCreatorId(creatorId.isEmpty() ? ownerId : creatorId);
	tenantPtr->SetParentTenantId(parentTenantId);
	tenantPtr->SetActive(true);
	tenantPtr->SetCreatedAt(now);
	tenantPtr->SetUpdatedAt(now);

	// Grant the tenant a default set of permissions that its roles are allowed to
	// use. Roles created within a tenant context may only reference permissions
	// present in this set (see CRoleDocumentValidatorComp); without a default set
	// a fresh tenant could not grant its roles any RBAC permission at all.
	static const QByteArrayList defaultTenantPermissions = {
		QByteArrayLiteral("/Administration/Users/ViewPage"), QByteArrayLiteral("/Administration/Users/View"), QByteArrayLiteral("/Administration/Users/Edit/Add"),
		QByteArrayLiteral("/Administration/Users/Edit/Change"), QByteArrayLiteral("/Administration/Users/Edit/Remove"),
		QByteArrayLiteral("/Administration/Roles/ViewPage"), QByteArrayLiteral("/Administration/Roles/View"), QByteArrayLiteral("/Administration/Roles/Edit/Add"),
		QByteArrayLiteral("/Administration/Roles/Edit/Change"), QByteArrayLiteral("/Administration/Roles/Edit/Remove"),
		QByteArrayLiteral("/Administration/Groups/ViewPage"), QByteArrayLiteral("/Administration/Groups/View"), QByteArrayLiteral("/Administration/Groups/Edit/Add"),
		QByteArrayLiteral("/Administration/Groups/Edit/Change"), QByteArrayLiteral("/Administration/Groups/Edit/Remove"),
		QByteArrayLiteral("/Administration/Users/Revisions/View"), QByteArrayLiteral("/Administration/Roles/Revisions/View"), QByteArrayLiteral("/Administration/Groups/Revisions/View")
	};
	tenantPtr->SetTenantPermissions(parentTenantId.isEmpty()
				? defaultTenantPermissions
				: RestrictPermissions(defaultTenantPermissions, GetTenantPermissions(parentTenantId)));

	QByteArray retVal = m_tenantCollectionCompPtr->InsertNewObject("Tenant", tenantName, description, tenantPtr.GetPtr(), tenantId);
	if (retVal.isEmpty()){
		SendErrorMessage(0, "Failed to store tenant in collection", "CTenantManagerComp");
		return QByteArray();
	}

	SendInfoMessage(0, QStringLiteral("Created tenant '%1' (id: %2)").arg(tenantName, tenantId), QStringLiteral("CTenantManagerComp"));

	return tenantId;
}


bool CTenantManagerComp::RemoveTenant(const QByteArray& tenantId)
{
	if (!m_tenantCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection not configured", "CTenantManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	if (!m_tenantCollectionCompPtr->RemoveElements({tenantId})){
		SendErrorMessage(0, QStringLiteral("Failed to remove tenant '%1'").arg(tenantId), "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, QStringLiteral("Removed tenant '%1'").arg(tenantId), "CTenantManagerComp");

	return true;
}


bool CTenantManagerComp::UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description, const QByteArray& ownerId, bool updateOwner)
{
	if (!m_tenantCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection not configured", "CTenantManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tenantCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		SendErrorMessage(0, QStringLiteral("Tenant '%1' not found").arg(tenantId), "CTenantManagerComp");
		return false;
	}

	ITenantInfo* tenantPtr = dynamic_cast<ITenantInfo*>(dataPtr.GetPtr());
	if (tenantPtr == nullptr){
		SendErrorMessage(0, "Invalid tenant object", "CTenantManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	const QByteArray previousOwnerId = tenantPtr->GetOwnerId();

	tenantPtr->SetTenantName(tenantName);
	tenantPtr->SetTenantDescription(description);
	if (updateOwner){
		tenantPtr->SetOwnerId(ownerId);
	}
	tenantPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

	if (!m_tenantCollectionCompPtr->SetObjectData(tenantId, *tenantPtr)){
		SendErrorMessage(0, QStringLiteral("Failed to update tenant '%1'").arg(tenantId), "CTenantManagerComp");
		return false;
	}

	if (updateOwner && previousOwnerId != ownerId){
		TenantEvent::Publish(this, {
					{TenantEvent::s_type, TenantEvent::s_ownershipTransferred},
					{TenantEvent::s_tenantId, tenantId},
					{TenantEvent::s_previousOwnerId, previousOwnerId},
					{TenantEvent::s_ownerId, ownerId}});
	}

	SendInfoMessage(0, QStringLiteral("Updated tenant '%1'").arg(tenantId), "CTenantManagerComp");

	return true;
}


bool CTenantManagerComp::SetTenantActive(const QByteArray& tenantId, bool isActive)
{
	if (!m_tenantCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection not configured", "CTenantManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tenantCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		SendErrorMessage(0, QStringLiteral("Tenant '%1' not found").arg(tenantId), "CTenantManagerComp");
		return false;
	}

	ITenantInfo* tenantPtr = dynamic_cast<ITenantInfo*>(dataPtr.GetPtr());
	if (tenantPtr == nullptr){
		SendErrorMessage(0, "Invalid tenant object", "CTenantManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	tenantPtr->SetActive(isActive);
	tenantPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

	if (!m_tenantCollectionCompPtr->SetObjectData(tenantId, *tenantPtr)){
		SendErrorMessage(0, QStringLiteral("Failed to set tenant '%1' active state").arg(tenantId), "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, QStringLiteral("Set tenant '%1' active=%2").arg(tenantId, QString::number(isActive)), QStringLiteral("CTenantManagerComp"));

	return true;
}


bool CTenantManagerComp::SetTenantPermissions(const QByteArray& tenantId, const QByteArrayList& permissions)
{
	QSet<QByteArray> visitedTenantIds;

	return SetTenantPermissionsTree(tenantId, permissions, visitedTenantIds);
}


bool CTenantManagerComp::SetTenantHierarchy(const QByteArray& tenantId, const QByteArray& parentTenantId)
{
	if (!m_tenantCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection not configured", "CTenantManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tenantCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		SendErrorMessage(0, QStringLiteral("Tenant '%1' not found").arg(tenantId), "CTenantManagerComp");
		return false;
	}

	ITenantInfo* tenantPtr = dynamic_cast<ITenantInfo*>(dataPtr.GetPtr());
	if (tenantPtr == nullptr){
		SendErrorMessage(0, "Invalid tenant object", "CTenantManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	tenantPtr->SetParentTenantId(parentTenantId);

	tenantPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

	if (!m_tenantCollectionCompPtr->SetObjectData(tenantId, *tenantPtr)){
		SendErrorMessage(0, QStringLiteral("Failed to set hierarchy for tenant '%1'").arg(tenantId), "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, QStringLiteral("Set hierarchy for tenant '%1' (parent: %2)").arg(
		QString::fromUtf8(tenantId), parentTenantId.isEmpty() ? "none" : QString::fromUtf8(parentTenantId)), "CTenantManagerComp");

	return true;
}


bool CTenantManagerComp::EnsureSystemTenant()
{
	if (!m_tenantCollectionCompPtr.IsValid() || !m_tenantFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection or factory not configured", "CTenantManagerComp");
		return false;
	}

	QByteArray systemTenantId = imtauth::GetSystemTenantId();

	// Check if System-Tenant already exists
	ITenantInfoUniquePtr existingTenant = GetTenant(systemTenantId);
	if (existingTenant.IsValid()){
		SendInfoMessage(0, "System-Tenant already exists", "CTenantManagerComp");
		return true;
	}

	// Create the System-Tenant
	istd::CChangeNotifier changeNotifier(this);

	ITenantInfoUniquePtr tenantPtr = m_tenantFactoryCompPtr.CreateInstance();
	if (!tenantPtr.IsValid()){
		SendErrorMessage(0, "Failed to create System-Tenant object", "CTenantManagerComp");
		return false;
	}

	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	tenantPtr->SetTenantId(systemTenantId);
	tenantPtr->SetTenantName(QStringLiteral("System"));
	tenantPtr->SetTenantDescription(QStringLiteral("Root system tenant"));
	tenantPtr->SetActive(true);
	tenantPtr->SetCreatedAt(now);
	tenantPtr->SetUpdatedAt(now);

	QByteArray retVal = m_tenantCollectionCompPtr->InsertNewObject("Tenant", QStringLiteral("System"), QStringLiteral("Root system tenant"), tenantPtr.GetPtr(), systemTenantId);
	if (retVal.isEmpty()){
		SendErrorMessage(0, "Failed to store System-Tenant in collection", "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, "System-Tenant created successfully", "CTenantManagerComp");

	return true;
}


QByteArray CTenantManagerComp::GetSystemTenantId() const
{
	return imtauth::GetSystemTenantId();
}


// private methods

bool CTenantManagerComp::SetTenantPermissionsTree(const QByteArray& tenantId, const QByteArrayList& permissions, QSet<QByteArray>& visitedTenantIds)
{
	visitedTenantIds.insert(tenantId);

	if (!m_tenantCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection not configured", "CTenantManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tenantCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		SendErrorMessage(0, QStringLiteral("Tenant '%1' not found").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
		return false;
	}

	ITenantInfo* tenantPtr = dynamic_cast<ITenantInfo*>(dataPtr.GetPtr());
	if (tenantPtr == nullptr){
		SendErrorMessage(0, "Invalid tenant object", "CTenantManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	if (tenantPtr->GetTenantPermissions() != permissions){
		tenantPtr->SetTenantPermissions(permissions);
		tenantPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

		if (!m_tenantCollectionCompPtr->SetObjectData(tenantId, *tenantPtr)){
			SendErrorMessage(0, QStringLiteral("Failed to set the permissions of tenant '%1'").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
			return false;
		}
	}

	// A sub-organization never holds more than its parent. Every subtree is visited, so a retry repairs descendants left over from a partially failed update
	bool retVal = true;
	for (const QByteArray& childTenantId : GetChildTenantIds(tenantId)){
		if (visitedTenantIds.contains(childTenantId)){
			continue;
		}

		const QByteArrayList restrictedPermissions = RestrictPermissions(GetTenantPermissions(childTenantId), permissions);
		retVal = SetTenantPermissionsTree(childTenantId, restrictedPermissions, visitedTenantIds) && retVal;
	}

	return retVal;
}


} // namespace imtauth
