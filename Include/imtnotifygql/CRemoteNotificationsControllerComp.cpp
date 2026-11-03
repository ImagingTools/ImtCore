// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifygql/CRemoteNotificationsControllerComp.h>


// ImtCore includes
#include <imtnotifygql/imtnotifygql.h>


namespace imtnotifygql
{


// reimplemented (sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase)

sdl::V1_0::imtnotify::CNotificationsListPayload CRemoteNotificationsControllerComp::OnNotificationsList(
			const sdl::V1_0::imtnotify::CNotificationsListGqlRequest& /*notificationsListRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsListPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsUnreadCountPayload CRemoteNotificationsControllerComp::OnNotificationsUnreadCount(
			const sdl::V1_0::imtnotify::CNotificationsUnreadCountGqlRequest& /*notificationsUnreadCountRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsUnreadCountPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationTypesPayload CRemoteNotificationsControllerComp::OnNotificationTypes(
			const sdl::V1_0::imtnotify::CNotificationTypesGqlRequest& /*notificationTypesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr || contextPtr->GetUserId().isEmpty()){
		errorMessage = QStringLiteral("Not signed in");

		return sdl::V1_0::imtnotify::CNotificationTypesPayload();
	}

	QStringList errors;
	const sdl::V1_0::imtnotify::CNotificationTypesPayload retVal = CreateTypeCatalog(
				GetTypeLists(m_notificationTypesCompPtr) + GetTypeLists(m_productNotificationTypesCompPtr),
				m_translationManagerCompPtr.GetPtr(),
				GetLanguageId(contextPtr),
				errors);
	for (const QString& error : std::as_const(errors)){
		if (m_reportedCatalogErrors.Insert(error)){
			SendWarningMessage(0, error, "CRemoteNotificationsControllerComp");
		}
	}

	return retVal;
}


sdl::V1_0::imtnotify::CNotificationsSyncPayload CRemoteNotificationsControllerComp::OnNotificationsSync(
			const sdl::V1_0::imtnotify::CNotificationsSyncGqlRequest& /*notificationsSyncRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsSyncPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CRemoteNotificationsControllerComp::OnMarkNotificationsRead(
			const sdl::V1_0::imtnotify::CMarkNotificationsReadGqlRequest& /*markNotificationsReadRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsChangedPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CRemoteNotificationsControllerComp::OnMarkAllNotificationsRead(
			const sdl::V1_0::imtnotify::CMarkAllNotificationsReadGqlRequest& /*markAllNotificationsReadRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsChangedPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CRemoteNotificationsControllerComp::OnAcknowledgeNotificationsDelivered(
			const sdl::V1_0::imtnotify::CAcknowledgeNotificationsDeliveredGqlRequest& /*acknowledgeNotificationsDeliveredRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsChangedPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CRemoteNotificationsControllerComp::OnAcknowledgeAllNotificationsDelivered(
			const sdl::V1_0::imtnotify::CAcknowledgeAllNotificationsDeliveredGqlRequest& /*acknowledgeAllNotificationsDeliveredRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsChangedPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CRemoteNotificationsControllerComp::OnRemoveNotifications(
			const sdl::V1_0::imtnotify::CRemoveNotificationsGqlRequest& /*removeNotificationsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtnotify::CNotificationsChangedPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsSentPayload CRemoteNotificationsControllerComp::OnSendNotification(
			const sdl::V1_0::imtnotify::CSendNotificationGqlRequest& /*sendNotificationRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	errorMessage = QStringLiteral("Notifications are sent by the servers");

	return sdl::V1_0::imtnotify::CNotificationsSentPayload();
}


sdl::V1_0::imtnotify::CNotificationsSentPayload CRemoteNotificationsControllerComp::OnWithdrawNotifications(
			const sdl::V1_0::imtnotify::CWithdrawNotificationsGqlRequest& /*withdrawNotificationsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	errorMessage = QStringLiteral("Notifications are withdrawn by the servers");

	return sdl::V1_0::imtnotify::CNotificationsSentPayload();
}


} // namespace imtnotifygql


