// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CAuthenticationCacheTest: public QObject
{
	Q_OBJECT

private slots:
	void TargetedInvalidation();
	void InFlightInvalidation();
	void SubscriptionCheckBypassesCache();
};
