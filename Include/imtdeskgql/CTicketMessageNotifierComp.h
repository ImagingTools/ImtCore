// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtdeskgql
{


/**
	Server-side publisher that broadcasts a notification when a new chat
	message is inserted into a conversation that is associated with a support
	ticket.

	The notification is delivered only to subscribers whose authenticated user
	is related to the ticket: the reporter, an assignee, or an administrator.
	Other subscribers (including users in the same group as the reporter, who
	can read the ticket) are not notified to keep the broadcast surface
	minimal.

	The component listens to the message collection via
	imod::TSingleModelObserverBase. When a new message is inserted, it locates
	the parent ticket by scanning the ticket collection for a matching
	conversationId, then publishes a JSON payload of the form

	    { "ticketId": ..., "ticketNumber": ..., "ticketTitle": ...,
	      "messageId": ..., "conversationId": ...,
	      "senderId": ..., "senderUserName": ...,
	      "content": ..., "createdAt": ... }

	via CGqlPublisherCompBase::PublishDataFiltered.

	The subscription endpoint name is configurable via the \c CommandId
	attribute and defaults to "OnTicketMessageReceived".
*/
class CTicketMessageNotifierComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtbase::IObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CTicketMessageNotifierComp);
		I_ASSIGN(m_messageCollectionCompPtr, "MessageCollection", "Chat message collection to observe", true, "MessageCollection");
		I_ASSIGN_TO(m_messageCollectionModelCompPtr, m_messageCollectionCompPtr, true);
		I_ASSIGN(m_ticketCollectionCompPtr, "TicketCollection", "Ticket collection used to resolve conversationId -> ticket", true, "TicketCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection (used by permission helpers)", false, "UserCollection");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider (used by permission helpers)", false, "UserGroupInfoProvider");
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "Subscription command-ID this notifier responds to", false, "OnTicketMessageReceived");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	QByteArray FindTicketIdByConversationId(const QByteArray& conversationId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_messageCollectionCompPtr);
	I_REF(imod::IModel, m_messageCollectionModelCompPtr);
	I_REF(imtbase::IObjectCollection, m_ticketCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtdeskgql

