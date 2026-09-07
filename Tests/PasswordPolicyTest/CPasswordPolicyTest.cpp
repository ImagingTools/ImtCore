// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPasswordPolicyTest.h"


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace
{


imtauth::CUserInfo MakeUserInfo(
			const QByteArray& passwordHash,
			const QByteArrayList& passwordHistory = QByteArrayList(),
			const QDateTime& passwordChangedAt = QDateTime(),
			bool mustChangePassword = false)
{
	imtauth::CUserInfo userInfo;
	userInfo.SetPasswordHash(passwordHash);
	userInfo.SetPasswordHistory(passwordHistory);
	userInfo.SetPasswordChangedAt(passwordChangedAt);
	userInfo.SetMustChangePassword(mustChangePassword);
	return userInfo;
}


} // anonymous namespace


void CPasswordPolicyTest::initTestCase()
{
	m_imtCorePath = qEnvironmentVariable("IMTCOREDIR");
	m_registryFile = m_imtCorePath + QString("/Tests/PasswordPolicyTest/Partitura/PasswordPolicyTest.acc");
	m_configFile = m_imtCorePath + "/Config/ImtCoreTest.awc";

	m_compositePtr.SetPtr(new ipackage::CComponentAccessor(m_registryFile, m_configFile));
	QVERIFY2(m_compositePtr.IsValid(), "Failed to create component accessor");

	m_passwordPolicyPtr = m_compositePtr->GetComponentInterface<imtauth::IPasswordPolicy>();
	QVERIFY2(m_passwordPolicyPtr != nullptr, "Failed to get PasswordPolicy component");
}


void CPasswordPolicyTest::cleanupTestCase()
{
	m_passwordPolicyPtr = nullptr;
	m_compositePtr.SetPtr(nullptr);
}


// Password strength
// Configured policy: MinPasswordLength = 8, RequireUppercase = true, RequireDigit = true,
// RejectLoginAsPassword = true, PasswordHistoryDepth = 3, MinPasswordAge = 1 day,
// MaxPasswordAge = 90 days, ExpirationWarningPeriod = 14 days.

void CPasswordPolicyTest::testPasswordStrength_AcceptedPassword()
{
	QStringList violatedRuleIds;
	QVERIFY(m_passwordPolicyPtr->ValidatePasswordStrength("user1", "Str0ngPassword", violatedRuleIds));
	QVERIFY(violatedRuleIds.isEmpty());
}


void CPasswordPolicyTest::testPasswordStrength_TooShort()
{
	QStringList violatedRuleIds;
	QVERIFY(!m_passwordPolicyPtr->ValidatePasswordStrength("user1", "Sh0rt", violatedRuleIds));
	QVERIFY(violatedRuleIds.contains("MinLength"));
}


void CPasswordPolicyTest::testPasswordStrength_MissingUppercase()
{
	QStringList violatedRuleIds;
	QVERIFY(!m_passwordPolicyPtr->ValidatePasswordStrength("user1", "l0ngpassword", violatedRuleIds));
	QVERIFY(violatedRuleIds.contains("UppercaseRequired"));
}


void CPasswordPolicyTest::testPasswordStrength_MissingDigit()
{
	QStringList violatedRuleIds;
	QVERIFY(!m_passwordPolicyPtr->ValidatePasswordStrength("user1", "LongPassword", violatedRuleIds));
	QVERIFY(violatedRuleIds.contains("DigitRequired"));
}


void CPasswordPolicyTest::testPasswordStrength_LoginAsPassword()
{
	QStringList violatedRuleIds;
	QVERIFY(!m_passwordPolicyPtr->ValidatePasswordStrength("Maximilian1", "Maximilian1", violatedRuleIds));
	QVERIFY(violatedRuleIds.contains("LoginAsPassword"));
}


void CPasswordPolicyTest::testPasswordStrength_MultipleViolations()
{
	QStringList violatedRuleIds;
	QVERIFY(!m_passwordPolicyPtr->ValidatePasswordStrength("user1", "abc", violatedRuleIds));
	QVERIFY(violatedRuleIds.contains("MinLength"));
	QVERIFY(violatedRuleIds.contains("UppercaseRequired"));
	QVERIFY(violatedRuleIds.contains("DigitRequired"));
}


// Password history

