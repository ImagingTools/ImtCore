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
	signal memberRoleChanged(string userId, string role)
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
	signal crossOrgGrantCreated(string grantId)
	signal crossOrgGrantRevoked(string grantId)
	signal crossOrgGrantsReceived(var grants)

	// --- Cooperation contracts ---
	signal contractCreated(string contractId)
	signal contractStatusUpdated(string contractId)
	signal contractTerminated(string contractId)
	signal contractsReceived(var contracts)

	// --- Tenant relationships (asymmetric) ---
	signal tenantRelationshipAdded(string relationshipId)
	signal tenantRelationshipRemoved(string relationshipId)
	signal tenantRelationshipsReceived(var relationships)

	// --- Tenant connection requests (discovery) ---
	signal connectionRequestCreated(string requestId)
	signal connectCodeCreated(string requestId, string connectCode)
	signal connectionRequestAccepted(string requestId)
	signal connectionRequestRejected(string requestId)
	signal connectionRequestRevoked(string requestId)
	signal connectionRequestsReceived(var requests)

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
	signal subscriptionInvitationReceived(var notification)
	signal subscriptionInvitationAccepted(var notification)
	signal subscriptionInvitationRejected(var notification)
	signal subscriptionOwnershipTransferred(var notification)

	// --- Real-time cross-tenant message subscription notifications ---
	signal subscriptionCrossTenantMessageReceived(var notification)
	signal subscriptionCrossTenantMessageStatusChanged(var notification)

	// --- Stub methods (overridden by concrete implementations) ---
	function createInvitation(tenantId, userId, role) {}
	function revokeInvitation(invitationId) {}
	function resendInvitation(invitationId) {}

	function transferOwnership(tenantId, newOwnerId) {}
	function setMemberRole(tenantId, userId, role) {}
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
	property var permissionsModel: null
	function fetchPermissions() {}

	// --- Cross-org grants ---
	// Model holding the cross-org grants for the current tenant (see
	// crossOrgGrantsReceived for the raw payload).
	property var crossOrgGrantsModel: null
	function fetchCrossOrgGrants(tenantId) {}
	function createCrossOrgGrant(sourceTenantId, targetTenantId, relationshipId, accessLevel, resourceScope, targetTeamId, description, expiresAt) {}
	function revokeCrossOrgGrant(grantId) {}

	// --- Cooperation contracts ---
	// Model holding the contracts for the current tenant (see
	// contractsReceived for the raw payload).
	property var contractsModel: null
	function fetchContracts(tenantId) {}
	function createContract(relationshipId, sourceTenantId, targetTenantId, scope, validFrom, validUntil, description, terms) {}
	function updateContractStatus(contractId, status) {}
	function terminateContract(contractId) {}

	// --- Tenant relationships (asymmetric) ---
	// Model holding the relationships for the current tenant (see
	// tenantRelationshipsReceived for the raw payload).
	property var tenantRelationshipsModel: null
	function fetchTenantRelationships(tenantId) {}
	function addTenantRelationship(sourceTenantId, targetTenantId, sourceRole, targetRole, scope, validFrom, validUntil, description) {}
	function removeTenantRelationship(tenantId, relationshipId) {}

	// --- Tenant connection requests (discovery) ---
	// Model holding the connection requests for the current tenant (see
	// connectionRequestsReceived for the raw payload).
	property var connectionRequestsModel: null
	function fetchConnectionRequests(tenantId) {}
	function createConnectionRequest(sourceTenantId, targetIdentifier, proposedSourceRole, proposedTargetRole, message, expiresAt) {}
	function createConnectCode(sourceTenantId, proposedSourceRole, proposedTargetRole, message, expiresAt) {}
	function acceptConnectionRequest(requestId, acceptingTenantId) {}
	function acceptConnectCode(connectCode, acceptingTenantId) {}
	function rejectConnectionRequest(requestId) {}
	function revokeConnectionRequest(requestId) {}

	// --- Cross-tenant messages (phase 2) ---
	// Model holding the cross-tenant messages for the current tenant (see
	// crossTenantMessagesReceived for the raw payload).
	property var crossTenantMessagesModel: null
	function fetchCrossTenantMessages(tenantId, direction) {}
	function sendCrossTenantMessage(sourceTenantId, targetTenantId, relationshipId, messageType, payload, sourceObjectId, customType, expiresAt) {}
	function updateCrossTenantMessageStatus(messageId, status, errorMessage) {}

	// --- Order requests (phase 3) ---
	// Model holding the order requests for the current tenant (see
	// orderRequestsReceived for the raw payload).
	property var orderRequestsModel: null
	function fetchOrderRequests(tenantId) {}
	function confirmOrderRequest(orderRequestId, note) {}
	function rejectOrderRequest(orderRequestId, reason) {}
	function updateOrderRequestStatus(orderRequestId, status, note) {}
}
