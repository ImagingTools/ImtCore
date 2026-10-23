// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtauthgui 1.0

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
	// Part of the organization permission tree the editor works in; scopes the
	// checks below the same way the GQL senders scope the requests they send.
	property string permissionPath: ""

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

	// Current user's organization-level permissions for this tenant.
	// Stored as a plain [ID] array in TenantData; exposed here so all canXxx
	// bindings react when the array is replaced by a fresh server response.
	readonly property var __currentUserOrgPerms: stateManager.tenantData
		? (stateManager.tenantData.m_currentUserOrganizationPermissions || [])
		: []

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

	// The server decides who edits the permission scope: only the creator, the owner reads it
	readonly property bool canEditTenantPermissions: stateManager.isNewTenant
		|| (stateManager.tenantData ? stateManager.tenantData.m_canEditTenantPermissions === true : false)

	readonly property bool isSubOrganization: stateManager.tenantData
		? (stateManager.tenantData.m_parentTenantId !== undefined && stateManager.tenantData.m_parentTenantId !== "")
		: false

	// Full-access users are owner or creator; members with specific org permissions
	// are controlled granularly via __currentUserOrgPerms, not through isAdmin.
	readonly property bool isAdmin: stateManager.isOwner || stateManager.isCreator

	readonly property bool canChangeOrganizationName: stateManager.hasPermission("/OrganizationManagement/Edit/ChangeName")
	readonly property bool canChangeOrganizationDescription: stateManager.hasPermission("/OrganizationManagement/Edit/ChangeDescription")
	readonly property bool canEditOrganization: stateManager.hasAnyPermission(["/OrganizationManagement/Edit/ChangeName", "/OrganizationManagement/Edit/ChangeDescription"])

	readonly property bool canViewOrganizationMembers: stateManager.hasPermission("/OrganizationManagement/Members/View")
	readonly property bool canInviteOrganizationMember: stateManager.hasPermission("/OrganizationManagement/Members/Edit/Invite")
	readonly property bool canExcludeOrganizationMember: stateManager.hasPermission("/OrganizationManagement/Members/Edit/Exclude")
	readonly property bool canChangeOrganizationMember: stateManager.hasPermission("/OrganizationManagement/Members/Edit/ChangeRole")
	readonly property bool canChangeOrganizationMemberRole: stateManager.hasPermission("/OrganizationManagement/Members/Edit/ChangeRole")
	readonly property bool canManageOrganizationMembers: stateManager.hasAnyPermission(["/OrganizationManagement/Members/Edit/Invite", "/OrganizationManagement/Members/Edit/Exclude", "/OrganizationManagement/Members/Edit/ChangeRole"])

	readonly property bool canViewOrganizationRoles: stateManager.hasPermission("/OrganizationManagement/Roles/View")
	readonly property bool canManageOrganizationRoles: stateManager.hasAnyPermission(["/OrganizationManagement/Roles/Edit/Change", "/OrganizationManagement/Roles/Edit/Remove", "/OrganizationManagement/Roles/Edit/Add"])

	readonly property bool canViewOrganizationGroups: stateManager.hasPermission("/OrganizationManagement/Groups/View")
	readonly property bool canManageOrganizationGroups: stateManager.hasAnyPermission(["/OrganizationManagement/Groups/Edit/Change", "/OrganizationManagement/Groups/Edit/Remove", "/OrganizationManagement/Groups/Edit/Add"])

	readonly property bool canViewOrganizationPermissions: stateManager.hasPermission("/OrganizationManagement/Permissions/View")
	readonly property bool canEditOrganizationMemberPermissions: stateManager.hasPermission("/OrganizationManagement/Permissions/Change")

	readonly property bool canViewOrganizationConnections: stateManager.hasPermission("/OrganizationManagement/Connections/View")
	readonly property bool canViewOrganizationConnectionCode: stateManager.hasPermission("/OrganizationManagement/Connections/ViewCode")
	readonly property bool canConnectOrganization: stateManager.hasPermission("/OrganizationManagement/Connections/Edit/Connect")
	readonly property bool canRemoveOrganizationConnection: stateManager.hasPermission("/OrganizationManagement/Connections/Edit/Remove")
	readonly property bool canManageOrganizationConnections: stateManager.hasAnyPermission(["/OrganizationManagement/Connections/Edit/Connect", "/OrganizationManagement/Connections/Edit/Remove"])

	// Computed key for page-list change detection.
	// Changes whenever any page-controlling permission flips (add or remove).
	// TenantEditor watches onPagesConfigKeyChanged to know when to rebuild the page list.
	readonly property string pagesConfigKey:
		(isNewTenant ? "N" : "") +
		(canViewOrganizationMembers ? "M" : "") +
		(canViewOrganizationRoles ? "R" : "") +
		(canViewOrganizationGroups ? "G" : "") +
		(canViewOrganizationPermissions ? "P" : "") +
		(isCreator ? "CR" : "") +
		(isOwner ? "O" : "") +
		(canViewOrganizationConnections ? "VC" : "") +
		(canViewOrganizationConnectionCode ? "VCC" : "") +
		(canConnectOrganization ? "CO" : "")

	readonly property bool canManageMembers: stateManager.canManageOrganizationMembers
		|| stateManager.canManageOrganizationRoles
		|| stateManager.canManageOrganizationGroups
		|| stateManager.canEditOrganization
		|| stateManager.canManageOrganizationConnections
	readonly property bool isReadOnly: !stateManager.isNewTenant && !stateManager.canManageMembers

	// --- Pure logic helpers ---

	// Client-side checks are UX hints only; server must enforce permissions on every operation.
	// Org-level permissions (the tree of COrganizationsPermissionsProviderComp) are
	// distinct from product-level role permissions and are served via
	// TenantData.currentUserOrganizationPermissions, NOT via PermissionsController.
	// They are held as full paths, e.g. "/OrganizationManagement/Members/View";
	// their ids repeat across the tree (View, Edit), so they are asked for by full path only.
	function hasPermission(permissionId) {
		if (!permissionId || permissionId === "")
			return false
		if (stateManager.isNewTenant || stateManager.isCreator || stateManager.isOwner || stateManager.isAdmin)
			return true

		return PermissionsController.hasPermission(stateManager.__currentUserOrgPerms, permissionId)
	}

	function hasAnyPermission(permissionIds) {
		if (!permissionIds || permissionIds.length === 0)
			return false
		for (var i = 0; i < permissionIds.length; i++) {
			if (stateManager.hasPermission(permissionIds[i]))
				return true
		}
		return false
	}

	function formatDateTime(value) {
		if (!value)
			return ""
		var date = new Date(value)
		if (isNaN(date.getTime()))
			return value
		return date.toLocaleDateString()
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

		var members = []
		if (serverMembers) {
			var count = serverMembers.count || 0
			for (var i = 0; i < count; i++) {
				var m = serverMembers.get(i).item
				if (m) {
					var userId = m.m_id || ""
					members.push({
						id: userId,
						name: m.m_name || userId,
						organizationPermissions: []
					})
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
				stateManager.apiClient.createInvitation(tenantId, selected.id)
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
