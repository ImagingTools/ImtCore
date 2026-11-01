// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifygql/CNotificationPublisherComp.h>


// Qt includes
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtnotifygql/imtnotifygql.h>


namespace imtnotifygql
{


// reimplemented (imtgql::IGqlSubscriberController)

bool CNotificationPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	return gqlRequest.GetCommandId() == sdl::V1_0::imtnotify::COnNotificationEventGqlRequest::GetCommandId();
}


// reimplemented (icomp::CComponentBase)

void CNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_inboxControllerModelCompPtr.IsValid()){
		RegisterModel(m_inboxControllerModelCompPtr.GetPtr(), MI_INBOX_CONTROLLER);
	}
}


void CNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(imtnotify::NotificationChange::CF_NOTIFICATIONS_CHANGED)){
		return;
	}

	// The changes of different threads can arrive merged in one change set, each under its own key
	const istd::IChangeable::ChangeInfoMap& changeInfoMap = changeSet.GetChangeInfoMap();
	for (auto it = changeInfoMap.cbegin(); it != changeInfoMap.cend(); ++it){
		if (it.key().startsWith(imtnotify::NotificationChange::s_changeKeyPrefix)){
			PublishChange(it.value().value<imtnotify::NotificationChange>());
		}
	}
}


// private methods

void CNotificationPublisherComp::PublishChange(const imtnotify::NotificationChange& change) const
{
	imtsdl::TElementList<sdl::V1_0::imtnotify::CUserNotification> notifications;

	sdl::V1_0::imtnotify::NotificationEventKind kind = sdl::V1_0::imtnotify::NotificationEventKind::Created;
	switch (change.kind){
	case imtnotify::NotificationChange::K_CREATED:
	case imtnotify::NotificationChange::K_UPDATED:
		kind = (change.kind == imtnotify::NotificationChange::K_CREATED)
					? sdl::V1_0::imtnotify::NotificationEventKind::Created
					: sdl::V1_0::imtnotify::NotificationEventKind::Updated;

		for (const QByteArray& notificationId : change.ids){
			imtbase::IObjectCollection::DataPtr dataPtr;
			const imtnotify::INotification* notificationPtr = nullptr;
			if (m_collectionCompPtr->GetObjectData(notificationId, dataPtr)){
				notificationPtr = dynamic_cast<const imtnotify::INotification*>(dataPtr.GetPtr());
			}

			if (notificationPtr != nullptr && notificationPtr->GetScope() == change.scope){
				notifications.append(GetSdlNotification(notificationId, *notificationPtr));
			}
		}

		if (notifications.isEmpty()){
			return;
		}

		Publish(kind, change.scope, notifications, QByteArrayList());
		return;

	case imtnotify::NotificationChange::K_READ:
		kind = sdl::V1_0::imtnotify::NotificationEventKind::Read;
		break;
	case imtnotify::NotificationChange::K_ALL_READ:
		Publish(sdl::V1_0::imtnotify::NotificationEventKind::AllRead, change.scope, notifications, QByteArrayList(), change.createdUntil);
		return;
	case imtnotify::NotificationChange::K_REMOVED:
		kind = sdl::V1_0::imtnotify::NotificationEventKind::Removed;
		break;
	}

	Publish(kind, change.scope, notifications, change.ids);
}


void CNotificationPublisherComp::Publish(
			sdl::V1_0::imtnotify::NotificationEventKind kind,
			const imtnotify::NotificationScope& scope,
			const imtsdl::TElementList<sdl::V1_0::imtnotify::CUserNotification>& notifications,
			const QByteArrayList& ids,
			const QDateTime& createdUntil) const
{
	if (!scope.IsValid()){
		return;
	}

	sdl::V1_0::imtnotify::CNotificationEvent sdlEvent;
	sdlEvent.kind = kind;

	// A negative count tells the client to read its count again: a personal notification reaches sessions
	// of every tenant and product of the user, and each of them counts its own inbox
	sdlEvent.unreadCount = scope.IsPersonal() ? -1 : GetUnreadCount(*m_collectionCompPtr, scope);
	if (!notifications.isEmpty()){
		sdlEvent.items = notifications;
	}
	if (!ids.isEmpty()){
		imtsdl::TElementList<QByteArray> sdlIds;
		sdlIds.FromList(ids);
		sdlEvent.ids = sdlIds;
	}
	if (createdUntil.isValid()){
		sdlEvent.createdUntil = createdUntil.toUTC().toString(Qt::ISODateWithMs);
	}

	QJsonObject jsonObject;
	if (!sdlEvent.WriteToJsonObject(jsonObject)){
		SendErrorMessage(0, QStringLiteral("A notification event could not be serialized"), "CNotificationPublisherComp");

		return;
	}

	const QByteArray data = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);

	PublishDataFiltered(sdl::V1_0::imtnotify::COnNotificationEventGqlRequest::GetCommandId(), data, [scope](const imtgql::CGqlRequest& gqlRequest) -> bool {
		return scope.IsVisibleIn(GetScope(gqlRequest.GetRequestContext()));
	});
}


} // namespace imtnotifygql


