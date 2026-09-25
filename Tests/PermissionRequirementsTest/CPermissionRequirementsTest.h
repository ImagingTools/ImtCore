// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests for the permissions a held permission implies through feature requirements.
*/
class CPermissionRequirementsTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testDirectRequirements();
	void testTransitiveRequirements();
	void testRequirementsOfBareFeatureId();
	void testRequirementsOfNestedFeature();
	void testUnknownPathIsNotResolvedByItsLastSegment();
	void testRequirementCycleTerminates();
	void testUnknownPermission();
	void testNoPermissions();

	// Permissions stored before feature paths existed
	void testBareFeatureIdCollectsRequirementsOfEveryFeatureCarryingIt();
	void testStoredFeatureIdSatisfiesPathCheck();
	void testStoredPathSatisfiesFeatureIdCheck();
	void testSameFeatureIdUnderAnotherParentIsAnotherPermission();

	// Feature data itself
	void testIsPermissionSurvivesCopy();
};
