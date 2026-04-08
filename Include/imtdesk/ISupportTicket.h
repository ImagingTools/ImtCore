// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtdesk
{


/**
	Interface representing a Service Desk support ticket.

	Tickets can be linked to a conversation or a specific message, have
	priorities and statuses.

	\ingroup imtdesk
*/
class ISupportTicket: virtual public iser::ISerializable
{
public:
	/**
		Ticket type — describes the nature of the request.
	*/
	enum TicketType
	{
		TT_ACCESS_REQUEST,
		TT_SUPPORT_REQUEST,
		TT_FEATURE_REQUEST,
		TT_BUG_REPORT
	};
	I_DECLARE_ENUM(TicketType,
				TT_ACCESS_REQUEST,
				TT_SUPPORT_REQUEST,
				TT_FEATURE_REQUEST,
				TT_BUG_REPORT);

	/**
		Ticket status — follows GitHub Issues pattern (Open / Closed).
	*/
	enum TicketStatus
	{
		TS_OPEN,
		TS_CLOSED
	};
	I_DECLARE_ENUM(TicketStatus,
				TS_OPEN,
				TS_CLOSED);

	/**
		State reason — why the ticket was closed (like GitHub state_reason).
	*/
	enum StateReason
	{
		SR_NONE,
		SR_COMPLETED,
		SR_NOT_PLANNED,
		SR_REOPENED
	};
	I_DECLARE_ENUM(StateReason,
				SR_NONE,
				SR_COMPLETED,
				SR_NOT_PLANNED,
				SR_REOPENED);

	/**
		Ticket priority — urgency level.
	*/
	enum TicketPriority
	{
		TP_LOW,
		TP_MEDIUM,
		TP_HIGH,
		TP_CRITICAL
	};
	I_DECLARE_ENUM(TicketPriority,
				TP_LOW,
				TP_MEDIUM,
				TP_HIGH,
				TP_CRITICAL);

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
		Get the ticket type.
	*/
	virtual TicketType GetTicketType() const = 0;

	/**
		Set the ticket type.
	*/
	virtual void SetTicketType(TicketType ticketType) = 0;

	/**
		Get the ticket status.
	*/
	virtual TicketStatus GetStatus() const = 0;

	/**
		Set the ticket status.
	*/
	virtual void SetStatus(TicketStatus status) = 0;

	/**
		Get the state reason (why the ticket was closed).
	*/
	virtual StateReason GetStateReason() const = 0;

	/**
		Set the state reason.
	*/
	virtual void SetStateReason(StateReason stateReason) = 0;

	/**
		Get the ticket priority.
	*/
	virtual TicketPriority GetPriority() const = 0;

	/**
		Set the ticket priority.
	*/
	virtual void SetPriority(TicketPriority priority) = 0;

	/**
		Get the list of assigned user IDs (supports multiple assignees like GitHub).
	*/
	virtual QByteArrayList GetAssigneeIds() const = 0;

	/**
		Set the list of assigned user IDs.
	*/
	virtual void SetAssigneeIds(const QByteArrayList& assigneeIds) = 0;

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
		Get the tags associated with this ticket.
	*/
	virtual QStringList GetTags() const = 0;

	/**
		Set the tags.
	*/
	virtual void SetTags(const QStringList& tags) = 0;

	/**
		Get the list of label IDs (structured labels with color/description).
	*/
	virtual QByteArrayList GetLabelIds() const = 0;

	/**
		Set the list of label IDs.
	*/
	virtual void SetLabelIds(const QByteArrayList& labelIds) = 0;

	/**
		Get whether the ticket is locked.
	*/
	virtual bool IsLocked() const = 0;

	/**
		Set the locked state.
	*/
	virtual void SetLocked(bool locked) = 0;

	/**
		Get the lock reason (e.g. "resolved", "off-topic", "too heated", "spam").
	*/
	virtual QString GetLockReason() const = 0;

	/**
		Set the lock reason.
	*/
	virtual void SetLockReason(const QString& lockReason) = 0;

	/**
		Get the human-readable ticket number (like GitHub #123).
	*/
	virtual int GetNumber() const = 0;

	/**
		Set the ticket number.
	*/
	virtual void SetNumber(int number) = 0;

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
		Get the ISO 8601 closed timestamp (empty if not closed).
	*/
	virtual QString GetClosedAt() const = 0;

	/**
		Set the closed timestamp.
	*/
	virtual void SetClosedAt(const QString& closedAt) = 0;

	/**
		Get the ISO 8601 resolution timestamp (empty if not resolved).
	*/
	virtual QString GetResolvedAt() const = 0;

	/**
		Set the resolution timestamp.
	*/
	virtual void SetResolvedAt(const QString& resolvedAt) = 0;

	/**
		Activity item type — comment or user action.
	*/
	enum ActivityItemType
	{
		AIT_COMMENT,
		AIT_ACTION
	};

	/**
		Represents a single entry in the ticket activity timeline.
		Can be either a conversation comment or a user action record.
	*/
	struct ActivityItem
	{
		ActivityItemType itemType = AIT_COMMENT;
		QByteArray userId;
		QString userName;
		QString timestamp;
		// Comment fields
		QString content;
		QStringList reactions;
		// Action fields
		QString actionType;
		QString actionDescription;

		bool operator==(const ActivityItem& other) const
		{
			return itemType == other.itemType
				&& userId == other.userId
				&& userName == other.userName
				&& timestamp == other.timestamp
				&& content == other.content
				&& reactions == other.reactions
				&& actionType == other.actionType
				&& actionDescription == other.actionDescription;
		}

		bool operator!=(const ActivityItem& other) const
		{
			return !(*this == other);
		}
	};

	/**
		Get the list of activity items (comments + actions) for the timeline.
	*/
	virtual QList<ActivityItem> GetActivityItems() const = 0;

	/**
		Set the activity items list.
	*/
	virtual void SetActivityItems(const QList<ActivityItem>& items) = 0;
};

typedef istd::TUniqueInterfacePtr<ISupportTicket> ISupportTicketUniquePtr;


} // namespace imtdesk


