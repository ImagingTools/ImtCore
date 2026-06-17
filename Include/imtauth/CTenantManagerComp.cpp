// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


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


QByteArrayList CTenantManagerComp::GetTenantPermissions(const QByteArray& tenantId) const
{
	ITenantInfoUniquePtr tenantPtr = GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		return QByteArrayList();
	}

	return tenantPtr->GetTenantPermissions();
}


QByteArray CTenantManagerComp::CreateTenant(const QString& tenantName, const QString& description, const QByteArray& ownerId)
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
	tenantPtr->SetCreatorId(ownerId);
	tenantPtr->SetActive(true);
	tenantPtr->SetCreatedAt(now);
	tenantPtr->SetUpdatedAt(now);

	QByteArray retVal = m_tenantCollectionCompPtr->InsertNewObject("Tenant", tenantName, description, tenantPtr.GetPtr(), tenantId);
	if (retVal.isEmpty()){
		SendErrorMessage(0, "Failed to store tenant in collection", "CTenantManagerComp");
		return QByteArray();
	}

	SendInfoMessage(0, QString("Created tenant '%1' (id: %2)").arg(tenantName, QString::fromUtf8(tenantId)), "CTenantManagerComp");

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
		SendErrorMessage(0, QString("Failed to remove tenant '%1'").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Removed tenant '%1'").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");

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
		SendErrorMessage(0, QString("Tenant '%1' not found").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
		return false;
	}

	ITenantInfo* tenantPtr = dynamic_cast<ITenantInfo*>(dataPtr.GetPtr());
	if (tenantPtr == nullptr){
		SendErrorMessage(0, "Invalid tenant object", "CTenantManagerComp");
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	tenantPtr->SetTenantName(tenantName);
	tenantPtr->SetTenantDescription(description);
	if (updateOwner){
		tenantPtr->SetOwnerId(ownerId);
	}
	tenantPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

	if (!m_tenantCollectionCompPtr->SetObjectData(tenantId, *tenantPtr)){
		SendErrorMessage(0, QString("Failed to update tenant '%1'").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Updated tenant '%1'").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");

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
		SendErrorMessage(0, QString("Tenant '%1' not found").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
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
		SendErrorMessage(0, QString("Failed to set tenant '%1' active state").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Set tenant '%1' active=%2").arg(QString::fromUtf8(tenantId)).arg(isActive), "CTenantManagerComp");

	return true;
}


bool CTenantManagerComp::SetTenantHierarchy(const QByteArray& tenantId, const QByteArray& parentTenantId)
{
	if (!m_tenantCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Tenant collection not configured", "CTenantManagerComp");
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_tenantCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		SendErrorMessage(0, QString("Tenant '%1' not found").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
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
		SendErrorMessage(0, QString("Failed to set hierarchy for tenant '%1'").arg(QString::fromUtf8(tenantId)), "CTenantManagerComp");
		return false;
	}

	SendInfoMessage(0, QString("Set hierarchy for tenant '%1' (parent: %2)").arg(
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


} // namespace imtauth
