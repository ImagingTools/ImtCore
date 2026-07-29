// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0

/**
 * TenantManagementApiClient
 *
 * Abstract contract (interface) for tenant management operations:
 *   - tenant membership / invitations / ownership
 *   - roles / groups / users CRUD
 *   - per-type document services (open / save / edit) and editor view registration
 *
 * Pages depend on this contract — not on any concrete transport (e.g. GQL/SDL).
 *
 * A concrete implementation (e.g. GqlBasedTenantManagementApiClient) provides the
 * actual transport and the wired-up document services / editor components.
 *
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	id: root

	property string tenantId: ""

	// --- Document services for the per-type document workflow ---
	// Concrete implementations expose DocumentServiceBase-compatible managers and
	// the typeId / Component pieces that pages need to drive the
	// SingleDocumentWorkspaceShellView. Pages MUST NOT instantiate any concrete
	// service or registrar themselves.
	property var roleDocumentManager: null
	property var groupDocumentManager: null
	property var userDocumentManager: null

	property string roleObjectTypeId: "Role"
	property string groupObjectTypeId: "Group"
	property string userObjectTypeId: "User"

	// --- Invitations ---
	signal invitationCreated()
	signal invitationRevoked(string invitationId)
	signal invitationResent(string invitationId)

	// --- Ownership / membership ---
	signal ownershipTransferred()
	signal memberRemoved(string userId)

	// --- Roles ---
	signal roleCreated()
	signal roleRemoved(string roleId)
	signal roleUpdated(string roleId)
	signal roleDataReceived(var data)

	// --- Groups ---
	signal groupCreated()
	signal groupRemoved(string groupId)
	signal groupUpdated(string groupId)
	signal groupDataReceived(var data)

	// --- Users (tenant members) ---
	signal userCreated()
	signal userRemoved(string userId)
	signal userUpdated(string userId)
	signal userDataReceived(var data)

	// --- Cross-org grants ---
	signal crossOrgGrantsRemoved()

	// --- Cooperation contracts ---
	signal contractCreated(string contractId)
	signal contractStatusUpdated(string contractId)
	signal contractTerminated(string contractId)
	signal contractsReceived(var contracts)

	// --- Connection Code ---
	signal connectionCodeReceived(string code, bool allowByCode)
	signal connectionCodeRegenerated(string newCode)
	signal allowConnectionsByCodeChanged(bool allow)

	// --- Connection Requests ---
	signal connectionRequestCreated(string requestId)
	signal connectionRequestError(string errorMessage)
	signal connectionRequestApproved(string connectionId)
	signal connectionRequestRejected()
	signal connectionRequestCanceled()
	signal connectionRequestsReceived()

	// --- Connections ---
	signal connectionsReceived()
	signal connectionRemoved(string connectionId)

	// --- Tenant Relationships ---
	signal tenantRelationshipRemoved()
	signal tenantRelationshipsReceived()

	// --- Relationship Proposals ---
	signal relationshipProposalCreated()
	signal relationshipProposalApproved(string relationshipId)
	signal relationshipProposalRejected()
	signal relationshipProposalCanceled()
	signal relationshipProposalsReceived()

	// --- Cross-tenant messages (phase 2) ---
	signal crossTenantMessageSent(string messageId)
	signal crossTenantMessageStatusUpdated(string messageId)
	signal crossTenantMessagesReceived(var messages)

	// --- Order requests (phase 3) ---
	signal orderRequestConfirmed(string orderRequestId)
	signal orderRequestRejected(string orderRequestId)
	signal orderRequestStatusUpdated(string orderRequestId)
	signal orderRequestsReceived(var orderRequests)

	// --- Generic error ---
	signal requestFailed(string message)

	// --- Real-time membership subscription notifications ---
	signal subscriptionInvitationReceived(string tenantId, string tenantName, string role)
	signal subscriptionInvitationAccepted(string tenantId, string membershipId)
	signal subscriptionInvitationRejected(string tenantId, string membershipId)
	signal subscriptionOwnershipTransferred(string tenantId)
	signal subscriptionMembershipRoleChanged(string tenantId, string userId, string role)
	signal subscriptionMembershipRemoved(string tenantId, string userId)

	// --- Real-time connection notifications ---
	signal subscriptionConnectionNotification(var notification)

	// --- Stub methods (overridden by concrete implementations) ---
	function createInvitation(tenantId, userId) {}
	function revokeInvitation(invitationId) {}
	function resendInvitation(invitationId) {}

	function transferOwnership(tenantId, newOwnerId) {}
	function removeMember(tenantId, userId) {}

	function createRoleData() {}
	function insertRole(roleId, roleData) {}
	function removeRole(roleId) {}
	function setRoleData(roleId, roleData) {}
	function getRoleData(roleId) {}

	function createGroupData() {}
	function insertGroup(groupId, groupData) {}
	function removeGroup(groupId) {}
	function setGroupData(groupId, groupData) {}
	function getGroupData(groupId) {}

	function createUserData() {}
	function insertUser(userId, userData) {}
	function removeUser(userId) {}
	function setUserData(userId, userData) {}
	function getUserData(userId) {}

	// --- Permissions ---
	property var permissionsProvider: null
	function setRolePermissionsTenantId(tenantId) {}

	// All product permissions (unfiltered) — for TenantPermissionsPage
	property var allPermissions: []
	signal allPermissionsReceived()
	function fetchAllPermissions() {}

	// Tenant-scoped permissions (filtered by tenant's tenantPermissions) — for RoleView
	property var tenantPermissions: []
	signal tenantPermissionsReceived()
	function fetchTenantPermissions(tenantId) {}

	// --- Cross-org grants ---
	property var crossOrgGrantDocumentManager: null
	property var crossOrgGrantsListDataProvider: null
	property var tenantsListDataProvider: null
	property var tenantRelationshipsListDataProvider: null
	function revokeCrossOrgGrant(grantId) {}
	function removeCrossOrgGrants(grantIds) {}

	// --- Cooperation contracts ---
	property var contractsModel: null
	function fetchContracts(tenantId) {}
	function createContract(relationshipId, sourceTenantId, targetTenantId, scope, validFrom, validUntil, description, terms) {}
	function updateContractStatus(contractId, status) {}
	function terminateContract(contractId) {}

	// --- Connection Code ---
	function getConnectionCode(tenantId) {}
	function regenerateConnectionCode(tenantId) {}
	function setAllowConnectionsByCode(tenantId, allow) {}

	// --- Connection Requests ---
	property var connectionRequestsModel: null
	function fetchConnectionRequests(tenantId) {}
	function createConnectionRequest(sourceTenantId, connectionCode, message) {}
	function approveConnectionRequest(requestId, tenantId) {}
	function rejectConnectionRequest(requestId, tenantId) {}
	function cancelConnectionRequest(requestId, tenantId) {}

	// --- Connections ---
	property var connectionsModel: null
	function fetchConnections(tenantId) {}
	function removeConnection(connectionId, tenantId) {}

	// --- Tenant Relationships ---
	property var tenantRelationshipsModel: null
	function fetchTenantRelationships(tenantId) {}
	function removeTenantRelationship(tenantId, relationshipId) {}

	// --- Relationship Proposals ---
	property var relationshipProposalsModel: null
	function fetchRelationshipProposals(tenantId) {}
	function createRelationshipProposal(connectionId, initiatorTenantId, proposalType, proposedSourceRole, proposedTargetRole, proposedScope, proposedDescription, message) {}
	function approveRelationshipProposal(proposalId, tenantId) {}
	function rejectRelationshipProposal(proposalId, tenantId) {}
	function cancelRelationshipProposal(proposalId, tenantId) {}

	// --- Cross-tenant messages (phase 2) ---
	property var crossTenantMessagesModel: null
	function fetchCrossTenantMessages(tenantId, direction) {}
	function sendCrossTenantMessage(sourceTenantId, targetTenantId, relationshipId, messageType, payload, sourceObjectId, customType, expiresAt) {}
	function updateCrossTenantMessageStatus(messageId, status, errorMessage) {}

	// --- Order requests (phase 3) ---
	property var orderRequestsModel: null
	function fetchOrderRequests(tenantId) {}
	function confirmOrderRequest(orderRequestId, note) {}
	function rejectOrderRequest(orderRequestId, reason) {}
	function updateOrderRequestStatus(orderRequestId, status, note) {}
}
