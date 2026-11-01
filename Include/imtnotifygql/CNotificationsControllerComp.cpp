// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifygql/CNotificationsControllerComp.h>


// Qt includes
#include <QtCore/QTimeZone>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtnotifygql/imtnotifygql.h>


namespace
{


const int s_defaultPageSize = 25;
const int s_defaultMaxUndelivered = 20;
const int s_maxPageSize = 200;
const int s_maxRecipients = 100;


QByteArrayList GetIds(const istd::TNullableValue<sdl::V1_0::imtnotify::CNotificationIdsInput>& input)
{
	QByteArrayList retVal;
	if (input && input->ids){
		for (const istd::TNullableValue<QByteArray>& id : *input->ids){
			if (id && !id->isEmpty()){
				retVal.append(*id);
			}
		}
	}

	return retVal;
}


} // namespace


namespace imtnotifygql
{


// reimplemented (sdl::V1_0::imtnotify::CNotificationsGqlHandlerCompBase)

sdl::V1_0::imtnotify::CNotificationsListPayload CNotificationsControllerComp::OnNotificationsList(
			const sdl::V1_0::imtnotify::CNotificationsListGqlRequest& notificationsListRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationsListPayload response;

	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return response;
	}

	int offset = 0;
	int count = s_defaultPageSize;

	const sdl::V1_0::imtnotify::NotificationsListRequestArguments arguments = notificationsListRequest.GetRequestedArguments();
	if (arguments.input){
		if (arguments.input->offset){
			offset = qMax(0, *arguments.input->offset);
		}
		if (arguments.input->pageSize){
			count = qBound(1, *arguments.input->pageSize, s_maxPageSize);
		}
	}

	imtsdl::TElementList<sdl::V1_0::imtnotify::CUserNotification> notifications;
	int totalCount = 0;
	const int unreadCount = GetUnreadCount(*m_collectionCompPtr, scope);
	if (!ReadNotifications(*m_collectionCompPtr, scope, imtnotify::FieldFilters(), offset, count, notifications, &totalCount) || unreadCount < 0){
		errorMessage = QStringLiteral("The notifications could not be read");

		return response;
	}

	response.items = notifications;
	response.totalCount = totalCount;
	response.unreadCount = unreadCount;

	return response;
}