void CPasswordPolicyTest::testPasswordReuse_CurrentPassword()
{
	imtauth::CUserInfo userInfo = MakeUserInfo("hash-current");

	QVERIFY(m_passwordPolicyPtr->IsPasswordReused(userInfo, "hash-current"));
	QVERIFY(!m_passwordPolicyPtr->IsPasswordReused(userInfo, "hash-new"));
}


void CPasswordPolicyTest::testPasswordReuse_HistoryGenerations()
{
	QByteArrayList passwordHistory;
	passwordHistory << "hash-1" << "hash-2" << "hash-3";
	imtauth::CUserInfo userInfo = MakeUserInfo("hash-current", passwordHistory);

	QVERIFY(m_passwordPolicyPtr->IsPasswordReused(userInfo, "hash-1"));
	QVERIFY(m_passwordPolicyPtr->IsPasswordReused(userInfo, "hash-2"));
	QVERIFY(m_passwordPolicyPtr->IsPasswordReused(userInfo, "hash-3"));
	QVERIFY(!m_passwordPolicyPtr->IsPasswordReused(userInfo, "hash-new"));
}


void CPasswordPolicyTest::testPasswordReuse_TrimmedHistory()
{
	// Only the configured number of generations (3) is checked,
	// older entries beyond the history depth are ignored.
	QByteArrayList passwordHistory;
	passwordHistory << "hash-1" << "hash-2" << "hash-3" << "hash-4";
	imtauth::CUserInfo userInfo = MakeUserInfo("hash-current", passwordHistory);

	QVERIFY(!m_passwordPolicyPtr->IsPasswordReused(userInfo, "hash-4"));
}


void CPasswordPolicyTest::testOnPasswordChanged_UpdatesHistoryAndTimestamp()
{
	QCOMPARE(m_passwordPolicyPtr->GetPasswordHistoryDepth(), 3);

	imtauth::CUserInfo userInfo = MakeUserInfo("hash-new");
	userInfo.SetMustChangePassword(true);

	m_passwordPolicyPtr->OnPasswordChanged(userInfo, "hash-old");

	QCOMPARE(userInfo.GetPasswordHistory(), QByteArrayList() << "hash-old");
	QVERIFY(userInfo.GetPasswordChangedAt().isValid());
	QVERIFY(!userInfo.MustChangePassword());

	// History is trimmed to the configured number of generations
	m_passwordPolicyPtr->OnPasswordChanged(userInfo, "hash-2");
	m_passwordPolicyPtr->OnPasswordChanged(userInfo, "hash-3");
	m_passwordPolicyPtr->OnPasswordChanged(userInfo, "hash-4");

	QCOMPARE(userInfo.GetPasswordHistory(), QByteArrayList() << "hash-4" << "hash-3" << "hash-2");
}


// Password lifetime

void CPasswordPolicyTest::testPasswordChangeAllowed_MinimumAge()
{
	// Password was changed just now, minimum age is 1 day
	imtauth::CUserInfo userInfo = MakeUserInfo("hash", QByteArrayList(), QDateTime::currentDateTimeUtc());
	QVERIFY(!m_passwordPolicyPtr->IsPasswordChangeAllowed(userInfo));

	// Password is older than the minimum age
	userInfo.SetPasswordChangedAt(QDateTime::currentDateTimeUtc().addDays(-2));
	QVERIFY(m_passwordPolicyPtr->IsPasswordChangeAllowed(userInfo));

	// A required change is always allowed
	userInfo.SetPasswordChangedAt(QDateTime::currentDateTimeUtc());
	userInfo.SetMustChangePassword(true);
	QVERIFY(m_passwordPolicyPtr->IsPasswordChangeAllowed(userInfo));
}


void CPasswordPolicyTest::testLifetimeStatus_Ok()
{
	imtauth::CUserInfo userInfo = MakeUserInfo("hash", QByteArrayList(), QDateTime::currentDateTimeUtc().addDays(-10));

	int daysUntilExpiration = -1;
	QCOMPARE(m_passwordPolicyPtr->GetPasswordLifetimeStatus(userInfo, daysUntilExpiration), imtauth::IPasswordPolicy::LS_OK);
	QVERIFY(daysUntilExpiration > 14);
}


