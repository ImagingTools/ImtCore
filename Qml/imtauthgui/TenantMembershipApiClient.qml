// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0

/**
 * TenantMembershipApiClient
 *
 * Abstract contract (interface) for tenant membership / roles / groups operations.
 * Pages depend on this contract — not on any concrete transport (e.g. GQL/SDL).
 *
 * A concrete implementation (e.g. GqlBasedTenantMembershipApiClient) provides the
 * actual transport and emits the signals declared here.
 *
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	id: root

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

	// --- Generic error ---
	signal requestFailed(string message)

	// --- Stub methods (overridden by concrete implementations) ---
	function createInvitation(tenantId, userId, role) {}
	function revokeInvitation(invitationId) {}
	function resendInvitation(invitationId) {}

	function transferOwnership(tenantId, newOwnerId) {}
	function setMemberRole(tenantId, userId, role) {}
	function removeMember(tenantId, userId) {}

	function insertRole(name, description) {}
	function removeRole(roleId) {}
	function setRoleData(roleId, name, description) {}
	function getRoleData(roleId) {}

	function insertGroup(name, description) {}
	function removeGroup(groupId) {}
	function setGroupData(groupId, name, description) {}
	function getGroupData(groupId) {}

	function insertUser(name, description) {}
	function removeUser(userId) {}
	function setUserData(userId, name, description) {}
	function getUserData(userId) {}
}
