// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtTest/QtTest>

#include <memory>


namespace isolation
{


struct TenantRecord
{
	QByteArray id;
	QString name;
	bool isActive = true;
};

struct MembershipRecord
{
	QByteArray id;
	QByteArray userId;
	QByteArray tenantId;
	QByteArray role;
};

struct PermissionRecord
{
	QByteArray tenantId;
	QString resourceId;
	QString level;
};

struct TeamRecord
{
	QByteArray teamId;
	QByteArray tenantId;
};

struct InvitationRecord
{
	QByteArray id;
	QByteArray userId;
	QByteArray tenantId;
	int status = 0; // TIS_PENDING
};

struct TicketRecord
{
	QByteArray id;
	QByteArray tenantId; // Tickets are tenant-scoped
	QString title;
	QByteArray reporterId;
};


/**
	Multi-tenant mock store simulating complete tenant isolation.
	Provides thread-safe operations for concurrency tests.
*/
class CIsolationStore
{
public:
	// Tenant operations
	QByteArray CreateTenant(const QString& name)
	{
		QMutexLocker locker(&m_mutex);
		TenantRecord t;
		t.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		t.name = name;
		t.isActive = true;
		m_tenants[t.id] = t;
		return t.id;
	}

	bool RemoveTenant(const QByteArray& tenantId)
	{
		QMutexLocker locker(&m_mutex);
		if (!m_tenants.contains(tenantId)){
			return false;
		}

		// Cascade all dependencies
		QByteArrayList membershipKeys;
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				membershipKeys.append(it.key());
			}
		}
		for (const auto& key : membershipKeys){
			m_memberships.remove(key);
		}

		m_permissions.erase(
			std::remove_if(m_permissions.begin(), m_permissions.end(),
				[&tenantId](const PermissionRecord& p){ return p.tenantId == tenantId; }),
			m_permissions.end());

		m_teams.erase(
			std::remove_if(m_teams.begin(), m_teams.end(),
				[&tenantId](const TeamRecord& t){ return t.tenantId == tenantId; }),
			m_teams.end());

