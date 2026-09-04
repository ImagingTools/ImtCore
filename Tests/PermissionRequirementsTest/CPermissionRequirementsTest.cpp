// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPermissionRequirementsTest.h"


// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CProductInfo.h>


namespace
{


imtlic::IFeatureInfo::FeatureInfoPtr CreatePermission(const QByteArray& featureId, const QByteArrayList& requirements)
{
	imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo();
	featureInfoPtr->SetFeatureId(featureId);
	featureInfoPtr->SetIsPermission(true);
	featureInfoPtr->SetRequirements(requirements);

	return imtlic::IFeatureInfo::FeatureInfoPtr(featureInfoPtr);
}


/**
	Product with two permission groups:

	/Administration/EditUser  -> requires /Administration/ViewUser
	/Administration/ViewUser  -> requires /Reports/ViewBasic
	/Reports/ViewBasic
*/
void SetupProduct(imtlic::CProductInfo& productInfo)
{
	imtlic::CFeatureInfo administration;
	administration.SetFeatureId("Administration");
	administration.SetIsPermission(true);
	administration.InsertSubFeature(CreatePermission("EditUser", QByteArrayList{"/Administration/ViewUser"}));
	administration.InsertSubFeature(CreatePermission("ViewUser", QByteArrayList{"/Reports/ViewBasic"}));

	imtlic::CFeatureInfo reports;
	reports.SetFeatureId("Reports");
	reports.SetIsPermission(true);
	reports.InsertSubFeature(CreatePermission("ViewBasic", QByteArrayList()));

	productInfo.AddFeature("Administration", administration);
	productInfo.AddFeature("Reports", reports);
}


} // anonymous namespace


void CPermissionRequirementsTest::testDirectRequirements()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo);

	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"/Administration/ViewUser"});

	QCOMPARE(impliedPermissions, QByteArrayList{"/Reports/ViewBasic"});
}


void CPermissionRequirementsTest::testTransitiveRequirements()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo);

	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"/Administration/EditUser"});

	QCOMPARE(impliedPermissions.count(), 2);
	QVERIFY(impliedPermissions.contains("/Administration/ViewUser"));
	QVERIFY(impliedPermissions.contains("/Reports/ViewBasic"));
}


void CPermissionRequirementsTest::testRequirementsOfBareFeatureId()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo);

	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"EditUser"});

	QCOMPARE(impliedPermissions.count(), 2);
	QVERIFY(impliedPermissions.contains("/Administration/ViewUser"));
	QVERIFY(impliedPermissions.contains("/Reports/ViewBasic"));
}


void CPermissionRequirementsTest::testRequirementsOfNestedFeature()
{
	imtlic::CProductInfo productInfo;

	imtlic::IFeatureInfo::FeatureInfoPtr usersPtr = CreatePermission("Users", QByteArrayList());
	usersPtr->InsertSubFeature(CreatePermission("EditUser", QByteArrayList{"/Administration/Users/ViewUser"}));
	usersPtr->InsertSubFeature(CreatePermission("ViewUser", QByteArrayList()));

	imtlic::CFeatureInfo administration;
	administration.SetFeatureId("Administration");
	administration.SetIsPermission(true);
	administration.InsertSubFeature(usersPtr);

	productInfo.AddFeature("Administration", administration);

	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"/Administration/Users/EditUser"});

	QCOMPARE(impliedPermissions, QByteArrayList{"/Administration/Users/ViewUser"});
}


void CPermissionRequirementsTest::testUnknownPathIsNotResolvedByItsLastSegment()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo);

	// "ViewUser" exists, but not under "/Reports": another branch may name its
	// own feature the same way, and it grants requirements of its own.
	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"/Reports/ViewUser"});

	QVERIFY(impliedPermissions.isEmpty());
}


