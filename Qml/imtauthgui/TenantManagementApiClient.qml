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

	// --- Generic error ---
	signal requestFailed(string message)

	// --- Real-time membership subscription notifications ---
	signal subscriptionInvitationReceived(var notification)
	signal subscriptionInvitationAccepted(var notification)
	signal subscriptionInvitationRejected(var notification)
	signal subscriptionOwnershipTransferred(var notification)

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
}
