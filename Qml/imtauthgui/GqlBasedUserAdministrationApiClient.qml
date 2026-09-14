// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
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
 * GqlBasedUserAdministrationApiClient
 *
 * Roles/Users/Groups document services, editors and permissions - the part of
 * GqlBasedTenantManagementApiClient that AdministrationView actually needs.
 * Also embedded by GqlBasedTenantManagementApiClient itself for TenantEditor's
 * Roles/Users/Groups pages, so both consumers share one implementation.
 */
QtObject {
	id: root

	property string permissionPath: ""
	property string productId: AuthorizationController.productId
	property string tenantId: AuthorizationController.currentTenantId
	property string rolePermissionsTenantId: ""

	property string roleObjectTypeId: "Role"
	property string groupObjectTypeId: "Group"
	property string userObjectTypeId: "User"

	readonly property var roleDocumentManager: __roleDocumentService
	readonly property var groupDocumentManager: __groupDocumentService
	readonly property var userDocumentManager: __userDocumentService
	readonly property var permissionsProvider: __permissionsProvider

	signal roleCreated()
	signal rolesRemoved()
	signal roleUpdated(string roleId)
	signal roleDataReceived(var data)

	signal groupCreated()
	signal groupsRemoved()
	signal groupUpdated(string groupId)
	signal groupDataReceived(var data)

	signal usersRemoved()
	signal userUpdated(string userId)
	signal userDataReceived(var data)

	signal requestFailed(string message)

	property GqlBasedPermissionsProvider __permissionsProvider: GqlBasedPermissionsProvider {
		productId: root.productId || ""
	}

	property Connections __permissionsProviderConnections: Connections {
		target: root.__permissionsProvider

		function onRequestFailed(message, tenantId) {
			if (message && message !== "") {
				PopupManager.addErrorMessage(message, true)
				root.requestFailed(message)
			}
		}

		function onAllPermissionsReceived() {
			root.allPermissionsReceived()
		}

		function onTenantPermissionsReceived(sourceTenantId) {
			root.tenantPermissionsReceived()
		}

		function onOrganizationPermissionsReceived() {
			root.organizationPermissionsReceived()
		}
	}

	property var allPermissions: root.__permissionsProvider ? root.__permissionsProvider.allPermissions : []
	signal allPermissionsReceived()

	property var tenantPermissions: root.__permissionsProvider ? root.__permissionsProvider.tenantPermissions : []
	signal tenantPermissionsReceived()

	property var organizationPermissions: root.__permissionsProvider ? root.__permissionsProvider.organizationPermissions : []
	property var memberOrganizationPermissions: root.__permissionsProvider ? root.__permissionsProvider.memberOrganizationPermissions : []
	signal organizationPermissionsReceived()

	function setRolePermissionsTenantId(tenantId) {
		root.rolePermissionsTenantId = tenantId || ""
	}

	function fetchAllPermissions() {
		if (!root.__permissionsProvider)
			return
		root.__permissionsProvider.productId = root.productId || ""
		root.__permissionsProvider.requestAllPermissions()
	}

	function fetchTenantPermissions(tenantId) {
		if (!root.__permissionsProvider)
			return
		root.__permissionsProvider.productId = root.productId || ""
		root.__permissionsProvider.requestPermissions(tenantId || root.tenantId || "")
	}

	function fetchOrganizationPermissions(tenantId, userId) {
		if (!root.__permissionsProvider)
			return
		root.__permissionsProvider.requestOrganizationPermissions(tenantId || root.tenantId || "", userId || "")
	}

	property RemoveElementsInput __removeRoleInput: RemoveElementsInput {}
	property GqlSdlRequestSender __removeRoleSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements

		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: { root.rolesRemoved() }
			}
		}
	}

	property RemoveElementsInput __removeGroupInput: RemoveElementsInput {}
	property GqlSdlRequestSender __removeGroupSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements

		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: { root.groupsRemoved() }
			}
		}
	}

	function removeRoles(roleIds) {
		root.__removeRoleInput.m_collectionId = "Roles"
		root.__removeRoleInput.m_elementIds = roleIds
		root.__removeRoleSender.send(root.__removeRoleInput)
	}

	function removeGroups(groupIds) {
		root.__removeGroupInput.m_collectionId = "Groups"
		root.__removeGroupInput.m_elementIds = groupIds
		root.__removeGroupSender.send(root.__removeGroupInput)
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
			tenantId: root.rolePermissionsTenantId
			permissionsProvider: root.__permissionsProvider
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

			onDocumentIdChanged: {
				if (documentId !== ""){
					var objId = root.__roleDocumentService.getDocumentObjectId(documentId)
					if (objId !== "")
						representationModel.m_id = objId
				}
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
				permissionPath: root.permissionPath
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
				permissionPath: root.permissionPath
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

			onDocumentIdChanged: {
				if (documentId !== ""){
					var objId = root.__groupDocumentService.getDocumentObjectId(documentId)
					if (objId !== "")
						representationModel.m_id = objId
				}
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
				permissionPath: root.permissionPath
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
				permissionPath: root.permissionPath
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
					userEditor.isNew = root.userDocumentManager.documentIsNew(userEditor.documentId)
				}
			}

			onDocumentSaved: {
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

			onDocumentIdChanged: {
				if (documentId !== ""){
					var objId = root.__userDocumentService.getDocumentObjectId(documentId)
					if (objId !== "")
						representationModel.m_id = objId
				}
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				getUserInput.m_id = documentId
				getUserInput.m_collectionId = "Users"
				getUserRequest.send(getUserInput)
			}

			property MailRegExpValidator mailRegExp: MailRegExpValidator {}

			function __failValidation(message){
				PopupManager.addErrorMessage(message, true)
				updateDocumentFailed(documentId, message)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateUserInput.m_documentId = documentId
				updateUserInput.m_user = representationModel
				updateUserInput.m_tenantId = root.tenantId
				updateUserRequest.send(updateUserInput)
			}

			property DocumentId getUserInput: DocumentId {}
			property UpdateUserFromRepresentationInput updateUserInput: UpdateUserFromRepresentationInput {}

			property GqlSdlRequestSender getUserRequest: GqlSdlRequestSender {
				permissionPath: root.permissionPath
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
				permissionPath: root.permissionPath
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

	Component.onCompleted: {
		root.__roleDocumentService.registerDocumentViewData(
			root.roleObjectTypeId, "Editor", root.__roleEditorComp, root.__roleControllerComp)
		root.__groupDocumentService.registerDocumentViewData(
			root.groupObjectTypeId, "Editor", root.__groupEditorComp, root.__groupControllerComp)
		root.__userDocumentService.registerDocumentViewData(
			root.userObjectTypeId, "Editor", root.__userEditorComp, root.__userControllerComp)
	}
}
