#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Test class for the account-level enabled state of user accounts (IEC 62443-3-3 SR 1.3).
	Tests the enable/disable lifecycle of a user account and its propagation through copy,
	comparison, clone and reset operations of the user data model.
*/
class CUserAccountStateTest: public QObject
{
	Q_OBJECT

private slots:
	void testAccountEnabledByDefault();
	void testDisableAccount();
	void testEnableAccount();
	void testCopyFromPreservesEnabledState();
	void testIsEqualDetectsEnabledStateChange();
	void testCloneMePreservesEnabledState();
	void testResetDataRestoresEnabledState();
};
