// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * TenantEditorStateManager
 *
 * Holds the local UI state (pending members / pending invitations / locally-modified
 * guard) and pure logic helpers for the TenantEditor and its sub-pages.
 *
 * It does NOT perform any transport — all server operations go through `apiClient`.
 *
 * MUST NOT import imtguigql or SDL modules.
 */
QtObject {
	id: stateManager

	// --- Wired by the orchestrator ---
	property var tenantData: null
	property var apiClient: null

	// --- Local UI state ---
	property var pendingMembers: []
	property var pendingInvitations: []

	// Guard: set when members are modified locally, prevents loadMembersFromModel
	// from overwriting the local change on the next document refresh.
	property bool __membersModifiedLocally: false

	// Last received role/group/user data payloads (populated via apiClient signals).
	property var receivedRoleData: null
	property var receivedGroupData: null
	property var receivedUserData: null

	// --- Computed role / permission flags ---
	readonly property bool isNewTenant: stateManager.tenantData
		? (!stateManager.tenantData.m_id || stateManager.tenantData.m_id === "")
		: true

	readonly property bool isCreator: stateManager.tenantData
			&& stateManager.tenantData.m_creatorId
			&& stateManager.tenantData.m_currentUserId
		? stateManager.tenantData.m_currentUserId === stateManager.tenantData.m_creatorId
		: false

	readonly property bool isOwner: stateManager.tenantData
			&& stateManager.tenantData.m_ownerId
			&& stateManager.tenantData.m_currentUserId
		? stateManager.tenantData.m_currentUserId === stateManager.tenantData.m_ownerId
		: false

	readonly property bool isAdmin: stateManager.getUserRole(
		stateManager.tenantData ? stateManager.tenantData.m_currentUserId : "") === "Admin"

	readonly property bool canManageMembers: stateManager.isCreator || stateManager.isOwner || stateManager.isAdmin
	readonly property bool isReadOnly: !stateManager.isNewTenant && !stateManager.canManageMembers

	// --- Pure logic helpers ---

	function getUserRole(userId) {
		if (!stateManager.tenantData || !userId)
			return "Member"
		var roles = stateManager.tenantData.m_memberRoles
		if (!roles) return "Member"
		var count = roles.count || roles.length || 0
		for (var i = 0; i < count; i++) {
			var entry = roles.get ? roles.get(i).item : roles[i]
			if (entry && entry.m_userId === userId)
				return entry.m_role || "Member"
		}
		return "Member"
	}

	function setUserRole(userId, role) {
		if (!stateManager.tenantData || !userId)
			return
		var roles = stateManager.tenantData.m_memberRoles
		if (!roles) return
		var count = roles.count || roles.length || 0
		for (var i = 0; i < count; i++) {
			var entry = roles.get ? roles.get(i).item : roles[i]
			if (entry && entry.m_userId === userId) {
				entry.m_role = role
				stateManager.pendingMembersChanged()
				return
			}
		}
	}

	function formatDateTime(value) {
		if (!value)
			return ""
		var date = new Date(value)
		if (isNaN(date.getTime()))
			return value
		return Qt.formatDateTime(date, Qt.DefaultLocaleShortDate)
	}

	function displayNameOrId(name, id) {
		return name ? name : id
	}

	function formatInvitationInfo(invitedByName, expiresAt) {
		var byPart = invitedByName ? qsTr("by %1").arg(invitedByName) : ""
		var expPart = expiresAt ? qsTr("expires %1").arg(stateManager.formatDateTime(expiresAt)) : ""
		return byPart && expPart ? byPart + " · " + expPart : byPart + expPart
	}

	function isInvitationExpired(expiresAt) {
		if (!expiresAt)
			return false
		var expDate = new Date(expiresAt)
		if (isNaN(expDate.getTime()))
			return false
		return expDate.getTime() < Date.now()
	}

	function loadMembersFromModel() {
		if (stateManager.__membersModifiedLocally) {
			stateManager.__membersModifiedLocally = false
			return
		}
		if (!stateManager.tenantData)
			return
		var serverMembers = stateManager.tenantData.m_members
		var serverRoles = stateManager.tenantData.m_memberRoles

		// Build userId → role lookup from memberRoles
		var roleMap = {}
		if (serverRoles) {
			var roleCount = serverRoles.count || 0
			for (var r = 0; r < roleCount; r++) {
				var re = serverRoles.get(r).item
				if (re)
					roleMap[re.m_userId || ""] = re.m_role || ""
			}
		}

		var members = []
		if (serverMembers) {
			var count = serverMembers.count || 0
			for (var i = 0; i < count; i++) {
				var m = serverMembers.get(i).item
				if (m) {
					var userId = m.m_id || ""
					members.push({ id: userId, name: m.m_name || userId, role: roleMap[userId] || "" })
				}
			}
		}
		stateManager.pendingMembers = members
	}

	function loadInvitationsFromModel() {
		if (!stateManager.tenantData)
			return
		var invitationsModel = stateManager.tenantData.m_pendingInvitations
		var invitations = []
		if (invitationsModel) {
			var count = invitationsModel.count || 0
			for (var i = 0; i < count; i++) {
				var invitation = invitationsModel.get(i).item
				if (invitation) {
					invitations.push({
						id: invitation.m_id || "",
						userId: invitation.m_userId || "",
						userName: invitation.m_userName || invitation.m_userId || "",
						status: invitation.m_status || "Pending",
						invitedByUserId: invitation.m_invitedByUserId || "",
						invitedByName: invitation.m_invitedByName || invitation.m_invitedByUserId || "",
						createdAt: invitation.m_createdAt || "",
						expiresAt: invitation.m_expiresAt || ""
					})
				}
			}
		}
		stateManager.pendingInvitations = invitations
	}

	function removeMemberById(userId) {
		var arr = stateManager.pendingMembers.slice()
		for (var i = 0; i < arr.length; i++) {
			if (arr[i].id === userId) {
				arr.splice(i, 1)
				break
			}
		}
		stateManager.pendingMembers = arr
		stateManager.__membersModifiedLocally = true
	}

	function removePendingInvitation(invitationId) {
		var invitations = []
		for (var i = 0; i < stateManager.pendingInvitations.length; i++) {
			if (stateManager.pendingInvitations[i].id !== invitationId)
				invitations.push(stateManager.pendingInvitations[i])
		}
		stateManager.pendingInvitations = invitations
	}

	function inviteSelectedUsers(selectedItems) {
		if (!stateManager.tenantData || !stateManager.apiClient)
			return
		var activeIds = {}
		for (var i = 0; i < stateManager.pendingMembers.length; i++)
			activeIds[stateManager.pendingMembers[i].id] = true
		for (var j = 0; j < stateManager.pendingInvitations.length; j++)
			activeIds[stateManager.pendingInvitations[j].userId] = true
		var tenantId = stateManager.tenantData.m_id || ""
		for (var k = 0; k < selectedItems.length; k++) {
			var selected = selectedItems[k]
			if (selected && selected.id && !activeIds[selected.id]) {
				stateManager.apiClient.createInvitation(tenantId, selected.id, "Member")
			}
		}
	}

	// --- Sync the local pendingMembers back into the tenantData model ---
	function syncMembersToModel() {
		if (!stateManager.tenantData)
			return
		if (!stateManager.tenantData.hasMembers())
			stateManager.tenantData.emplaceMembers()
		stateManager.tenantData.m_members.clear()
		var pendingMembers = stateManager.pendingMembers
		for (var i = 0; i < pendingMembers.length; i++) {
			var memberEntry = stateManager.tenantData.createMembersArrayElement()
			if (memberEntry) {
				memberEntry.m_id = pendingMembers[i].id
				memberEntry.m_name = pendingMembers[i].name || pendingMembers[i].id
				stateManager.tenantData.m_members.addElement(memberEntry)
			}
		}
	}
}
