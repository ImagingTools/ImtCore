// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	The system flag of user groups and the synchronization of the LDAP group hierarchy
	into the internal group hierarchy and of the group members by CLdapUserGroupSynchronizerComp.
*/
class CLdapUserGroupSynchronizerTest: public QObject
{
	Q_OBJECT

private slots:
	// System flag of groups
	void testSystemInfos_SerializationRoundTrip();
	void testSystemInfos_LegacyArchiveIsInternal();
	void testSystemInfos_CopyAndReset();

	// Synchronization
	void testSynchronize_CreatesHierarchy();
	void testSynchronize_UpdateKeepsInternalAssignments();
	void testSynchronize_RemovesOnlyVanishedLdapGroups();
	void testSynchronize_FailedReadChangesNothing();
	void testSynchronize_FailedListReadChangesNothing();
	void testSynchronize_FailedWriteKeepsVanishedGroups();
	void testSynchronize_RemovesMembershipsOfVanishedGroups();
	void testSynchronize_RejectsInvalidSnapshotAndConfiguration();
	void testSynchronize_RejectsMirrorWithoutSystemFlag();
	void testSynchronize_KeepsMirrorsOutsideOfFilter();
	void testGetMirroredLdapGroups();
	void testSynchronize_OmitsNestingCycles();
	void testSynchronize_DisambiguatesGroupNames();

	// Synchronization of the members
	void testSynchronize_AddsExistingUsersToGroups();
	void testSynchronize_CreatesUnknownUsers();
	void testSynchronize_UpdatesMembers();
	void testSynchronize_RejectsInvalidUserSnapshot();
	void testIsMatchingName();
	void testGetSearchPath();
	void testGetDomainSearchPath();
	void testGetSearchFilter();
	void testGetUserSearchFilters();
	void testGetBaseDn();
	void testGetGroupNameFilter();
	void testLdapSettingsLabels();
	void testCreateGroupTree();
	void testCreateGroupTreeCycle();
	void testIsSynchronizationEnabled();
	void testIsSystemFlagStored();
};
