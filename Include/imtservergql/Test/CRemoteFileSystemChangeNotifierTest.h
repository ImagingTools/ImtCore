// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CRemoteFileSystemChangeNotifierTest: public QObject
{
	Q_OBJECT

private slots:
	void SubscriptionNamesTheFolderTest();
	void ConfirmedSubscriptionIsReportedTest();
	void ChangesWithinTheDelayAreReportedOnceTest();
	void ReconnectionIsReportedTest();
	void RefusalIsReportedOnceTest();
	void RemovedWatchIsNotReportedTest();
	void WatchesOfOneFolderShareASubscriptionTest();
	void ListenerMayRemoveItsWatchTest();
	void WatchRemovedDuringTheNotificationIsNotReportedTest();
	void RemovalWaitsForTheListenerTest();
	void FailedRegistrationFailsTheWatchTest();
	void RoutedSubscriptionIsNotStoppedTest();
};
