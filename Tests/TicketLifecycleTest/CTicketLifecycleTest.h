// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtTest/QtTest>

#include <memory>


namespace lifecycle
{


enum TicketType { TT_ACCESS_REQUEST, TT_SUPPORT_REQUEST, TT_FEATURE_REQUEST, TT_BUG_REPORT };
enum TicketStatus { TS_OPEN, TS_CLOSED };
enum StateReason { SR_NONE, SR_COMPLETED, SR_NOT_PLANNED, SR_REOPENED };
enum TicketPriority { TP_LOW, TP_MEDIUM, TP_HIGH, TP_CRITICAL };


struct LifecycleTicket
{
	QByteArray id;
	int number = 0;
	QString title;
	TicketType ticketType = TT_BUG_REPORT;
	TicketStatus status = TS_OPEN;
	StateReason stateReason = SR_NONE;
	TicketPriority priority = TP_MEDIUM;
	QByteArrayList assigneeIds;
	QByteArray reporterId;
	QByteArrayList labelIds;
	bool locked = false;
	QString lockReason;
	QDateTime createdAt;
	QDateTime updatedAt;
	QDateTime closedAt;
	QDateTime resolvedAt;
};


/**
	Mock service focusing on ticket state machine transitions.
*/
class CLifecycleTicketService
{
public:
	CLifecycleTicketService() : m_nextNumber(1) {}

	QByteArray CreateTicket(const QString& title, TicketType type, TicketPriority priority, const QByteArray& reporterId)
	{
		LifecycleTicket ticket;
		ticket.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		ticket.number = m_nextNumber++;
		ticket.title = title;
		ticket.ticketType = type;
		ticket.status = TS_OPEN;
		ticket.stateReason = SR_NONE;
		ticket.priority = priority;
		ticket.reporterId = reporterId;
		ticket.locked = false;
		ticket.createdAt = QDateTime::currentDateTimeUtc();
		ticket.updatedAt = ticket.createdAt;

		m_tickets[ticket.id] = ticket;
		return ticket.id;
	}

	std::shared_ptr<LifecycleTicket> GetTicket(const QByteArray& id) const
	{
		if (m_tickets.contains(id)){
			return std::make_shared<LifecycleTicket>(m_tickets[id]);
		}
		return nullptr;
	}

	bool CloseTicket(const QByteArray& id, StateReason reason)
	{
		if (!m_tickets.contains(id)){
			return false;
		}
		LifecycleTicket& ticket = m_tickets[id];
		if (ticket.status != TS_OPEN){
			return false;
		}

		ticket.status = TS_CLOSED;
		ticket.stateReason = reason;
		ticket.closedAt = QDateTime::currentDateTimeUtc();
		ticket.updatedAt = ticket.closedAt;

		if (reason == SR_COMPLETED){
			ticket.resolvedAt = ticket.closedAt;
		}
		return true;
	}

	bool ReopenTicket(const QByteArray& id)
	{
		if (!m_tickets.contains(id)){
			return false;
		}
		LifecycleTicket& ticket = m_tickets[id];
		if (ticket.status != TS_CLOSED){
			return false;
		}

		ticket.status = TS_OPEN;
		ticket.stateReason = SR_REOPENED;
		ticket.closedAt = QDateTime(); // Clear
		ticket.updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	bool SetPriority(const QByteArray& id, TicketPriority priority)
	{
		if (!m_tickets.contains(id)){
			return false;
		}
		m_tickets[id].priority = priority;
		m_tickets[id].updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	bool LockTicket(const QByteArray& id, const QString& reason)
	{
		if (!m_tickets.contains(id)){
			return false;
		}
		m_tickets[id].locked = true;
		m_tickets[id].lockReason = reason;
		m_tickets[id].updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	bool UnlockTicket(const QByteArray& id)
	{
		if (!m_tickets.contains(id)){
			return false;
		}
		m_tickets[id].locked = false;
		m_tickets[id].lockReason.clear();
		m_tickets[id].updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	bool SetAssignees(const QByteArray& id, const QByteArrayList& assignees)
	{
		if (!m_tickets.contains(id)){
			return false;
		}
		if (m_tickets[id].locked){
			return false; // Locked tickets cannot be modified
		}
		m_tickets[id].assigneeIds = assignees;
		m_tickets[id].updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	bool SetLabels(const QByteArray& id, const QByteArrayList& labels)
	{
		if (!m_tickets.contains(id)){
			return false;
		}
		m_tickets[id].labelIds = labels;
		m_tickets[id].updatedAt = QDateTime::currentDateTimeUtc();
		return true;
	}

	QMap<QByteArray, LifecycleTicket> m_tickets;
	int m_nextNumber;
};


} // namespace lifecycle


class CTicketLifecycleTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// State transitions
	void testTicketCreation_InitialState();
	void testTicketOpen_ToClose_Completed();
	void testTicketOpen_ToClose_NotPlanned();
	void testTicketClosed_Reopen();
	void testTicketReopen_ToClose();

	// Priority
	void testTicketPriorityEscalation();
	void testTicketPriorityDeescalation();

	// Locking
	void testTicketLock_PreventsUpdates();
	void testTicketUnlock_AllowsUpdates();

	// Assignment
	void testTicketAssignment_Single();
	void testTicketAssignment_Multiple();
	void testTicketAssignment_Reassign();
	void testTicketAssignment_Unassign();

	// Labels
	void testTicketLabels_AddRemove();

	// Timestamps
	void testTicketTimestamps_CreatedAtImmutable();
	void testTicketTimestamps_UpdatedAtChanges();
	void testTicketTimestamps_ClosedAtOnClose();
	void testTicketTimestamps_ResolvedAtOnResolve();

	// Number
	void testTicketNumber_Sequential();

private:
	lifecycle::CLifecycleTicketService* m_servicePtr = nullptr;
	QByteArray m_reporterId;
};
