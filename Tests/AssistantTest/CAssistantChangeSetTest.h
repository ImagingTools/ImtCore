// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the change sets of the assistant: a confirmed change writes into a change set (the handlers get its ID
	in the header of the request), and changes.list / changes.rollback list and roll back a change set as a whole,
	only after the checks of the owner, the state, the current documents and the permissions.
*/
class CAssistantChangeSetTest: public QObject
{
	Q_OBJECT

private slots:
	void testAddUserToGroup_WritesIntoChangeSet();
	void testAddUserToGroup_FailedChangeSet();
	void testAddUserToGroup_NothingChangedWithoutChangeSet();
	void testPlan_ChangeSetOfAllSteps();
	void testPlan_PartiallyAppliedChangeSet();
	void testServerHeadersAreRemovedFromClientRequests();
	void testList_OwnChangeSetsAndReversibility();
	void testList_AllUsersNeedsPermission();
	void testRollback_RestoresFormerRevision();
	void testRollback_RemovesCreatedDocument();
	void testRollback_RefusedIfDocumentChangedLater();
	void testRollback_ChangeSetOfOtherUser();
	void testRollback_NeedsPermissionOfCollection();
	void testRollback_StaleAfterPreview();
	void testRollback_RefusedForRemovalsAndUnrecordedWrites();
	void testRollback_RefusedTwice();
};


