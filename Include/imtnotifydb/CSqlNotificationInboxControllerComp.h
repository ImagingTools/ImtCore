// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlQuery>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtnotify/INotificationInboxController.h>


namespace imtnotifydb
{


/**
	The changes of the notifications in the tables "UserNotifications" and "UserNotificationDedupKeys" of PostgreSQL or
	SQLite (created by CNotificationDbDelegateComp). Every change is one statement, so concurrent senders cannot lose
	an event; the changed rows come back with RETURNING and are told as NotificationChange.
	It has to be wrapped as a model (icomp::TModelCompWrap) for the publisher of the changes.
*/
class CSqlNotificationInboxControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtnotify::INotificationInboxController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSqlNotificationInboxControllerComp);
		I_REGISTER_INTERFACE(imtnotify::INotificationInboxController);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine of the notification tables", true, "DatabaseEngine");
	I_END_COMPONENT;

	// reimplemented (imtnotify::INotificationInboxController)
	virtual bool Add(const imtnotify::INotification& notification, QByteArray& notificationId) override;
	virtual bool AddToGroup(const imtnotify::INotification& notification, QByteArray& notificationId) override;
	virtual bool AddDedupKey(const imtnotify::NotificationScope& scope, const QByteArray& dedupKey, bool& isNew) override;
	virtual bool RemoveDedupKey(const imtnotify::NotificationScope& scope, const QByteArray& dedupKey) override;
	virtual bool MarkRead(const imtnotify::NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& changedIds) override;
	virtual bool MarkAllRead(const imtnotify::NotificationScope& scope, const QDateTime& createdUntil, QByteArrayList& changedIds) override;
	virtual bool MarkDelivered(const imtnotify::NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& changedIds) override;
	virtual bool MarkAllDelivered(const imtnotify::NotificationScope& scope, const QDateTime& createdUntil, QByteArrayList& changedIds) override;
	virtual bool Remove(const imtnotify::NotificationScope& scope, const QByteArrayList& ids, QByteArrayList& removedIds) override;
	virtual bool RemoveGroup(const QByteArray& productId, const QByteArray& tenantId, const QByteArray& groupKey) override;
	virtual bool RemoveUserNotifications(const QByteArray& tenantId, const QByteArray& userId) override;
	virtual bool RemoveExpired(const QDateTime& createdBefore, int maxCount, bool& isComplete) override;

private:
	bool ExecQuery(const QString& query, const QVariantMap& bindValues, QSqlQuery& sqlQuery) const;
	bool ReadIds(QSqlQuery& sqlQuery, QByteArrayList& ids) const;

	/**
		Update the notifications of an inbox matching a condition, by their ids or all of them (ids == nullptr).
	*/
	bool UpdateInbox(
				const imtnotify::NotificationScope& scope,
				const QByteArrayList* idsPtr,
				const QString& setClause,
				const QString& condition,
				const QVariantMap& conditionValues,
				imtnotify::NotificationChange::Kind kind,
				QByteArrayList& changedIds,
				bool isPublished,
				const QDateTime& createdUntil = QDateTime());

	/**
		Read the rows of a statement returning "Id", "ProductId", "TenantId", "RecipientId" and tell one change per inbox.
	*/
	bool ReadChangedRows(
				QSqlQuery& sqlQuery,
				imtnotify::NotificationChange::Kind kind,
				QByteArrayList& changedIds,
				const QDateTime& createdUntil = QDateTime());
	void PublishChange(
				imtnotify::NotificationChange::Kind kind,
				const imtnotify::NotificationScope& scope,
				const QByteArrayList& ids,
				const QDateTime& createdUntil = QDateTime());

private:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtnotifydb


