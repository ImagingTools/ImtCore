// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifydb/CSqlNotificationInboxControllerComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlError>

// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imtnotify/CNotification.h>
#include <imtnotifydb/NotificationRecord.h>


namespace imtnotifydb
{


namespace
{


using namespace NotificationRecord;


// An id list becomes one placeholder per id
const int s_maxIdsPerRequest = 1000;

// Concurrent events of one group make an update fail; it is tried again with the actors read again
const int s_maxGroupAttempts = 5;

const QString s_changedRowColumns = QStringLiteral(R"("Id", "ProductId", "TenantId", "RecipientId")");


} // namespace


// reimplemented (imtnotify::INotificationInboxController)

bool CSqlNotificationInboxControllerComp::Store(const imtnotify::INotification& notification, const QByteArray& dedupKey, bool& isStored)
{
	isStored = false;

	if (!notification.GetScope().IsValid()){
		return false;
	}

	// The key and the notification are stored together: a stop between them neither loses the event nor stores it twice
	if (!m_databaseEngineCompPtr->BeginTransaction()){
		SendErrorMessage(0, QStringLiteral("A notification transaction could not be started"), "CSqlNotificationInboxControllerComp");

		return false;
	}

	bool isNew = true;
	QByteArray notificationId;
	imtnotify::NotificationChange::Kind kind = imtnotify::NotificationChange::K_CREATED;

	bool retVal = dedupKey.isEmpty() || ClaimDedupKey(notification.GetScope(), dedupKey, isNew);
	if (retVal && isNew){
		retVal = StoreEvent(notification, notificationId, kind);
	}

	if (!retVal || !m_databaseEngineCompPtr->FinishTransaction()){
		m_databaseEngineCompPtr->CancelTransaction();

		return false;
	}

	// The sessions learn only what was committed
	if (!notificationId.isEmpty()){
		isStored = true;
		PublishChange(kind, notification.GetScope(), {notificationId});
	}

	return true;
}


bool CSqlNotificationInboxControllerComp::MarkRead(const imtnotify::NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& changedIds)
{
	return UpdateInbox(
				scope,
				&ids,
				QStringLiteral(R"("ReadAt" = :Now, "DeliveredAt" = COALESCE("DeliveredAt", :Now))"),
				QStringLiteral(R"("ReadAt" IS NULL)"),
				QVariantMap(),
				imtnotify::NotificationChange::K_READ,
				changedIds,
				true);
}


bool CSqlNotificationInboxControllerComp::MarkAllRead(const imtnotify::NotificationScope& scope, const QDateTime& createdUntil, QByteArrayList& changedIds)
{
	// The sessions learn the bound, not the ids, which can be the whole inbox
	const QDateTime readUntil = createdUntil.isValid() ? createdUntil : QDateTime::currentDateTimeUtc();

	QVariantMap conditionValues;
	conditionValues.insert(QStringLiteral(":CreatedUntil"), TimeToText(readUntil));

	return UpdateInbox(
				scope,
				nullptr,
				QStringLiteral(R"("ReadAt" = :Now, "DeliveredAt" = COALESCE("DeliveredAt", :Now))"),
				QStringLiteral(R"("ReadAt" IS NULL AND "CreatedAt" <= :CreatedUntil)"),
				conditionValues,
				imtnotify::NotificationChange::K_ALL_READ,
				changedIds,
				true,
				readUntil);
}


bool CSqlNotificationInboxControllerComp::MarkDelivered(const imtnotify::NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& changedIds)
{
	// Delivered is a state of the server only; the sessions do not show it
	return UpdateInbox(
				scope,
				&ids,
				QStringLiteral(R"("DeliveredAt" = :Now)"),
				QStringLiteral(R"("DeliveredAt" IS NULL)"),
				QVariantMap(),
				imtnotify::NotificationChange::K_UPDATED,
				changedIds,
				false);
}


bool CSqlNotificationInboxControllerComp::MarkAllDelivered(const imtnotify::NotificationScope& scope, const QDateTime& createdUntil, QByteArrayList& changedIds)
{
	QString condition = QStringLiteral(R"("DeliveredAt" IS NULL)");
	QVariantMap conditionValues;
	if (createdUntil.isValid()){
		condition += QStringLiteral(R"( AND "CreatedAt" <= :CreatedUntil)");
		conditionValues.insert(QStringLiteral(":CreatedUntil"), TimeToText(createdUntil));
	}

	return UpdateInbox(
				scope,
				nullptr,
				QStringLiteral(R"("DeliveredAt" = :Now)"),
				condition,
				conditionValues,
				imtnotify::NotificationChange::K_UPDATED,
				changedIds,
				false);
}


bool CSqlNotificationInboxControllerComp::Remove(const imtnotify::NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& removedIds)
{
	if (!scope.IsValid() || ids.size() > s_maxIdsPerRequest){
		return false;
	}

	if (ids.isEmpty()){
		return true;
	}

	QVariantMap bindValues;
	BindScope(scope, bindValues);
	const QString idList = BindIds(ids, bindValues);

	QSqlQuery sqlQuery;

	return ExecQuery(
				QStringLiteral(R"(DELETE FROM %1 WHERE %2 AND "Id" IN (%3) RETURNING %4)").arg(s_tableName, s_inboxCondition, idList, s_changedRowColumns),
				bindValues,
				sqlQuery) && ReadChangedRows(sqlQuery, imtnotify::NotificationChange::K_REMOVED, removedIds);
}


bool CSqlNotificationInboxControllerComp::RemoveGroup(const QByteArray& productId, const QByteArray& tenantId, const QByteArray& groupKey, const QByteArrayList& userIds)
{
	if (groupKey.isEmpty() || userIds.size() > s_maxIdsPerRequest){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":ProductId"), Text(productId));
	bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));
	bindValues.insert(QStringLiteral(":GroupKey"), Text(groupKey));

	// Only the inboxes of these users
	QString recipientCondition;
	if (!userIds.isEmpty()){
		recipientCondition = QStringLiteral(R"( AND "RecipientId" IN (%1))").arg(BindIds(userIds, bindValues));
	}

	// The withdrawn event can happen again, e.g. a removed assignee assigned again
	return RemoveWithKeys(
				QStringLiteral(R"("ProductId" = :ProductId AND "TenantId" = :TenantId AND "DedupKey" = :GroupKey)") + recipientCondition,
				QStringLiteral(R"("ProductId" = :ProductId AND "TenantId" = :TenantId AND "GroupKey" = :GroupKey AND "GroupKey" <> '')") + recipientCondition,
				bindValues);
}


