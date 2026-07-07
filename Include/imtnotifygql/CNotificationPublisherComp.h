// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtnotifygql
{


/**
	Server-side publisher that broadcasts a live notification whenever a new
	imtnotify::INotification is inserted into the notification collection.

	This is the generic counterpart of the per-feature ticket notifiers: because
	notifications already carry their recipient, the publisher simply delivers
	each one to the single subscriber whose authenticated user matches
	\c recipientId. Any producer that raises a notification through
	imtnotify::INotificationService therefore gets real-time delivery for free,
	without adding a new publisher (Open-Closed).

	The subscription endpoint name is configurable via the \c CommandId
	attribute and defaults to "OnNotificationReceived".
*/
class CNotificationPublisherComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtbase::IObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CNotificationPublisherComp);
		I_ASSIGN(m_notificationCollectionCompPtr, "NotificationCollection", "Notification collection to observe", true, "NotificationCollection");
		I_ASSIGN_TO(m_notificationCollectionModelCompPtr, m_notificationCollectionCompPtr, true);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "Subscription command-ID this publisher responds to", false, "OnNotificationReceived");
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
	I_REF(imtbase::IObjectCollection, m_notificationCollectionCompPtr);
	I_REF(imod::IModel, m_notificationCollectionModelCompPtr);
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtnotifygql
