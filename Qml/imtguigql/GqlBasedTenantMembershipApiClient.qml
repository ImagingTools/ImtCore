// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthTenantMembershipsSdl 1.0
import imtbaseImtCollectionSdl 1.0
import imtauthRolesSdl 1.0
import imtauthGroupsSdl 1.0
import imtauthUsersSdl 1.0

/**
 * GqlBasedTenantMembershipApiClient
 *
 * GQL/SDL implementation of the abstract TenantMembershipApiClient contract.
 * This is the ONLY place that imports the membership / roles / groups / users SDL modules
 * and that owns GqlSdlRequestSender instances for these operations.
 *
 * Pages depend only on the abstract contract; the orchestrator (TenantEditor)
 * injects this concrete client.
 */
QtObject {
	id: root

	// =========================================================================
	// Configuration
	// =========================================================================

	property string productId: ""

	// =========================================================================
	// Abstract contract (must mirror TenantMembershipApiClient.qml)
	// =========================================================================

	signal invitationCreated()
	signal invitationRevoked(string invitationId)
	signal invitationResent(string invitationId)

	signal ownershipTransferred()
	signal memberRoleChanged(string userId, string role)
	signal memberRemoved(string userId)

	signal roleCreated()
	signal roleRemoved(string roleId)
	signal roleUpdated(string roleId)
	signal roleDataReceived(var data)

	signal groupCreated()
	signal groupRemoved(string groupId)
	signal groupUpdated(string groupId)
	signal groupDataReceived(var data)

	signal userCreated()
	signal userRemoved(string userId)
	signal userUpdated(string userId)
	signal userDataReceived(var data)

	signal requestFailed(string message)

	// =========================================================================
	// GQL implementation
	// =========================================================================

	// --- Invitations ---
	property CreateTenantInvitationInput __createInvitationInput: CreateTenantInvitationInput {}
	property GqlSdlRequestSender __createInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_createTenantInvitation

		sdlObjectComp: Component {
			CreateTenantInvitationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.invitationCreated()
					}
				}
			}
		}
	}

	property RevokeTenantInvitationInput __revokeInvitationInput: RevokeTenantInvitationInput {}
	property GqlSdlRequestSender __revokeInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_revokeTenantInvitation
	}

	property ResendTenantInvitationInput __resendInvitationInput: ResendTenantInvitationInput {}
	property GqlSdlRequestSender __resendInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_resendTenantInvitation
	}

	property TransferTenantOwnershipInput __transferOwnershipInput: TransferTenantOwnershipInput {}
	property GqlSdlRequestSender __transferOwnershipSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_transferTenantOwnership

		sdlObjectComp: Component {
			TransferTenantOwnershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.ownershipTransferred()
					}
				}
			}
		}
	}

	// --- Roles (Roles.sdl: RoleItem / RoleAdd / RoleUpdate) ---
	property RoleDataInput __roleAddInput: RoleDataInput {}
	property GqlSdlRequestSender __roleAddSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthRolesSdlCommandIds.s_roleAdd

		sdlObjectComp: Component {
			AddedNotificationPayload {
				onFinished: { root.roleCreated() }
			}
		}
	}

	property RemoveElementsInput __removeRoleInput: RemoveElementsInput {}
	property string __pendingRemoveRoleId: ""
	property GqlSdlRequestSender __removeRoleSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements

		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: { root.roleRemoved(root.__pendingRemoveRoleId) }
			}
		}
	}

	property RoleDataInput __roleUpdateInput: RoleDataInput {}
	property string __pendingSetRoleId: ""
	property GqlSdlRequestSender __roleUpdateSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthRolesSdlCommandIds.s_roleUpdate

		sdlObjectComp: Component {
			UpdatedNotificationPayload {
				onFinished: { root.roleUpdated(root.__pendingSetRoleId) }
			}
		}
	}

	property RoleItemInput __roleItemInput: RoleItemInput {}
	property GqlSdlRequestSender __roleItemSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthRolesSdlCommandIds.s_roleItem

		sdlObjectComp: Component {
			RoleData {
				onFinished: { root.__handleRoleDataReceived(this) }
			}
		}
	}

	// --- Groups (Groups.sdl: GroupItem / GroupAdd / GroupUpdate) ---
	property GroupDataInput __groupAddInput: GroupDataInput {}
	property GqlSdlRequestSender __groupAddSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthGroupsSdlCommandIds.s_groupAdd

		sdlObjectComp: Component {
			AddedNotificationPayload {
				onFinished: { root.groupCreated() }
			}
		}
	}

	property RemoveElementsInput __removeGroupInput: RemoveElementsInput {}
	property string __pendingRemoveGroupId: ""
	property GqlSdlRequestSender __removeGroupSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements

		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: { root.groupRemoved(root.__pendingRemoveGroupId) }
			}
		}
	}

	property GroupDataInput __groupUpdateInput: GroupDataInput {}
	property string __pendingSetGroupId: ""
	property GqlSdlRequestSender __groupUpdateSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthGroupsSdlCommandIds.s_groupUpdate

		sdlObjectComp: Component {
			UpdatedNotificationPayload {
				onFinished: { root.groupUpdated(root.__pendingSetGroupId) }
			}
		}
	}

	property GroupItemInput __groupItemInput: GroupItemInput {}
	property GqlSdlRequestSender __groupItemSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthGroupsSdlCommandIds.s_groupItem

		sdlObjectComp: Component {
			GroupData {
				onFinished: { root.__handleGroupDataReceived(this) }
			}
		}
	}

	// --- Users (Users.sdl: UserItem / UserAdd / UserUpdate) ---
	property UserDataInput __userAddInput: UserDataInput {}
	property GqlSdlRequestSender __userAddSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthUsersSdlCommandIds.s_userAdd

		sdlObjectComp: Component {
			AddedNotificationPayload {
				onFinished: { root.userCreated() }
			}
		}
	}

	property RemoveElementsInput __removeUserInput: RemoveElementsInput {}
	property string __pendingRemoveUserId: ""
	property GqlSdlRequestSender __removeUserSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements

		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: { root.userRemoved(root.__pendingRemoveUserId) }
			}
		}
	}

	property UserDataInput __userUpdateInput: UserDataInput {}
	property string __pendingSetUserId: ""
	property GqlSdlRequestSender __userUpdateSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthUsersSdlCommandIds.s_userUpdate

		sdlObjectComp: Component {
			UpdatedNotificationPayload {
				onFinished: { root.userUpdated(root.__pendingSetUserId) }
			}
		}
	}

	property UserItemInput __userItemInput: UserItemInput {}
	property GqlSdlRequestSender __userItemSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthUsersSdlCommandIds.s_userItem

		sdlObjectComp: Component {
			UserData {
				onFinished: { root.__handleUserDataReceived(this) }
			}
		}
	}

	// =========================================================================
	// Public methods (override the abstract stubs)
	// =========================================================================

	function createInvitation(tenantId, userId, role) {
		root.__createInvitationInput.m_tenantId = tenantId || ""
		root.__createInvitationInput.m_userId = userId || ""
		root.__createInvitationInput.m_role = role || "Member"
		root.__createInvitationSender.send(root.__createInvitationInput)
	}

	function revokeInvitation(invitationId) {
		root.__revokeInvitationInput.m_invitationId = invitationId || ""
		root.__revokeInvitationSender.send(root.__revokeInvitationInput)
		root.invitationRevoked(invitationId)
	}

	function resendInvitation(invitationId) {
		root.__resendInvitationInput.m_invitationId = invitationId || ""
		root.__resendInvitationSender.send(root.__resendInvitationInput)
		root.invitationResent(invitationId)
	}

	function transferOwnership(tenantId, newOwnerId) {
		root.__transferOwnershipInput.m_tenantId = tenantId || ""
		root.__transferOwnershipInput.m_newOwnerId = newOwnerId || ""
		root.__transferOwnershipSender.send(root.__transferOwnershipInput)
	}

	function setMemberRole(tenantId, userId, role) {
		// First we need to find the membership to get its ID
		root.__pendingChangeRoleUserId = userId
		root.__pendingChangeRoleNewRole = role
		root.__findMembershipForRoleInput.m_userId = userId
		root.__findMembershipForRoleInput.m_tenantId = tenantId
		root.__findMembershipForRoleSender.send(root.__findMembershipForRoleInput)
	}

	function removeMember(tenantId, userId) {
		// First find the membership to get its ID, then remove it
		root.__pendingRemoveMemberUserId = userId
		root.__findMembershipForRemoveInput.m_userId = userId
		root.__findMembershipForRemoveInput.m_tenantId = tenantId
		root.__findMembershipForRemoveSender.send(root.__findMembershipForRemoveInput)
	}

	function insertRole(roleId, roleData) {
		if (!roleData){
			return
		}

		root.__roleAddInput.m_id = roleId
		root.__roleAddInput.m_typeId = "Role"
		root.__roleAddInput.m_productId = root.productId
		root.__roleAddInput.m_name = roleData.m_name
		root.__roleAddInput.m_description = roleData.m_description
		root.__roleAddInput.m_item = roleData

		root.__roleAddSender.send(root.__roleAddInput)
	}

	function removeRole(roleId) {
		root.__pendingRemoveRoleId = roleId || ""
		root.__removeRoleInput.m_collectionId = "Roles"
		root.__removeRoleInput.m_elementIds = [roleId]
		root.__removeRoleSender.send(root.__removeRoleInput)
	}

	function setRoleData(roleId, roleData) {
		if (!roleData){
			return
		}

		root.__pendingSetRoleId = roleId
		root.__roleUpdateInput.m_id = roleId || ""
		root.__roleUpdateInput.m_typeId = "Role"
		root.__roleUpdateInput.m_productId = root.productId
		root.__roleUpdateInput.m_name = roleData.m_name
		root.__roleUpdateInput.m_description = roleData.m_description || ""
		root.__roleUpdateInput.m_item = roleData

		root.__roleUpdateSender.send(root.__roleUpdateInput)
	}

	function getRoleData(roleId) {
		root.__roleItemInput.m_id = roleId || ""
		root.__roleItemInput.m_productId = root.productId
		root.__roleItemSender.send(root.__roleItemInput)
	}

	function insertGroup(name, description, groupData) {
		root.__groupAddInput.m_id = ""
		root.__groupAddInput.m_typeId = "Group"
		root.__groupAddInput.m_productId = root.productId
		root.__groupAddInput.m_name = name || ""
		root.__groupAddInput.m_description = description || ""
		if (groupData)
			root.__groupAddInput.m_item = groupData
		root.__groupAddSender.send(root.__groupAddInput)
	}

	function removeGroup(groupId) {
		root.__pendingRemoveGroupId = groupId || ""
		root.__removeGroupInput.m_collectionId = "Groups"
		root.__removeGroupInput.m_elementIds = [groupId]
		root.__removeGroupSender.send(root.__removeGroupInput)
	}

	function setGroupData(groupId, name, description, groupData) {
		root.__pendingSetGroupId = groupId || ""
		root.__groupUpdateInput.m_id = groupId || ""
		root.__groupUpdateInput.m_typeId = "Group"
		root.__groupUpdateInput.m_productId = root.productId
		root.__groupUpdateInput.m_name = name || ""
		root.__groupUpdateInput.m_description = description || ""
		if (groupData)
			root.__groupUpdateInput.m_item = groupData
		root.__groupUpdateSender.send(root.__groupUpdateInput)
	}

	function getGroupData(groupId) {
		root.__groupItemInput.m_id = groupId || ""
		root.__groupItemInput.m_productId = root.productId
		root.__groupItemSender.send(root.__groupItemInput)
	}

	function insertUser(name, description, userData) {
		root.__userAddInput.m_id = ""
		root.__userAddInput.m_typeId = "User"
		root.__userAddInput.m_productId = root.productId
		root.__userAddInput.m_name = name || ""
		root.__userAddInput.m_description = description || ""
		if (userData)
			root.__userAddInput.m_item = userData
		root.__userAddSender.send(root.__userAddInput)
	}

	function removeUser(userId) {
		root.__pendingRemoveUserId = userId || ""
		root.__removeUserInput.m_collectionId = "Users"
		root.__removeUserInput.m_elementIds = [userId]
		root.__removeUserSender.send(root.__removeUserInput)
	}

	function setUserData(userId, name, description, userData) {
		root.__pendingSetUserId = userId || ""
		root.__userUpdateInput.m_id = userId || ""
		root.__userUpdateInput.m_typeId = "User"
		root.__userUpdateInput.m_productId = root.productId
		root.__userUpdateInput.m_name = name || ""
		root.__userUpdateInput.m_description = description || ""
		if (userData)
			root.__userUpdateInput.m_item = userData
		root.__userUpdateSender.send(root.__userUpdateInput)
	}

	function getUserData(userId) {
		root.__userItemInput.m_id = userId || ""
		root.__userItemInput.m_productId = root.productId
		root.__userItemSender.send(root.__userItemInput)
	}

	// --- Internal parse helpers ---

	// --- Membership operations (FindMembership → RemoveMembership / UpdateMembershipRole) ---
	property string __pendingRemoveMemberUserId: ""
	property string __pendingChangeRoleUserId: ""
	property string __pendingChangeRoleNewRole: ""

	property FindMembershipInput __findMembershipForRemoveInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForRemoveSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership

		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else if (m_membership && m_membership.m_id) {
						root.__removeMembershipInput.m_membershipId = m_membership.m_id
						root.__removeMembershipSender.send(root.__removeMembershipInput)
					}
				}
			}
		}
	}

	property FindMembershipInput __findMembershipForRoleInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForRoleSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership

		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else if (m_membership && m_membership.m_id) {
						root.__updateMembershipRoleInput.m_membershipId = m_membership.m_id
						root.__updateMembershipRoleInput.m_role = root.__pendingChangeRoleNewRole
						root.__updateMembershipRoleSender.send(root.__updateMembershipRoleInput)
					}
				}
			}
		}
	}

	property RemoveMembershipInput __removeMembershipInput: RemoveMembershipInput {}
	property GqlSdlRequestSender __removeMembershipSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_removeMembership

		sdlObjectComp: Component {
			RemoveMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.memberRemoved(root.__pendingRemoveMemberUserId)
					}
				}
			}
		}
	}

	property UpdateMembershipRoleInput __updateMembershipRoleInput: UpdateMembershipRoleInput {}
	property GqlSdlRequestSender __updateMembershipRoleSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_updateMembershipRole

		sdlObjectComp: Component {
			UpdateMembershipRolePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.memberRoleChanged(root.__pendingChangeRoleUserId, root.__pendingChangeRoleNewRole)
					}
				}
			}
		}
	}

	function __handleRoleDataReceived(roleData) {
		if (!roleData)
			return
		var data = {
			name: roleData.m_name || "",
			description: roleData.m_description || "",
			roleId: roleData.m_roleId || "",
			productId: roleData.m_productId || "",
			parentRoles: roleData.m_parentRoles || [],
			permissions: roleData.m_permissions || "",
			isDefault: roleData.m_isDefault || false,
			isGuest: roleData.m_isGuest || false
		}
		root.roleDataReceived(data)
	}

	function __handleGroupDataReceived(groupData) {
		if (!groupData)
			return
		var data = {
			name: groupData.m_name || "",
			description: groupData.m_description || "",
			productId: groupData.m_productId || "",
			roles: groupData.m_roles || [],
			users: groupData.m_users || [],
			parentGroups: groupData.m_parentGroups || []
		}
		root.groupDataReceived(data)
	}

	function __handleUserDataReceived(userData) {
		if (!userData)
			return
		var data = {
			name: userData.m_name || "",
			description: userData.m_email || "",
			username: userData.m_username || "",
			email: userData.m_email || "",
			productId: userData.m_productId || "",
			groups: userData.m_groups || [],
			roles: userData.m_roles || [],
			permissions: userData.m_permissions || []
		}
		root.userDataReceived(data)
	}
}