sdl::V1_0::imtnotify::CNotificationsUnreadCountPayload CNotificationsControllerComp::OnNotificationsUnreadCount(
			const sdl::V1_0::imtnotify::CNotificationsUnreadCountGqlRequest& /*notificationsUnreadCountRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationsUnreadCountPayload response;

	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return response;
	}

	const int unreadCount = GetUnreadCount(*m_collectionCompPtr, scope);
	if (unreadCount < 0){
		errorMessage = QStringLiteral("The notifications could not be read");

		return response;
	}

	response.unreadCount = unreadCount;

	return response;
}


sdl::V1_0::imtnotify::CNotificationTypesPayload CNotificationsControllerComp::OnNotificationTypes(
			const sdl::V1_0::imtnotify::CNotificationTypesGqlRequest& /*notificationTypesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return sdl::V1_0::imtnotify::CNotificationTypesPayload();
	}

	QStringList errors;
	const sdl::V1_0::imtnotify::CNotificationTypesPayload retVal = CreateTypeCatalog(
				GetTypeLists(m_notificationTypesCompPtr),
				m_translationManagerCompPtr.GetPtr(),
				GetLanguageId(gqlRequest.GetRequestContext()),
				errors);
	for (const QString& error : std::as_const(errors)){
		if (m_reportedCatalogErrors.Insert(error)){
			SendWarningMessage(0, error, "CNotificationsControllerComp");
		}
	}

	return retVal;
}


sdl::V1_0::imtnotify::CNotificationsSyncPayload CNotificationsControllerComp::OnNotificationsSync(
			const sdl::V1_0::imtnotify::CNotificationsSyncGqlRequest& notificationsSyncRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationsSyncPayload response;

	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return response;
	}

	int count = s_defaultPageSize;
	int maxUndelivered = s_defaultMaxUndelivered;

	const sdl::V1_0::imtnotify::NotificationsSyncRequestArguments arguments = notificationsSyncRequest.GetRequestedArguments();
	if (arguments.input){
		if (arguments.input->pageSize){
			count = qBound(1, *arguments.input->pageSize, s_maxPageSize);
		}
		if (arguments.input->maxUndelivered){
			maxUndelivered = qBound(1, *arguments.input->maxUndelivered, s_maxPageSize);
		}
	}

	// Not received by any session yet: they pop up at the login
	const imtnotify::FieldFilters undeliveredFilters = {
		imtbase::IComplexCollectionFilter::FieldFilter(imtnotify::NotificationField::s_isDelivered, false),
		imtbase::IComplexCollectionFilter::FieldFilter(imtnotify::NotificationField::s_isRead, false)};

	// Taken before the reads: acknowledging until it never confirms a notification this answer did not count
	const QDateTime syncedAt = QDateTime::currentDateTimeUtc();

	imtsdl::TElementList<sdl::V1_0::imtnotify::CUserNotification> notifications;
	imtsdl::TElementList<sdl::V1_0::imtnotify::CUserNotification> undeliveredNotifications;
	int totalCount = 0;
	int undeliveredCount = 0;
	const int unreadCount = GetUnreadCount(*m_collectionCompPtr, scope);
	if (		!ReadNotifications(*m_collectionCompPtr, scope, imtnotify::FieldFilters(), 0, count, notifications, &totalCount) ||
				!ReadNotifications(*m_collectionCompPtr, scope, undeliveredFilters, 0, maxUndelivered, undeliveredNotifications, &undeliveredCount) ||
				unreadCount < 0 ||
				undeliveredCount < 0){
		errorMessage = QStringLiteral("The notifications could not be read");

		return response;
	}

	response.items = notifications;
	response.undelivered = undeliveredNotifications;
	response.totalCount = totalCount;
	response.unreadCount = unreadCount;
	response.undeliveredCount = undeliveredCount;
	response.syncedAt = syncedAt.toString(Qt::ISODateWithMs);

	return response;
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CNotificationsControllerComp::OnMarkNotificationsRead(
			const sdl::V1_0::imtnotify::CMarkNotificationsReadGqlRequest& markNotificationsReadRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return sdl::V1_0::imtnotify::CNotificationsChangedPayload();
	}

	QByteArrayList changedIds;
	const bool isChanged = m_inboxControllerCompPtr->MarkRead(scope, GetIds(markNotificationsReadRequest.GetRequestedArguments().input), changedIds);

	return CreateChangedPayload(scope, isChanged, changedIds, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CNotificationsControllerComp::OnMarkAllNotificationsRead(
			const sdl::V1_0::imtnotify::CMarkAllNotificationsReadGqlRequest& markAllNotificationsReadRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return sdl::V1_0::imtnotify::CNotificationsChangedPayload();
	}

	QDateTime createdUntil;
	const sdl::V1_0::imtnotify::MarkAllNotificationsReadRequestArguments arguments = markAllNotificationsReadRequest.GetRequestedArguments();
	if (!ParseCreatedUntil(arguments.input ? arguments.input->createdUntil : istd::TNullableValue<QString>(), createdUntil)){
		errorMessage = QStringLiteral("createdUntil is not an ISO 8601 time");

		return sdl::V1_0::imtnotify::CNotificationsChangedPayload();
	}

	QByteArrayList changedIds;
	const bool isChanged = m_inboxControllerCompPtr->MarkAllRead(scope, createdUntil, changedIds);

	return CreateChangedPayload(scope, isChanged, QByteArrayList(), errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CNotificationsControllerComp::OnAcknowledgeAllNotificationsDelivered(
			const sdl::V1_0::imtnotify::CAcknowledgeAllNotificationsDeliveredGqlRequest& acknowledgeAllNotificationsDeliveredRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return sdl::V1_0::imtnotify::CNotificationsChangedPayload();
	}

	QDateTime createdUntil;
	const sdl::V1_0::imtnotify::AcknowledgeAllNotificationsDeliveredRequestArguments arguments = acknowledgeAllNotificationsDeliveredRequest.GetRequestedArguments();
	if (!ParseCreatedUntil(arguments.input ? arguments.input->createdUntil : istd::TNullableValue<QString>(), createdUntil)){
		errorMessage = QStringLiteral("createdUntil is not an ISO 8601 time");

		return sdl::V1_0::imtnotify::CNotificationsChangedPayload();
	}

	QByteArrayList changedIds;
	const bool isChanged = m_inboxControllerCompPtr->MarkAllDelivered(scope, createdUntil, changedIds);

	return CreateChangedPayload(scope, isChanged, QByteArrayList(), errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CNotificationsControllerComp::OnAcknowledgeNotificationsDelivered(
			const sdl::V1_0::imtnotify::CAcknowledgeNotificationsDeliveredGqlRequest& acknowledgeNotificationsDeliveredRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return sdl::V1_0::imtnotify::CNotificationsChangedPayload();
	}

	QByteArrayList changedIds;
	const bool isChanged = m_inboxControllerCompPtr->MarkDelivered(scope, GetIds(acknowledgeNotificationsDeliveredRequest.GetRequestedArguments().input), changedIds);

	return CreateChangedPayload(scope, isChanged, changedIds, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CNotificationsControllerComp::OnRemoveNotifications(
			const sdl::V1_0::imtnotify::CRemoveNotificationsGqlRequest& removeNotificationsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtnotify::NotificationScope scope;
	if (!GetCallerScope(gqlRequest, scope, errorMessage)){
		return sdl::V1_0::imtnotify::CNotificationsChangedPayload();
	}

	QByteArrayList removedIds;
	const bool isChanged = m_inboxControllerCompPtr->Remove(scope, GetIds(removeNotificationsRequest.GetRequestedArguments().input), removedIds);

	return CreateChangedPayload(scope, isChanged, removedIds, errorMessage);
}


sdl::V1_0::imtnotify::CNotificationsSentPayload CNotificationsControllerComp::OnSendNotification(
			const sdl::V1_0::imtnotify::CSendNotificationGqlRequest& sendNotificationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationsSentPayload response;

	const sdl::V1_0::imtnotify::SendNotificationRequestArguments arguments = sendNotificationRequest.GetRequestedArguments();
	if (!arguments.input){
		errorMessage = QStringLiteral("The notification is missing");

		return response;
	}

	imtnotify::INotificationSender::Audience audience;
	imtnotify::INotificationSender::Message message;
	ReadSendInput(*arguments.input, audience, message);

	if (!CanSend(gqlRequest, audience.productId, errorMessage)){
		return response;
	}

	if (audience.userIds.size() > s_maxRecipients){
		errorMessage = QStringLiteral("A notification is sent to at most %1 users").arg(s_maxRecipients);

		return response;
	}

	// A notification of a tenant goes to its members only
	if (!audience.tenantId.isEmpty()){
		for (const QByteArray& userId : std::as_const(audience.userIds)){
			if (!m_membershipManagerCompPtr.IsValid() || !m_membershipManagerCompPtr->IsMember(userId, audience.tenantId)){
				errorMessage = QStringLiteral("User '%1' is not a member of the tenant").arg(QString::fromUtf8(userId));

				return response;
			}
		}
	}

	if (!m_senderCompPtr->Send(audience, message)){
		errorMessage = QStringLiteral("The notification could not be sent");

		return response;
	}

	response.success = true;

	return response;
}


sdl::V1_0::imtnotify::CNotificationsSentPayload CNotificationsControllerComp::OnWithdrawNotifications(
			const sdl::V1_0::imtnotify::CWithdrawNotificationsGqlRequest& withdrawNotificationsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationsSentPayload response;

	const sdl::V1_0::imtnotify::WithdrawNotificationsRequestArguments arguments = withdrawNotificationsRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->groupKey || arguments.input->groupKey->isEmpty()){
		errorMessage = QStringLiteral("The group key is missing");

		return response;
	}

	const QByteArray productId = arguments.input->productId ? *arguments.input->productId : QByteArray();
	const QByteArray tenantId = arguments.input->tenantId ? *arguments.input->tenantId : QByteArray();
	if (!CanSend(gqlRequest, productId, errorMessage)){
		return response;
	}
	if (!m_senderCompPtr->Withdraw(productId, tenantId, *arguments.input->groupKey)){
		errorMessage = QStringLiteral("The notifications could not be withdrawn");

		return response;
	}

	response.success = true;

	return response;
}


// private methods

bool CNotificationsControllerComp::CanSend(const ::imtgql::CGqlRequest& gqlRequest, const QByteArray& productId, QString& errorMessage) const
{
	// Fails closed: a server sends only with an access token for sending to its own product, never with a session of a user
	if (!m_senderCompPtr.IsValid()){
		errorMessage = QStringLiteral("Sending notifications is not available");

		return false;
	}

	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr || productId.isEmpty() || contextPtr->GetProductId() != productId){
		errorMessage = QStringLiteral("Notifications are sent only to the product of the sender");

		return false;
	}

	if (!contextPtr->GetScopes().contains(imtnotify::GetSendScope(productId))){
		errorMessage = QStringLiteral("The access token may not send notifications of the product");

		return false;
	}

	return true;
}


bool CNotificationsControllerComp::GetCallerScope(const ::imtgql::CGqlRequest& gqlRequest, imtnotify::NotificationScope& scope, QString& errorMessage) const
{
	if (!m_collectionCompPtr.IsValid() || !m_inboxControllerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Notification inbox is not configured");

		return false;
	}

	scope = GetScope(gqlRequest.GetRequestContext());
	if (!scope.IsValid()){
		errorMessage = QStringLiteral("Notifications are available only to a signed-in user");

		return false;
	}

	return true;
}


sdl::V1_0::imtnotify::CNotificationsChangedPayload CNotificationsControllerComp::CreateChangedPayload(
			const imtnotify::NotificationScope& scope,
			bool isChanged,
			const QByteArrayList& changedIds,
			QString& errorMessage) const
{
	sdl::V1_0::imtnotify::CNotificationsChangedPayload response;

	const int unreadCount = isChanged ? GetUnreadCount(*m_collectionCompPtr, scope) : -1;
	if (unreadCount < 0){
		errorMessage = QStringLiteral("The notifications could not be changed");

		return response;
	}

	imtsdl::TElementList<QByteArray> ids;
	ids.FromList(changedIds);

	response.changedIds = ids;
	response.unreadCount = unreadCount;

	return response;
}


} // namespace imtnotifygql


