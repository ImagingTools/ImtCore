// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/CNotificationSenderComp.h>


// Qt includes
#include <QtCore/QSet>

// ImtCore includes
#include <imtnotify/CNotification.h>
#include <imtnotify/NotificationMessageFormat.h>


namespace imtnotify
{


namespace
{


// A user wrote it; the rest is kept by the source of the event
const int s_maxBodyLength = 500;
const int s_maxNameLength = 200;
const int s_maxIdLength = 100;
const int s_maxKeyLength = 200;
const int s_maxArgumentCount = 10;
const int s_maxArgumentIdLength = 50;


bool IsArgumentId(const QByteArray& id)
{
	if (id.isEmpty() || id.size() > s_maxArgumentIdLength){
		return false;
	}

	for (char character : id){
		if (!((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9') || character == '_')){
			return false;
		}
	}

	return true;
}


bool IsArgumentValueValid(const NotificationArgument& argument)
{
	if (argument.kind == NotificationArgument::K_NUMBER){
		bool isNumber = false;
		argument.value.toDouble(&isNumber);

		return isNumber;
	}

	if (argument.kind == NotificationArgument::K_DATE_TIME){
		return QDateTime::fromString(argument.value, Qt::ISODateWithMs).isValid();
	}

	return true;
}


bool AreArgumentsValid(const NotificationArguments& arguments)
{
	if (arguments.size() > s_maxArgumentCount){
		return false;
	}

	QSet<QByteArray> ids;
	for (const NotificationArgument& argument : arguments){
		// The standard values come from the notification itself, an argument does not replace them
		if (		!IsArgumentId(argument.id) ||
					NotificationMessageFormat::s_standardArgumentIds.contains(argument.id) ||
					!IsArgumentValueValid(argument) ||
					ids.contains(argument.id) ||
					argument.objectId.size() > s_maxIdLength ||
					argument.objectTypeId.size() > s_maxIdLength){
			return false;
		}

		ids.insert(argument.id);
	}

	return true;
}


NotificationArguments CutArguments(const NotificationArguments& arguments)
{
	NotificationArguments retVal = arguments;
	for (NotificationArgument& argument : retVal){
		argument.value = argument.value.left(s_maxNameLength);
	}

	return retVal;
}


ObjectLinkInfo CutLink(const ObjectLinkInfo& link)
{
	ObjectLinkInfo retVal = link;
	retVal.name = link.name.left(s_maxNameLength);

	return retVal;
}


} // namespace


// reimplemented (imtnotify::INotificationSender)

bool CNotificationSenderComp::Send(const Audience& audience, const Message& message)
{
	if (		message.typeId.isEmpty() ||
				message.typeId.size() > s_maxIdLength ||
				message.subject.id.size() > s_maxIdLength ||
				message.subject.typeId.size() > s_maxIdLength ||
				message.groupKey.size() > s_maxKeyLength ||
				message.dedupKey.size() > s_maxKeyLength ||
				message.actor.id.size() > s_maxIdLength ||
				message.actor.typeId.size() > s_maxIdLength ||
				!AreArgumentsValid(message.arguments)){
		SendErrorMessage(0, QStringLiteral("A notification needs a type, keys and arguments of a limited length"), "CNotificationSenderComp");

		return false;
	}

	NotificationScope scope;
	scope.productId = audience.productId;
	scope.tenantId = audience.tenantId;

	bool retVal = true;

	QSet<QByteArray> processedUserIds;
	for (const QByteArray& userId : audience.userIds){
		if (userId.isEmpty() || userId == message.actor.id || processedUserIds.contains(userId)){
			continue;
		}

		processedUserIds.insert(userId);

		scope.userId = userId;
		if (!SendToInbox(scope, message)){
			SendErrorMessage(0, QStringLiteral("Notification '%1' could not be stored for user '%2'")
						.arg(QString::fromUtf8(message.typeId), QString::fromUtf8(userId)),
						"CNotificationSenderComp");
			retVal = false;
		}
	}

	return retVal;
}


bool CNotificationSenderComp::Withdraw(const QByteArray& productId, const QByteArray& tenantId, const QByteArray& groupKey)
{
	return !groupKey.isEmpty() && m_inboxControllerCompPtr->RemoveGroup(productId, tenantId, groupKey);
}


// private methods

bool CNotificationSenderComp::SendToInbox(const NotificationScope& scope, const Message& message)
{
	CNotification notification;
	notification.SetScope(scope);
	notification.SetCategory(message.severity);
	notification.SetDescription(message.body.left(s_maxBodyLength));
	if (!message.actor.IsEmpty()){
		notification.SetActors({CutLink(message.actor)});
	}

	notification.SetArguments(CutArguments(message.arguments));
	notification.SetTimeStamp(QDateTime::currentDateTimeUtc());
	notification.SetTypeId(message.typeId);
	notification.SetSubject(CutLink(message.subject));
	notification.SetGroupKey(message.groupKey);

	// A repeated or concurrent message with the same dedup key is stored once
	bool isStored = false;

	return m_inboxControllerCompPtr->Store(notification, message.dedupKey, isStored);
}


} // namespace imtnotify


