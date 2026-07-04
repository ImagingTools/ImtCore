// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifygql/CNotificationsControllerComp.h>
#include <GeneratedFiles/imtnotifysdl/SDL/1.0/CPP/Notifications.h>

// Qt includes
#include <QtCore/QtGlobal>

// ImtCore includes
#include <imtnotify/INotification.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace
{


sdl::V1_0::imtnotify::ENotificationSeverity GetSdlSeverity(imtnotify::INotification::Severity severity)
{
	switch (severity){
	case imtnotify::INotification::SV_SUCCESS:
		return sdl::V1_0::imtnotify::ENotificationSeverity::Success;
	case imtnotify::INotification::SV_WARNING:
		return sdl::V1_0::imtnotify::ENotificationSeverity::Warning;
	case imtnotify::INotification::SV_CRITICAL:
		return sdl::V1_0::imtnotify::ENotificationSeverity::Critical;
	case imtnotify::INotification::SV_INFO:
	default:
		return sdl::V1_0::imtnotify::ENotificationSeverity::Info;
	}
}


imtnotify::INotification::Severity GetSeverityFromSdl(sdl::V1_0::imtnotify::ENotificationSeverity severity)
{
	switch (severity){
	case sdl::V1_0::imtnotify::ENotificationSeverity::Success:
		return imtnotify::INotification::SV_SUCCESS;
	case sdl::V1_0::imtnotify::ENotificationSeverity::Warning:
		return imtnotify::INotification::SV_WARNING;
	case sdl::V1_0::imtnotify::ENotificationSeverity::Critical:
		return imtnotify::INotification::SV_CRITICAL;
	case sdl::V1_0::imtnotify::ENotificationSeverity::Info:
	default:
		return imtnotify::INotification::SV_INFO;
	}
}


QString BuildPreview(const QString& body)
{
	QString preview = body.simplified();
	const int maxLength = 120;
	if (preview.length() > maxLength){
		preview = preview.left(maxLength) + QStringLiteral("…");
	}
	return preview;
}


} // namespace


namespace imtnotifygql
{


// protected methods

sdl::V1_0::imtnotify::CNotificationsListPayload CNotificationsControllerComp::OnNotificationsList(
			const sdl::V1_0::imtnotify::CNotificationsListGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationsListPayload response;

	if (!m_notificationServiceCompPtr.IsValid() || !m_notificationCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Notification service is not configured");
		return response;
	}

	sdl::V1_0::imtnotify::NotificationsListRequestArguments arguments = request.GetRequestedArguments();

	QByteArray explicitRecipientId;
	if (arguments.input->recipientId){
		explicitRecipientId = *arguments.input->recipientId;
	}

	imtnotify::INotificationService::ListFilter filter;
	filter.recipientId = ResolveRecipientId(gqlRequest, explicitRecipientId);

	if (arguments.input->unreadOnly){
		filter.unreadOnly = *arguments.input->unreadOnly;
	}
	if (arguments.input->category){
		filter.category = *arguments.input->category;
	}
	if (arguments.input->severity){
		filter.severity = *arguments.input->severity;
	}
	if (arguments.input->sortOrder && *arguments.input->sortOrder == sdl::V1_0::imtnotify::ENotificationSortOrder::OldestFirst){
		filter.sortOrder = imtnotify::INotificationService::SO_OLDEST_FIRST;
	}
	if (arguments.input->viewParams){
		if (arguments.input->viewParams->offset){
			filter.offset = qMax(0, *arguments.input->viewParams->offset);
		}
		if (arguments.input->viewParams->count){
			filter.limit = qMax(0, *arguments.input->viewParams->count);
		}
	}

	const QByteArrayList ids = m_notificationServiceCompPtr->GetNotifications(filter);

	QList<sdl::V1_0::imtnotify::CNotificationItemData> itemList;
	itemList.reserve(ids.size());
	for (const QByteArray& id: ids){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_notificationCollectionCompPtr->GetObjectData(id, dataPtr)){
			continue;
		}
		const imtnotify::INotification* notification = dynamic_cast<const imtnotify::INotification*>(dataPtr.GetPtr());
		if (notification == nullptr){
			continue;
		}

		sdl::V1_0::imtnotify::CNotificationItemData itemData;
		itemData.id = notification->GetId();
		itemData.category = notification->GetCategory();
		itemData.title = notification->GetTitle();
		itemData.preview = BuildPreview(notification->GetBody());
		itemData.iconName = notification->GetIconName();
		itemData.severity = GetSdlSeverity(notification->GetSeverity());
		itemData.isRead = notification->IsRead();
		itemData.sourceType = notification->GetSourceType();
		itemData.sourceId = notification->GetSourceId();
		itemData.targetRoute = notification->GetTargetRoute();
		itemData.createdAt = notification->GetCreatedAt();

		itemList << itemData;
	}

	if (!itemList.isEmpty()){
		response.items.Emplace().FromList(itemList);
	}
	response.unreadCount = m_notificationServiceCompPtr->GetUnreadCount(filter.recipientId);
	response.totalCount = m_notificationServiceCompPtr->GetTotalCount(filter);

	return response;
}


sdl::V1_0::imtnotify::CNotificationData CNotificationsControllerComp::OnNotificationItem(
			const sdl::V1_0::imtnotify::CNotificationItemGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationData response;

	if (!m_notificationCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Notification collection is not configured");
		return response;
	}

	sdl::V1_0::imtnotify::NotificationItemRequestArguments arguments = request.GetRequestedArguments();
	QByteArray notificationId;
	if (arguments.input->id){
		notificationId = *arguments.input->id;
	}

	const QByteArray recipientId = ResolveRecipientId(gqlRequest, QByteArray());
	if (!IsOwnedBy(notificationId, recipientId)){
		errorMessage = QStringLiteral("Notification not found");
		return response;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_notificationCollectionCompPtr->GetObjectData(notificationId, dataPtr)){
		errorMessage = QStringLiteral("Notification not found");
		return response;
	}

	const imtnotify::INotification* notification = dynamic_cast<const imtnotify::INotification*>(dataPtr.GetPtr());
	if (notification == nullptr){
		errorMessage = QStringLiteral("Notification not found");
		return response;
	}

	response.id = notification->GetId();
	response.recipientId = notification->GetRecipientId();
	response.category = notification->GetCategory();
	response.title = notification->GetTitle();
	response.body = notification->GetBody();
	response.iconName = notification->GetIconName();
	response.severity = GetSdlSeverity(notification->GetSeverity());
	response.isRead = notification->IsRead();
	response.sourceType = notification->GetSourceType();
	response.sourceId = notification->GetSourceId();
	response.targetRoute = notification->GetTargetRoute();
	response.metadata = notification->GetMetadata();
	response.createdAt = notification->GetCreatedAt();
	response.updatedAt = notification->GetUpdatedAt();

	return response;
}


sdl::V1_0::imtnotify::CUnreadNotificationsCountPayload CNotificationsControllerComp::OnUnreadNotificationsCount(
			const sdl::V1_0::imtnotify::CUnreadNotificationsCountGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	Q_UNUSED(errorMessage);

	sdl::V1_0::imtnotify::CUnreadNotificationsCountPayload response;
	response.unreadCount = 0;

	if (!m_notificationServiceCompPtr.IsValid()){
		return response;
	}

	sdl::V1_0::imtnotify::UnreadNotificationsCountRequestArguments arguments = request.GetRequestedArguments();
	QByteArray explicitRecipientId;
	if (arguments.input->recipientId){
		explicitRecipientId = *arguments.input->recipientId;
	}
	const QByteArray recipientId = ResolveRecipientId(gqlRequest, explicitRecipientId);

	response.unreadCount = m_notificationServiceCompPtr->GetUnreadCount(recipientId);

	return response;
}


sdl::V1_0::imtbase::CUpdatedNotificationPayload CNotificationsControllerComp::OnMarkNotificationRead(
			const sdl::V1_0::imtnotify::CMarkNotificationReadGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CUpdatedNotificationPayload response;

	if (!m_notificationServiceCompPtr.IsValid()){
		errorMessage = QStringLiteral("Notification service is not configured");
		return response;
	}

	sdl::V1_0::imtnotify::MarkNotificationReadRequestArguments arguments = request.GetRequestedArguments();
	QByteArray notificationId;
	if (arguments.input->id){
		notificationId = *arguments.input->id;
	}

	bool isRead = true;
	if (arguments.input->isRead){
		isRead = *arguments.input->isRead;
	}

	const QByteArray recipientId = ResolveRecipientId(gqlRequest, QByteArray());
	if (!IsOwnedBy(notificationId, recipientId)){
		errorMessage = QStringLiteral("Notification not found");
		return response;
	}

	if (m_notificationServiceCompPtr->SetNotificationRead(notificationId, isRead)){
		response.id = notificationId;
	}

	return response;
}


