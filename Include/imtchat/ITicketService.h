// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtchat/ITicket.h>


namespace imtchat
{


/**
	Service interface for ticket management operations.

	Provides CRUD operations on Service Desk tickets.

	\ingroup imtchat
*/
class ITicketService: virtual public istd::IInterface
{
public:
	/**
		Create a new support ticket.
		\param title          Short summary of the issue.
		\param description    Detailed description.
		\param ticketType     Type (0=AccessRequest, 1=SupportRequest, 2=FeatureRequest, 3=BugReport).
		\param priority       Priority (0=Low, 1=Medium, 2=High, 3=Critical).
		\param assigneeId     Optional assignee user ID.
		\param conversationId Optional linked conversation ID.
		\param messageId      Optional triggering message ID.
		\param environment    Target environment (0=Development, 1=Staging, 2=Production).
		\param tags           Optional tag list.
		\return New ticket ID, or empty on failure.
	*/
	virtual QByteArray CreateTicket(
				const QString& title,
				const QString& description,
				int ticketType,
				int priority,
				const QByteArray& assigneeId = QByteArray(),
				const QByteArray& conversationId = QByteArray(),
				const QByteArray& messageId = QByteArray(),
				int environment = 2,
				const QStringList& tags = QStringList()) = 0;

	/**
		Update an existing ticket.
		\param ticketId  Ticket to update.
		\param ticket    New ticket data.
		\return True on success.
	*/
	virtual bool UpdateTicket(const QByteArray& ticketId, const ITicket& ticket) = 0;

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
	virtual ITicketUniquePtr GetTicket(const QByteArray& ticketId) const = 0;
};

} // namespace imtchat