		QByteArrayList invKeys;
		for (auto it = m_invitations.constBegin(); it != m_invitations.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				invKeys.append(it.key());
			}
		}
		for (const auto& key : invKeys){
			m_invitations.remove(key);
		}

		QByteArrayList ticketKeys;
		for (auto it = m_tickets.constBegin(); it != m_tickets.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				ticketKeys.append(it.key());
			}
		}
		for (const auto& key : ticketKeys){
			m_tickets.remove(key);
		}

		m_tenants.remove(tenantId);
		return true;
	}

	// Membership operations with UNIQUE constraint
	QByteArray AddMembership(const QByteArray& userId, const QByteArray& tenantId, const QByteArray& role)
	{
		QMutexLocker locker(&m_mutex);

		// UNIQUE(userId, tenantId) constraint
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().userId == userId && it.value().tenantId == tenantId){
				return QByteArray(); // Constraint violation
			}
		}

		MembershipRecord m;
		m.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		m.userId = userId;
		m.tenantId = tenantId;
		m.role = role;
		m_memberships[m.id] = m;
		return m.id;
	}

	QByteArrayList GetMembershipsByTenant(const QByteArray& tenantId) const
	{
		QMutexLocker locker(&m_mutex);
		QByteArrayList result;
		for (auto it = m_memberships.constBegin(); it != m_memberships.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				result.append(it.key());
			}
		}
		return result;
	}

	// Permission operations
	void AddPermission(const QByteArray& tenantId, const QString& resourceId, const QString& level)
	{
		QMutexLocker locker(&m_mutex);
		m_permissions.append({tenantId, resourceId, level});
	}

	QList<PermissionRecord> GetPermissionsByTenant(const QByteArray& tenantId) const
	{
		QMutexLocker locker(&m_mutex);
		QList<PermissionRecord> result;
		for (const auto& p : m_permissions){
			if (p.tenantId == tenantId){
				result.append(p);
			}
		}
		return result;
	}

	// Team operations
	void AddTeam(const QByteArray& teamId, const QByteArray& tenantId)
	{
		QMutexLocker locker(&m_mutex);
		m_teams.append({teamId, tenantId});
	}

	QList<TeamRecord> GetTeamsByTenant(const QByteArray& tenantId) const
	{
		QMutexLocker locker(&m_mutex);
		QList<TeamRecord> result;
		for (const auto& t : m_teams){
			if (t.tenantId == tenantId){
				result.append(t);
			}
		}
		return result;
	}

	// Invitation operations
	QByteArray AddInvitation(const QByteArray& userId, const QByteArray& tenantId)
	{
		QMutexLocker locker(&m_mutex);
		InvitationRecord inv;
		inv.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		inv.userId = userId;
		inv.tenantId = tenantId;
		inv.status = 0;
		m_invitations[inv.id] = inv;
		return inv.id;
	}

	QByteArrayList GetInvitationsByTenant(const QByteArray& tenantId) const
	{
		QMutexLocker locker(&m_mutex);
		QByteArrayList result;
		for (auto it = m_invitations.constBegin(); it != m_invitations.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				result.append(it.key());
			}
		}
		return result;
	}

	// Ticket operations
	QByteArray AddTicket(const QByteArray& tenantId, const QString& title, const QByteArray& reporterId)
	{
		QMutexLocker locker(&m_mutex);
		TicketRecord ticket;
		ticket.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		ticket.tenantId = tenantId;
		ticket.title = title;
		ticket.reporterId = reporterId;
		m_tickets[ticket.id] = ticket;
		return ticket.id;
	}

	QByteArrayList GetTicketsByTenant(const QByteArray& tenantId) const
	{
		QMutexLocker locker(&m_mutex);
		QByteArrayList result;
		for (auto it = m_tickets.constBegin(); it != m_tickets.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				result.append(it.key());
			}
		}
		return result;
	}

	// Role update (for concurrency test)
	bool UpdateMembershipRole(const QByteArray& membershipId, const QByteArray& newRole)
	{
		QMutexLocker locker(&m_mutex);
		if (!m_memberships.contains(membershipId)){
			return false;
		}
		m_memberships[membershipId].role = newRole;
		return true;
	}

	int GetTenantCount() const
	{
		QMutexLocker locker(&m_mutex);
		return m_tenants.size();
	}

	int GetMembershipCount() const
	{
		QMutexLocker locker(&m_mutex);
		return m_memberships.size();
	}

private:
	mutable QMutex m_mutex;
	QMap<QByteArray, TenantRecord> m_tenants;
	QMap<QByteArray, MembershipRecord> m_memberships;
	QList<PermissionRecord> m_permissions;
	QList<TeamRecord> m_teams;
	QMap<QByteArray, InvitationRecord> m_invitations;
	QMap<QByteArray, TicketRecord> m_tickets;
};


} // namespace isolation


class CTenantIsolationTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Isolation tests
	void testMembershipIsolation_DifferentTenants();
	void testPermissionIsolation_DifferentTenants();
	void testTicketIsolation_DifferentTenants();
	void testTeamIsolation_BetweenTenants();
	void testInvitationIsolation_PerTenant();

	// Concurrency tests
	void testConcurrentMembershipCreation_UniqueConstraint();
	void testConcurrentTenantDeletion_Consistency();
	void testConcurrentRoleUpdate_LastWriteWins();

	// Cascade tests
	void testTenantDeletion_CleansAllDependencies();
	void testDeactivatedTenant_DataPreserved();

	// Cross-tenant access
	void testCrossTenantAccess_Denied();
	void testResourcePermission_IsolatedPerTenant();

private:
	isolation::CIsolationStore* m_storePtr = nullptr;
	QByteArray m_tenantA;
	QByteArray m_tenantB;
};
