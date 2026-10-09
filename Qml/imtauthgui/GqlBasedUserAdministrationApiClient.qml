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

	property string context: ""

	// Password policy for the user editor's password fields (imtauthgui owns no GQL).
	property GqlBasedPasswordPolicyProvider passwordPolicy: GqlBasedPasswordPolicyProvider {
		context: root.context
	}

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
		context: root.context
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
		context: root.context
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

	// Ids of the direct assignments of an SDL Assignment list.
	function __directIds(assignments) {
		var ids = []
		for (var i = 0; assignments && i < assignments.count; i++) {
			var assignment = assignments.get(i).item
			if (assignment.m_direct)
				ids.push(String(assignment.m_id))
		}
		return ids
	}

	// Selection of an Assignment list: id, name, direct and the paths it comes through.
	function __assignmentsField(fieldId) {
		var steps = Gql.GqlObject("steps")
		steps.InsertField("kind")
		steps.InsertField("id")
		steps.InsertField("name")
		var sources = Gql.GqlObject("sources")
		sources.InsertFieldObject(steps)
		var field = Gql.GqlObject(fieldId)
		field.InsertField("id")
		field.InsertField("name")
		field.InsertField("direct")
		field.InsertFieldObject(sources)
		return field
	}

	// Representation query of an open document that asks only for the given assignment lists and the warnings.
	function __addAssignmentsQuery(query, documentId, collectionId, fieldIds) {
		var input = Gql.GqlObject("input")
		input.InsertField("id", documentId)
		input.InsertField("collectionId", collectionId)
		query.AddParam(input)

		var fields = Gql.GqlObject("fields")
		for (var i = 0; i < fieldIds.length; i++)
			fields.InsertFieldObject(root.__assignmentsField(fieldIds[i]))
		var warnings = Gql.GqlObject("accessWarnings")
		warnings.InsertField("code")
		warnings.InsertField("ids")
		warnings.InsertField("message")
		fields.InsertFieldObject(warnings)
		query.AddField(fields)
	}

	// Replaces an SDL list of the representation by a copy of the same list of an answer.
	function __copyList(target, source, propertyId) {
		if (!target[propertyId])
			target["emplace" + propertyId.charAt(2).toUpperCase() + propertyId.slice(3)]()
		target[propertyId].clear()
		var items = source[propertyId]
		for (var i = 0; items && i < items.count; i++) {
			// Created like fromObject() does: copyMe() resolves its component by a relative path the web build cannot.
			var element = target.createElement(propertyId).createObject(target)
			element.fromJSON(items.get(i).item.toJson())
			// The web BaseModel has no appendElement().
			target[propertyId].insertElement(target[propertyId].count, element)
		}
	}

	// Writes the given lists of an answer into the editor's model and lets the editor show them.
	function __applyAssignments(controller, data, propertyIds) {
		var view = controller.view
		if (!view)
			return
		view.setBlockingUpdateModel(true)
		for (var i = 0; i < propertyIds.length; i++)
			root.__copyList(controller.representationModel, data, propertyIds[i])
		view.setBlockingUpdateModel(false)
		view.updateAssignments()
	}

	function __handleRoleDataReceived(roleData) {
		if (!roleData)
			return
		var data = {
			name: roleData.m_name || "",
			description: roleData.m_description || "",
			roleId: roleData.m_roleId || "",
			productId: roleData.m_productId || "",
			parentRoles: root.__directIds(roleData.m_parentRoles),
			permissions: root.__directIds(roleData.m_permissions).join(";"),
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
			roles: root.__directIds(groupData.m_roles),
			users: root.__directIds(groupData.m_users),
			parentGroups: root.__directIds(groupData.m_parentGroups)
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
			groups: root.__directIds(userData.m_groups),
			roles: root.__directIds(userData.m_roles),
			permissions: root.__directIds(userData.m_permissions)
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
				context: root.context
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
				context: root.context
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

			property AssignmentsRefresher assignmentsRefresher: AssignmentsRefresher {
				controller: roleReprController

				onBusyChanged: {
					if (roleReprController.view)
						roleReprController.view.assignmentsUpdating = busy
				}

				onSendRequested: {
					roleReprController.getRoleAssignmentsRequest.send()
				}
			}

			property Connections viewConnections: Connections {
				target: roleReprController.view

				function onAssignmentsChanged(){
					roleReprController.assignmentsRefresher.request()
				}
			}

			property GqlSdlRequestSender getRoleAssignmentsRequest: GqlSdlRequestSender {
				context: root.context
				gqlCommandId: ImtauthRoleCollectionDocumentServiceSdlCommandIds.s_getRoleRepresentation
				sdlObjectComp: Component {
					RoleData {
						onFinished: {
							if (roleReprController.assignmentsRefresher.finished())
								root.__applyAssignments(roleReprController, this, ["m_parentRoles", "m_permissions", "m_accessWarnings"])
						}
					}
				}

				function createQueryParams(query){
					root.__addAssignmentsQuery(query, roleReprController.documentId, "Roles", ["parentRoles", "permissions"])
				}

				function onError(message, type){
					roleReprController.assignmentsRefresher.failed()
					PopupManager.addErrorMessage(message, true)
				}
			}
		}
	}

	// --- Group editor + representation controller ---
	property Component __groupEditorComp: Component {
		UserGroupView {
			productId: root.productId
			permissionGroups: root.allPermissions

			Component.onCompleted: {
				if (!root.allPermissions || root.allPermissions.length === 0){
					root.fetchAllPermissions()
				}
			}
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
				context: root.context
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
				context: root.context
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

			property AssignmentsRefresher assignmentsRefresher: AssignmentsRefresher {
				controller: groupReprController

				onBusyChanged: {
					if (groupReprController.view)
						groupReprController.view.assignmentsUpdating = busy
				}

				onSendRequested: {
					groupReprController.getGroupAssignmentsRequest.send()
				}
			}

			property Connections viewConnections: Connections {
				target: groupReprController.view

				function onAssignmentsChanged(){
					groupReprController.assignmentsRefresher.request()
				}
			}

			// Members are not requested: they are edited here and inherit nothing.
			property GqlSdlRequestSender getGroupAssignmentsRequest: GqlSdlRequestSender {
				context: root.context
				gqlCommandId: ImtauthGroupCollectionDocumentServiceSdlCommandIds.s_getGroupRepresentation
				sdlObjectComp: Component {
					GroupData {
						onFinished: {
							if (groupReprController.assignmentsRefresher.finished())
								root.__applyAssignments(groupReprController, this, ["m_roles", "m_parentGroups", "m_permissions", "m_accessWarnings"])
						}
					}
				}

				function createQueryParams(query){
					root.__addAssignmentsQuery(query, groupReprController.documentId, "Groups", ["roles", "parentGroups", "permissions"])
				}

				function onError(message, type){
					groupReprController.assignmentsRefresher.failed()
					PopupManager.addErrorMessage(message, true)
				}
			}
		}
	}

	// --- User editor + representation controller ---
	property Component __userEditorComp: Component {
		UserView {
			id: userEditor
			productId: root.productId
			passwordPolicy: root.passwordPolicy
			permissionGroups: root.allPermissions

			Component.onCompleted: {
				if (!root.allPermissions || root.allPermissions.length === 0){
					root.fetchAllPermissions()
				}
			}
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
				context: root.context
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
				context: root.context
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

			property AssignmentsRefresher assignmentsRefresher: AssignmentsRefresher {
				controller: userReprController

				onBusyChanged: {
					if (userReprController.view)
						userReprController.view.assignmentsUpdating = busy
				}

				onSendRequested: {
					userReprController.getUserAssignmentsRequest.send()
				}
			}

			property Connections viewConnections: Connections {
				target: userReprController.view

				function onAssignmentsChanged(){
					userReprController.assignmentsRefresher.request()
				}
			}

			property GqlSdlRequestSender getUserAssignmentsRequest: GqlSdlRequestSender {
				context: root.context
				gqlCommandId: ImtauthUserCollectionDocumentServiceSdlCommandIds.s_getUserRepresentation
				sdlObjectComp: Component {
					UserData {
						onFinished: {
							if (userReprController.assignmentsRefresher.finished())
								root.__applyAssignments(userReprController, this, ["m_roles", "m_groups", "m_permissions", "m_accessWarnings"])
						}
					}
				}

				function createQueryParams(query){
					root.__addAssignmentsQuery(query, userReprController.documentId, "Users", ["roles", "groups", "permissions"])
				}

				function onError(message, type){
					userReprController.assignmentsRefresher.failed()
					PopupManager.addErrorMessage(message, true)
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
