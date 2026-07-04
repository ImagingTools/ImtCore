// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CNotificationTest.h"


void CNotificationTest::fill(imtnotify::CNotificationComp& notification)
{
	notification.SetId("notif-1");
	notification.SetRecipientId("user-42");
	notification.SetCategory("Ticket");
	notification.SetTitle("New message");
	notification.SetBody("You have a new message on ticket #17");
	notification.SetIconName("Icons/Message");
	notification.SetSeverity(imtnotify::INotification::SV_WARNING);
	notification.SetRead(false);
	notification.SetSourceType("SupportTicket");
	notification.SetSourceId("ticket-17");
	notification.SetTargetRoute("Tickets/Ticket/ticket-17");
	notification.SetMetadata("{\"number\":17}");
	notification.SetCreatedAt("2026-07-04T10:00:00Z");
	notification.SetUpdatedAt("2026-07-04T10:00:00Z");
}


void CNotificationTest::testDefaults()
{
	imtnotify::CNotificationComp notification;

	QVERIFY(notification.GetId().isEmpty());
	QVERIFY(notification.GetRecipientId().isEmpty());
	QCOMPARE(notification.GetSeverity(), imtnotify::INotification::SV_INFO);
	QVERIFY(!notification.IsRead());
}


void CNotificationTest::testSettersGetters()
{
	imtnotify::CNotificationComp notification;
	fill(notification);

	QCOMPARE(notification.GetId(), QByteArray("notif-1"));
	QCOMPARE(notification.GetRecipientId(), QByteArray("user-42"));
	QCOMPARE(notification.GetCategory(), QString("Ticket"));
	QCOMPARE(notification.GetTitle(), QString("New message"));
	QCOMPARE(notification.GetBody(), QString("You have a new message on ticket #17"));
	QCOMPARE(notification.GetIconName(), QString("Icons/Message"));
	QCOMPARE(notification.GetSeverity(), imtnotify::INotification::SV_WARNING);
	QVERIFY(!notification.IsRead());
	QCOMPARE(notification.GetSourceType(), QString("SupportTicket"));
	QCOMPARE(notification.GetSourceId(), QByteArray("ticket-17"));
	QCOMPARE(notification.GetTargetRoute(), QString("Tickets/Ticket/ticket-17"));
	QCOMPARE(notification.GetMetadata(), QString("{\"number\":17}"));
	QCOMPARE(notification.GetCreatedAt(), QString("2026-07-04T10:00:00Z"));

	notification.SetRead(true);
	QVERIFY(notification.IsRead());
}


void CNotificationTest::testCopyFrom()
{
	imtnotify::CNotificationComp source;
	fill(source);

	imtnotify::CNotificationComp target;
	QVERIFY(target.CopyFrom(source));

	QVERIFY(target.IsEqual(source));
	QCOMPARE(target.GetId(), source.GetId());
	QCOMPARE(target.GetRecipientId(), source.GetRecipientId());
	QCOMPARE(target.GetTargetRoute(), source.GetTargetRoute());
}


void CNotificationTest::testIsEqual_DetectsEveryField()
{
	imtnotify::CNotificationComp a;
	fill(a);

	imtnotify::CNotificationComp b;
	fill(b);
	QVERIFY(a.IsEqual(b));

	// Each individual field difference must break equality.
	b.SetTitle("Different title");
	QVERIFY(!a.IsEqual(b));

	fill(b);
	b.SetRecipientId("other-user");
	QVERIFY(!a.IsEqual(b));

	fill(b);
	b.SetSeverity(imtnotify::INotification::SV_CRITICAL);
	QVERIFY(!a.IsEqual(b));

	fill(b);
	b.SetRead(true);
	QVERIFY(!a.IsEqual(b));
}


void CNotificationTest::testCloneMe()
{
	imtnotify::CNotificationComp source;
	fill(source);

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());

	imtnotify::INotification* clonedPtr = dynamic_cast<imtnotify::INotification*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(source.IsEqual(*clonePtr));
	QCOMPARE(clonedPtr->GetTitle(), source.GetTitle());
}


void CNotificationTest::testResetData()
{
	imtnotify::CNotificationComp notification;
	fill(notification);

	QVERIFY(notification.ResetData());

	QVERIFY(notification.GetId().isEmpty());
	QVERIFY(notification.GetRecipientId().isEmpty());
	QVERIFY(notification.GetTitle().isEmpty());
	QVERIFY(!notification.IsRead());
	QCOMPARE(notification.GetSeverity(), imtnotify::INotification::SV_INFO);
}
