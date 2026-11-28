// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the document actions of the assistant: a confirmed change writes into a document action (the handlers get its ID
	in the header of the request), and changes.list / changes.rollback list and roll back a document action as a whole,
	only after the checks of the owner, the state, the current documents and the permissions.
*/
class CAssistantDocumentActionTest: public QObject
{
	Q_OBJECT

private slots:
	void testAddUserToGroup_WritesIntoDocumentAction();
	void testAddUserToGroup_FailedDocumentAction();
	void testAddUserToGroup_NothingChangedWithoutDocumentAction();
	void testPlan_DocumentActionOfAllSteps();
	void testPlan_PartiallyAppliedDocumentAction();
	void testPlan_StaleIfObjectChangedAfterPreview();
	void testPlan_RefusedIfChangesCannotBeDetected();
	void testServerHeadersAreRemovedFromClientRequests();
	void testList_OwnDocumentActionsAndReversibility();
	void testList_AllUsersNeedsPermission();
	void testRollback_RestoresFormerRevision();
	void testRollback_RemovesCreatedDocument();
	void testRollback_RefusedIfDocumentChangedLater();
	void testRollback_DocumentActionOfOtherUser();
	void testRollback_NeedsPermissionOfCollection();
	void testRollback_StaleAfterPreview();
	void testRollback_RefusedForRemovalsAndUnrecordedWrites();
	void testRollback_RefusedTwice();
};


