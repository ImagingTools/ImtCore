// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtauth/IPasswordPolicy.h>


/**
	Test class for the configurable password policy (IEC 62443-4-2 CR 1.7).
	Tests password strength rules, password history (reuse) checks,
	password lifetime evaluation and the extended user info data model.
*/
class CPasswordPolicyTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

	// Password strength
	void testPasswordStrength_AcceptedPassword();
	void testPasswordStrength_TooShort();
	void testPasswordStrength_MissingUppercase();
	void testPasswordStrength_MissingDigit();
	void testPasswordStrength_LoginAsPassword();
	void testPasswordStrength_MultipleViolations();

	// Password history
	void testPasswordReuse_CurrentPassword();
	void testPasswordReuse_HistoryGenerations();
	void testPasswordReuse_TrimmedHistory();
	void testOnPasswordChanged_UpdatesHistoryAndTimestamp();

	// Password lifetime
	void testPasswordChangeAllowed_MinimumAge();
	void testLifetimeStatus_Ok();
	void testLifetimeStatus_ExpiresSoon();
	void testLifetimeStatus_Expired();
	void testLifetimeStatus_MustChangePassword();
	void testLifetimeStatus_LegacyUserWithoutTimestamp();

	// User info data model
	void testUserInfo_CopyFromAndClone();
	void testUserInfo_IsEqual();
	void testUserInfo_ResetDataDefaults();

private:
	QString m_imtCorePath;
	QString m_registryFile;
	QString m_configFile;
	istd::TDelPtr<ipackage::CComponentAccessor> m_compositePtr;
	imtauth::IPasswordPolicy* m_passwordPolicyPtr = nullptr;
};