void CPermissionRequirementsTest::testRequirementCycleTerminates()
{
	imtlic::CProductInfo productInfo;

	imtlic::CFeatureInfo administration;
	administration.SetFeatureId("Administration");
	administration.SetIsPermission(true);
	administration.InsertSubFeature(CreatePermission("First", QByteArrayList{"/Administration/Second"}));
	administration.InsertSubFeature(CreatePermission("Second", QByteArrayList{"/Administration/First"}));

	productInfo.AddFeature("Administration", administration);

	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"/Administration/First"});

	QCOMPARE(impliedPermissions, QByteArrayList{"/Administration/Second"});
}


void CPermissionRequirementsTest::testUnknownPermission()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo);

	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"/Administration/NoSuchPermission"});

	QVERIFY(impliedPermissions.isEmpty());
}


void CPermissionRequirementsTest::testNoPermissions()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo);

	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(productInfo, QByteArrayList());

	QVERIFY(impliedPermissions.isEmpty());
}


void CPermissionRequirementsTest::testBareFeatureIdCollectsRequirementsOfEveryFeatureCarryingIt()
{
	imtlic::CProductInfo productInfo;

	imtlic::CFeatureInfo documents;
	documents.SetFeatureId("Documents");
	documents.SetIsPermission(true);
	documents.InsertSubFeature(CreatePermission("Edit", QByteArrayList{"/Documents/View"}));
	documents.InsertSubFeature(CreatePermission("View", QByteArrayList()));

	imtlic::CFeatureInfo users;
	users.SetFeatureId("Users");
	users.SetIsPermission(true);
	users.InsertSubFeature(CreatePermission("Edit", QByteArrayList{"/Users/View"}));
	users.InsertSubFeature(CreatePermission("View", QByteArrayList()));

	productInfo.AddFeature("Documents", documents);
	productInfo.AddFeature("Users", users);

	// A permission stored as a bare "Edit" passes the check for both features,
	// so it grants what both of them require.
	const QByteArrayList impliedPermissions = imtlic::CollectImpliedPermissions(
			productInfo,
			QByteArrayList{"Edit"});

	QCOMPARE(impliedPermissions.count(), 2);
	QVERIFY(impliedPermissions.contains("/Documents/View"));
	QVERIFY(impliedPermissions.contains("/Users/View"));
}


void CPermissionRequirementsTest::testStoredFeatureIdSatisfiesPathCheck()
{
	// What an older version stored for this user.
	const QByteArrayList userPermissions{"EditUser"};

	QVERIFY(imtauth::HasPermission(userPermissions, QByteArrayLiteral("/Administration/EditUser")));
	QVERIFY(imtauth::HasPermission(userPermissions, QByteArrayLiteral("EditUser")));
	QVERIFY(!imtauth::HasPermission(userPermissions, QByteArrayLiteral("/Administration/ViewUser")));
}


void CPermissionRequirementsTest::testStoredPathSatisfiesFeatureIdCheck()
{
	const QByteArrayList userPermissions{"/Administration/EditUser"};

	QVERIFY(imtauth::HasPermission(userPermissions, QByteArrayLiteral("EditUser")));
	QVERIFY(imtauth::HasPermission(userPermissions, QByteArrayLiteral("/Administration/EditUser")));
}


void CPermissionRequirementsTest::testSameFeatureIdUnderAnotherParentIsAnotherPermission()
{
	const QByteArrayList userPermissions{"/Documents/Edit"};

	QVERIFY(!imtauth::HasPermission(userPermissions, QByteArrayLiteral("/Users/Edit")));
}


void CPermissionRequirementsTest::testIsPermissionSurvivesCopy()
{
	imtlic::CFeatureInfo sellableFeature;
	sellableFeature.SetFeatureId("AdvancedAnalytics");
	sellableFeature.SetIsPermission(false);

	imtlic::CFeatureInfo copiedFeature;
	QVERIFY(copiedFeature.CopyFrom(sellableFeature));

	QCOMPARE(copiedFeature.IsPermission(), false);
	QVERIFY(copiedFeature.IsEqual(sellableFeature));
}


I_ADD_TEST(CPermissionRequirementsTest);
