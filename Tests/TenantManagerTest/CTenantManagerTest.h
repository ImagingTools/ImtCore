// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtTest/QtTest>

#include <memory>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


struct TenantData
{
	QByteArray id;
	QString name;
	QString description;
	QByteArray ownerId;
	bool isActive = true;
	QDateTime createdAt;
	QDateTime updatedAt;
	QByteArray parentTenantId;
	int depth = 0;
	QString materializedPath;
	bool isSystemTenant = false;
};

struct MembershipData
{
	QByteArray id;
	QByteArray userId;
	QByteArray tenantId;
	QByteArray role;
	bool isActive = true;
};

struct PermissionData
{
	QByteArray tenantId;
	QString productId;
	QString permissionId;
};


/**
	Mock implementation of ITenantManager for unit testing.
	Simulates database behavior including cascading deletes.
*/
class CMockTenantManager
{
public:
	QByteArrayList GetTenantIds() const
	{
		return m_tenants.keys();
	}

	std::shared_ptr<TenantData> GetTenant(const QByteArray& id) const
	{
		if (m_tenants.contains(id)){
			return std::make_shared<TenantData>(m_tenants[id]);
		}
		return nullptr;
	}

	QByteArray CreateTenant(const QString& name, const QString& description = QString(), const QByteArray& ownerId = QByteArray())
	{
		if (name.isEmpty()){
			return QByteArray();
		}

		TenantData tenant;
		tenant.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		tenant.name = name;
		tenant.description = description;
		tenant.ownerId = ownerId;
		tenant.isActive = true;
		tenant.createdAt = QDateTime::currentDateTimeUtc();
		tenant.updatedAt = tenant.createdAt;
		tenant.depth = 0;

		m_tenants[tenant.id] = tenant;
		return tenant.id;
	}

	QByteArray CreateChildTenant(const QString& name, const QByteArray& parentTenantId, const QString& description = QString(), const QByteArray& ownerId = QByteArray())
	{
		if (name.isEmpty() || !m_tenants.contains(parentTenantId)){
			return QByteArray();
		}

		TenantData tenant;
		tenant.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		tenant.name = name;
		tenant.description = description;
		tenant.ownerId = ownerId;
		tenant.isActive = true;
		tenant.createdAt = QDateTime::currentDateTimeUtc();
		tenant.updatedAt = tenant.createdAt;
		tenant.parentTenantId = parentTenantId;

		const TenantData& parent = m_tenants[parentTenantId];
		tenant.depth = parent.depth + 1;
		tenant.materializedPath = parent.materializedPath + "/" + QString::fromUtf8(tenant.id);

		m_tenants[tenant.id] = tenant;
		return tenant.id;
	}

	bool EnsureSystemTenant()
	{
		QByteArray systemId = imtauth::SystemTenantId();
		if (m_tenants.contains(systemId)){
			return true;
		}

		TenantData tenant;
		tenant.id = systemId;
		tenant.name = QStringLiteral("System");
		tenant.description = QStringLiteral("Root system tenant");
		tenant.isActive = true;
		tenant.createdAt = QDateTime::currentDateTimeUtc();
		tenant.updatedAt = tenant.createdAt;
		tenant.isSystemTenant = true;
		tenant.depth = 0;
		tenant.materializedPath = QString("/%1").arg(QString::fromUtf8(systemId));

		m_tenants[tenant.id] = tenant;
		return true;
	}

	QByteArray GetSystemTenantId() const
	{
		return imtauth::SystemTenantId();
	}

	bool RemoveTenant(const QByteArray& id)
	{
		if (!m_tenants.contains(id)){
			return false;
		}

		// Cascade memberships (FK ON DELETE CASCADE)
		QByteArrayList membershipKeysToRemove;
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().tenantId == id){
				membershipKeysToRemove.append(it.key());
			}
		}
		for (const auto& key : membershipKeysToRemove){
			m_memberships.remove(key);
		}

		// Cascade permissions (FK ON DELETE CASCADE)
		m_permissions.erase(
			std::remove_if(m_permissions.begin(), m_permissions.end(),
				[&id](const PermissionData& p){ return p.tenantId == id; }),
			m_permissions.end());

		m_tenants.remove(id);
		return true;
	}

	bool UpdateTenant(const QByteArray& id, const QString& name, const QString& description, const QByteArray& ownerId = QByteArray(), bool updateOwner = false)
	{
		if (!m_tenants.contains(id)){
			return false;
		}
		if (name.isEmpty()){
			return false;
		}

		m_tenants[id].name = name;
		m_tenants[id].description = description;
		if (updateOwner){
			m_tenants[id].ownerId = ownerId;
		}
		m_tenants[id].updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	bool SetTenantActive(const QByteArray& id, bool active)
	{
		if (!m_tenants.contains(id)){
			return false;
		}

		m_tenants[id].isActive = active;
		m_tenants[id].updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	// Expose internal storage for cascade verification in tests
	QMap<QByteArray, TenantData> m_tenants;
	QMap<QByteArray, MembershipData> m_memberships;
	QList<PermissionData> m_permissions;
};


} // namespace imtauth


class CTenantManagerTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// CRUD tests
	void testCreateTenant_Success();
	void testCreateTenant_EmptyName_Fails();
	void testGetTenant_Existing();
	void testGetTenant_NonExisting_ReturnsNull();
	void testGetTenantIds_Empty();
	void testGetTenantIds_Multiple();
	void testRemoveTenant_Existing();
	void testRemoveTenant_NonExisting_ReturnsFalse();
	void testUpdateTenant_Success();
	void testUpdateTenant_NonExisting_ReturnsFalse();
	void testUpdateTenant_EmptyName_Fails();
	void testSetTenantActive_Activate();
	void testSetTenantActive_Deactivate();
	void testSetTenantActive_NonExisting_ReturnsFalse();

	// Edge cases
	void testCreateTenant_DuplicateNameAllowed();
	void testRemoveTenant_CascadesMemberships();
	void testRemoveTenant_CascadesPermissions();

	// System-Tenant tests
	void testEnsureSystemTenant_CreatesOnFirstCall();
	void testEnsureSystemTenant_Idempotent();
	void testSystemTenant_HasCorrectProperties();

	// Hierarchy tests
	void testCreateChildTenant_SetsParentAndDepth();
	void testCreateChildTenant_CalculatesMaterializedPath();
	void testCreateChildTenant_InvalidParent_Fails();
	void testHierarchy_MultiLevel();

private:
	imtauth::CMockTenantManager* m_managerPtr = nullptr;
};