void CPasswordPolicyTest::testLifetimeStatus_ExpiresSoon()
{
	// 85 of 90 days passed, warning period is 14 days
	imtauth::CUserInfo userInfo = MakeUserInfo("hash", QByteArrayList(), QDateTime::currentDateTimeUtc().addDays(-85));

	int daysUntilExpiration = -1;
	QCOMPARE(m_passwordPolicyPtr->GetPasswordLifetimeStatus(userInfo, daysUntilExpiration), imtauth::IPasswordPolicy::LS_EXPIRES_SOON);
	QVERIFY(daysUntilExpiration >= 0);
	QVERIFY(daysUntilExpiration <= 14);
}


void CPasswordPolicyTest::testLifetimeStatus_Expired()
{
	imtauth::CUserInfo userInfo = MakeUserInfo("hash", QByteArrayList(), QDateTime::currentDateTimeUtc().addDays(-91));

	int daysUntilExpiration = -1;
	QCOMPARE(m_passwordPolicyPtr->GetPasswordLifetimeStatus(userInfo, daysUntilExpiration), imtauth::IPasswordPolicy::LS_EXPIRED);
}


void CPasswordPolicyTest::testLifetimeStatus_MustChangePassword()
{
	imtauth::CUserInfo userInfo = MakeUserInfo("hash", QByteArrayList(), QDateTime::currentDateTimeUtc(), true);

	int daysUntilExpiration = -1;
	QCOMPARE(m_passwordPolicyPtr->GetPasswordLifetimeStatus(userInfo, daysUntilExpiration), imtauth::IPasswordPolicy::LS_CHANGE_REQUIRED);
}


void CPasswordPolicyTest::testLifetimeStatus_LegacyUserWithoutTimestamp()
{
	// Users without a stored password change timestamp are not locked out
	imtauth::CUserInfo userInfo = MakeUserInfo("hash");

	int daysUntilExpiration = -1;
	QCOMPARE(m_passwordPolicyPtr->GetPasswordLifetimeStatus(userInfo, daysUntilExpiration), imtauth::IPasswordPolicy::LS_OK);
	QVERIFY(m_passwordPolicyPtr->IsPasswordChangeAllowed(userInfo));
}


// User info data model

void CPasswordPolicyTest::testUserInfo_CopyFromAndClone()
{
	imtauth::CUserInfo source = MakeUserInfo(
				"hash-current",
				QByteArrayList() << "hash-1" << "hash-2",
				QDateTime::currentDateTimeUtc().addDays(-5),
				true);

	imtauth::CUserInfo copy;
	QVERIFY(copy.CopyFrom(source));
	QCOMPARE(copy.GetPasswordHash(), source.GetPasswordHash());
	QCOMPARE(copy.GetPasswordHistory(), source.GetPasswordHistory());
	QCOMPARE(copy.GetPasswordChangedAt(), source.GetPasswordChangedAt());
	QCOMPARE(copy.MustChangePassword(), source.MustChangePassword());

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());
	QVERIFY(clonePtr->IsEqual(source));
}


void CPasswordPolicyTest::testUserInfo_IsEqual()
{
	imtauth::CUserInfo first = MakeUserInfo("hash", QByteArrayList() << "hash-1", QDateTime::currentDateTimeUtc(), false);
	imtauth::CUserInfo second;
	QVERIFY(second.CopyFrom(first));
	QVERIFY(first.IsEqual(second));

	second.SetMustChangePassword(true);
	QVERIFY(!first.IsEqual(second));

	QVERIFY(second.CopyFrom(first));
	second.SetPasswordHistory(QByteArrayList() << "hash-other");
	QVERIFY(!first.IsEqual(second));
}


void CPasswordPolicyTest::testUserInfo_ResetDataDefaults()
{
	imtauth::CUserInfo userInfo = MakeUserInfo("hash", QByteArrayList() << "hash-1", QDateTime::currentDateTimeUtc(), true);

	QVERIFY(userInfo.ResetData());
	QVERIFY(userInfo.GetPasswordHash().isEmpty());
	QVERIFY(userInfo.GetPasswordHistory().isEmpty());
	QVERIFY(!userInfo.GetPasswordChangedAt().isValid());
	QVERIFY(!userInfo.MustChangePassword());
}


I_ADD_TEST(CPasswordPolicyTest);
