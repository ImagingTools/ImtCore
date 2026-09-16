// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPasswordPolicyTest.h"


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CPasswordPolicyControllerComp.h>
#include <imtauth/CUserInfo.h>
#include <imtcore/Version.h>


namespace
{


// Keep in sync with the version gate in CUserInfo::Serialize.
const quint32 s_passwordFieldsVersion = 23239;


class CFixedVersionInfo: virtual public iser::IVersionInfo
{
public:
	explicit CFixedVersionInfo(quint32 imtCoreVersion): m_imtCoreVersion(imtCoreVersion)
	{
	}

	// reimplemented (iser::IVersionInfo)
	virtual VersionIds GetVersionIds() const override
	{
		VersionIds retVal;
		retVal.insert(imtcore::VI_IMTCORE);

		return retVal;
	}

	virtual bool GetVersionNumber(int versionId, quint32& result) const override
	{
		if (versionId == imtcore::VI_IMTCORE){
			result = m_imtCoreVersion;

			return true;
		}

		return false;
	}

	virtual QString GetVersionIdDescription(int /*versionId*/) const override
	{
		return "ImtCore";
	}

	virtual QString GetEncodedVersionName(int /*versionId*/, quint32 versionNumber) const override
	{
		return QString::number(versionNumber);
	}

private:
	quint32 m_imtCoreVersion;
};


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

	m_unconfiguredPasswordPolicyPtr = m_compositePtr->GetComponentInterface<imtauth::IPasswordPolicy>("UnconfiguredPasswordPolicy");
	QVERIFY2(m_unconfiguredPasswordPolicyPtr != nullptr, "Failed to get UnconfiguredPasswordPolicy component");
}


void CPasswordPolicyTest::cleanupTestCase()
{
	m_passwordPolicyPtr = nullptr;
	m_unconfiguredPasswordPolicyPtr = nullptr;
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


void CPasswordPolicyTest::testUserInfo_SerializationRoundTrip()
{
	// The archive version must be at or above the one the password fields were added at,
	// otherwise they are neither written nor read.
	CFixedVersionInfo versionInfo(s_passwordFieldsVersion);

	QDateTime passwordChangedAt(QDate(2026, 1, 2), QTime(3, 4, 5));
	passwordChangedAt.setTimeSpec(Qt::UTC);
	imtauth::CUserInfo source = MakeUserInfo("hash-current", QByteArrayList() << "hash-1" << "hash-2", passwordChangedAt, true);
	source.SetId("user1");

	iser::CMemoryWriteArchive writeArchive(&versionInfo);
	QVERIFY(source.Serialize(writeArchive));
	QVERIFY(writeArchive.GetBufferSize() > 0);

	iser::CMemoryReadArchive readArchive(writeArchive);
	imtauth::CUserInfo restored;
	QVERIFY(restored.Serialize(readArchive));

	QCOMPARE(restored.GetPasswordHash(), QByteArray("hash-current"));
	QCOMPARE(restored.GetPasswordHistory(), QByteArrayList() << "hash-1" << "hash-2");
	QCOMPARE(restored.GetPasswordChangedAt(), passwordChangedAt);
	QVERIFY(restored.MustChangePassword());
}


void CPasswordPolicyTest::testUserInfo_SerializationLegacyArchive()
{
	// An archive written before the password fields existed must still be readable,
	// and must not be interpreted as carrying them.
	CFixedVersionInfo versionInfo(s_passwordFieldsVersion - 1);

	imtauth::CUserInfo source = MakeUserInfo("hash-current", QByteArrayList() << "hash-1", QDateTime::currentDateTimeUtc(), true);
	source.SetId("user1");

	iser::CMemoryWriteArchive writeArchive(&versionInfo);
	QVERIFY(source.Serialize(writeArchive));

	iser::CMemoryReadArchive readArchive(writeArchive);
	imtauth::CUserInfo restored = MakeUserInfo("stale", QByteArrayList() << "stale-1", QDateTime::currentDateTimeUtc(), true);
	QVERIFY(restored.Serialize(readArchive));

	QCOMPARE(restored.GetPasswordHash(), QByteArray("hash-current"));
	QVERIFY(restored.GetPasswordHistory().isEmpty());
	QVERIFY(!restored.GetPasswordChangedAt().isValid());
	QVERIFY(!restored.MustChangePassword());
}


void CPasswordPolicyTest::testDefaults_UnsetAttributes()
{
	// ACF applies an I_ASSIGN default only to obligatory attributes, so a component
	// whose optional attributes are all unset must still report the documented defaults.
	imtauth::IPasswordPolicy::StrengthRules rules = m_unconfiguredPasswordPolicyPtr->GetStrengthRules();

	QCOMPARE(rules.minLength, imtauth::PasswordPolicyDefaults::MIN_PASSWORD_LENGTH);
	QCOMPARE(rules.maxLength, imtauth::PasswordPolicyDefaults::MAX_PASSWORD_LENGTH);
	QCOMPARE(rules.rejectLoginAsPassword, imtauth::PasswordPolicyDefaults::REJECT_LOGIN_AS_PASSWORD);
	QCOMPARE(m_unconfiguredPasswordPolicyPtr->GetPasswordHistoryDepth(), imtauth::PasswordPolicyDefaults::PASSWORD_HISTORY_DEPTH);
}


void CPasswordPolicyTest::testDefaults_HistoryIsKeptWhenUnconfigured()
{
	// A zero history depth would silently drop the previous hash instead of storing it.
	imtauth::CUserInfo userInfo = MakeUserInfo("hash-new");

	m_unconfiguredPasswordPolicyPtr->OnPasswordChanged(userInfo, "hash-old");

	QCOMPARE(userInfo.GetPasswordHistory(), QByteArrayList() << "hash-old");
	QVERIFY(m_unconfiguredPasswordPolicyPtr->IsPasswordReused(userInfo, "hash-old"));
}


I_ADD_TEST(CPasswordPolicyTest);
