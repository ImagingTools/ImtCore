// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtnotify/INotification.h>
#include <imtservergql/CGqlPublisherCompBase.h>
#include <GeneratedFiles/imtnotifysdl/SDL/1.0/CPP/Notifications.h>


namespace imtnotifygql
{


/**
	Pushes the changes of an inbox to the sessions over the OnNotificationEvent subscription.
	It observes the inbox controller, the only source of the changes, and reads the notifications from the collection;
	a change reaches only the sessions of its user signed in to its tenant and product.
*/
class CNotificationPublisherComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CNotificationPublisherComp);
		I_ASSIGN(m_collectionCompPtr, "NotificationCollection", "Collection of the notifications", true, "NotificationCollection");
		I_ASSIGN(m_inboxControllerModelCompPtr, "InboxController", "Inbox controller telling the changed notifications", true, "InboxController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	enum ModelId
	{
		MI_INBOX_CONTROLLER
	};

	void PublishChange(const imtnotify::NotificationChange& change) const;
	void Publish(
				sdl::V1_0::imtnotify::NotificationEventKind kind,
				const imtnotify::NotificationScope& scope,
				const imtsdl::TElementList<sdl::V1_0::imtnotify::CUserNotification>& notifications,
				const QByteArrayList& ids,
				const QDateTime& createdUntil = QDateTime()) const;

private:
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_REF(imod::IModel, m_inboxControllerModelCompPtr);
};


} // namespace imtnotifygql


