// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests for the permission check of a server request: what a command declares,
	what the user of the request holds in its product and organization, and the
	permission path the request runs under.
*/
class CPermissionCheckTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	// Who is checked
	void testRequestWithoutUserIsRefused();
	void testSuperuserIsNotChecked();
	void testTenantOwnerIsChecked();

	// What a command declares
	void testUndeclaredCommandIsRefused();
	void testCommandWithoutPermissionsIsOpen();
	void testUserWithoutPermissionIsRefused();
	void testAllPermissionsOfCommandAreRequired();

	// Every kind of requirement: what a held permission includes
	void testLeafRequiresLeaf();
	void testGroupRequiresLeaf();
	void testLeafRequiresGroup();
	void testGroupRequiresGroup();
	void testRequirementsAreTransitive();
	void testRequirementsChainThroughGroup();
	void testRequirementCycleIncludesBothWays();
	void testGroupAndOwnRequirementsCombine();
	void testRequirementOnOwnBranchIsIgnored();
	void testUnknownRequirementTargetIsIgnored();

	// Only paths are held and declared
	void testBarePermissionOfUserOpensNothing();
	void testBareDeclarationOfCommandIsHeldByNobody();

	// Product and organization of the request
	void testPermissionsOfAnotherProductDoNotOpenCommands();
	void testTenantLimitsIncludedPermissions();
	void testEmptyTenantScopeIncludesNothing();
	void testTenantLimitsLeavesOfRequiredGroup();

	// Permission path of the request
	void testPermissionPathEqualToDeclaredIsAccepted();
	void testPermissionPathInsideDeclaredIsAccepted();
	void testPermissionPathWiderThanDeclaredIsRefused();
	void testPermissionPathNotDeclaredIsRefused();
	void testPermissionPathMustBeHeldByUser();
	void testPermissionPathIsIncludedByHeldPermission();

	// Revisions of a collection
	void testRevisionPermissionOpensOnlyItsCollection();
	void testRevisionActionNeedsItsOwnLeaf();
	void testRevisionPermissionIncludesItsRequirements();
	void testRevisionPermissionIsLimitedByTenant();
	void testRevisionsOfUndeclaredCollectionAreRefused();
	void testUnknownRevisionCommandIsRefused();

	// Elements of the user interface
	void testGuiElementOpensForAnyDeclaredPermission();
	void testGuiElementPermissionPathWiderThanDeclaredIsHidden();
};


