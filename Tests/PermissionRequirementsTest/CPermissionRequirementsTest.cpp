// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPermissionRequirementsTest.h"


// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlContext.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CProductInfo.h>
#include <imtlicgql/imtlicgql.h>


namespace
{


typedef QHash<QByteArray, QByteArrayList> Requirements;


const QByteArray s_viewAdministration = "/Administration/ViewAdministration";
const QByteArray s_viewUsers = "/Administration/ViewAdministration/ViewUsers";
const QByteArray s_viewRoles = "/Administration/ViewAdministration/ViewRoles";
const QByteArray s_editAdministration = "/Administration/EditAdministration";
const QByteArray s_editUser = "/Administration/EditAdministration/EditUser";
const QByteArray s_changeUser = "/Administration/EditAdministration/EditUser/ChangeUser";
const QByteArray s_removeUser = "/Administration/EditAdministration/EditUser/RemoveUser";
const QByteArray s_changeRole = "/Administration/EditAdministration/EditRole/ChangeRole";
const QByteArray s_revisionManagement = "/RevisionManagement";
const QByteArray s_viewRevisions = "/RevisionManagement/ViewRevisions";
const QByteArray s_exportRevision = "/RevisionManagement/ExportRevision";
const QByteArray s_reportsViewUsers = "/Reports/ViewUsers";


void FillFeature(imtlic::CFeatureInfo& featureInfo, const QByteArray& path, const Requirements& requirements)
{
	featureInfo.SetFeatureId(path.mid(path.lastIndexOf('/') + 1));
	featureInfo.SetIsPermission(true);
	featureInfo.SetRequirements(requirements.value(path));
}


imtlic::IFeatureInfo::FeatureInfoPtr CreateFeature(const QByteArray& path, const Requirements& requirements, const QByteArrayList& subPaths = QByteArrayList())
{
	imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo();
	FillFeature(*featureInfoPtr, path, requirements);

	for (const QByteArray& subPath : subPaths){
		featureInfoPtr->InsertSubFeature(CreateFeature(subPath, requirements));
	}

	return imtlic::IFeatureInfo::FeatureInfoPtr(featureInfoPtr);
}


/**
	Product the requirements of a test are declared in:

	/Administration
	  ViewAdministration: ViewUsers, ViewRoles
	  EditAdministration
	    EditUser: ChangeUser, RemoveUser
	    EditRole: ChangeRole
	/RevisionManagement: ViewRevisions, ExportRevision
	/Reports: ViewUsers
*/
void SetupProduct(imtlic::CProductInfo& productInfo, const Requirements& requirements)
{
	imtlic::IFeatureInfo::FeatureInfoPtr editUserPtr = CreateFeature(s_editUser, requirements, {s_changeUser, s_removeUser});
	imtlic::IFeatureInfo::FeatureInfoPtr editRolePtr = CreateFeature("/Administration/EditAdministration/EditRole", requirements, {s_changeRole});
	imtlic::IFeatureInfo::FeatureInfoPtr editAdministrationPtr = CreateFeature(s_editAdministration, requirements);
	editAdministrationPtr->InsertSubFeature(editUserPtr);
	editAdministrationPtr->InsertSubFeature(editRolePtr);

	imtlic::CIdentifiableFeatureInfo administration;
	FillFeature(administration, "/Administration", requirements);
	administration.InsertSubFeature(CreateFeature(s_viewAdministration, requirements, {s_viewUsers, s_viewRoles}));
	administration.InsertSubFeature(editAdministrationPtr);

	imtlic::CIdentifiableFeatureInfo revisions;
	FillFeature(revisions, s_revisionManagement, requirements);
	revisions.InsertSubFeature(CreateFeature(s_viewRevisions, requirements));
	revisions.InsertSubFeature(CreateFeature(s_exportRevision, requirements));

	imtlic::CIdentifiableFeatureInfo reports;
	FillFeature(reports, "/Reports", requirements);
	reports.InsertSubFeature(CreateFeature(s_reportsViewUsers, requirements));

	QVERIFY(productInfo.AddFeature("Administration", administration));
	QVERIFY(productInfo.AddFeature("RevisionManagement", revisions));
	QVERIFY(productInfo.AddFeature("Reports", reports));
}


QByteArrayList Sorted(QByteArrayList permissions)
{
	std::sort(permissions.begin(), permissions.end());

	return permissions;
}


QByteArrayList CollectImplied(const Requirements& requirements, const QByteArrayList& permissions)
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo, requirements);

	return Sorted(imtlic::CollectImpliedPermissions(productInfo, permissions));
}


} // anonymous namespace


