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
	void testContracts_ProductIsFixedByActor();
	void testContracts_ForeignProductCommandsAreUnavailable();
	void testContracts_SelectionAndCacheAreProductScoped();
	void testContracts_PermissionsMustBelongToProduct();
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
	void testOwnedOrCreated_ReportsAFailedRead();
	void testRemoveMembership_RefusedWhenItCannotBeRead();
	void testRemoveVerificationData_UnreadableCodeIsRemovedAnyway();
	void testConsumeVerification_OnlyOneConcurrentCallerSucceeds();
	void testRevokeInvitationsSentBy_ReportsAnUnreadInvitation();
	void testShareDocument_RefusesUnreadRelationships();
	void testShareDocument_RefusesAnUnreadShareOfTheDocument();
	void testShareDocument_RefusesWhenTheEarlierShareStays();
	void testGetShareOwnerTenantIds_UnreadShareTrustsNoPartner();

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
	void testShareDocument_RefusesAPartnerWithoutAContractInForce();
	void testRemoveUnrelatedShares_ReadsTheRelationshipsOncePerCachePeriod();

	// Connections
	void testGetConnectionIds_LoadsOnlyTheTenants();
	void testConnectionLookups_NonUuidIds_MatchNothing();
	void testRemoveConnection_PublishesTheConnectionRemoval();

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