bool CSqlNotificationInboxControllerComp::RemoveUserNotifications(const QByteArray& tenantId, const QByteArray& userId)
{
	if (userId.isEmpty()){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":RecipientId"), Text(userId));

	QString condition = QStringLiteral(R"("RecipientId" = :RecipientId)");
	if (!tenantId.isEmpty()){
		condition += QStringLiteral(R"( AND "TenantId" = :TenantId)");
		bindValues.insert(QStringLiteral(":TenantId"), Text(tenantId));
	}

	return RemoveWithKeys(condition, condition, bindValues);
}


bool CSqlNotificationInboxControllerComp::RemoveExpired(const QDateTime& createdBefore, int maxCount, bool& isComplete)
{
	isComplete = false;

	if (!createdBefore.isValid() || maxCount <= 0){
		return false;
	}

	QVariantMap bindValues;
	bindValues.insert(QStringLiteral(":CreatedBefore"), TimeToText(createdBefore));

	// A portion is removed whole or not at all, and the sessions learn it after the commit
	if (!m_databaseEngineCompPtr->BeginTransaction()){
		SendErrorMessage(0, QStringLiteral("A notification transaction could not be started"), "CSqlNotificationInboxControllerComp");

		return false;
	}

	QSqlQuery dedupQuery;
	QSqlQuery sqlQuery;
	QByteArrayList removedIds;
	QList<InboxChange> changes;
	bool retVal = ExecQuery(
				QStringLiteral(
							R"(DELETE FROM %1 WHERE ("ProductId", "TenantId", "RecipientId", "DedupKey") IN )"
							R"((SELECT "ProductId", "TenantId", "RecipientId", "DedupKey" FROM %1 WHERE "CreatedAt" < :CreatedBefore LIMIT %2))")
							.arg(s_dedupTableName).arg(maxCount),
				bindValues,
				dedupQuery);

	const int removedKeyCount = retVal ? dedupQuery.numRowsAffected() : 0;

	retVal = retVal && ExecQuery(
				QStringLiteral(
							R"(DELETE FROM %1 WHERE "Id" IN (SELECT "Id" FROM %1 WHERE "CreatedAt" < :CreatedBefore LIMIT %2) RETURNING %3)")
							.arg(s_tableName).arg(maxCount).arg(s_changedRowColumns),
				bindValues,
				sqlQuery) && ReadInboxChanges(sqlQuery, removedIds, changes);

	if (!retVal || !m_databaseEngineCompPtr->FinishTransaction()){
		m_databaseEngineCompPtr->CancelTransaction();

		return false;
	}

	PublishChanges(imtnotify::NotificationChange::K_REMOVED, changes);

	isComplete = removedIds.size() < maxCount && removedKeyCount < maxCount;

	return true;
}