void CPermissionRequirementsTest::testLeafIncludesLeaf()
{
	const Requirements requirements{{s_changeUser, {s_viewUsers}}};

	QCOMPARE(CollectImplied(requirements, {s_changeUser}), QByteArrayList{s_viewUsers});
}


void CPermissionRequirementsTest::testIncludedPermissionDoesNotIncludeItsSource()
{
	const Requirements requirements{{s_changeUser, {s_viewUsers}}};

	QVERIFY(CollectImplied(requirements, {s_viewUsers}).isEmpty());
}


void CPermissionRequirementsTest::testInclusionIsTransitive()
{
	const Requirements requirements{
				{s_changeUser, {s_viewUsers}},
				{s_viewUsers, {s_viewRevisions}}};

	QCOMPARE(CollectImplied(requirements, {s_changeUser}), Sorted({s_viewUsers, s_viewRevisions}));
}


void CPermissionRequirementsTest::testRequirementCycleTerminates()
{
	const Requirements requirements{
				{s_changeUser, {s_viewUsers}},
				{s_viewUsers, {s_changeUser}}};

	QCOMPARE(CollectImplied(requirements, {s_changeUser}), QByteArrayList{s_viewUsers});
}


void CPermissionRequirementsTest::testGroupRequirementAppliesToItsLeaves()
{
	const Requirements requirements{{s_editUser, {s_viewUsers}}};

	QCOMPARE(CollectImplied(requirements, {s_removeUser}), QByteArrayList{s_viewUsers});
	QVERIFY(CollectImplied(requirements, {s_changeRole}).isEmpty());
}


void CPermissionRequirementsTest::testRequiredGroupGrantsAllItsLeaves()
{
	const Requirements requirements{{s_changeUser, {s_viewAdministration}}};

	QCOMPARE(CollectImplied(requirements, {s_changeUser}), Sorted({s_viewUsers, s_viewRoles}));
	QVERIFY(CollectImplied(requirements, {s_viewRoles}).isEmpty());
}


void CPermissionRequirementsTest::testGroupRequiringGroup()
{
	const Requirements requirements{{s_editAdministration, {s_viewAdministration}}};

	QCOMPARE(CollectImplied(requirements, {s_changeRole}), Sorted({s_viewUsers, s_viewRoles}));
	QVERIFY(CollectImplied(requirements, {s_viewUsers}).isEmpty());
}


void CPermissionRequirementsTest::testIncludedLeafBringsRequirementsOfItsGroup()
{
	const Requirements requirements{
				{s_editAdministration, {s_viewAdministration}},
				{s_viewAdministration, {s_viewRevisions}}};

	QCOMPARE(CollectImplied(requirements, {s_changeUser}), Sorted({s_viewUsers, s_viewRoles, s_viewRevisions}));
}


void CPermissionRequirementsTest::testRequirementOnOwnBranchIsIgnored()
{
	const Requirements requirements{
				{s_changeUser, {s_changeUser, s_editUser, "/Administration"}},
				{s_editUser, {s_changeUser}},
				{s_editAdministration, {s_editUser}}};

	QVERIFY(CollectImplied(requirements, {s_changeUser}).isEmpty());
	QVERIFY(CollectImplied(requirements, {s_removeUser}).isEmpty());
	QVERIFY(CollectImplied(requirements, {s_changeRole}).isEmpty());
}


