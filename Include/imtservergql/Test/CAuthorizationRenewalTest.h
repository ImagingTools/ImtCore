// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CAuthorizationRenewalTest: public QObject
{
	Q_OBJECT

private slots:
	void RoleChangeIsDetected();
	void GroupChangeIsDetected();
	void ParentGroupRoleChangeIsDetected();
	void AdministratorIsKept();
	void ProductOfGroupIsKept();
	void MissedInvalidationIsRead();
	void RoleAndGroupReadFailure_data();
	void RoleAndGroupReadFailure();
	void CyclesAreReadOnce();
	void ContextOutlivesSnapshot();
	void OrganizationPermissionsAreCompared();
	void TokenBoundariesAreCompared_data();
	void TokenBoundariesAreCompared();
	void ExpiredAccessToken_data();
	void ExpiredAccessToken();
	void SessionCheckCrossesNetwork_data();
	void SessionCheckCrossesNetwork();
	void UnavailableIsNotDenied_data();
	void UnavailableIsNotDenied();
	void OrganizationReadFailure_data();
	void OrganizationReadFailure();
	void UnreadOrganizationIsNotCached();
	void ChangeDuringCheckIsStale();
	void StaleReadDoesNotFillCache();
	void GrantingAuthorityOfRequest_data();
	void GrantingAuthorityOfRequest();
	void GrantedPermissionsAreChecked_data();
	void GrantedPermissionsAreChecked();
	void GrantingAuthorityIsCopied_data();
	void GrantingAuthorityIsCopied();
	void GrantingAuthorityChangeIsDetected_data();
	void GrantingAuthorityChangeIsDetected();
	void PersonalTokenThroughServer_data();
	void PersonalTokenThroughServer();
	void PersonalTokenWorksInItsProductOnly();
	void ComputedUserFollowsOrganization_data();
	void ComputedUserFollowsOrganization();
	void InvisibleAssignmentsGrantNothing();
	void SessionCheckAfterFailedRead_data();
	void SessionCheckAfterFailedRead();
	void SessionAuthorizationConfirmsExpiredToken();
	void SessionAuthorizationRefusals_data();
	void SessionAuthorizationRefusals();
	void SessionAuthorizationOutcomes_data();
	void SessionAuthorizationOutcomes();
	void SessionAuthorizationKeepsOrganizationRules_data();
	void SessionAuthorizationKeepsOrganizationRules();
	void SessionAuthorizationRejectsIncompleteReply_data();
	void SessionAuthorizationRejectsIncompleteReply();
	void SessionAuthorizationContextIsDetached();
	void SessionAuthorizationIsKeptForRequests();
	void SessionOfAnotherProductIsRefused();
	void SessionAuthorizationSyncTransportFailure_data();
	void SessionAuthorizationSyncTransportFailure();
	void SessionAuthorizationHttpTransportFailure_data();
	void SessionAuthorizationHttpTransportFailure();
	void TenantLookupReportsCompleteness_data();
	void TenantLookupReportsCompleteness();
	void PersonalTokenKeepsItsRoute();
	void AccessWithinTheRegistration_data();
	void AccessWithinTheRegistration();
	void PersonalTokenRemoteUserRead_data();
	void PersonalTokenRemoteUserRead();
	void RemoteCollectionReadStatus_data();
	void RemoteCollectionReadStatus();
	void SnapshotAccessRefusal_data();
	void SnapshotAccessRefusal();
};
