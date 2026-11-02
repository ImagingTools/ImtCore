// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>


namespace imtnotify
{


/**
	Link to an object of the system, mirroring the SDL type imtbase::ObjectLink.
	The name is a snapshot taken when the event happened, so a notification stays readable after the object is renamed or removed.
*/
struct ObjectLinkInfo
{
	QByteArray id;
	QByteArray typeId;
	QString name;

	bool IsEmpty() const
	{
		return id.isEmpty() && typeId.isEmpty() && name.isEmpty();
	}

	bool operator==(const ObjectLinkInfo& other) const
	{
		return (id == other.id) && (typeId == other.typeId) && (name == other.name);
	}

	bool operator!=(const ObjectLinkInfo& other) const
	{
		return !operator==(other);
	}
};


typedef QList<ObjectLinkInfo> ObjectLinkInfos;


/**
	A value the message of the notification type puts in, e.g. the other organization of a connection request.
	It is data, never translated: the client shows a text and the name of an object as they are and formats a number
	and a time by its locale.
*/
struct NotificationArgument
{
	enum Kind
	{
		K_TEXT,
		K_NUMBER,
		K_DATE_TIME,
		K_OBJECT
	};

	QByteArray id;
	Kind kind = K_TEXT;

	/**
		The text, the number, the time as ISO 8601 UTC, or the name of the object.
	*/
	QString value;

	/**
		The object, for K_OBJECT.
	*/
	QByteArray objectId;
	QByteArray objectTypeId;

	static NotificationArgument Text(const QByteArray& id, const QString& text)
	{
		NotificationArgument retVal;
		retVal.id = id;
		retVal.kind = K_TEXT;
		retVal.value = text;

		return retVal;
	}

	static NotificationArgument Number(const QByteArray& id, double number)
	{
		NotificationArgument retVal;
		retVal.id = id;
		retVal.kind = K_NUMBER;
		retVal.value = QString::number(number);

		return retVal;
	}

	static NotificationArgument DateTime(const QByteArray& id, const QDateTime& time)
	{
		NotificationArgument retVal;
		retVal.id = id;
		retVal.kind = K_DATE_TIME;
		retVal.value = time.toUTC().toString(Qt::ISODateWithMs);

		return retVal;
	}

	static NotificationArgument Object(const QByteArray& id, const ObjectLinkInfo& object)
	{
		NotificationArgument retVal;
		retVal.id = id;
		retVal.kind = K_OBJECT;
		retVal.value = object.name;
		retVal.objectId = object.id;
		retVal.objectTypeId = object.typeId;

		return retVal;
	}

	static QByteArray GetKindName(Kind kind)
	{
		switch (kind){
		case K_NUMBER:
			return QByteArrayLiteral("Number");
		case K_DATE_TIME:
			return QByteArrayLiteral("DateTime");
		case K_OBJECT:
			return QByteArrayLiteral("Object");
		default:
			return QByteArrayLiteral("Text");
		}
	}

	static bool FindKind(const QByteArray& kindName, Kind& kind)
	{
		for (Kind candidate : {K_TEXT, K_NUMBER, K_DATE_TIME, K_OBJECT}){
			if (GetKindName(candidate) == kindName){
				kind = candidate;

				return true;
			}
		}

		return false;
	}

	bool operator==(const NotificationArgument& other) const
	{
		return		(id == other.id) &&
					(kind == other.kind) &&
					(value == other.value) &&
					(objectId == other.objectId) &&
					(objectTypeId == other.objectTypeId);
	}

	bool operator!=(const NotificationArgument& other) const
	{
		return !operator==(other);
	}
};


typedef QList<NotificationArgument> NotificationArguments;


/**
	A grouped notification keeps the actors of its latest events, the newest first, at most this many.
*/
static const inline int s_maxActorCount = 3;


/**
	Owner of an inbox: a user signed in to a product inside a tenant.
	A notification is visible only in the scope it was sent to. A personal notification is sent without a tenant
	or without a product and is visible in every tenant or product of the user, e.g. an invitation to a tenant.
*/
struct NotificationScope
{
	QByteArray productId;
	QByteArray tenantId;
	QByteArray userId;

	bool IsValid() const
	{
		return !userId.isEmpty();
	}

	bool IsPersonal() const
	{
		return tenantId.isEmpty() || productId.isEmpty();
	}

	/**
		Whether a session of the given scope sees the notifications of this one.
	*/
	bool IsVisibleIn(const NotificationScope& sessionScope) const
	{
		return		!userId.isEmpty() &&
					(userId == sessionScope.userId) &&
					(tenantId.isEmpty() || tenantId == sessionScope.tenantId) &&
					(productId.isEmpty() || productId == sessionScope.productId);
	}

	bool operator==(const NotificationScope& other) const
	{
		return (productId == other.productId) && (tenantId == other.tenantId) && (userId == other.userId);
	}

	bool operator!=(const NotificationScope& other) const
	{
		return !operator==(other);
	}
};


/**
	A change of an inbox, told by the components that change the notifications with the change set CF_NOTIFICATIONS_CHANGED.
	Every change has its own key beginning with s_changeKeyPrefix: the changes of different threads can be merged into one change set.
*/
struct NotificationChange
{
	enum ChangeFlags
	{
		CF_NOTIFICATIONS_CHANGED = 0x6e7100
	};

	enum Kind
	{
		K_CREATED,
		K_UPDATED,
		K_READ,
		K_ALL_READ,
		K_REMOVED
	};

	Kind kind = K_CREATED;
	NotificationScope scope;
	QByteArrayList ids;

	/**
		K_ALL_READ: the notifications created up to this time are read; the sessions are not told their ids.
	*/
	QDateTime createdUntil;

	static const inline QByteArray s_changeKeyPrefix = QByteArrayLiteral("NotificationChange/");
};


/**
	Scope of a personal access token that may send and withdraw the notifications of the product, e.g. "notifications:send:ProLife".
*/
inline QByteArray GetSendScope(const QByteArray& productId)
{
	return QByteArrayLiteral("notifications:send:") + productId;
}


/**
	At most \c maxLength characters of a text, never half of a surrogate pair.
*/
inline QString CutText(const QString& text, int maxLength)
{
	if (text.size() <= maxLength){
		return text;
	}

	return text.left(maxLength > 0 && text.at(maxLength - 1).isHighSurrogate() ? maxLength - 1 : maxLength);
}


/**
	Ids of the selection parameters (iprm::IIdParam) restricting a collection of the notifications to the inbox of a session.
	Without the user the selection is empty.
*/
static const QByteArray s_inboxProductIdParamId = QByteArrayLiteral("InboxProductId");
static const QByteArray s_inboxTenantIdParamId = QByteArrayLiteral("InboxTenantId");
static const QByteArray s_inboxUserIdParamId = QByteArrayLiteral("InboxUserId");


} // namespace imtnotify


Q_DECLARE_METATYPE(imtnotify::NotificationChange);


