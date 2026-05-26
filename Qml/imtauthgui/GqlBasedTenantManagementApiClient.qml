// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtauthTenantMembershipsSdl 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtauthRolesSdl 1.0
import imtauthGroupsSdl 1.0
import imtauthUsersSdl 1.0
import imtauthRoleCollectionDocumentServiceSdl 1.0
import imtauthGroupCollectionDocumentServiceSdl 1.0
import imtauthUserCollectionDocumentServiceSdl 1.0

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

	signal requestFailed(string message)

	// --- Real-time membership subscription notifications ---
	signal subscriptionInvitationAccepted(var notification)
	signal subscriptionInvitationRejected(var notification)
	signal subscriptionOwnershipTransferred(var notification)

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

			if (notificationType === "InvitationAccepted" || notificationType === 1)
				root.subscriptionInvitationAccepted(notification)
			else if (notificationType === "InvitationRejected" || notificationType === 2)
				root.subscriptionInvitationRejected(notification)
			else if (notificationType === "OwnershipTransferred" || notificationType === 3)
				root.subscriptionOwnershipTransferred(notification)
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
	// List data providers (Roles / Groups / invitable Users)
	// =========================================================================

	property Component roleListDataProviderComp: Component {
		FilterableSelectGqlDataProvider {
			collectionId: "Roles"
			tenantId: root.productId
			pageSize: 50
		}
	}

	property Component groupListDataProviderComp: Component {
		FilterableSelectGqlDataProvider {
			collectionId: "Groups"
			tenantId: root.productId
			pageSize: 50
		}
	}

	property Component invitableUsersListDataProviderComp: Component {
		FilterableSelectGqlDataProvider {
			collectionId: "UsersForInvitation"
			tenantId: root.productId
			multiSelect: true
		}
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
			productId: root.productId
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: root.userObjectTypeId
				}
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
