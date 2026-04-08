// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtdesk
{


/**
	Interface representing a single user action on a ticket.

	TicketActions are stored in a separate DB table and linked to tickets
	by TicketId. They are loaded independently and merged with conversation
	messages at the SDL/GQL level to form a unified activity timeline.

	\ingroup imtdesk
*/
class ITicketAction: virtual public iser::ISerializable
{
public:
	/**
		Get the unique action identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique action identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the ticket ID this action belongs to.
	*/
	virtual QByteArray GetTicketId() const = 0;

	/**
		Set the ticket ID.
	*/
	virtual void SetTicketId(const QByteArray& ticketId) = 0;

	/**
		Get the user ID who performed the action.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set the user ID.
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
		Get the user display name.
	*/
	virtual QString GetUserName() const = 0;

	/**
		Set the user display name.
	*/
	virtual void SetUserName(const QString& userName) = 0;

	/**
		Get the action type identifier.
	*/
	virtual QByteArray GetActionTypeId() const = 0;

	/**
		Set the action type identifier.
	*/
	virtual void SetActionTypeId(const QByteArray& actionTypeId) = 0;

	/**
		Get the action type display name (e.g. "StatusChange", "AssigneeChange").
	*/
	virtual QString GetActionTypeName() const = 0;

	/**
		Set the action type display name.
	*/
	virtual void SetActionTypeName(const QString& actionTypeName) = 0;

	/**
		Get the action type description (e.g. "changed status to Closed").
	*/
	virtual QString GetActionTypeDescription() const = 0;

	/**
		Set the action type description.
	*/
	virtual void SetActionTypeDescription(const QString& actionTypeDescription) = 0;

	/**
		Get the ISO 8601 timestamp of when the action occurred.
	*/
	virtual QString GetTimestamp() const = 0;

	/**
		Set the action timestamp.
	*/
	virtual void SetTimestamp(const QString& timestamp) = 0;

	/**
		Get additional action data (free-form text/JSON).
	*/
	virtual QString GetActionData() const = 0;

	/**
		Set additional action data.
	*/
	virtual void SetActionData(const QString& actionData) = 0;
};

typedef istd::TUniqueInterfacePtr<ITicketAction> ITicketActionUniquePtr;


} // namespace imtdesk


