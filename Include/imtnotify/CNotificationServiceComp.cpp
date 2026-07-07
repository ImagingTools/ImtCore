// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/CNotificationServiceComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <algorithm>


namespace imtnotify
{


// reimplemented (imtnotify::INotificationService)

QByteArray CNotificationServiceComp::RaiseNotification(const NotificationParams& params)
{
	imtbase::IObjectCollection* collectionPtr = m_notificationCollectionCompPtr.GetPtr();
	if (collectionPtr == nullptr){
		return QByteArray();
	}

	if (params.recipientId.isEmpty() || params.title.isEmpty()){
		return QByteArray();
	}

	istd::IChangeableUniquePtr notificationPtr(m_notificationFactCompPtr.CreateInstance());
	if (!notificationPtr.IsValid()){
		return QByteArray();
	}

	INotification* notification = dynamic_cast<INotification*>(notificationPtr.GetPtr());
	if (notification == nullptr){
		return QByteArray();
	}

	const QByteArray newId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	const QString nowIso = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	notification->SetId(newId);
	notification->SetRecipientId(params.recipientId);
	notification->SetCategory(params.category);
	notification->SetTitle(params.title);
	notification->SetBody(params.body);
	notification->SetIconName(params.iconName);
	notification->SetSeverity(params.severity);
	notification->SetRead(false);
	notification->SetSourceType(params.sourceType);
	notification->SetSourceId(params.sourceId);
	notification->SetTargetRoute(params.targetRoute);
	notification->SetMetadata(params.metadata);
	notification->SetCreatedAt(nowIso);
	notification->SetUpdatedAt(nowIso);

	const QByteArray typeId = m_objectTypeIdAttrPtr.IsValid() ? *m_objectTypeIdAttrPtr : QByteArray("Notification");

	const QByteArray objectId = collectionPtr->InsertNewObject(
				typeId,
				params.title,
				QString(),
				notificationPtr.GetPtr(),
				newId);

	return objectId;
}


QByteArrayList CNotificationServiceComp::RaiseNotificationForRecipients(
			const QByteArrayList& recipientIds,
			const NotificationParams& params)
{
	QByteArrayList result;
	result.reserve(recipientIds.size());

	NotificationParams perRecipient = params;
	for (const QByteArray& recipientId: recipientIds){
		if (recipientId.isEmpty()){
			continue;
		}
		perRecipient.recipientId = recipientId;
		const QByteArray id = RaiseNotification(perRecipient);
		if (!id.isEmpty()){
			result.append(id);
		}
	}

	return result;
}


QByteArrayList CNotificationServiceComp::GetNotifications(const ListFilter& filter) const
{
	const QByteArrayList sortedIds = GetFilteredSortedIds(filter);

	int offset = filter.offset;
	if (offset < 0){
		offset = 0;
	}
	if (offset >= sortedIds.size()){
		return QByteArrayList();
	}

	const int available = sortedIds.size() - offset;
	const int count = (filter.limit < 0) ? available : std::min(filter.limit, available);

	return sortedIds.mid(offset, count);
}


int CNotificationServiceComp::GetTotalCount(const ListFilter& filter) const
{
	return GetFilteredSortedIds(filter).size();
}


int CNotificationServiceComp::GetUnreadCount(const QByteArray& recipientId) const
{
	ListFilter filter;
	filter.recipientId = recipientId;
	filter.unreadOnly = true;
	return GetFilteredSortedIds(filter).size();
}


bool CNotificationServiceComp::SetNotificationRead(const QByteArray& notificationId, bool isRead)
{
	imtbase::IObjectCollection* collectionPtr = m_notificationCollectionCompPtr.GetPtr();
	if (collectionPtr == nullptr){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!collectionPtr->GetObjectData(notificationId, dataPtr)){
		return false;
	}

	INotification* notification = dynamic_cast<INotification*>(dataPtr.GetPtr());
	if (notification == nullptr){
		return false;
	}

	if (notification->IsRead() == isRead){
		return true;
	}

	notification->SetRead(isRead);
	notification->SetUpdatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

	return collectionPtr->SetObjectData(notificationId, *notification);
}


int CNotificationServiceComp::MarkAllRead(const QByteArray& recipientId)
{
	ListFilter filter;
	filter.recipientId = recipientId;
	filter.unreadOnly = true;

	int updatedCount = 0;
	const QByteArrayList unreadIds = GetFilteredSortedIds(filter);
	for (const QByteArray& id: unreadIds){
		if (SetNotificationRead(id, true)){
			++updatedCount;
		}
	}

	return updatedCount;
}


bool CNotificationServiceComp::DeleteNotification(const QByteArray& notificationId)
{
	imtbase::IObjectCollection* collectionPtr = m_notificationCollectionCompPtr.GetPtr();
	if (collectionPtr == nullptr){
		return false;
	}

	imtbase::ICollectionInfo::Ids ids;
	ids << notificationId;
	return collectionPtr->RemoveElements(ids);
}


// private methods

QByteArrayList CNotificationServiceComp::GetFilteredSortedIds(const ListFilter& filter) const
{
	const imtbase::IObjectCollection* collectionPtr = m_notificationCollectionCompPtr.GetPtr();
	if (collectionPtr == nullptr){
		return QByteArrayList();
	}

	const imtbase::ICollectionInfo::Ids ids = collectionPtr->GetElementIds();

	// Collect (createdAt, id) so we can order by creation time regardless of the
	// underlying collection ordering.
	QList<QPair<QString, QByteArray> > filtered;
	filtered.reserve(ids.size());

	for (const QByteArray& id: ids){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!collectionPtr->GetObjectData(id, dataPtr)){
			continue;
		}

		const INotification* notification = dynamic_cast<const INotification*>(dataPtr.GetPtr());
		if (notification == nullptr){
			continue;
		}

		if (!filter.recipientId.isEmpty() && notification->GetRecipientId() != filter.recipientId){
			continue;
		}
		if (filter.unreadOnly && notification->IsRead()){
			continue;
		}
		if (!filter.category.isEmpty() && notification->GetCategory() != filter.category){
			continue;
		}
		if (filter.severity >= 0 && static_cast<int>(notification->GetSeverity()) != filter.severity){
			continue;
		}

		filtered.append(qMakePair(notification->GetCreatedAt(), id));
	}

	const bool newestFirst = (filter.sortOrder == SO_NEWEST_FIRST);
	std::sort(filtered.begin(), filtered.end(),
				[newestFirst](const QPair<QString, QByteArray>& lhs, const QPair<QString, QByteArray>& rhs){
					// ISO 8601 timestamps sort lexicographically.
					if (newestFirst){
						return lhs.first > rhs.first;
					}
					return lhs.first < rhs.first;
				});

	QByteArrayList result;
	result.reserve(filtered.size());
	for (const auto& entry: filtered){
		result.append(entry.second);
	}

	return result;
}


} // namespace imtnotify