// private methods

bool CSqlNotificationInboxControllerComp::StoreEvent(
			const imtnotify::INotification& notification,
			QByteArray& notificationId,
			imtnotify::NotificationChange::Kind& kind)
{
	const bool isGrouped = !notification.GetGroupKey().isEmpty();
	if (isGrouped){
		if (!UpdateGroup(notification, notificationId)){
			return false;
		}

		if (!notificationId.isEmpty()){
			kind = imtnotify::NotificationChange::K_UPDATED;

			return true;
		}
	}

	if (!InsertNotification(notification, notificationId)){
		return false;
	}

	if (!notificationId.isEmpty()){
		kind = imtnotify::NotificationChange::K_CREATED;

		return true;
	}

	// A concurrent event opened the group first
	if (!isGrouped || !UpdateGroup(notification, notificationId)){
		return false;
	}

	kind = imtnotify::NotificationChange::K_UPDATED;

	return !notificationId.isEmpty();
}


bool CSqlNotificationInboxControllerComp::InsertNotification(const imtnotify::INotification& notification, QByteArray& notificationId)
{
	notificationId.clear();

	const QByteArray id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	// A second open notification of the same group is not added: the unique index "UIX_UserNotifications_OpenGroup"
	// makes the insert do nothing, without an error that would end the transaction
	QSqlQuery sqlQuery;
	if (!ExecQuery(
				QStringLiteral(
							R"(INSERT INTO %1 ("Id", "ProductId", "TenantId", "RecipientId", "TypeId", "Severity", "Actors", )"
							R"("SubjectId", "SubjectTypeId", "SubjectName", "Arguments", "Body", "SearchText", "GroupKey", "GroupCount", )"
							R"("CreatedAt", "DeliveredAt", "ReadAt") )"
							R"(VALUES (:Id, :ProductId, :TenantId, :RecipientId, :TypeId, :Severity, :Actors, )"
							R"(:SubjectId, :SubjectTypeId, :SubjectName, :Arguments, :Body, :SearchText, :GroupKey, :GroupCount, )"
							R"(:CreatedAt, :DeliveredAt, :ReadAt) ON CONFLICT DO NOTHING RETURNING "Id")").arg(s_tableName),
				CreateBindValues(id, notification),
				sqlQuery)){
		return false;
	}

	QByteArrayList ids;
	if (!ReadIds(sqlQuery, ids)){
		return false;
	}

	if (!ids.isEmpty()){
		notificationId = ids.first();
	}

	return true;
}


