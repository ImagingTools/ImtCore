// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtauthTenantMembershipsSdl 1.0
import imtauthTenantsSdl 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtauthRolesSdl 1.0
import imtauthGroupsSdl 1.0
import imtauthUsersSdl 1.0
import imtauthRoleCollectionDocumentServiceSdl 1.0
import imtauthGroupCollectionDocumentServiceSdl 1.0
import imtauthUserCollectionDocumentServiceSdl 1.0
import imtauthgui 1.0

/**
 * GqlBasedTenantManagementApiClient
 *
 * GQL/SDL implementation of the abstract TenantManagementApiClient contract.
 * This is the ONLY place that imports the membership / roles / groups / users SDL
 * modules and owns GqlSdlRequestSender / GqlBasedCollectionDocumentService /
 * GqlBasedCommandsController instances for these operations.
 *
 * Pages depend only on the abstract contract; the orchestrator (TenantEditor)
 * injects this concrete client.
 */
QtObject {
	id: root

	// =========================================================================
	// Configuration
	// =========================================================================

	property string productId: AuthorizationController.productId

	property Component __roleDataComp: Component { RoleData {} }
	property Component __groupDataComp: Component { GroupData {} }
	property Component __userDataComp: Component { UserData {} }

	// =========================================================================
	// Abstract contract (must mirror TenantManagementApiClient.qml)
	// =========================================================================

	// --- Document services (concrete instances expose abstract managers) ---
	property string roleObjectTypeId: "Role"
	property string groupObjectTypeId: "Group"
	property string userObjectTypeId: "User"

	readonly property var roleDocumentManager: __roleDocumentService
	readonly property var groupDocumentManager: __groupDocumentService
	readonly property var userDocumentManager: __userDocumentService

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

	signal crossOrgGrantCreated(string grantId)
	signal crossOrgGrantRevoked(string grantId)
	signal crossOrgGrantsReceived(var grants)

	signal tenantRelationshipAdded(string relationshipId)
	signal tenantRelationshipRemoved(string relationshipId)
	signal tenantRelationshipsReceived(var relationships)

	signal connectionRequestCreated(string requestId)
	signal connectCodeCreated(string requestId, string connectCode)
	signal connectionRequestAccepted(string requestId)
	signal connectionRequestRejected(string requestId)
	signal connectionRequestRevoked(string requestId)
	signal connectionRequestsReceived(var requests)

	signal requestFailed(string message)

	// --- Real-time membership subscription notifications ---
	signal subscriptionInvitationReceived(var notification)
	signal subscriptionInvitationAccepted(var notification)
	signal subscriptionInvitationRejected(var notification)
	signal subscriptionOwnershipTransferred(var notification)
	signal subscriptionMembershipRoleChanged(var notification)
	signal subscriptionMembershipRemoved(var notification)

	// --- Subscription client for membership notifications ---
	property SubscriptionClient __membershipSubscription: SubscriptionClient {
		gqlCommandId: "OnMembershipNotification"

		function getHeaders() { return {} }

		onMessageReceived: {
			if (!data) return
			var notificationType = ""
			if (data.containsKey("notificationType"))
				notificationType = data.getData("notificationType")

			var notification = {
				"membershipId": data.containsKey("membershipId") ? data.getData("membershipId") : "",
				"userId": data.containsKey("userId") ? data.getData("userId") : "",
				"tenantId": data.containsKey("tenantId") ? data.getData("tenantId") : "",
				"tenantName": data.containsKey("tenantName") ? data.getData("tenantName") : "",
				"role": data.containsKey("role") ? data.getData("role") : ""
			}

			if (notificationType === "InvitationReceived" || notificationType === 0) {
				var tName = notification.tenantName ? notification.tenantName : qsTr("a tenant")
				PopupManager.addInfoMessage(qsTr("You have been invited to join \"%1\"").arg(tName), true)
				AuthorizationController.tenantInvitationReceived(notification)
				root.subscriptionInvitationReceived(notification)
			} else if (notificationType === "InvitationAccepted" || notificationType === 1) {
				AuthorizationController.tenantInvitationAccepted(notification)
				root.subscriptionInvitationAccepted(notification)
			} else if (notificationType === "InvitationRejected" || notificationType === 2) {
				AuthorizationController.tenantInvitationRejected(notification)
				root.subscriptionInvitationRejected(notification)
			} else if (notificationType === "OwnershipTransferred" || notificationType === 3) {
				AuthorizationController.tenantOwnershipTransferred(notification)
				root.subscriptionOwnershipTransferred(notification)
			} else if (notificationType === "MembershipRoleChanged" || notificationType === 4) {
				AuthorizationController.tenantMembershipRoleChanged(notification)
				root.subscriptionMembershipRoleChanged(notification)
			} else if (notificationType === "MembershipRemoved" || notificationType === 5) {
				AuthorizationController.tenantMembershipRemoved(notification)
				root.subscriptionMembershipRemoved(notification)
			}
		}
	}

	// --- Real-time cross-tenant message subscription notifications ---
	signal subscriptionCrossTenantMessageReceived(var notification)
	signal subscriptionCrossTenantMessageStatusChanged(var notification)

	// --- Subscription client for cross-tenant message notifications ---
	property SubscriptionClient __crossTenantMessageSubscription: SubscriptionClient {
		gqlCommandId: "OnCrossTenantMessageNotification"

		function getHeaders() { return {} }

		onMessageReceived: {
			if (!data) return
			var notificationType = ""
			if (data.containsKey("notificationType"))
				notificationType = data.getData("notificationType")

			var notification = {
				"messageId": data.containsKey("messageId") ? data.getData("messageId") : "",
				"sourceTenantId": data.containsKey("sourceTenantId") ? data.getData("sourceTenantId") : "",
				"targetTenantId": data.containsKey("targetTenantId") ? data.getData("targetTenantId") : "",
				"relationshipId": data.containsKey("relationshipId") ? data.getData("relationshipId") : "",
				"messageType": data.containsKey("messageType") ? data.getData("messageType") : "",
				"status": data.containsKey("status") ? data.getData("status") : "",
				"tenantName": data.containsKey("tenantName") ? data.getData("tenantName") : ""
			}

			if (notificationType === "MessageReceived" || notificationType === 0) {
				var tName = notification.tenantName ? notification.tenantName : qsTr("another tenant")
				PopupManager.addInfoMessage(qsTr("New cross-tenant message from \"%1\"").arg(tName), true)
				root.subscriptionCrossTenantMessageReceived(notification)
			} else if (notificationType === "MessageStatusChanged" || notificationType === 1) {
				root.subscriptionCrossTenantMessageStatusChanged(notification)
			}
		}
	}

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
	property string __pendingAddedUserId: ""
	property GqlSdlRequestSender __userAddSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthUsersSdlCommandIds.s_userAdd

		sdlObjectComp: Component {
			AddedNotificationPayload {
				onFinished: {
					root.__pendingAddedUserId = m_id || ""
					if (root.productId !== "" && root.__pendingAddedUserId !== "") {
						root.__addMembershipInput.m_userId = root.__pendingAddedUserId
						root.__addMembershipInput.m_tenantId = root.productId
						root.__addMembershipInput.m_role = "Member"
						root.__addMembershipSender.send(root.__addMembershipInput)
					} else {
						root.userCreated()
					}
				}
			}
		}
	}

	property AddMembershipInput __addMembershipInput: AddMembershipInput {}
	property GqlSdlRequestSender __addMembershipSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_addMembership

		sdlObjectComp: Component {
			AddMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					}
					root.userCreated()
				}
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

	function createRoleData() {
		return root.__roleDataComp.createObject(root, {"m_id": UuidGenerator.generateUUID()})
	}

	function insertRole(roleId, roleData) {
		if (!roleData){
			return
		}

		roleData.m_productId = root.productId
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

		roleData.m_productId = root.productId
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

	function createGroupData() {
		return root.__groupDataComp.createObject(root, {"m_id": UuidGenerator.generateUUID()})
	}

	function insertGroup(groupId, groupData) {
		if (!groupData){
			return
		}

		groupData.m_productId = root.productId
		root.__groupAddInput.m_id = groupId
		root.__groupAddInput.m_typeId = "Group"
		root.__groupAddInput.m_productId = root.productId
		root.__groupAddInput.m_name = groupData.m_name
		root.__groupAddInput.m_description = groupData.m_description
		root.__groupAddInput.m_item = groupData

		root.__groupAddSender.send(root.__groupAddInput)
	}

	function removeGroup(groupId) {
		root.__pendingRemoveGroupId = groupId || ""
		root.__removeGroupInput.m_collectionId = "Groups"
		root.__removeGroupInput.m_elementIds = [groupId]
		root.__removeGroupSender.send(root.__removeGroupInput)
	}

	function setGroupData(groupId, groupData) {
		if (!groupData){
			return
		}

		groupData.m_productId = root.productId
		root.__pendingSetGroupId = groupId
		root.__groupUpdateInput.m_id = groupId || ""
		root.__groupUpdateInput.m_typeId = "Group"
		root.__groupUpdateInput.m_productId = root.productId
		root.__groupUpdateInput.m_name = groupData.m_name
		root.__groupUpdateInput.m_description = groupData.m_description || ""
		root.__groupUpdateInput.m_item = groupData

		root.__groupUpdateSender.send(root.__groupUpdateInput)
	}

	function getGroupData(groupId) {
		root.__groupItemInput.m_id = groupId || ""
		root.__groupItemInput.m_productId = root.productId
		root.__groupItemSender.send(root.__groupItemInput)
	}

	function createUserData() {
		return root.__userDataComp.createObject(root, {"m_id": UuidGenerator.generateUUID()})
	}

	function insertUser(userId, userData) {
		if (!userData){
			return
		}

		userData.m_productId = AuthorizationController.productId
		root.__userAddInput.m_id = userId
		root.__userAddInput.m_typeId = "User"
		root.__userAddInput.m_productId = root.productId
		root.__userAddInput.m_name = userData.m_name
		root.__userAddInput.m_description = userData.m_description
		root.__userAddInput.m_item = userData

		root.__userAddSender.send(root.__userAddInput)
	}

	function removeUser(userId) {
		root.__pendingRemoveUserId = userId || ""
		root.__removeUserInput.m_collectionId = "Users"
		root.__removeUserInput.m_elementIds = [userId]
		root.__removeUserSender.send(root.__removeUserInput)
	}

	function setUserData(userId, userData) {
		if (!userData){
			return
		}

		userData.m_productId = root.productId
		root.__pendingSetUserId = userId
		root.__userUpdateInput.m_id = userId || ""
		root.__userUpdateInput.m_typeId = "User"
		root.__userUpdateInput.m_productId = root.productId
		root.__userUpdateInput.m_name = userData.m_name
		root.__userUpdateInput.m_description = userData.m_description || ""
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

	// =========================================================================
	// Permissions (via GqlBasedPermissionsProvider)
	// =========================================================================

	property var permissionsModel: __permissionsProvider.permissionsModel

	property GqlBasedPermissionsProvider __permissionsProvider: GqlBasedPermissionsProvider {
		productId: root.productId
	}

	function fetchPermissions() {
		__permissionsProvider.productId = root.productId
		__permissionsProvider.updateModel()
	}

	// =========================================================================
	// Cross-org grants (Tenants.sdl: CreateCrossOrgGrant / RevokeCrossOrgGrant /
	// GetCrossOrgGrants)
	// =========================================================================

	property ListModel crossOrgGrantsModel: ListModel {}

	property CreateCrossOrgGrantInput __createCrossOrgGrantInput: CreateCrossOrgGrantInput {}
	property GqlSdlRequestSender __createCrossOrgGrantSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_createCrossOrgGrant

		sdlObjectComp: Component {
			CreateCrossOrgGrantPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.crossOrgGrantCreated(m_grantId || "")
					}
				}
			}
		}
	}

	property RevokeCrossOrgGrantInput __revokeCrossOrgGrantInput: RevokeCrossOrgGrantInput {}
	property string __pendingRevokeGrantId: ""
	property GqlSdlRequestSender __revokeCrossOrgGrantSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_revokeCrossOrgGrant

		sdlObjectComp: Component {
			RevokeCrossOrgGrantPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.crossOrgGrantRevoked(root.__pendingRevokeGrantId)
					}
				}
			}
		}
	}

	property GetCrossOrgGrantsInput __getCrossOrgGrantsInput: GetCrossOrgGrantsInput {}
	property GqlSdlRequestSender __getCrossOrgGrantsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getCrossOrgGrants

		sdlObjectComp: Component {
			GetCrossOrgGrantsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateCrossOrgGrantsModel(m_grants)
					}
				}
			}
		}
	}

	function __populateCrossOrgGrantsModel(grants) {
		root.crossOrgGrantsModel.clear()
		if (grants) {
			for (var i = 0; i < grants.length; ++i) {
				var grant = grants[i]
				if (!grant)
					continue
				root.crossOrgGrantsModel.append({
					"grantId": grant.m_id || "",
					"sourceTenantId": grant.m_sourceTenantId || "",
					"targetTenantId": grant.m_targetTenantId || "",
					"relationshipId": grant.m_relationshipId || "",
					"targetTeamId": grant.m_targetTeamId || "",
					"accessLevel": grant.m_accessLevel || CrossOrgAccessLevelEnum.s_none,
					"resourceScope": grant.m_resourceScope || "",
					"description": grant.m_description || "",
					"createdAt": grant.m_createdAt || "",
					"expiresAt": grant.m_expiresAt || "",
					"isActive": grant.m_isActive === undefined ? true : grant.m_isActive
				})
			}
		}
		root.crossOrgGrantsReceived(grants || [])
	}

	function fetchCrossOrgGrants(tenantId) {
		root.__getCrossOrgGrantsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getCrossOrgGrantsSender.send(root.__getCrossOrgGrantsInput)
	}

	function createCrossOrgGrant(sourceTenantId, targetTenantId, relationshipId, accessLevel, resourceScope, targetTeamId, description, expiresAt) {
		root.__createCrossOrgGrantInput.m_sourceTenantId = sourceTenantId || ""
		root.__createCrossOrgGrantInput.m_targetTenantId = targetTenantId || ""
		root.__createCrossOrgGrantInput.m_relationshipId = relationshipId || ""
		root.__createCrossOrgGrantInput.m_accessLevel = accessLevel || CrossOrgAccessLevelEnum.s_read
		root.__createCrossOrgGrantInput.m_resourceScope = resourceScope || ""
		root.__createCrossOrgGrantInput.m_targetTeamId = targetTeamId || ""
		root.__createCrossOrgGrantInput.m_description = description || ""
		root.__createCrossOrgGrantInput.m_expiresAt = expiresAt || ""
		root.__createCrossOrgGrantSender.send(root.__createCrossOrgGrantInput)
	}

	function revokeCrossOrgGrant(grantId) {
		root.__pendingRevokeGrantId = grantId || ""
		root.__revokeCrossOrgGrantInput.m_grantId = grantId || ""
		root.__revokeCrossOrgGrantSender.send(root.__revokeCrossOrgGrantInput)
	}

	// =========================================================================
	// Tenant relationships (Tenants.sdl: AddTenantRelationship /
	// RemoveTenantRelationship / GetTenantRelationships)
	// =========================================================================

	property ListModel tenantRelationshipsModel: ListModel {}

	property AddTenantRelationshipInput __addTenantRelationshipInput: AddTenantRelationshipInput {}
	property GqlSdlRequestSender __addTenantRelationshipSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_addTenantRelationship

		sdlObjectComp: Component {
			AddTenantRelationshipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.tenantRelationshipAdded(m_relationshipId || "")
					}
				}
			}
		}
	}

	property RemoveTenantRelationshipInput __removeTenantRelationshipInput: RemoveTenantRelationshipInput {}
	property string __pendingRemoveRelationshipId: ""
	property GqlSdlRequestSender __removeTenantRelationshipSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_removeTenantRelationship

		sdlObjectComp: Component {
			RemoveTenantRelationshipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.tenantRelationshipRemoved(root.__pendingRemoveRelationshipId)
					}
				}
			}
		}
	}

	property GetTenantRelationshipsInput __getTenantRelationshipsInput: GetTenantRelationshipsInput {}
	property GqlSdlRequestSender __getTenantRelationshipsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getTenantRelationships

		sdlObjectComp: Component {
			GetTenantRelationshipsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateTenantRelationshipsModel(m_relationships)
					}
				}
			}
		}
	}

	function __populateTenantRelationshipsModel(relationships) {
		root.tenantRelationshipsModel.clear()
		if (relationships) {
			for (var i = 0; i < relationships.length; ++i) {
				var rel = relationships[i]
				if (!rel)
					continue
				root.tenantRelationshipsModel.append({
					"relationshipId": rel.m_id || "",
					"sourceTenantId": rel.m_sourceTenantId || "",
					"targetTenantId": rel.m_targetTenantId || "",
					"role": rel.m_role || TenantRelationshipRoleEnum.s_partner,
					"sourceRole": rel.m_sourceRole || TenantRelationshipRoleEnum.s_partner,
					"targetRole": rel.m_targetRole || TenantRelationshipRoleEnum.s_partner,
					"scope": rel.m_scope || "",
					"validFrom": rel.m_validFrom || "",
					"validUntil": rel.m_validUntil || "",
					"isActive": rel.m_isActive === undefined ? true : rel.m_isActive,
					"description": rel.m_description || "",
					"createdAt": rel.m_createdAt || ""
				})
			}
		}
		root.tenantRelationshipsReceived(relationships || [])
	}

	function fetchTenantRelationships(tenantId) {
		root.__getTenantRelationshipsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getTenantRelationshipsSender.send(root.__getTenantRelationshipsInput)
	}

	function addTenantRelationship(sourceTenantId, targetTenantId, sourceRole, targetRole, scope, validFrom, validUntil, description) {
		root.__addTenantRelationshipInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__addTenantRelationshipInput.m_targetTenantId = targetTenantId || ""
		root.__addTenantRelationshipInput.m_role = sourceRole || TenantRelationshipRoleEnum.s_partner
		root.__addTenantRelationshipInput.m_sourceRole = sourceRole || TenantRelationshipRoleEnum.s_partner
		root.__addTenantRelationshipInput.m_targetRole = targetRole || TenantRelationshipRoleEnum.s_partner
		root.__addTenantRelationshipInput.m_scope = scope || ""
		root.__addTenantRelationshipInput.m_validFrom = validFrom || ""
		root.__addTenantRelationshipInput.m_validUntil = validUntil || ""
		root.__addTenantRelationshipInput.m_description = description || ""
		root.__addTenantRelationshipSender.send(root.__addTenantRelationshipInput)
	}

	function removeTenantRelationship(tenantId, relationshipId) {
		root.__pendingRemoveRelationshipId = relationshipId || ""
		root.__removeTenantRelationshipInput.m_tenantId = tenantId || root.tenantId || ""
		root.__removeTenantRelationshipInput.m_relationshipId = relationshipId || ""
		root.__removeTenantRelationshipSender.send(root.__removeTenantRelationshipInput)
	}

	// =========================================================================
	// Tenant connection requests (Tenants.sdl: CreateTenantConnectionRequest /
	// CreateTenantConnectCode / AcceptTenantConnectionRequest /
	// AcceptTenantConnectCode / RejectTenantConnectionRequest /
	// RevokeTenantConnectionRequest / GetTenantConnectionRequests)
	// =========================================================================

	property ListModel connectionRequestsModel: ListModel {}

	property CreateTenantConnectionRequestInput __createConnectionRequestInput: CreateTenantConnectionRequestInput {}
	property GqlSdlRequestSender __createConnectionRequestSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_createTenantConnectionRequest

		sdlObjectComp: Component {
			CreateTenantConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestCreated(m_requestId || "")
					}
				}
			}
		}
	}

	property CreateTenantConnectCodeInput __createConnectCodeInput: CreateTenantConnectCodeInput {}
	property GqlSdlRequestSender __createConnectCodeSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_createTenantConnectCode

		sdlObjectComp: Component {
			CreateTenantConnectCodePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.connectCodeCreated(m_requestId || "", m_connectCode || "")
					}
				}
			}
		}
	}

	property AcceptTenantConnectionRequestInput __acceptConnectionRequestInput: AcceptTenantConnectionRequestInput {}
	property string __pendingAcceptRequestId: ""
	property GqlSdlRequestSender __acceptConnectionRequestSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_acceptTenantConnectionRequest

		sdlObjectComp: Component {
			AcceptTenantConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestAccepted(root.__pendingAcceptRequestId)
					}
				}
			}
		}
	}

	property AcceptTenantConnectCodeInput __acceptConnectCodeInput: AcceptTenantConnectCodeInput {}
	property GqlSdlRequestSender __acceptConnectCodeSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_acceptTenantConnectCode

		sdlObjectComp: Component {
			AcceptTenantConnectCodePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestAccepted(m_requestId || "")
					}
				}
			}
		}
	}

	property RejectTenantConnectionRequestInput __rejectConnectionRequestInput: RejectTenantConnectionRequestInput {}
	property string __pendingRejectRequestId: ""
	property GqlSdlRequestSender __rejectConnectionRequestSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_rejectTenantConnectionRequest

		sdlObjectComp: Component {
			RejectTenantConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestRejected(root.__pendingRejectRequestId)
					}
				}
			}
		}
	}

	property RevokeTenantConnectionRequestInput __revokeConnectionRequestInput: RevokeTenantConnectionRequestInput {}
	property string __pendingRevokeRequestId: ""
	property GqlSdlRequestSender __revokeConnectionRequestSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_revokeTenantConnectionRequest

		sdlObjectComp: Component {
			RevokeTenantConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestRevoked(root.__pendingRevokeRequestId)
					}
				}
			}
		}
	}

	property GetTenantConnectionRequestsInput __getConnectionRequestsInput: GetTenantConnectionRequestsInput {}
	property GqlSdlRequestSender __getConnectionRequestsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getTenantConnectionRequests

		sdlObjectComp: Component {
			GetTenantConnectionRequestsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateConnectionRequestsModel(m_requests)
					}
				}
			}
		}
	}

	function __populateConnectionRequestsModel(requests) {
		root.connectionRequestsModel.clear()
		if (requests) {
			for (var i = 0; i < requests.length; ++i) {
				var req = requests[i]
				if (!req)
					continue
				root.connectionRequestsModel.append({
					"requestId": req.m_id || "",
					"sourceTenantId": req.m_sourceTenantId || "",
					"targetTenantId": req.m_targetTenantId || "",
					"targetIdentifier": req.m_targetIdentifier || "",
					"connectCode": req.m_connectCode || "",
					"proposedSourceRole": req.m_proposedSourceRole || TenantRelationshipRoleEnum.s_partner,
					"proposedTargetRole": req.m_proposedTargetRole || TenantRelationshipRoleEnum.s_partner,
					"message": req.m_message || "",
					"status": req.m_status || TenantConnectionStatusEnum.s_pending,
					"createdAt": req.m_createdAt || "",
					"expiresAt": req.m_expiresAt || "",
					"respondedAt": req.m_respondedAt || ""
				})
			}
		}
		root.connectionRequestsReceived(requests || [])
	}

	function fetchConnectionRequests(tenantId) {
		root.__getConnectionRequestsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getConnectionRequestsSender.send(root.__getConnectionRequestsInput)
	}

	function createConnectionRequest(sourceTenantId, targetIdentifier, proposedSourceRole, proposedTargetRole, message, expiresAt) {
		root.__createConnectionRequestInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__createConnectionRequestInput.m_targetIdentifier = targetIdentifier || ""
		root.__createConnectionRequestInput.m_proposedSourceRole = proposedSourceRole || TenantRelationshipRoleEnum.s_partner
		root.__createConnectionRequestInput.m_proposedTargetRole = proposedTargetRole || TenantRelationshipRoleEnum.s_partner
		root.__createConnectionRequestInput.m_message = message || ""
		root.__createConnectionRequestInput.m_expiresAt = expiresAt || ""
		root.__createConnectionRequestSender.send(root.__createConnectionRequestInput)
	}

	function createConnectCode(sourceTenantId, proposedSourceRole, proposedTargetRole, message, expiresAt) {
		root.__createConnectCodeInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__createConnectCodeInput.m_proposedSourceRole = proposedSourceRole || TenantRelationshipRoleEnum.s_partner
		root.__createConnectCodeInput.m_proposedTargetRole = proposedTargetRole || TenantRelationshipRoleEnum.s_partner
		root.__createConnectCodeInput.m_message = message || ""
		root.__createConnectCodeInput.m_expiresAt = expiresAt || ""
		root.__createConnectCodeSender.send(root.__createConnectCodeInput)
	}

	function acceptConnectionRequest(requestId, acceptingTenantId) {
		root.__pendingAcceptRequestId = requestId || ""
		root.__acceptConnectionRequestInput.m_requestId = requestId || ""
		root.__acceptConnectionRequestInput.m_acceptingTenantId = acceptingTenantId || root.tenantId || ""
		root.__acceptConnectionRequestSender.send(root.__acceptConnectionRequestInput)
	}

	function acceptConnectCode(connectCode, acceptingTenantId) {
		root.__acceptConnectCodeInput.m_connectCode = connectCode || ""
		root.__acceptConnectCodeInput.m_acceptingTenantId = acceptingTenantId || root.tenantId || ""
		root.__acceptConnectCodeSender.send(root.__acceptConnectCodeInput)
	}

	function rejectConnectionRequest(requestId) {
		root.__pendingRejectRequestId = requestId || ""
		root.__rejectConnectionRequestInput.m_requestId = requestId || ""
		root.__rejectConnectionRequestSender.send(root.__rejectConnectionRequestInput)
	}

	function revokeConnectionRequest(requestId) {
		root.__pendingRevokeRequestId = requestId || ""
		root.__revokeConnectionRequestInput.m_requestId = requestId || ""
		root.__revokeConnectionRequestSender.send(root.__revokeConnectionRequestInput)
	}

	// =========================================================================
	// Cross-tenant messages (phase 2)
	// =========================================================================

	property ListModel crossTenantMessagesModel: ListModel {}

	property SendCrossTenantMessageInput __sendMessageInput: SendCrossTenantMessageInput {}
	property GqlSdlRequestSender __sendMessageSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_sendCrossTenantMessage

		sdlObjectComp: Component {
			SendCrossTenantMessagePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.crossTenantMessageSent(m_messageId || "")
					}
				}
			}
		}
	}

	property UpdateCrossTenantMessageStatusInput __updateMessageStatusInput: UpdateCrossTenantMessageStatusInput {}
	property string __pendingUpdateMessageId: ""
	property GqlSdlRequestSender __updateMessageStatusSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_updateCrossTenantMessageStatus

		sdlObjectComp: Component {
			UpdateCrossTenantMessageStatusPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.crossTenantMessageStatusUpdated(root.__pendingUpdateMessageId)
					}
				}
			}
		}
	}

	property GetCrossTenantMessagesInput __getMessagesInput: GetCrossTenantMessagesInput {}
	property GqlSdlRequestSender __getMessagesSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getCrossTenantMessages

		sdlObjectComp: Component {
			GetCrossTenantMessagesPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateCrossTenantMessagesModel(m_messages)
					}
				}
			}
		}
	}

	function __populateCrossTenantMessagesModel(messages) {
		root.crossTenantMessagesModel.clear()
		if (messages) {
			for (var i = 0; i < messages.length; ++i) {
				var msg = messages[i]
				if (!msg)
					continue
				root.crossTenantMessagesModel.append({
					"messageId": msg.m_id || "",
					"sourceTenantId": msg.m_sourceTenantId || "",
					"targetTenantId": msg.m_targetTenantId || "",
					"relationshipId": msg.m_relationshipId || "",
					"sourceObjectId": msg.m_sourceObjectId || "",
					"targetObjectId": msg.m_targetObjectId || "",
					"messageType": msg.m_messageType || CrossTenantMessageTypeEnum.s_custom,
					"customType": msg.m_customType || "",
					"payload": msg.m_payload || "",
					"status": msg.m_status || CrossTenantMessageStatusEnum.s_created,
					"errorMessage": msg.m_errorMessage || "",
					"createdAt": msg.m_createdAt || "",
					"updatedAt": msg.m_updatedAt || "",
					"expiresAt": msg.m_expiresAt || ""
				})
			}
		}
		root.crossTenantMessagesReceived(messages || [])
	}

	function fetchCrossTenantMessages(tenantId, direction) {
		root.__getMessagesInput.m_tenantId = tenantId || root.tenantId || ""
		if (direction && direction !== "")
			root.__getMessagesInput.m_direction = direction
		root.__getMessagesSender.send(root.__getMessagesInput)
	}

	function sendCrossTenantMessage(sourceTenantId, targetTenantId, relationshipId, messageType, payload, sourceObjectId, customType, expiresAt) {
		root.__sendMessageInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__sendMessageInput.m_targetTenantId = targetTenantId || ""
		root.__sendMessageInput.m_relationshipId = relationshipId || ""
		root.__sendMessageInput.m_messageType = messageType || CrossTenantMessageTypeEnum.s_custom
		root.__sendMessageInput.m_payload = payload || ""
		root.__sendMessageInput.m_sourceObjectId = sourceObjectId || ""
		root.__sendMessageInput.m_customType = customType || ""
		root.__sendMessageInput.m_expiresAt = expiresAt || ""
		root.__sendMessageSender.send(root.__sendMessageInput)
	}

	function updateCrossTenantMessageStatus(messageId, status, errorMessage) {
		root.__pendingUpdateMessageId = messageId || ""
		root.__updateMessageStatusInput.m_messageId = messageId || ""
		root.__updateMessageStatusInput.m_status = status || CrossTenantMessageStatusEnum.s_created
		root.__updateMessageStatusInput.m_errorMessage = errorMessage || ""
		root.__updateMessageStatusSender.send(root.__updateMessageStatusInput)
	}

	// =========================================================================
	// Order requests (phase 3)
	// =========================================================================

	property ListModel orderRequestsModel: ListModel {}

	property ConfirmOrderRequestInput __confirmOrderRequestInput: ConfirmOrderRequestInput {}
	property string __pendingConfirmOrderRequestId: ""
	property GqlSdlRequestSender __confirmOrderRequestSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_confirmOrderRequest

		sdlObjectComp: Component {
			ConfirmOrderRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.orderRequestConfirmed(root.__pendingConfirmOrderRequestId)
					}
				}
			}
		}
	}

	property RejectOrderRequestInput __rejectOrderRequestInput: RejectOrderRequestInput {}
	property string __pendingRejectOrderRequestId: ""
	property GqlSdlRequestSender __rejectOrderRequestSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_rejectOrderRequest

		sdlObjectComp: Component {
			RejectOrderRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.orderRequestRejected(root.__pendingRejectOrderRequestId)
					}
				}
			}
		}
	}

	property UpdateOrderRequestStatusInput __updateOrderRequestStatusInput: UpdateOrderRequestStatusInput {}
	property string __pendingUpdateOrderRequestId: ""
	property GqlSdlRequestSender __updateOrderRequestStatusSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_updateOrderRequestStatus

		sdlObjectComp: Component {
			UpdateOrderRequestStatusPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.orderRequestStatusUpdated(root.__pendingUpdateOrderRequestId)
					}
				}
			}
		}
	}

	property GetOrderRequestsInput __getOrderRequestsInput: GetOrderRequestsInput {}
	property GqlSdlRequestSender __getOrderRequestsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getOrderRequests

		sdlObjectComp: Component {
			GetOrderRequestsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateOrderRequestsModel(m_orderRequests)
					}
				}
			}
		}
	}

	function __populateOrderRequestsModel(orderRequests) {
		root.orderRequestsModel.clear()
		if (orderRequests) {
			for (var i = 0; i < orderRequests.length; ++i) {
				var ord = orderRequests[i]
				if (!ord)
					continue
				root.orderRequestsModel.append({
					"orderRequestId": ord.m_id || "",
					"messageId": ord.m_messageId || "",
					"sourceTenantId": ord.m_sourceTenantId || "",
					"targetTenantId": ord.m_targetTenantId || "",
					"relationshipId": ord.m_relationshipId || "",
					"sourceOrderId": ord.m_sourceOrderId || "",
					"articleNumber": ord.m_articleNumber || "",
					"quantity": ord.m_quantity || 0,
					"note": ord.m_note || "",
					"status": ord.m_status || OrderRequestStatusEnum.s_received,
					"statusNote": ord.m_statusNote || "",
					"createdAt": ord.m_createdAt || "",
					"updatedAt": ord.m_updatedAt || ""
				})
			}
		}
		root.orderRequestsReceived(orderRequests || [])
	}

	function fetchOrderRequests(tenantId) {
		root.__getOrderRequestsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getOrderRequestsSender.send(root.__getOrderRequestsInput)
	}

	function confirmOrderRequest(orderRequestId, note) {
		root.__pendingConfirmOrderRequestId = orderRequestId || ""
		root.__confirmOrderRequestInput.m_orderRequestId = orderRequestId || ""
		root.__confirmOrderRequestInput.m_note = note || ""
		root.__confirmOrderRequestSender.send(root.__confirmOrderRequestInput)
	}

	function rejectOrderRequest(orderRequestId, reason) {
		root.__pendingRejectOrderRequestId = orderRequestId || ""
		root.__rejectOrderRequestInput.m_orderRequestId = orderRequestId || ""
		root.__rejectOrderRequestInput.m_reason = reason || ""
		root.__rejectOrderRequestSender.send(root.__rejectOrderRequestInput)
	}

	function updateOrderRequestStatus(orderRequestId, status, note) {
		root.__pendingUpdateOrderRequestId = orderRequestId || ""
		root.__updateOrderRequestStatusInput.m_orderRequestId = orderRequestId || ""
		root.__updateOrderRequestStatusInput.m_status = status || OrderRequestStatusEnum.s_received
		root.__updateOrderRequestStatusInput.m_note = note || ""
		root.__updateOrderRequestStatusSender.send(root.__updateOrderRequestStatusInput)
	}

	// =========================================================================
	// List data providers (Roles / Groups / invitable Users)
	// =========================================================================

	property FilterableSelectGqlDataProvider roleListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "Roles"
		pageSize: 50
	}

	property FilterableSelectGqlDataProvider groupListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "Groups"
		pageSize: 50
	}

	property FilterableSelectGqlDataProvider invitableUsersListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "UsersForInvitation"
		multiSelect: true
	}

	// =========================================================================
	// Document services (Roles / Groups / Users)
	//
	// These three services drive the SingleDocumentWorkspaceShellView in the
	// Roles / Groups / Members pages. The pages themselves do NOT instantiate
	// any GqlBasedCollectionDocumentService / SingleDocumentTypeRegistrar /
	// DocumentRepresentationController — they only consume the abstract
	// `roleDocumentManager` / `groupDocumentManager` / `userDocumentManager`
	// properties.
	// =========================================================================

	property GqlBasedCollectionDocumentService __roleDocumentService: GqlBasedCollectionDocumentService {
		collectionId: "Roles"
	}

	property GqlBasedCollectionDocumentService __groupDocumentService: GqlBasedCollectionDocumentService {
		collectionId: "Groups"
	}

	property GqlBasedCollectionDocumentService __userDocumentService: GqlBasedCollectionDocumentService {
		collectionId: "Users"
	}

	// --- Role editor + representation controller ---
	property Component __roleEditorComp: Component {
		RoleView {
			productId: root.productId
			permissionsModel: root.permissionsModel
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: root.roleObjectTypeId
				}
			}
			Component.onCompleted: {
				root.fetchPermissions()
			}
		}
	}

	property Component __roleControllerComp: Component {
		DocumentRepresentationController {
			id: roleReprController

			representationModel: RoleData {
				m_id: UuidGenerator.generateUUID()
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				getRoleInput.m_id = documentId
				getRoleInput.m_collectionId = "Roles"
				getRoleRequest.send(getRoleInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateRoleInput.m_documentId = documentId
				updateRoleInput.m_role = representationModel
				updateRoleRequest.send(updateRoleInput)
			}

			property DocumentId getRoleInput: DocumentId {}
			property UpdateRoleFromRepresentationInput updateRoleInput: UpdateRoleFromRepresentationInput {}

			property GqlSdlRequestSender getRoleRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthRoleCollectionDocumentServiceSdlCommandIds.s_getRoleRepresentation
				sdlObjectComp: Component {
					RoleData {
						onFinished: {
							roleReprController.representationModel.copyFrom(this)
							roleReprController.representationUpdated(
								roleReprController.documentId,
								roleReprController.representationModel)
						}
					}
				}

				function onError(message, type){
					roleReprController.updateRepresentationFailed(roleReprController.documentId, message)
				}
			}

			property GqlSdlRequestSender updateRoleRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthRoleCollectionDocumentServiceSdlCommandIds.s_updateRoleFromRepresentation
				requestType: 1
				sdlObjectComp: Component {
					DocumentOperationStatus {
						onFinished: {
							if (m_status === "Success"){
								roleReprController.documentUpdated(roleReprController.documentId)
							}
						}
					}
				}

				function onError(message, type){
					roleReprController.updateDocumentFailed(roleReprController.documentId, message)
				}
			}
		}
	}

	// --- Group editor + representation controller ---
	property Component __groupEditorComp: Component {
		UserGroupView {
			productId: root.productId
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: root.groupObjectTypeId
				}
			}
		}
	}

	property Component __groupControllerComp: Component {
		DocumentRepresentationController {
			id: groupReprController

			representationModel: GroupData {
				m_id: UuidGenerator.generateUUID()
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				getGroupInput.m_id = documentId
				getGroupInput.m_collectionId = "Groups"
				getGroupRequest.send(getGroupInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateGroupInput.m_documentId = documentId
				updateGroupInput.m_group = representationModel
				updateGroupRequest.send(updateGroupInput)
			}

			property DocumentId getGroupInput: DocumentId {}
			property UpdateGroupFromRepresentationInput updateGroupInput: UpdateGroupFromRepresentationInput {}

			property GqlSdlRequestSender getGroupRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthGroupCollectionDocumentServiceSdlCommandIds.s_getGroupRepresentation
				sdlObjectComp: Component {
					GroupData {
						onFinished: {
							groupReprController.representationModel.copyFrom(this)
							groupReprController.representationUpdated(
								groupReprController.documentId,
								groupReprController.representationModel)
						}
					}
				}

				function onError(message, type){
					groupReprController.updateRepresentationFailed(groupReprController.documentId, message)
				}
			}

			property GqlSdlRequestSender updateGroupRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthGroupCollectionDocumentServiceSdlCommandIds.s_updateGroupFromRepresentation
				requestType: 1
				sdlObjectComp: Component {
					DocumentOperationStatus {
						onFinished: {
							if (m_status === "Success"){
								groupReprController.documentUpdated(groupReprController.documentId)
							}
						}
					}
				}

				function onError(message, type){
					groupReprController.updateDocumentFailed(groupReprController.documentId, message)
				}
			}
		}
	}

	// --- User editor + representation controller ---
	property Component __userEditorComp: Component {
		UserView {
			id: userEditor
			productId: root.productId
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: root.userObjectTypeId
				}
			}

			onUserDataChanged: {
				if (userData && root.userDocumentManager){
					userEditor.isNew = root.userDocumentManager.documentIsNew(userData.m_id)
				}
			}

			function documentSaved(){
				userEditor.isNew = false
				userEditor.checkChangePasswordLogic()
			}
		}
	}

	property Component __userControllerComp: Component {
		DocumentRepresentationController {
			id: userReprController

			representationModel: UserData {
				m_id: UuidGenerator.generateUUID()
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				getUserInput.m_id = documentId
				getUserInput.m_collectionId = "Users"
				getUserRequest.send(getUserInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateUserInput.m_documentId = documentId
				updateUserInput.m_user = representationModel
				updateUserRequest.send(updateUserInput)
			}

			property DocumentId getUserInput: DocumentId {}
			property UpdateUserFromRepresentationInput updateUserInput: UpdateUserFromRepresentationInput {}

			property GqlSdlRequestSender getUserRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthUserCollectionDocumentServiceSdlCommandIds.s_getUserRepresentation
				sdlObjectComp: Component {
					UserData {
						onFinished: {
							userReprController.representationModel.copyFrom(this)
							userReprController.representationUpdated(
								userReprController.documentId,
								userReprController.representationModel)
						}
					}
				}

				function onError(message, type){
					userReprController.updateRepresentationFailed(userReprController.documentId, message)
				}
			}

			property GqlSdlRequestSender updateUserRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthUserCollectionDocumentServiceSdlCommandIds.s_updateUserFromRepresentation
				requestType: 1
				sdlObjectComp: Component {
					DocumentOperationStatus {
						onFinished: {
							if (m_status === "Success"){
								userReprController.documentUpdated(userReprController.documentId)
							}
						}
					}
				}

				function onError(message, type){
					userReprController.updateDocumentFailed(userReprController.documentId, message)
				}
			}
		}
	}

	// Register each editor + controller pair with its document service so that
	// pages only need to bind to the abstract `xDocumentManager` / `xObjectTypeId`.
	Component.onCompleted: {
		root.__roleDocumentService.registerDocumentViewData(
			root.roleObjectTypeId, "Editor", root.__roleEditorComp, root.__roleControllerComp)
		root.__groupDocumentService.registerDocumentViewData(
			root.groupObjectTypeId, "Editor", root.__groupEditorComp, root.__groupControllerComp)
		root.__userDocumentService.registerDocumentViewData(
			root.userObjectTypeId, "Editor", root.__userEditorComp, root.__userControllerComp)
	}
}
