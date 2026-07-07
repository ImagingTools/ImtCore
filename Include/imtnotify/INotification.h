// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <iser/ISerializable.h>


namespace imtnotify
{


/**
	Interface representing a single user notification (an "event" in the
	Activity feed).

	A notification is addressed to exactly one recipient user and carries the
	information required both to render it in the Events list / popup and to
	deep-link back to the entity that produced it.

	\ingroup imtnotify
*/
class INotification: virtual public iser::ISerializable
{
public:
	/**
		Visual severity, mirroring the \c NotificationSeverity SDL enum.
	*/
	enum Severity
	{
		SV_INFO,
		SV_SUCCESS,
		SV_WARNING,
		SV_CRITICAL
	};
	I_DECLARE_ENUM(Severity,
				SV_INFO,
				SV_SUCCESS,
				SV_WARNING,
				SV_CRITICAL);

	/**
		Get the unique notification identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique notification identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the identifier of the user this notification is addressed to.
	*/
	virtual QByteArray GetRecipientId() const = 0;

	/**
		Set the recipient user identifier.
	*/
	virtual void SetRecipientId(const QByteArray& recipientId) = 0;

	/**
		Get the producer category (e.g. "Ticket", "Chat", "Tenant", "System").
		Purely informational; used for grouping and iconography.
	*/
	virtual QString GetCategory() const = 0;

	/**
		Set the producer category.
	*/
	virtual void SetCategory(const QString& category) = 0;

	/**
		Get the short notification title.
	*/
	virtual QString GetTitle() const = 0;

	/**
		Set the notification title.
	*/
	virtual void SetTitle(const QString& title) = 0;

	/**
		Get the notification body / long description.
	*/
	virtual QString GetBody() const = 0;

	/**
		Set the notification body.
	*/
	virtual void SetBody(const QString& body) = 0;

	/**
		Get the icon name (an ImtCore icon path such as "Icons/Message").
	*/
	virtual QString GetIconName() const = 0;

	/**
		Set the icon name.
	*/
	virtual void SetIconName(const QString& iconName) = 0;

	/**
		Get the visual severity.
	*/
	virtual Severity GetSeverity() const = 0;

	/**
		Set the visual severity.
	*/
	virtual void SetSeverity(Severity severity) = 0;

	/**
		Whether the recipient has already read this notification.
	*/
	virtual bool IsRead() const = 0;

	/**
		Mark this notification read / unread.
	*/
	virtual void SetRead(bool isRead) = 0;

	/**
		Get the type of the entity that produced the notification
		(e.g. "SupportTicket"). May be empty.
	*/
	virtual QString GetSourceType() const = 0;

	/**
		Set the source entity type.
	*/
	virtual void SetSourceType(const QString& sourceType) = 0;

	/**
		Get the identifier of the entity that produced the notification.
		May be empty.
	*/
	virtual QByteArray GetSourceId() const = 0;

	/**
		Set the source entity identifier.
	*/
	virtual void SetSourceId(const QByteArray& sourceId) = 0;

	/**
		Get the client navigation route to open when the notification is
		clicked (e.g. "Tickets/Ticket/<id>"). May be empty.
	*/
	virtual QString GetTargetRoute() const = 0;

	/**
		Set the client navigation route.
	*/
	virtual void SetTargetRoute(const QString& targetRoute) = 0;

	/**
		Get free-form producer metadata, serialized as a JSON string.
	*/
	virtual QString GetMetadata() const = 0;

	/**
		Set free-form producer metadata (JSON string).
	*/
	virtual void SetMetadata(const QString& metadata) = 0;

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
};

typedef istd::TUniqueInterfacePtr<INotification> INotificationUniquePtr;

} // namespace imtnotify
