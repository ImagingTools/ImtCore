// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

// ImtCore includes
#include <imtnotify/INotification.h>


namespace imtnotifydb
{


/**
	The table of the notifications as the SQL statements of the collection and of the inbox controller see it.
	Times are ISO 8601 UTC text in both databases, so they compare and sort the same way.
*/
namespace NotificationRecord
{


static const inline QString s_tableName = QStringLiteral(R"("UserNotifications")");
static const inline QString s_dedupTableName = QStringLiteral(R"("UserNotificationDedupKeys")");

// What a session sees: its own inbox and the personal notifications sent without a tenant or a product
static const inline QString s_inboxCondition = QStringLiteral(
			R"("ProductId" IN (:ProductId, '') AND "TenantId" IN (:TenantId, '') AND "RecipientId" = :RecipientId)");

// One inbox exactly, e.g. of a group or of a dedup key
static const inline QString s_exactInboxCondition = QStringLiteral(
			R"("ProductId" = :ProductId AND "TenantId" = :TenantId AND "RecipientId" = :RecipientId)");


inline QString TimeToText(const QDateTime& time)
{
	return time.toUTC().toString(Qt::ISODateWithMs);
}


/**
	A null string binds as NULL; the text columns are NOT NULL, so an empty value is bound as an empty string.
*/
inline QVariant Text(const QString& text)
{
	return text.isNull() ? QVariant(QStringLiteral("")) : QVariant(text);
}


inline QVariant Text(const QByteArray& text)
{
	return Text(QString::fromUtf8(text));
}


inline QVariant NullableTime(bool isSet)
{
	return isSet ? QVariant(TimeToText(QDateTime::currentDateTimeUtc())) : QVariant(QMetaType::fromType<QString>());
}


inline void BindScope(const imtnotify::NotificationScope& scope, QVariantMap& bindValues)
{
	bindValues.insert(QStringLiteral(":ProductId"), Text(scope.productId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(scope.tenantId));
	bindValues.insert(QStringLiteral(":RecipientId"), Text(scope.userId));
}


/**
	One placeholder per id.
*/
inline QString BindIds(const QByteArrayList& ids, QVariantMap& bindValues)
{
	QStringList placeholders;
	for (int i = 0; i < ids.size(); ++i){
		const QString placeholder = QStringLiteral(":Id%1").arg(i);
		placeholders.append(placeholder);
		bindValues.insert(placeholder, Text(ids[i]));
	}

	return placeholders.join(QStringLiteral(", "));
}


/**
	The actors as the JSON text of the column "Actors": [{"id", "typeId", "name"}].
*/
inline QString ActorsToJson(const imtnotify::ObjectLinkInfos& actors)
{
	QJsonArray array;
	for (const imtnotify::ObjectLinkInfo& actor : actors){
		QJsonObject object;
		object.insert(QStringLiteral("id"), QString::fromUtf8(actor.id));
		object.insert(QStringLiteral("typeId"), QString::fromUtf8(actor.typeId));
		object.insert(QStringLiteral("name"), actor.name);
		array.append(object);
	}

	return QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact));
}


/**
	The actors of the column; a text that is no list gives none.
*/
inline imtnotify::ObjectLinkInfos ActorsFromJson(const QString& json)
{
	imtnotify::ObjectLinkInfos retVal;
	for (const QJsonValue& value : QJsonDocument::fromJson(json.toUtf8()).array()){
		const QJsonObject object = value.toObject();

		imtnotify::ObjectLinkInfo actor;
		actor.id = object.value(QStringLiteral("id")).toString().toUtf8();
		actor.typeId = object.value(QStringLiteral("typeId")).toString().toUtf8();
		actor.name = object.value(QStringLiteral("name")).toString();
		if (!actor.IsEmpty()){
			retVal.append(actor);
		}
	}

	return retVal;
}


/**
	The arguments as the JSON text of the column "Arguments": [{"id", "kind", "value", "objectId", "objectTypeId"}].
*/
inline QString ArgumentsToJson(const imtnotify::NotificationArguments& arguments)
{
	QJsonArray array;
	for (const imtnotify::NotificationArgument& argument : arguments){
		QJsonObject object;
		object.insert(QStringLiteral("id"), QString::fromUtf8(argument.id));
		object.insert(QStringLiteral("kind"), QString::fromUtf8(imtnotify::NotificationArgument::GetKindName(argument.kind)));
		object.insert(QStringLiteral("value"), argument.value);
		if (argument.kind == imtnotify::NotificationArgument::K_OBJECT){
			object.insert(QStringLiteral("objectId"), QString::fromUtf8(argument.objectId));
			object.insert(QStringLiteral("objectTypeId"), QString::fromUtf8(argument.objectTypeId));
		}

		array.append(object);
	}

	return QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact));
}


