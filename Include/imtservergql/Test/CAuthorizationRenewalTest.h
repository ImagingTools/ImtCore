// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CAuthorizationRenewalTest: public QObject
{
	Q_OBJECT

private slots:
	void RoleChangeIsDetected();
	void OrganizationPermissionsAreCompared();
	void TokenBoundariesAreCompared_data();
	void TokenBoundariesAreCompared();
	void ExpiredAccessToken_data();
	void ExpiredAccessToken();
	void UnavailableIsNotDenied_data();
	void UnavailableIsNotDenied();
	void ChangeDuringCheckIsStale();
	void StaleReadDoesNotFillCache();
};
