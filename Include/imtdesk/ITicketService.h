// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>


namespace imtdesk
{


/**
	Service interface for ticket management operations.

	Provides CRUD operations on Service Desk tickets.

	\ingroup imtdesk
*/
class ITicketService: virtual public istd::IPolymorphic
{
public:
	/**
		Create a new support ticket.
		\param title          Short summary of the issue.
		\param description    Detailed description.
		\param ticketType     Type of the ticket.
		\param priority       Priority of the ticket.
		\param assigneeIds    Optional list of assignee user IDs.
		\param conversationId Optional linked conversation ID.
		\param messageId      Optional triggering message ID.
		\param environment    Target deployment environment.
		\param tags           Optional tag list.
		\return New ticket ID, or empty on failure.
	*/
	virtual QByteArray CreateTicket(
				const QString& title,
				const QString& description,
				ISupportTicket::TicketType ticketType,
				ISupportTicket::TicketPriority priority,
				const QByteArrayList& assigneeIds = QByteArrayList(),
				const QByteArray& conversationId = QByteArray(),
				const QByteArray& messageId = QByteArray(),
				ISupportTicket::Environment environment = ISupportTicket::ENV_PRODUCTION,
				const QStringList& tags = QStringList()) = 0;

	/**
		Update an existing ticket.
		\param ticketId  Ticket to update.
		\param ticket    New ticket data.
		\return True on success.
	*/
	virtual bool UpdateTicket(const QByteArray& ticketId, const ISupportTicket& ticket) = 0;

	/**
		Retrieve ticket IDs matching optional filters.
		\param statusFilter  Filter by status (-1 = no filter).
		\param offset        Pagination offset.
		\param limit         Maximum number of tickets (-1 = all).
		\return List of ticket IDs.
	*/
	virtual QByteArrayList GetTickets(
				int statusFilter = -1,
				int offset = 0,
				int limit = -1) const = 0;

	/**
		Retrieve a single ticket by ID.
		\param ticketId  Ticket to fetch.
		\return Pointer to ticket data, or null if not found.
	*/
	virtual ISupportTicketUniquePtr GetTicket(const QByteArray& ticketId) const = 0;
};

} // namespace imtdesk
