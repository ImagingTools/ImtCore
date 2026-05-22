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
	Server-side publisher that broadcasts a notification when the assignee
	list of a support ticket changes.

	The notification is delivered only to subscribers whose authenticated user
	is a newly added assignee. The user who made the change is excluded
	(no self-notification).

	The component listens to the ticket collection via
	imod::TSingleModelObserverBase. When a ticket is updated, it compares
	the old and new assignee lists and publishes a JSON payload of the form

	    { "ticketId": ..., "ticketNumber": ..., "ticketTitle": ...,
	      "changedByUserId": ..., "changedByUserName": ... }

	via CGqlPublisherCompBase::PublishDataFiltered.

	The subscription endpoint name is configurable via the \c CommandId
	attribute and defaults to "OnTicketAssigneeChanged".
*/
class CTicketAssigneeNotifierComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtbase::IObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CTicketAssigneeNotifierComp);
		I_ASSIGN(m_ticketCollectionCompPtr, "TicketCollection", "Ticket collection to observe for assignee changes", true, "TicketCollection");
		I_ASSIGN_TO(m_ticketCollectionModelCompPtr, m_ticketCollectionCompPtr, true);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "Subscription command-ID this notifier responds to", false, "OnTicketAssigneeChanged");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtbase::IObjectCollection, m_ticketCollectionCompPtr);
	I_REF(imod::IModel, m_ticketCollectionModelCompPtr);
	I_ATTR(QByteArray, m_commandIdAttrPtr);

private:
	// Cache of ticketId → assigneeIds for change detection.
	mutable QMap<QByteArray, QByteArrayList> m_cachedAssignees;
};


} // namespace imtdeskgql