sdl::V1_0::imtnotify::CMarkAllNotificationsReadPayload CNotificationsControllerComp::OnMarkAllNotificationsRead(
			const sdl::V1_0::imtnotify::CMarkAllNotificationsReadGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	Q_UNUSED(errorMessage);

	sdl::V1_0::imtnotify::CMarkAllNotificationsReadPayload response;
	response.updatedCount = 0;

	if (!m_notificationServiceCompPtr.IsValid()){
		return response;
	}

	sdl::V1_0::imtnotify::MarkAllNotificationsReadRequestArguments arguments = request.GetRequestedArguments();
	QByteArray explicitRecipientId;
	if (arguments.input->recipientId){
		explicitRecipientId = *arguments.input->recipientId;
	}
	const QByteArray recipientId = ResolveRecipientId(gqlRequest, explicitRecipientId);

	response.updatedCount = m_notificationServiceCompPtr->MarkAllRead(recipientId);

	return response;
}


sdl::V1_0::imtbase::CRemovedNotificationPayload CNotificationsControllerComp::OnDeleteNotification(
			const sdl::V1_0::imtnotify::CDeleteNotificationGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRemovedNotificationPayload response;

	if (!m_notificationServiceCompPtr.IsValid()){
		errorMessage = QStringLiteral("Notification service is not configured");
		return response;
	}

	sdl::V1_0::imtnotify::DeleteNotificationRequestArguments arguments = request.GetRequestedArguments();
	QByteArray notificationId;
	if (arguments.input->id){
		notificationId = *arguments.input->id;
	}

	const QByteArray recipientId = ResolveRecipientId(gqlRequest, QByteArray());
	if (!IsOwnedBy(notificationId, recipientId)){
		errorMessage = QStringLiteral("Notification not found");
		return response;
	}

	if (m_notificationServiceCompPtr->DeleteNotification(notificationId)){
		QList<QByteArray> removedIds;
		removedIds << notificationId;
		response.elementIds.Emplace().FromList(removedIds);
	}

	return response;
}


sdl::V1_0::imtbase::CAddedNotificationPayload CNotificationsControllerComp::OnRaiseNotification(
			const sdl::V1_0::imtnotify::CRaiseNotificationGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	Q_UNUSED(gqlRequest);

	sdl::V1_0::imtbase::CAddedNotificationPayload response;

	if (!m_notificationServiceCompPtr.IsValid()){
		errorMessage = QStringLiteral("Notification service is not configured");
		return response;
	}

	sdl::V1_0::imtnotify::RaiseNotificationRequestArguments arguments = request.GetRequestedArguments();

	imtnotify::INotificationService::NotificationParams params;
	if (arguments.input->recipientId){
		params.recipientId = *arguments.input->recipientId;
	}
	if (arguments.input->category){
		params.category = *arguments.input->category;
	}
	if (arguments.input->title){
		params.title = *arguments.input->title;
	}
	if (arguments.input->body){
		params.body = *arguments.input->body;
	}
	if (arguments.input->iconName){
		params.iconName = *arguments.input->iconName;
	}
	if (arguments.input->severity){
		params.severity = GetSeverityFromSdl(*arguments.input->severity);
	}
	if (arguments.input->sourceType){
		params.sourceType = *arguments.input->sourceType;
	}
	if (arguments.input->sourceId){
		params.sourceId = *arguments.input->sourceId;
	}
	if (arguments.input->targetRoute){
		params.targetRoute = *arguments.input->targetRoute;
	}
	if (arguments.input->metadata){
		params.metadata = *arguments.input->metadata;
	}

	const QByteArray newId = m_notificationServiceCompPtr->RaiseNotification(params);
	if (newId.isEmpty()){
		errorMessage = QStringLiteral("Unable to raise notification");
		return response;
	}

	response.id = newId;

	return response;
}


// private methods

QByteArray CNotificationsControllerComp::ResolveRecipientId(
			const ::imtgql::CGqlRequest& gqlRequest,
			const QByteArray& explicitRecipientId) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		const QByteArray userId = contextPtr->GetUserId();
		if (!userId.isEmpty()){
			return userId;
		}
	}
	return explicitRecipientId;
}


bool CNotificationsControllerComp::IsOwnedBy(const QByteArray& notificationId, const QByteArray& recipientId) const
{
	if (notificationId.isEmpty() || !m_notificationCollectionCompPtr.IsValid()){
		return false;
	}

	// An empty recipient scope means an internal/service caller with no bound
	// user; ownership cannot be restricted in that case.
	if (recipientId.isEmpty()){
		return true;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_notificationCollectionCompPtr->GetObjectData(notificationId, dataPtr)){
		return false;
	}

	const imtnotify::INotification* notification = dynamic_cast<const imtnotify::INotification*>(dataPtr.GetPtr());
	if (notification == nullptr){
		return false;
	}

	return notification->GetRecipientId() == recipientId;
}


} // namespace imtnotifygql
