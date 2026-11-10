// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	The system flag of user groups and the synchronization of the LDAP group hierarchy
	into the internal group hierarchy by CLdapUserGroupSynchronizerComp.
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
};


