// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CWebSocketCredentialsTest: public QObject
{
	Q_OBJECT

private slots:
	void RegistrationWithoutChange();
	void RegistrationRollback_data();
	void RegistrationRollback();
	void RegistrationKeptWithSameAccess();
	void CheckOfSubscription_data();
	void CheckOfSubscription();
	void AnonymousSubscription();
};
