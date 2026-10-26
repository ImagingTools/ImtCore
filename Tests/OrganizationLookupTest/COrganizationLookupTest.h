// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	The organization lookups of the real managers: they must read only the documents related to the asked user or organization
	(a lookup over every organization made sign-in take minutes with a thousand organizations),
	and stay correct over a collection that ignores the filters.
*/
class COrganizationLookupTest: public QObject
{
	Q_OBJECT

private slots:
	// Memberships
	void testFindMembership_LoadsOnlyTheCandidate();
	void testFindMembership_CollectionWithoutFilters_FindsTheRightOne();
	void testFindMembership_NonUuidIds_MatchNothing();
	void testGetMembershipsByUser_ReturnsOnlyTheUsers();
	void testGetMembershipsByTenant_ReturnsOnlyTheTenants();
	void testIsMember_InactiveMembershipIsNoMember();

	// Organizations
	void testOwnedOrCreated_ReturnsOwnedAndCreatedOnly();
	void testOwnedOrCreated_CollectionWithoutFilters_ReturnsOwnedAndCreatedOnly();
	void testOwnedOrCreated_InvalidUserId_ReturnsNothing();
	void testChildTenantIds_ReturnsDirectChildrenOnly();
	void testFindTenantIdsByName_IgnoresCaseAndSpacesNotParts();
	void testFindTenantIdsByName_SemicolonAndBackslashStayOutOfTheSelection();
	void testChildTenantIds_ReportsAFailedRead();
	void testGetGrantsBySourceTenant_ReportsAFailedRead();
	void testGetRelatedSince_ReportsAFailedHistoryRead();
	void testRemoveVerificationData_UnreadableCodeIsRemovedAnyway();
	void testConsumeVerification_OnlyOneConcurrentCallerSucceeds();
	void testRevokeInvitationsSentBy_ReportsAnUnreadInvitation();
	void testShareDocument_RefusesUnreadRelationships();
	void testShareDocument_RefusesAnUnreadShareOfTheDocument();
	void testShareDocument_RefusesWhenTheEarlierShareStays();
	void testGetShareOwnerTenantIds_UnreadShareTrustsNoPartner();
	void testRemoveConnection_KeepsTheConnectionWhenARelationshipStays();
	void testApproveProposal_IsNotAppliedTwice();
	void testApproveProposal_AfterTheConnectionWasRemovedCreatesNoRelationship();
	void testApproveProposal_ConcurrentApprovalsCreateOneRelationship();

	// Invitations
	void testRevokeInvitationsSentBy_RevokesOnlyPendingInvitationsOfTheInviter();
	void testGetInvitationsByUser_LoadsOnlyTheUsers();

	// Document shares
	void testRemoveUnrelatedShares_RemovesTheSharesWithoutRelationship();
	void testRemoveUnrelatedShares_UnreadableRelationshipsRemoveNothing();
	void testGetShareOwnerTenantIds_FailedRemovalTrustsNoPartner();
	void testRemoveUnrelatedShares_CollectionWithoutFilters_KeepsSharesOfOtherTenants();
	void testRemoveUnrelatedShares_RemovesTheSharesOfAnEarlierRelationship();
	void testGetShareOwnerTenantIds_DropsTheSharesOfAnEarlierRelationship();
	void testShareDocument_ReplacesTheShareOfAnEarlierRelationship();
	void testRemoveUnrelatedShares_KeepsTheSharesOfARelationshipOutsideItsValidity();
	void testShareDocument_RefusesARelationshipOutsideItsValidity();
	void testRemoveUnrelatedShares_ReadsTheRelationshipsOncePerCachePeriod();

	// Connections and relationships
	void testGetConnectionIds_LoadsOnlyTheTenants();
	void testGetTenantRelationshipIds_LoadsOnlyTheTenants();
	void testRemoveConnection_ArchivesOnlyItsRelationships();
	void testConnectionLookups_NonUuidIds_MatchNothing();
	void testGetTenantRelationship_UnknownIdIsNoRelationship();
	void testGetTenantRelationshipIds_ReportsAFailedRead();
	void testGetRelatedTenants_PartialReadIsNotRead();
	void testRemoveTenantRelationship_RepeatedRemovalKeepsTheEnd();
	void testUpdateProposal_RefusesAnEndedRelationship();
	void testValidateRelationship_RefusesAnEndedRelationship();

	// The period of relationships of two organizations
	void testGetUninterruptedStart_FollowsTheRelationshipsThatOverlap();
	void testGetUninterruptedStart_StopsAtAGap();
	void testGetUninterruptedStart_UnknownOrNoActiveRelationship();
	void testGetRelatedSince_KeepsTheStartWhenARelationshipIsReplaced();
	void testGetRelatedSince_StartsAnewAfterAGap();

	// Removal events
	void testRemoveTenantRelationship_PublishesTheRemoval();
	void testDeleteProposal_PublishesTheRemoval();
	void testRemoveConnection_PublishesOnlyTheConnectionRemoval();

	// Cross-organization grants
	void testGetGrantsBySourceTenant_LoadsOnlyTheTenants();

	// Personal access tokens
	void testValidateToken_LoadsOnlyTheToken();
	void testGetTokenIds_LoadsOnlyTheUsers();

	// Verification codes
	void testVerifyUser_RightCodeAfterSomeWrongOnes();
	void testVerifyUser_TooManyWrongCodesEndTheVerification();
	void testVerifyUser_NewCodeResetsTheAttempts();
	void testRemoveVerificationData_EndsTheVerification();
	void testVerifyUser_CodeThatCannotBeRemovedStaysBlocked();
	void testGenerateVerificationData_FailsWhileTheOldCodeStays();
};