/**
	The arguments of the column; an entry of an unknown kind is left out, the client then shows the type without them.
*/
inline imtnotify::NotificationArguments ArgumentsFromJson(const QString& json)
{
	imtnotify::NotificationArguments retVal;
	for (const QJsonValue& value : QJsonDocument::fromJson(json.toUtf8()).array()){
		const QJsonObject object = value.toObject();

		imtnotify::NotificationArgument argument;
		argument.id = object.value(QStringLiteral("id")).toString().toUtf8();
		argument.value = object.value(QStringLiteral("value")).toString();
		argument.objectId = object.value(QStringLiteral("objectId")).toString().toUtf8();
		argument.objectTypeId = object.value(QStringLiteral("objectTypeId")).toString().toUtf8();
		if (!argument.id.isEmpty() && imtnotify::NotificationArgument::FindKind(object.value(QStringLiteral("kind")).toString().toUtf8(), argument.kind)){
			retVal.append(argument);
		}
	}

	return retVal;
}


/**
	The actors of a group: the new one first, then the earlier ones without it, at most s_maxActorCount.
*/
inline imtnotify::ObjectLinkInfos MergeActors(const imtnotify::ObjectLinkInfos& newActors, const imtnotify::ObjectLinkInfos& actors)
{
	imtnotify::ObjectLinkInfos retVal = newActors;
	for (const imtnotify::ObjectLinkInfo& actor : actors){
		if (retVal.size() >= imtnotify::s_maxActorCount){
			break;
		}

		bool isKnown = false;
		for (const imtnotify::ObjectLinkInfo& knownActor : std::as_const(retVal)){
			isKnown = isKnown || (actor.id.isEmpty() ? knownActor.name == actor.name : knownActor.id == actor.id);
		}

		if (!isKnown){
			retVal.append(actor);
		}
	}

	return retVal;
}


/**
	The text the search looks in, lowered here: SQLite lowers only ASCII, so the databases would differ.
	The names of the actors and the texts and objects of the arguments are found too.
*/
inline QString GetSearchText(const imtnotify::INotification& notification)
{
	QStringList parts{
				notification.GetInformationDescription(),
				notification.GetSubject().name};
	for (const imtnotify::ObjectLinkInfo& actor : notification.GetActors()){
		parts.append(actor.name);
	}

	for (const imtnotify::NotificationArgument& argument : notification.GetArguments()){
		if (argument.kind == imtnotify::NotificationArgument::K_TEXT || argument.kind == imtnotify::NotificationArgument::K_OBJECT){
			parts.append(argument.value);
		}
	}

	return parts.join(QLatin1Char('\n')).toLower();
}


/**
	Every column of a row; the state columns keep their time of the change, or are NULL.
*/
inline QVariantMap CreateBindValues(const QByteArray& objectId, const imtnotify::INotification& notification)
{
	QVariantMap bindValues;
	BindScope(notification.GetScope(), bindValues);
	bindValues.insert(QStringLiteral(":Id"), Text(objectId));
	bindValues.insert(QStringLiteral(":TypeId"), Text(notification.GetTypeId()));
	bindValues.insert(QStringLiteral(":Severity"), int(notification.GetInformationCategory()));
	bindValues.insert(QStringLiteral(":Actors"), ActorsToJson(notification.GetActors()));
	bindValues.insert(QStringLiteral(":SubjectId"), Text(notification.GetSubject().id));
	bindValues.insert(QStringLiteral(":SubjectTypeId"), Text(notification.GetSubject().typeId));
	bindValues.insert(QStringLiteral(":SubjectName"), Text(notification.GetSubject().name));
	bindValues.insert(QStringLiteral(":Arguments"), ArgumentsToJson(notification.GetArguments()));
	bindValues.insert(QStringLiteral(":Body"), Text(notification.GetInformationDescription()));
	bindValues.insert(QStringLiteral(":SearchText"), Text(GetSearchText(notification)));
	bindValues.insert(QStringLiteral(":GroupKey"), Text(notification.GetGroupKey()));
	bindValues.insert(QStringLiteral(":GroupCount"), notification.GetGroupCount());
	bindValues.insert(QStringLiteral(":CreatedAt"), TimeToText(notification.GetInformationTimeStamp()));
	bindValues.insert(QStringLiteral(":DeliveredAt"), NullableTime(notification.IsDelivered()));
	bindValues.insert(QStringLiteral(":ReadAt"), NullableTime(notification.IsRead()));

	return bindValues;
}


} // namespace NotificationRecord


} // namespace imtnotifydb