bool CSqlNotificationInboxControllerComp::UpdateGroup(const imtnotify::INotification& notification, QByteArray& notificationId)
{
	notificationId.clear();

	QVariantMap groupValues;
	BindScope(notification.GetScope(), groupValues);
	groupValues.insert(QStringLiteral(":GroupKey"), Text(notification.GetGroupKey()));

	// The actors of the group are merged here: the update changes the row only if they are still the ones read,
	// otherwise a concurrent event changed the group and they are read again
	for (int attempt = 0; attempt < s_maxGroupAttempts; ++attempt){
		QSqlQuery groupQuery;
		if (!ExecQuery(
					QStringLiteral(R"(SELECT "Id", "Actors" FROM %1 WHERE %2 AND "GroupKey" = :GroupKey AND "ReadAt" IS NULL)")
								.arg(s_tableName, s_exactInboxCondition),
					groupValues,
					groupQuery)){
			return false;
		}

		if (!groupQuery.next()){
			// No open group
			return groupQuery.lastError().type() == QSqlError::NoError;
		}

		const QByteArray groupId = imtdb::VariantToByteArray(groupQuery.value(0));
		const QString previousActors = groupQuery.value(1).toString();

		imtnotify::CNotification groupNotification;
		if (!groupNotification.CopyFrom(notification)){
			return false;
		}

		groupNotification.SetActors(MergeActors(notification.GetActors(), ActorsFromJson(previousActors)));

		QVariantMap bindValues = CreateBindValues(groupId, groupNotification);
		bindValues.remove(QStringLiteral(":ProductId"));
		bindValues.remove(QStringLiteral(":TenantId"));
		bindValues.remove(QStringLiteral(":RecipientId"));
		bindValues.remove(QStringLiteral(":GroupKey"));
		bindValues.remove(QStringLiteral(":GroupCount"));
		bindValues.remove(QStringLiteral(":DeliveredAt"));
		bindValues.remove(QStringLiteral(":ReadAt"));
		bindValues.insert(QStringLiteral(":PreviousActors"), previousActors);

		// The group shows its latest event and is new again for the sessions; an earlier event that comes late only counts
		QStringList latestEventValues;
		for (const QString& column : {
					QStringLiteral("TypeId"),
					QStringLiteral("Severity"),
					QStringLiteral("SubjectId"),
					QStringLiteral("SubjectTypeId"),
					QStringLiteral("SubjectName"),
					QStringLiteral("Arguments"),
					QStringLiteral("Body"),
					QStringLiteral("SearchText"),
					QStringLiteral("CreatedAt")}){
			// Every placeholder once: a repeated named placeholder is not bound the same way by every driver
			const QString eventTimePlaceholder = QStringLiteral(":EventTimeFor%1").arg(column);
			bindValues.insert(eventTimePlaceholder, bindValues.value(QStringLiteral(":CreatedAt")));
			latestEventValues.append(QStringLiteral(R"("%1" = CASE WHEN "CreatedAt" <= %2 THEN :%1 ELSE "%1" END)").arg(column, eventTimePlaceholder));
		}

		QSqlQuery sqlQuery;
		if (!ExecQuery(
					QStringLiteral(
								R"(UPDATE %1 SET %2, "Actors" = :Actors, "GroupCount" = "GroupCount" + 1, "DeliveredAt" = NULL )"
								R"(WHERE "Id" = :Id AND "ReadAt" IS NULL AND "Actors" = :PreviousActors RETURNING "Id")")
								.arg(s_tableName, latestEventValues.join(QStringLiteral(", "))),
					bindValues,
					sqlQuery)){
			return false;
		}

		QByteArrayList ids;
		if (!ReadIds(sqlQuery, ids)){
			return false;
		}

		if (!ids.isEmpty()){
			notificationId = ids.first();

			return true;
		}
	}

	SendErrorMessage(0, QStringLiteral("Group '%1' changed too often to add an event").arg(QString::fromUtf8(notification.GetGroupKey())), "CSqlNotificationInboxControllerComp");

	return false;
}


