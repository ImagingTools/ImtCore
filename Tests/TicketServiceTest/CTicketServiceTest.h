// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtTest/QtTest>

#include <memory>


namespace imtdesk
{


enum TicketType { TT_ACCESS_REQUEST, TT_SUPPORT_REQUEST, TT_FEATURE_REQUEST, TT_BUG_REPORT };
enum TicketStatus { TS_OPEN, TS_CLOSED };
enum StateReason { SR_NONE, SR_COMPLETED, SR_NOT_PLANNED, SR_REOPENED };
enum TicketPriority { TP_LOW, TP_MEDIUM, TP_HIGH, TP_CRITICAL };


struct MockTicketData
{
	QByteArray id;
	int number = 0;
	QString title;
	QString description;
	TicketType ticketType = TT_SUPPORT_REQUEST;
	TicketStatus status = TS_OPEN;
	StateReason stateReason = SR_NONE;
	TicketPriority priority = TP_MEDIUM;
	QByteArrayList assigneeIds;
	QByteArray reporterId;
	QByteArray conversationId;
	QByteArray messageId;
	QByteArrayList labelIds;
	bool locked = false;
	QString lockReason;
	QString createdAt;
	QString updatedAt;
	QString closedAt;
	QString resolvedAt;
};


/**
	Mock implementation of ITicketService for unit testing.
	Simulates auto-increment ticket number and pagination.
*/
class CMockTicketService
{
public:
	CMockTicketService() : m_nextNumber(1) {}

	QByteArray CreateTicket(
		const QString& title,
		const QString& description,
		TicketType ticketType,
		TicketPriority priority,
		const QByteArrayList& assigneeIds = QByteArrayList(),
		const QByteArray& conversationId = QByteArray(),
		const QByteArray& messageId = QByteArray())
	{
		if (title.isEmpty()){
			return QByteArray();
		}

		MockTicketData ticket;
		ticket.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		ticket.number = m_nextNumber++;
		ticket.title = title;
		ticket.description = description;
		ticket.ticketType = ticketType;
		ticket.status = TS_OPEN;
		ticket.stateReason = SR_NONE;
		ticket.priority = priority;
		ticket.assigneeIds = assigneeIds;
		ticket.conversationId = conversationId;
		ticket.messageId = messageId;
		ticket.locked = false;
		ticket.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
		ticket.updatedAt = ticket.createdAt;

		m_tickets[ticket.id] = ticket;
		m_orderedIds.append(ticket.id);
		return ticket.id;
	}

	bool UpdateTicket(const QByteArray& ticketId, const MockTicketData& newData)
	{
		if (!m_tickets.contains(ticketId)){
			return false;
		}

		MockTicketData& ticket = m_tickets[ticketId];
		ticket.title = newData.title;
		ticket.description = newData.description;
		ticket.ticketType = newData.ticketType;
		ticket.status = newData.status;
		ticket.stateReason = newData.stateReason;
		ticket.priority = newData.priority;
		ticket.assigneeIds = newData.assigneeIds;
		ticket.labelIds = newData.labelIds;
		ticket.locked = newData.locked;
		ticket.lockReason = newData.lockReason;
		ticket.closedAt = newData.closedAt;
		ticket.resolvedAt = newData.resolvedAt;
		ticket.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
		return true;
	}

	QByteArrayList GetTickets(int statusFilter = -1, int offset = 0, int limit = -1) const
	{
		QByteArrayList result;
		for (const QByteArray& id : m_orderedIds){
			const MockTicketData& ticket = m_tickets[id];
			if (statusFilter >= 0 && static_cast<int>(ticket.status) != statusFilter){
				continue;
			}
			result.append(id);
		}

		// Apply pagination
		if (offset > 0){
			if (offset >= result.size()){
				return QByteArrayList();
			}
			result = result.mid(offset);
		}
		if (limit >= 0 && limit < result.size()){
			result = result.mid(0, limit);
		}
		return result;
	}

	std::shared_ptr<MockTicketData> GetTicket(const QByteArray& ticketId) const
	{
		if (m_tickets.contains(ticketId)){
			return std::make_shared<MockTicketData>(m_tickets[ticketId]);
		}
		return nullptr;
	}

	QMap<QByteArray, MockTicketData> m_tickets;
	QByteArrayList m_orderedIds;
	int m_nextNumber;
};


} // namespace imtdesk


class CTicketServiceTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Create tests
	void testCreateTicket_Success();
	void testCreateTicket_EmptyTitle_Fails();
	void testCreateTicket_AllFieldsSet();
	void testCreateTicket_AutoIncrementNumber();

	// Get tests
	void testGetTicket_Existing();
	void testGetTicket_NonExisting_ReturnsNull();
	void testGetTickets_NoFilter();
	void testGetTickets_FilterByStatus();
	void testGetTickets_Pagination();
	void testGetTickets_EmptyCollection();

	// Update tests
	void testUpdateTicket_Success();
	void testUpdateTicket_NonExisting_ReturnsFalse();
	void testUpdateTicket_ChangeStatus();
	void testUpdateTicket_ChangePriority();
	void testUpdateTicket_AddAssignees();
	void testUpdateTicket_LockTicket();
	void testUpdateTicket_SetClosedAt();

private:
	imtdesk::CMockTicketService* m_servicePtr = nullptr;
};