void CPermissionRequirementsTest::testUnknownRequirementTargetIsIgnored()
{
	const Requirements requirements{{s_changeUser, {"/Administration/NoSuchPermission", "ViewUsers"}}};

	QVERIFY(CollectImplied(requirements, {s_changeUser}).isEmpty());
}


void CPermissionRequirementsTest::testUnknownPathIsNotResolvedByItsLastSegment()
{
	const Requirements requirements{{s_changeUser, {s_viewUsers}}};

	// "ChangeUser" exists, but not under "/Other": a path says which parent it belongs to.
	QVERIFY(CollectImplied(requirements, {"/Other/ChangeUser"}).isEmpty());
}


void CPermissionRequirementsTest::testNoPermissions()
{
	const Requirements requirements{{s_changeUser, {s_viewUsers}}};

	QVERIFY(CollectImplied(requirements, QByteArrayList()).isEmpty());
}


void CPermissionRequirementsTest::testProductTakesRequiredGroup()
{
	imtlic::CProductInfo catalog;
	SetupProduct(catalog, {
				{s_changeUser, {s_revisionManagement, "/Administration"}},
				{s_reportsViewUsers, {s_viewUsers}}});

	// Export is an optional part of the revisions feature.
	imtlic::CFeatureInfo* exportRevisionPtr = new imtlic::CFeatureInfo();
	FillFeature(*exportRevisionPtr, s_exportRevision, Requirements());
	exportRevisionPtr->SetOptional(true);

	imtlic::CIdentifiableFeatureInfo revisions;
	FillFeature(revisions, s_revisionManagement, Requirements());
	revisions.InsertSubFeature(CreateFeature(s_viewRevisions, Requirements()));
	revisions.InsertSubFeature(imtlic::IFeatureInfo::FeatureInfoPtr(exportRevisionPtr));

	imtbase::IObjectCollection* catalogPtr = catalog.GetFeatures();
	QVERIFY(catalogPtr->SetObjectData("RevisionManagement", revisions));

	const imtlicgql::ProductFeatureClosure closure = imtlicgql::CompleteProductFeatures(
				*catalogPtr,
				{"Administration"},
				imtlic::IProductInfo::OptionalFeatureInfos());

	QCOMPARE(closure.featureIds, QByteArrayList({"Administration", "RevisionManagement"}));
	QVERIFY(closure.unresolvedRequirements.isEmpty());
	QCOMPARE(closure.optionalFeatures.count(), 1);
	QCOMPARE(closure.optionalFeatures.first().featureId, QByteArrayLiteral("RevisionManagement"));
	QCOMPARE(closure.optionalFeatures.first().subFeatureIds, QByteArrayList{"ExportRevision"});
}


void CPermissionRequirementsTest::testImpliedPermissionsAreLimitedByTenant()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo, {{s_changeUser, {s_viewAdministration}}});

	imtauth::CUserInfo userInfo;
	userInfo.SetLocalPermissions("Lisa", {s_changeUser});

	imtgql::CGqlContext gqlContext;
	gqlContext.SetProductId("Lisa");
	gqlContext.SetProductInfo(&productInfo);
	gqlContext.SetUserInfo(&userInfo);
	QCOMPARE(Sorted(gqlContext.GetImpliedPermissions()), Sorted({s_viewUsers, s_viewRoles}));

	gqlContext.SetTenantPermissions({s_changeUser, s_viewUsers});
	QCOMPARE(gqlContext.GetImpliedPermissions(), QByteArrayList{s_viewUsers});
}