bool CSqlNotificationInboxControllerComp::ClaimDedupKey(const imtnotify::NotificationScope& scope, const QByteArray& dedupKey, bool& isNew)
{
	isNew = false;

	if (!scope.IsValid() || dedupKey.isEmpty()){
		return false;
	}

	QVariantMap bindValues;
	BindScope(scope, bindValues);
	bindValues.insert(QStringLiteral(":DedupKey"), Text(dedupKey));
	bindValues.insert(QStringLiteral(":CreatedAt"), TimeToText(QDateTime::currentDateTimeUtc()));

	// The primary key makes it one statement for concurrent senders: only one of them gets the row back
	QSqlQuery sqlQuery;
	if (!ExecQuery(
				QStringLiteral(
							R"(INSERT INTO %1 ("ProductId", "TenantId", "RecipientId", "DedupKey", "CreatedAt") )"
							R"(VALUES (:ProductId, :TenantId, :RecipientId, :DedupKey, :CreatedAt) )"
							R"(ON CONFLICT DO NOTHING RETURNING "DedupKey")").arg(s_dedupTableName),
				bindValues,
				sqlQuery)){
		return false;
	}

	QByteArrayList keys;
	if (!ReadIds(sqlQuery, keys)){
		return false;
	}

	isNew = !keys.isEmpty();

	return true;
}


bool CSqlNotificationInboxControllerComp::ExecQuery(const QString& query, const QVariantMap& bindValues, QSqlQuery& sqlQuery) const
{
	QSqlError sqlError;
	sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), bindValues, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("A notification statement failed: %1").arg(sqlError.text()), "CSqlNotificationInboxControllerComp");

		return false;
	}

	return true;
}


bool CSqlNotificationInboxControllerComp::ReadIds(QSqlQuery& sqlQuery, QByteArrayList& ids) const
{
	while (sqlQuery.next()){
		ids.append(imtdb::VariantToByteArray(sqlQuery.value(0)));
	}

	return sqlQuery.lastError().type() == QSqlError::NoError;
}


bool CSqlNotificationInboxControllerComp::UpdateInbox(
			const imtnotify::NotificationScope& scope,
			const QByteArrayList* idsPtr,
			const QString& setClause,
			const QString& condition,
			const QVariantMap& conditionValues,
			imtnotify::NotificationChange::Kind kind,
			QByteArrayList& changedIds,
			bool isPublished,
			const QDateTime& createdUntil)
{
	if (!scope.IsValid() || (idsPtr != nullptr && idsPtr->size() > s_maxIdsPerRequest)){
		return false;
	}

	if (idsPtr != nullptr && idsPtr->isEmpty()){
		return true;
	}

	QVariantMap bindValues = conditionValues;
	BindScope(scope, bindValues);
	bindValues.insert(QStringLiteral(":Now"), TimeToText(QDateTime::currentDateTimeUtc()));

	QString whereClause = s_inboxCondition + QStringLiteral(" AND ") + condition;
	if (idsPtr != nullptr){
		whereClause += QStringLiteral(R"( AND "Id" IN (%1))").arg(BindIds(*idsPtr, bindValues));
	}

	QSqlQuery sqlQuery;
	if (!ExecQuery(
				QStringLiteral("UPDATE %1 SET %2 WHERE %3 RETURNING %4").arg(s_tableName, setClause, whereClause, s_changedRowColumns),
				bindValues,
				sqlQuery)){
		return false;
	}

	if (!isPublished){
		return ReadIds(sqlQuery, changedIds);
	}

	return ReadChangedRows(sqlQuery, kind, changedIds, createdUntil);
}


bool CSqlNotificationInboxControllerComp::ReadChangedRows(
			QSqlQuery& sqlQuery,
			imtnotify::NotificationChange::Kind kind,
			QByteArrayList& changedIds,
			const QDateTime& createdUntil)
{
	QList<InboxChange> changes;
	if (!ReadInboxChanges(sqlQuery, changedIds, changes)){
		return false;
	}

	PublishChanges(kind, changes, createdUntil);

	return true;
}


