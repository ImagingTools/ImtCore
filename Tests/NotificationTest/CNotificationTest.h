// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtnotify/CNotificationComp.h>


/**
	Unit tests for the imtnotify::CNotificationComp data component, covering the
	istd::IChangeable contract (CopyFrom / IsEqual / CloneMe / ResetData) and the
	property accessors.
*/
class CNotificationTest: public QObject
{
	Q_OBJECT

private:
	// Populate a notification with representative, distinct field values.
	void fill(imtnotify::CNotificationComp& notification);

private Q_SLOTS:
	void testDefaults();
	void testSettersGetters();
	void testCopyFrom();
	void testIsEqual_DetectsEveryField();
	void testCloneMe();
	void testResetData();
};