void CPermissionRequirementsTest::testImpliedPermissionsComeFromProductOfRequest()
{
	imtlic::CProductInfo productInfo;
	SetupProduct(productInfo, {{s_changeUser, {s_viewUsers}}});

	imtauth::CUserInfo userInfo;
	userInfo.SetLocalPermissions("OtherProduct", {s_changeUser});

	imtgql::CGqlContext gqlContext;
	gqlContext.SetProductId("Lisa");
	gqlContext.SetProductInfo(&productInfo);
	gqlContext.SetUserInfo(&userInfo);
	QVERIFY(gqlContext.GetImpliedPermissions().isEmpty());

	gqlContext.SetProductId("OtherProduct");
	QCOMPARE(gqlContext.GetImpliedPermissions(), QByteArrayList{s_viewUsers});
}


void CPermissionRequirementsTest::testBareIdIsNeverHeld()
{
	const QByteArrayList userPermissions{"ChangeUser"};

	QVERIFY(!imtauth::HasPermission(userPermissions, s_changeUser));
	QVERIFY(!imtauth::HasPermission(userPermissions, s_editUser));
	QVERIFY(!imtauth::HasPermission(userPermissions, QByteArrayLiteral("ChangeUser")));
}


void CPermissionRequirementsTest::testBareIdIsNeverRequested()
{
	const QByteArrayList userPermissions{s_changeUser};

	QVERIFY(imtauth::HasPermission(userPermissions, s_changeUser));
	QVERIFY(!imtauth::HasPermission(userPermissions, QByteArrayLiteral("ChangeUser")));
	QVERIFY(!imtauth::HasPermission(userPermissions, QByteArrayLiteral("EditUser")));
}


void CPermissionRequirementsTest::testBareIdIncludesNothing()
{
	const Requirements requirements{
				{s_viewUsers, {s_viewRevisions}},
				{s_reportsViewUsers, {s_exportRevision}}};

	QVERIFY(CollectImplied(requirements, {"ViewUsers"}).isEmpty());
	QCOMPARE(CollectImplied(requirements, {s_viewUsers}), QByteArrayList{s_viewRevisions});
}


void CPermissionRequirementsTest::testSameFeatureIdUnderAnotherParentIsAnotherPermission()
{
	const QByteArrayList userPermissions{s_viewUsers};

	QVERIFY(!imtauth::HasPermission(userPermissions, s_reportsViewUsers));
}


void CPermissionRequirementsTest::testDescendantPathSatisfiesParentPathCheck()
{
	const QByteArrayList userPermissions{s_changeUser};

	QVERIFY(imtauth::HasPermission(userPermissions, s_editUser));
	QVERIFY(imtauth::HasPermission(userPermissions, QByteArrayLiteral("/Administration")));
	QVERIFY(!imtauth::HasPermission(QByteArrayList{s_editUser}, s_changeUser));
}


void CPermissionRequirementsTest::testDescendantPathDoesNotCrossBranches()
{
	const QByteArrayList userPermissions{s_changeUser};

	QVERIFY(!imtauth::HasPermission(userPermissions, s_removeUser));
	QVERIFY(!imtauth::HasPermission(userPermissions, s_viewAdministration));
	QVERIFY(!imtauth::HasPermission(userPermissions, QByteArrayLiteral("/Administration/EditAdministration/EditUs")));
}


void CPermissionRequirementsTest::testPermissionPathMustNotBeWiderThanDeclared()
{
	const QByteArrayList declaredPermissions{"/Devices/ViewDeviceTickets", "/Support/ViewTickets"};

	QVERIFY(imtauth::IsPermissionPathDeclared(declaredPermissions, "/Devices/ViewDeviceTickets"));
	QVERIFY(imtauth::IsPermissionPathDeclared(declaredPermissions, "/Devices/ViewDeviceTickets/Assigned"));
	QVERIFY(!imtauth::IsPermissionPathDeclared(declaredPermissions, "/Devices"));
	QVERIFY(!imtauth::IsPermissionPathDeclared(declaredPermissions, "/Devices/ViewDevices"));
	QVERIFY(!imtauth::IsPermissionPathDeclared(QByteArrayList{"ViewDeviceTickets"}, "/Devices/ViewDeviceTickets"));
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