bool CSqlNotificationInboxControllerComp::ReadInboxChanges(QSqlQuery& sqlQuery, QByteArrayList& changedIds, QList<InboxChange>& changes) const
{
	// A personal notification belongs to its own inbox: its change reaches the sessions of every tenant and product of the user
	QHash<QByteArray, int> changeIndexes;
	while (sqlQuery.next()){
		InboxChange change;
		change.scope.productId = imtdb::VariantToByteArray(sqlQuery.value(1));
		change.scope.tenantId = imtdb::VariantToByteArray(sqlQuery.value(2));
		change.scope.userId = imtdb::VariantToByteArray(sqlQuery.value(3));

		const QByteArray id = imtdb::VariantToByteArray(sqlQuery.value(0));
		changedIds.append(id);

		const QByteArray scopeKey = change.scope.productId + '\x1f' + change.scope.tenantId + '\x1f' + change.scope.userId;
		const auto indexIter = changeIndexes.constFind(scopeKey);
		if (indexIter == changeIndexes.constEnd()){
			changeIndexes.insert(scopeKey, changes.size());
			change.ids.append(id);
			changes.append(change);
		}
		else{
			changes[*indexIter].ids.append(id);
		}
	}

	return sqlQuery.lastError().type() == QSqlError::NoError;
}


void CSqlNotificationInboxControllerComp::PublishChanges(
			imtnotify::NotificationChange::Kind kind,
			const QList<InboxChange>& changes,
			const QDateTime& createdUntil)
{
	for (const InboxChange& change : changes){
		PublishChange(kind, change.scope, change.ids, createdUntil);
	}
}


bool CSqlNotificationInboxControllerComp::RemoveWithKeys(const QString& keyCondition, const QString& condition, const QVariantMap& bindValues)
{
	// Without its notification a kept key would refuse the event, without its key a kept notification would be told again
	if (!m_databaseEngineCompPtr->BeginTransaction()){
		SendErrorMessage(0, QStringLiteral("A notification transaction could not be started"), "CSqlNotificationInboxControllerComp");

		return false;
	}

	QSqlQuery dedupQuery;
	QSqlQuery sqlQuery;
	QByteArrayList removedIds;
	QList<InboxChange> changes;
	const bool retVal =
				ExecQuery(QStringLiteral("DELETE FROM %1 WHERE %2").arg(s_dedupTableName, keyCondition), bindValues, dedupQuery) &&
				ExecQuery(QStringLiteral("DELETE FROM %1 WHERE %2 RETURNING %3").arg(s_tableName, condition, s_changedRowColumns), bindValues, sqlQuery) &&
				ReadInboxChanges(sqlQuery, removedIds, changes);

	if (!retVal || !m_databaseEngineCompPtr->FinishTransaction()){
		m_databaseEngineCompPtr->CancelTransaction();

		return false;
	}

	PublishChanges(imtnotify::NotificationChange::K_REMOVED, changes);

	return true;
}


void CSqlNotificationInboxControllerComp::PublishChange(
			imtnotify::NotificationChange::Kind kind,
			const imtnotify::NotificationScope& scope,
			const QByteArrayList& ids,
			const QDateTime& createdUntil)
{
	if (ids.isEmpty()){
		return;
	}

	imtnotify::NotificationChange change;
	change.kind = kind;
	change.scope = scope;
	change.createdUntil = createdUntil;

	// A bound reaches only the sessions of this inbox; a personal inbox is seen in sessions that hold notifications of
	// other inboxes too, so they learn the ids
	if (kind == imtnotify::NotificationChange::K_ALL_READ && scope.IsPersonal()){
		change.kind = imtnotify::NotificationChange::K_READ;
		change.createdUntil = QDateTime();
	}

	if (change.kind != imtnotify::NotificationChange::K_ALL_READ){
		change.ids = ids;
	}

	// Changes of different threads can be merged into one change set: every change has its own key
	istd::IChangeable::ChangeSet changeSet(imtnotify::NotificationChange::CF_NOTIFICATIONS_CHANGED);
	changeSet.SetChangeInfo(imtnotify::NotificationChange::s_changeKeyPrefix + QUuid::createUuid().toByteArray(QUuid::WithoutBraces), QVariant::fromValue(change));
	istd::CChangeNotifier changeNotifier(this, &changeSet);
}


} // namespace imtnotifydb


