// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests for the permissions a held permission includes through feature requirements.
*/
class CPermissionRequirementsTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	// Leaf -> leaf
	void testLeafIncludesLeaf();
	void testIncludedPermissionDoesNotIncludeItsSource();
	void testInclusionIsTransitive();
	void testRequirementCycleTerminates();

	// Group -> leaf
	void testGroupRequirementAppliesToItsLeaves();

	// Leaf -> group
	void testRequiredGroupGrantsAllItsLeaves();

	// Group -> group
	void testGroupRequiringGroup();
	void testIncludedLeafBringsRequirementsOfItsGroup();

	// Requirements that are never followed
	void testRequirementOnOwnBranchIsIgnored();
	void testUnknownRequirementTargetIsIgnored();
	void testUnknownPathIsNotResolvedByItsLastSegment();
	void testNoPermissions();

	// Requirements listed per permission
	void testRequirementsListDirectLeaves();
	void testRequirementsListIsNotTransitive();

	// Features a product takes through requirements
	void testProductTakesRequiredGroup();

	// Requirements in the context of a request
	void testImpliedPermissionsAreLimitedByTenant();
	void testImpliedPermissionsComeFromProductOfRequest();

	// Matching of held and requested paths
	void testBareIdIsNeverHeld();
	void testBareIdIsNeverRequested();
	void testBareIdIncludesNothing();
	void testSameFeatureIdUnderAnotherParentIsAnotherPermission();
	void testDescendantPathSatisfiesParentPathCheck();
	void testDescendantPathDoesNotCrossBranches();

	// Permission path of a request
	void testPermissionPathMustNotBeWiderThanDeclared();

	// Feature data itself
	void testIsPermissionSurvivesCopy();
};


