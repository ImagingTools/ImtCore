// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <iser/ISerializable.h>


namespace imtchat
{


/**
	Interface representing a Service Desk support ticket.

	Tickets can be linked to a conversation or a specific message, have
	priorities, statuses, and may target a specific deployment environment.

	\ingroup imtchat
*/
class ITicket: virtual public iser::ISerializable
{
public:
	/**
		Get the unique ticket identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique ticket identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the ticket title / summary.
	*/
	virtual QString GetTitle() const = 0;

	/**
		Set the ticket title.
	*/
	virtual void SetTitle(const QString& title) = 0;

	/**
		Get the detailed ticket description.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set the ticket description.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get the ticket type as integer (maps to TicketType enum).
		0 = AccessRequest, 1 = SupportRequest, 2 = FeatureRequest, 3 = BugReport
	*/
	virtual int GetTicketType() const = 0;

	/**
		Set the ticket type.
	*/
	virtual void SetTicketType(int ticketType) = 0;

	/**
		Get the ticket status as integer (maps to TicketStatus enum).
		0 = Open, 1 = InProgress, 2 = Resolved, 3 = Closed
	*/
	virtual int GetStatus() const = 0;

	/**
		Set the ticket status.
	*/
	virtual void SetStatus(int status) = 0;

	/**
		Get the ticket priority as integer (maps to TicketPriority enum).
		0 = Low, 1 = Medium, 2 = High, 3 = Critical
	*/
	virtual int GetPriority() const = 0;

	/**
		Set the ticket priority.
	*/
	virtual void SetPriority(int priority) = 0;

	/**
		Get the user ID of the assigned agent (may be empty).
	*/
	virtual QByteArray GetAssigneeId() const = 0;

	/**
		Set the assignee user ID.
	*/
	virtual void SetAssigneeId(const QByteArray& assigneeId) = 0;

	/**
		Get the user ID of the reporter who created the ticket.
	*/
	virtual QByteArray GetReporterId() const = 0;

	/**
		Set the reporter user ID.
	*/
	virtual void SetReporterId(const QByteArray& reporterId) = 0;

	/**
		Get the conversation ID linked to this ticket (may be empty).
	*/
	virtual QByteArray GetConversationId() const = 0;

	/**
		Set the conversation ID.
	*/
	virtual void SetConversationId(const QByteArray& conversationId) = 0;

	/**
		Get the message ID that triggered this ticket (may be empty).
	*/
	virtual QByteArray GetMessageId() const = 0;

	/**
		Set the triggering message ID.
	*/
	virtual void SetMessageId(const QByteArray& messageId) = 0;

	/**
		Get the target environment as integer (maps to Environment enum).
		0 = Development, 1 = Staging, 2 = Production
	*/
	virtual int GetEnvironment() const = 0;

	/**
		Set the target environment.
	*/
	virtual void SetEnvironment(int environment) = 0;

	/**
		Get the tags associated with this ticket.
	*/
	virtual QStringList GetTags() const = 0;

	/**
		Set the tags.
	*/
	virtual void SetTags(const QStringList& tags) = 0;

	/**
		Get the ISO 8601 creation timestamp.
	*/
	virtual QString GetCreatedAt() const = 0;

	/**
		Set the creation timestamp.
	*/
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	/**
		Get the ISO 8601 last-update timestamp.
	*/
	virtual QString GetUpdatedAt() const = 0;

	/**
		Set the last-update timestamp.
	*/
	virtual void SetUpdatedAt(const QString& updatedAt) = 0;

	/**
		Get the ISO 8601 resolution timestamp (empty if not resolved).
	*/
	virtual QString GetResolvedAt() const = 0;

	/**
		Set the resolution timestamp.
	*/
	virtual void SetResolvedAt(const QString& resolvedAt) = 0;
};

typedef istd::TUniqueInterfacePtr<ITicket> ITicketUniquePtr;

} // namespace imtchat
