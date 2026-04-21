// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserRecentAction.h>


namespace imtdesk
{


/**
	Interface representing a single user action on a ticket.

	Extends IUserRecentAction with a TicketId link. TicketActions are stored
	in a separate DB table and linked to tickets by TicketId. They are loaded
	independently and merged with conversation messages at the SDL/GQL level
	to form a unified activity timeline.

	All user, action-type, target, timestamp, and action-data fields are
	inherited from IUserRecentAction.

	\ingroup imtdesk
*/
class ITicketAction: virtual public imtauth::IUserRecentAction
{
public:
	/**
		Get the ticket ID this action belongs to.
	*/
	virtual QByteArray GetTicketId() const = 0;

	/**
		Set the ticket ID.
	*/
	virtual void SetTicketId(const QByteArray& ticketId) = 0;
};

typedef istd::TUniqueInterfacePtr<ITicketAction> ITicketActionUniquePtr;


} // namespace imtdesk


