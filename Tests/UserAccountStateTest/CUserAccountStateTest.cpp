#include "CUserAccountStateTest.h"


// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


void CUserAccountStateTest::testAccountEnabledByDefault()
{
	imtauth::CUserInfo userInfo;

	QVERIFY(userInfo.IsEnabled());
}


void CUserAccountStateTest::testDisableAccount()
{
	imtauth::CUserInfo userInfo;

	userInfo.SetEnabled(false);

	QVERIFY(!userInfo.IsEnabled());
}


void CUserAccountStateTest::testEnableAccount()
{
	imtauth::CUserInfo userInfo;

	userInfo.SetEnabled(false);
	userInfo.SetEnabled(true);

	QVERIFY(userInfo.IsEnabled());
}


void CUserAccountStateTest::testCopyFromPreservesEnabledState()
{
	imtauth::CUserInfo sourceUserInfo;
	sourceUserInfo.SetId("user");
	sourceUserInfo.SetName("User");
	sourceUserInfo.SetEnabled(false);

	imtauth::CUserInfo targetUserInfo;
	QVERIFY(targetUserInfo.CopyFrom(sourceUserInfo));
	QVERIFY(!targetUserInfo.IsEnabled());
}


void CUserAccountStateTest::testIsEqualDetectsEnabledStateChange()
{
	imtauth::CUserInfo firstUserInfo;
	firstUserInfo.SetId("user");
	firstUserInfo.SetName("User");

	imtauth::CUserInfo secondUserInfo;
	QVERIFY(secondUserInfo.CopyFrom(firstUserInfo));
	QVERIFY(firstUserInfo.IsEqual(secondUserInfo));

	secondUserInfo.SetEnabled(false);

	QVERIFY(!firstUserInfo.IsEqual(secondUserInfo));
}


void CUserAccountStateTest::testCloneMePreservesEnabledState()
{
	imtauth::CUserInfo userInfo;
	userInfo.SetId("user");
	userInfo.SetName("User");
	userInfo.SetEnabled(false);

	istd::IChangeableUniquePtr clonePtr = userInfo.CloneMe();
	QVERIFY(clonePtr.IsValid());

	auto clonedUserInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(clonePtr.GetPtr());
	QVERIFY(clonedUserInfoPtr != nullptr);
	QVERIFY(!clonedUserInfoPtr->IsEnabled());
}


void CUserAccountStateTest::testResetDataRestoresEnabledState()
{
	imtauth::CUserInfo userInfo;
	userInfo.SetEnabled(false);

	QVERIFY(userInfo.ResetData());
	QVERIFY(userInfo.IsEnabled());
}


I_ADD_TEST(CUserAccountStateTest);
