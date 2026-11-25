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
import imtauthgui 1.0

/**
 * GqlBasedTenantManagementApiClient
 *
 * GQL/SDL implementation of the abstract TenantManagementApiClient contract.
 * Roles/Users/Groups are delegated to GqlBasedUserAdministrationApiClient
 * (shared with AdministrationView); everything else here is TenantEditor-only.
 *
 * Pages depend only on the abstract contract; the orchestrator (TenantEditor)
 * injects this concrete client.
 */
QtObject {
	id: root

	property string permissionPath: ""
	// =========================================================================
	// Configuration
	// =========================================================================

	property string productId: AuthorizationController.productId
	property string tenantId: AuthorizationController.currentTenantId
	property alias rolePermissionsTenantId: __userAdminObj.rolePermissionsTenantId

	// =========================================================================
	// Abstract contract (must mirror TenantManagementApiClient.qml)
	// =========================================================================

	// --- Document services (concrete instances expose abstract managers) ---
	property alias roleObjectTypeId: __userAdminObj.roleObjectTypeId
	property alias groupObjectTypeId: __userAdminObj.groupObjectTypeId
	property alias userObjectTypeId: __userAdminObj.userObjectTypeId
	property string crossTenantMessageObjectTypeId: "CrossTenantMessage"

	readonly property alias roleDocumentManager: __userAdminObj.roleDocumentManager
	readonly property alias groupDocumentManager: __userAdminObj.groupDocumentManager
	readonly property alias userDocumentManager: __userAdminObj.userDocumentManager
	readonly property alias permissionsProvider: __userAdminObj.permissionsProvider

	signal invitationCreated()
	signal invitationRevoked(string invitationId)
	signal invitationResent(string invitationId)

	signal ownershipTransferred()
	signal memberRemoved(string userId)

	signal roleCreated()
	signal rolesRemoved()
	signal roleUpdated(string roleId)
	signal roleDataReceived(var data)

	signal groupCreated()
	signal groupsRemoved()
	signal groupUpdated(string groupId)
	signal groupDataReceived(var data)

	signal userCreated()
	signal usersRemoved()
	signal userUpdated(string userId)
	signal userDataReceived(var data)


	signal requestFailed(string message)

	// --- Real-time membership subscription notifications ---
	signal subscriptionInvitationReceived(string tenantId, string tenantName, string role)
	signal subscriptionInvitationAccepted(string tenantId, string membershipId)
	signal subscriptionInvitationRejected(string tenantId, string membershipId)
	signal subscriptionOwnershipTransferred(string tenantId)
	signal subscriptionMembershipRoleChanged(string tenantId, string userId, string role)
	signal subscriptionMembershipRemoved(string tenantId, string userId)

	signal crossTenantMessageSent(string messageId)
	signal crossTenantMessageStatusUpdated(string messageId)
	signal crossTenantMessagesReceived(var messages)
	signal orderRequestConfirmed(string requestId)
	signal orderRequestRejected(string requestId)
	signal orderRequestStatusUpdated(string requestId)
	signal orderRequestsReceived(var orderRequests)

	// Roles/Users/Groups document services, editors and permissions - shared with AdministrationView.
	property GqlBasedUserAdministrationApiClient __userAdmin: GqlBasedUserAdministrationApiClient {
		id: __userAdminObj
		productId: root.productId
		tenantId: root.tenantId
	}

	property Connections __userAdminConnections: Connections {
		target: root.__userAdmin

		function onRequestFailed(message) { root.requestFailed(message) }
		function onRoleCreated() { root.roleCreated() }
		function onRolesRemoved() { root.rolesRemoved() }
		function onRoleUpdated(roleId) { root.roleUpdated(roleId) }
		function onRoleDataReceived(data) { root.roleDataReceived(data) }
		function onGroupCreated() { root.groupCreated() }
		function onGroupsRemoved() { root.groupsRemoved() }
		function onGroupUpdated(groupId) { root.groupUpdated(groupId) }
		function onGroupDataReceived(data) { root.groupDataReceived(data) }
		function onUsersRemoved() { root.usersRemoved() }
		function onUserUpdated(userId) { root.userUpdated(userId) }
		function onUserDataReceived(data) { root.userDataReceived(data) }
		function onAllPermissionsReceived() { root.allPermissionsReceived() }
		function onTenantPermissionsReceived() { root.tenantPermissionsReceived() }
		function onOrganizationPermissionsReceived() { root.organizationPermissionsReceived() }
	}

	function __refreshDataProvider(provider) {
		if (!provider)
			return
		if (provider.refetch)
			provider.refetch()
		else if (provider.fetch)
			provider.fetch("")
	}

	// --- Relay membership notifications from AuthorizationController to local signals ---
	property Connections __membershipNotificationRelay: Connections {
		target: AuthorizationController

		function onTenantInvitationReceived(tenantId, tenantName, role) {
			root.subscriptionInvitationReceived(tenantId, tenantName, role)
		}
		function onTenantInvitationAccepted(tenantId, membershipId) {
			root.subscriptionInvitationAccepted(tenantId, membershipId)
		}
		function onTenantInvitationRejected(tenantId, membershipId) {
			root.subscriptionInvitationRejected(tenantId, membershipId)
		}
		function onTenantOwnershipTransferred(tenantId) {
			root.subscriptionOwnershipTransferred(tenantId)
		}
		function onTenantMembershipRoleChanged(tenantId, userId, role) {
			root.subscriptionMembershipRoleChanged(tenantId, userId, role)
		}
		function onTenantMembershipRemoved(tenantId, userId) {
			root.subscriptionMembershipRemoved(tenantId, userId)
		}
		function onMembershipNotificationsResumed() {
			root.__subscriptionsResumedTimer.restart()
		}
	}

	// --- Real-time cross-tenant message subscription notifications ---
	signal subscriptionCrossTenantMessageReceived(var notification)
	signal subscriptionCrossTenantMessageStatusChanged(var notification)
	signal subscriptionConnectionCodesChanged(var notification)

	// Notifications were lost while the connection was down: the tenant views re-read their data.
	signal subscriptionsResumed()

	// The three notification sources resume together after a reconnect; one refresh is enough.
	property Timer __subscriptionsResumedTimer: Timer {
		interval: 0
		repeat: false
		onTriggered: {
			root.subscriptionsResumed()
		}
	}

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
				"contractId": data.containsKey("contractId") ? data.getData("contractId") : "",
				"messageType": data.containsKey("messageType") ? data.getData("messageType") : "",
				"status": data.containsKey("status") ? data.getData("status") : "",
				"tenantName": data.containsKey("tenantName") ? data.getData("tenantName") : ""
			}

			// The notification center keeps the message for the owner; here only the state of the views follows
			if (notificationType === "MessageReceived" || notificationType === 0) {
				root.subscriptionCrossTenantMessageReceived(notification)
			} else if (notificationType === "MessageStatusChanged" || notificationType === 1) {
				root.subscriptionCrossTenantMessageStatusChanged(notification)
			}
		}

		onRefreshRequired: {
			root.__subscriptionsResumedTimer.restart()
		}
	}

	property SubscriptionClient __connectionCodesSubscription: SubscriptionClient {
		gqlCommandId: "OnConnectionCodesNotification"

		function getHeaders() { return {} }

		onMessageReceived: {
			if (!data) return
			var notification = {
				"notificationType": data.containsKey("notificationType") ? data.getData("notificationType") : "",
				"tenantId": data.containsKey("tenantId") ? data.getData("tenantId") : "",
				"requestId": data.containsKey("requestId") ? data.getData("requestId") : "",
				"connectionId": data.containsKey("connectionId") ? data.getData("connectionId") : ""
			}

			// The notification center tells the owners; here only the state of the views follows
			root.subscriptionConnectionCodesChanged(notification)
		}

		onRefreshRequired: {
			root.__subscriptionsResumedTimer.restart()
		}
	}

	// =========================================================================
	// GQL implementation
	// =========================================================================

	// --- Invitations ---
	property CreateTenantInvitationInput __createInvitationInput: CreateTenantInvitationInput {}
	property GqlSdlRequestSender __createInvitationSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_createTenantInvitation

		sdlObjectComp: Component {
			CreateTenantInvitationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_revokeTenantInvitation

		sdlObjectComp: Component {
			RevokeTenantInvitationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.invitationRevoked(root.__revokeInvitationInput.m_invitationId)
					}
				}
			}
		}
	}

	property ResendTenantInvitationInput __resendInvitationInput: ResendTenantInvitationInput {}
	property GqlSdlRequestSender __resendInvitationSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_resendTenantInvitation

		sdlObjectComp: Component {
			ResendTenantInvitationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						PopupManager.addSuccessMessage(qsTr("Invitation resent"), true)
						root.invitationResent(root.__resendInvitationInput.m_invitationId)
					}
				}
			}
		}
	}

	property TransferTenantOwnershipInput __transferOwnershipInput: TransferTenantOwnershipInput {}
	property GqlSdlRequestSender __transferOwnershipSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_transferTenantOwnership

		sdlObjectComp: Component {
			TransferTenantOwnershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.ownershipTransferred()
					}
				}
			}
		}
	}

	property AddMembershipInput __addMembershipInput: AddMembershipInput {}
	property GqlSdlRequestSender __addMembershipSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_addMembership

		sdlObjectComp: Component {
			AddMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.userCreated()
					}
				}
			}
		}
	}

	property RemoveElementsInput __removeUserInput: RemoveElementsInput {}
	property GqlSdlRequestSender __removeUserSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements

		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: { root.usersRemoved() }
			}
		}
	}

	// =========================================================================
	// Public methods (override the abstract stubs)
	// =========================================================================

	function createInvitation(tenantId, userId) {
		root.__createInvitationInput.m_tenantId = tenantId || ""
		root.__createInvitationInput.m_userId = userId || ""
		root.__createInvitationSender.send(root.__createInvitationInput)
	}

	function revokeInvitation(invitationId) {
		root.__revokeInvitationInput.m_invitationId = invitationId || ""
		root.__revokeInvitationSender.send(root.__revokeInvitationInput)
	}

	function resendInvitation(invitationId) {
		root.__resendInvitationInput.m_invitationId = invitationId || ""
		root.__resendInvitationSender.send(root.__resendInvitationInput)
	}

	function transferOwnership(tenantId, newOwnerId) {
		root.__transferOwnershipInput.m_tenantId = tenantId || ""
		root.__transferOwnershipInput.m_newOwnerId = newOwnerId || ""
		root.__transferOwnershipSender.send(root.__transferOwnershipInput)
	}

	function setMemberOrganizationPermissionsByUser(tenantId, userId, permissions) {
		root.__pendingOrgPermsUserId = userId || ""
		root.__pendingOrgPermsList = permissions || []
		root.__findMembershipForOrgPermsInput.m_userId = userId
		root.__findMembershipForOrgPermsInput.m_tenantId = tenantId
		root.__findMembershipForOrgPermsSender.send(root.__findMembershipForOrgPermsInput)
	}

	// pending for chaining find -> set org perms
	property string __pendingOrgPermsUserId: ""
	property var __pendingOrgPermsList: null

	function setMemberOrganizationPermissions(membershipId, permissionsList) {
		root.__updateMembershipPermissionsInput.m_membershipId = membershipId || ""
		// permissionsList should be array of strings
		// The SDL input expects list, set it
		root.__updateMembershipPermissionsInput.m_permissions = permissionsList || []
		root.__updateMembershipPermissionsSender.send(root.__updateMembershipPermissionsInput)
	}

	// Members are removed one after another: the requests share one input, and memberRemoved names the removed user.
	property var __removeMemberQueue: []
	property string __removingMemberUserId: ""

	function removeMember(tenantId, userId) {
		root.__removeMemberQueue.push({"tenantId": tenantId, "userId": userId})
		if (root.__removingMemberUserId === "") {
			root.__removeNextMember()
		}
	}

	function __removeNextMember() {
		if (root.__removeMemberQueue.length === 0) {
			root.__removingMemberUserId = ""
			return
		}

		let next = root.__removeMemberQueue.shift()
		root.__removingMemberUserId = next.userId

		// First find the membership to get its ID, then remove it
		root.__findMembershipForRemoveInput.m_userId = next.userId
		root.__findMembershipForRemoveInput.m_tenantId = next.tenantId
		root.__findMembershipForRemoveSender.send(root.__findMembershipForRemoveInput)
	}

	function removeRoles(roleIds) {
		__userAdmin.removeRoles(roleIds)
	}

	function removeGroups(groupIds) {
		__userAdmin.removeGroups(groupIds)
	}

	// --- Internal parse helpers ---

	property FindMembershipInput __findMembershipForRemoveInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForRemoveSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership

		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
						root.__removeNextMember()
					} else if (m_membership && m_membership.m_id) {
						root.__removeMembershipInput.m_membershipId = m_membership.m_id
						root.__removeMembershipSender.send(root.__removeMembershipInput)
					} else {
						root.__removeNextMember()
					}
				}
			}
		}

		function onError(message, type) {
			PopupManager.addErrorMessage(message, true)
			root.requestFailed(message)
			root.__removeNextMember()
		}
	}

	property FindMembershipInput __findMembershipForOrgPermsInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForOrgPermsSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership

		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else if (root.__pendingOrgPermsUserId && root.__pendingOrgPermsList !== null && m_membership && m_membership.m_id) {
						root.setMemberOrganizationPermissions(m_membership.m_id, root.__pendingOrgPermsList)
						root.__pendingOrgPermsUserId = ""
						root.__pendingOrgPermsList = null
					}
				}
			}
		}
	}

	// Support updating org permissions on a specific membership
	property UpdateMembershipPermissionsInput __updateMembershipPermissionsInput: UpdateMembershipPermissionsInput {}
	property GqlSdlRequestSender __updateMembershipPermissionsSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_updateMembershipPermissions

		sdlObjectComp: Component {
			UpdateMembershipPermissionsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.memberPermissionsUpdated()
					}
				}
			}
		}
	}
	signal memberPermissionsUpdated()

	property RemoveMembershipInput __removeMembershipInput: RemoveMembershipInput {}
	property GqlSdlRequestSender __removeMembershipSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_removeMembership

		sdlObjectComp: Component {
			RemoveMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.memberRemoved(root.__removingMemberUserId)
					}
					root.__removeNextMember()
				}
			}
		}

		function onError(message, type) {
			PopupManager.addErrorMessage(message, true)
			root.requestFailed(message)
			root.__removeNextMember()
		}
	}

	// =========================================================================
	// Permissions (delegated to __userAdmin's GqlBasedPermissionsProvider)
	// =========================================================================

	property alias allPermissions: __userAdminObj.allPermissions
	signal allPermissionsReceived()

	property alias tenantPermissions: __userAdminObj.tenantPermissions
	signal tenantPermissionsReceived()

	property alias organizationPermissions: __userAdminObj.organizationPermissions
	property alias memberOrganizationPermissions: __userAdminObj.memberOrganizationPermissions
	property alias administratorOrganizationPermissions: __userAdminObj.administratorOrganizationPermissions
	signal organizationPermissionsReceived()

	function setRolePermissionsTenantId(tenantId) {
		__userAdmin.setRolePermissionsTenantId(tenantId)
	}

	function fetchAllPermissions() {
		__userAdmin.fetchAllPermissions()
	}

	function fetchTenantPermissions(tenantId) {
		__userAdmin.fetchTenantPermissions(tenantId)
	}

	function invalidateTenantPermissions() {
		__userAdmin.invalidateTenantPermissions()
	}

	function fetchOrganizationPermissions(tenantId, userId) {
		__userAdmin.fetchOrganizationPermissions(tenantId, userId)
	}

	// =========================================================================
	// Connection Code, Connection Requests, Connections: the address book
	// =========================================================================

	// --- Signals ---
	signal connectionCodeReceived(string code, bool allowByCode)
	signal connectionCodeRegenerated(string newCode)
	signal allowConnectionsByCodeChanged(bool allow)
	signal connectionRequestCreated(string requestId)
	signal connectionRequestError(string errorMessage)
	signal connectionRequestApproved(string connectionId)
	signal connectionRequestRejected()
	signal connectionRequestCanceled()
	signal connectionRequestsReceived()
	signal connectionsReceived()
	signal connectionRemoved(string connectionId)

	// --- Models ---
	property ListModel connectionRequestsModel: ListModel {}
	property ListModel connectionsModel: ListModel {}

	// --- Connection Code ---
	property GetConnectionCodeInput __getConnectionCodeInput: GetConnectionCodeInput {}
	property GqlSdlRequestSender __getConnectionCodeSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getConnectionCode
		sdlObjectComp: Component {
			GetConnectionCodePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						var cc = m_connectionCode
						root.connectionCodeReceived(
							cc ? cc.m_connectionCode || "" : "",
							cc ? cc.m_allowConnectionsByCode !== false : true)
					}
				}
			}
		}
	}

	property RegenerateConnectionCodeInput __regenerateConnectionCodeInput: RegenerateConnectionCodeInput {}
	property GqlSdlRequestSender __regenerateConnectionCodeSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_regenerateConnectionCode
		sdlObjectComp: Component {
			RegenerateConnectionCodePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionCodeRegenerated(m_connectionCode || "")
					}
				}
			}
		}
	}

	property SetAllowConnectionsByCodeInput __setAllowConnectionsByCodeInput: SetAllowConnectionsByCodeInput {}
	property GqlSdlRequestSender __setAllowConnectionsByCodeSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_setAllowConnectionsByCode
		sdlObjectComp: Component {
			SetAllowConnectionsByCodePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.allowConnectionsByCodeChanged(root.__setAllowConnectionsByCodeInput.m_allow)
					}
				}
			}
		}
	}

	function getConnectionCode(tenantId) {
		root.__getConnectionCodeInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getConnectionCodeSender.send(root.__getConnectionCodeInput)
	}

	function regenerateConnectionCode(tenantId) {
		root.__regenerateConnectionCodeInput.m_tenantId = tenantId || root.tenantId || ""
		root.__regenerateConnectionCodeSender.send(root.__regenerateConnectionCodeInput)
	}

	function setAllowConnectionsByCode(tenantId, allow) {
		root.__setAllowConnectionsByCodeInput.m_tenantId = tenantId || root.tenantId || ""
		root.__setAllowConnectionsByCodeInput.m_allow = allow
		root.__setAllowConnectionsByCodeSender.send(root.__setAllowConnectionsByCodeInput)
	}

	// --- Connection Requests ---
	property CreateConnectionRequestInput __createConnectionRequestInput: CreateConnectionRequestInput {}
	property GqlSdlRequestSender __createConnectionRequestSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_createConnectionRequest
		sdlObjectComp: Component {
			CreateConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.connectionRequestError(m_errorMessage)
					} else {
						root.connectionRequestCreated(m_requestId || "")
					}
				}
			}
		}
	}

	property ApproveConnectionRequestInput __approveConnectionRequestInput: ApproveConnectionRequestInput {}
	property GqlSdlRequestSender __approveConnectionRequestSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_approveConnectionRequest
		sdlObjectComp: Component {
			ApproveConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
							root.connectionRequestError(m_errorMessage)
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestApproved(m_connectionId || "")
					}
				}
			}
		}
	}

	property RejectConnectionRequestInput __rejectConnectionRequestInput: RejectConnectionRequestInput {}
	property GqlSdlRequestSender __rejectConnectionRequestSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_rejectConnectionRequest
		sdlObjectComp: Component {
			RejectConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestRejected()
					}
				}
			}
		}
	}

	property CancelConnectionRequestInput __cancelConnectionRequestInput: CancelConnectionRequestInput {}
	property GqlSdlRequestSender __cancelConnectionRequestSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_cancelConnectionRequest
		sdlObjectComp: Component {
			CancelConnectionRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRequestCanceled()
					}
				}
			}
		}
	}

	property GetConnectionRequestsInput __getConnectionRequestsInput: GetConnectionRequestsInput {}
	property GqlSdlRequestSender __getConnectionRequestsSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getConnectionRequests
		sdlObjectComp: Component {
			GetConnectionRequestsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
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
			for (var i = 0; i < requests.count; ++i) {
				var req = requests.get(i).item
				if (!req) {
					continue
				}
				root.connectionRequestsModel.append({
					"id": req.m_id || "",
					"sourceTenantId": req.m_sourceTenantId || "",
					"sourceTenantName": req.m_sourceTenantName || "",
					"targetTenantId": req.m_targetTenantId || "",
					"targetTenantName": req.m_targetTenantName || "",
					"connectionCode": req.m_connectionCode || "",
					"message": req.m_message || "",
					"status": req.m_status || ConnectionRequestStatusEnum.s_pending,
					"createdAt": req.m_createdAt || "",
					"respondedAt": req.m_respondedAt || ""
				})
			}
		}
		root.connectionRequestsReceived()
	}

	function fetchConnectionRequests(tenantId) {
		root.__getConnectionRequestsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getConnectionRequestsSender.send(root.__getConnectionRequestsInput)
	}

	// By the connection code of the organization, or to an organization found in the directory
	function createConnectionRequest(sourceTenantId, connectionCode, message, targetTenantId) {
		root.__createConnectionRequestInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__createConnectionRequestInput.m_message = message || ""
		// The input is reused: the way of an earlier request is dropped
		if (targetTenantId && targetTenantId !== "") {
			root.__createConnectionRequestInput.m_targetTenantId = targetTenantId
			root.__createConnectionRequestInput.removeConnectionCode()
		}
		else {
			root.__createConnectionRequestInput.m_connectionCode = connectionCode || ""
			root.__createConnectionRequestInput.removeTargetTenantId()
		}
		root.__createConnectionRequestSender.send(root.__createConnectionRequestInput)
	}

	function approveConnectionRequest(requestId, tenantId) {
		root.__approveConnectionRequestInput.m_requestId = requestId || ""
		root.__approveConnectionRequestInput.m_tenantId = tenantId || root.tenantId || ""
		root.__approveConnectionRequestSender.send(root.__approveConnectionRequestInput)
	}

	function rejectConnectionRequest(requestId, tenantId) {
		root.__rejectConnectionRequestInput.m_requestId = requestId || ""
		root.__rejectConnectionRequestInput.m_tenantId = tenantId || root.tenantId || ""
		root.__rejectConnectionRequestSender.send(root.__rejectConnectionRequestInput)
	}

	function cancelConnectionRequest(requestId, tenantId) {
		root.__cancelConnectionRequestInput.m_requestId = requestId || ""
		root.__cancelConnectionRequestInput.m_tenantId = tenantId || root.tenantId || ""
		root.__cancelConnectionRequestSender.send(root.__cancelConnectionRequestInput)
	}

	// --- Connections ---
	property GetConnectionsInput __getConnectionsInput: GetConnectionsInput {}
	property GqlSdlRequestSender __getConnectionsSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getConnections
		sdlObjectComp: Component {
			GetConnectionsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateConnectionsModel(m_connections)
					}
				}
			}
		}
	}

	property RemoveConnectionInput __removeConnectionInput: RemoveConnectionInput {}
	property GqlSdlRequestSender __removeConnectionSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_removeConnection
		sdlObjectComp: Component {
			RemoveConnectionPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.connectionRemoved(root.__removeConnectionInput.m_connectionId || "")
					}
				}
			}
		}
	}

	function __populateConnectionsModel(connections) {
		root.connectionsModel.clear()
		if (connections) {
			for (var i = 0; i < connections.count; ++i) {
				var conn = connections.get(i).item
				if (!conn) {
					continue
				}
				root.connectionsModel.append({
					"id": conn.m_id || "",
					"tenantAId": conn.m_tenantAId || "",
					"tenantAName": conn.m_tenantAName || "",
					"tenantAOwnerName": conn.m_tenantAOwnerName || "",
					"tenantBId": conn.m_tenantBId || "",
					"tenantBName": conn.m_tenantBName || "",
					"tenantBOwnerName": conn.m_tenantBOwnerName || "",
					"status": conn.m_status || ConnectionStatusEnum.s_active,
					"createdAt": conn.m_createdAt || "",
					"updatedAt": conn.m_updatedAt || "",
					"partnerId": (conn.m_tenantAId === root.tenantId) ? conn.m_tenantBId : conn.m_tenantAId,
					"partnerName": (conn.m_tenantAId === root.tenantId) ? (conn.m_tenantBName || conn.m_tenantBId) : (conn.m_tenantAName || conn.m_tenantAId),
					"partnerOwnerName": (conn.m_tenantAId === root.tenantId) ? (conn.m_tenantBOwnerName || "") : (conn.m_tenantAOwnerName || "")
				})
			}
		}
		root.connectionsReceived()
	}

	function fetchConnections(tenantId) {
		root.__getConnectionsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getConnectionsSender.send(root.__getConnectionsInput)
	}

	function removeConnection(connectionId, tenantId) {
		root.__removeConnectionInput.m_connectionId = connectionId || ""
		root.__removeConnectionInput.m_tenantId = tenantId || root.tenantId || ""
		root.__removeConnectionSender.send(root.__removeConnectionInput)
	}

	// =========================================================================
	// Cross-tenant messages (phase 2)
	// =========================================================================

	property ListModel crossTenantMessagesModel: ListModel {}

	property SendCrossTenantMessageInput __sendMessageInput: SendCrossTenantMessageInput {}
	property GqlSdlRequestSender __sendMessageSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_sendCrossTenantMessage

		sdlObjectComp: Component {
			SendCrossTenantMessagePayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_updateCrossTenantMessageStatus

		sdlObjectComp: Component {
			UpdateCrossTenantMessageStatusPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.crossTenantMessageStatusUpdated(root.__pendingUpdateMessageId)
					}
				}
			}
		}
	}

	property GetCrossTenantMessagesInput __getMessagesInput: GetCrossTenantMessagesInput {
		m_direction: "Incoming"
	}
	property GqlSdlRequestSender __getMessagesSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getCrossTenantMessages

		sdlObjectComp: Component {
			GetCrossTenantMessagesPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
			for (var i = 0; i < messages.count; ++i) {
				var msg = messages.get(i).item
				if (!msg)
					continue
				root.crossTenantMessagesModel.append({
					"id": msg.m_id || "",
					"messageId": msg.m_id || "",
					"sourceTenantId": msg.m_sourceTenantId || "",
					"targetTenantId": msg.m_targetTenantId || "",
					"contractId": msg.m_contractId || "",
					"linkId": msg.m_linkId || "",
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

	// The link of the two organizations in the contract authorizes the message
	function sendCrossTenantMessage(sourceTenantId, targetTenantId, contractId, linkId, messageType, payload, sourceObjectId, customType, expiresAt) {
		root.__sendMessageInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__sendMessageInput.m_targetTenantId = targetTenantId || ""
		root.__sendMessageInput.m_contractId = contractId || ""
		root.__sendMessageInput.m_linkId = linkId || ""
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
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_confirmOrderRequest

		sdlObjectComp: Component {
			ConfirmOrderRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_rejectOrderRequest

		sdlObjectComp: Component {
			RejectOrderRequestPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_updateOrderRequestStatus

		sdlObjectComp: Component {
			UpdateOrderRequestStatusPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getOrderRequests

		sdlObjectComp: Component {
			GetOrderRequestsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
			for (var i = 0; i < orderRequests.count; ++i) {
				var ord = orderRequests.get(i).item
				if (!ord)
					continue
				root.orderRequestsModel.append({
					"orderRequestId": ord.m_id || "",
					"messageId": ord.m_messageId || "",
					"sourceTenantId": ord.m_sourceTenantId || "",
					"targetTenantId": ord.m_targetTenantId || "",
					"contractId": ord.m_contractId || "",
					"linkId": ord.m_linkId || "",
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
	// List data providers (Roles / Groups / invitable Users / Tenants)
	// =========================================================================

	property FilterableSelectGqlDataProvider roleListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "Roles"
		followsCollectionChanges: true
		pageSize: 50
	}

	property RemoteCollectionChangeListener __rolesCollectionChangeListener: RemoteCollectionChangeListener {
		collectionId: "Roles"
		currentUserId: AuthorizationController.getUserId()

		function getHeaders() {
			if (root.roleListDataProvider && root.roleListDataProvider.getHeaders)
				return root.roleListDataProvider.getHeaders()
			return {}
		}

		onChanged: {
			if (changeInfo && changeInfo.valid)
				root.__refreshDataProvider(root.roleListDataProvider)
		}
	}

	property FilterableSelectGqlDataProvider groupListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "Groups"
		followsCollectionChanges: true
		pageSize: 50
	}

	property RemoteCollectionChangeListener __groupsCollectionChangeListener: RemoteCollectionChangeListener {
		collectionId: "Groups"
		currentUserId: AuthorizationController.getUserId()

		function getHeaders() {
			if (root.groupListDataProvider && root.groupListDataProvider.getHeaders)
				return root.groupListDataProvider.getHeaders()
			return {}
		}

		onChanged: {
			if (changeInfo && changeInfo.valid)
				root.__refreshDataProvider(root.groupListDataProvider)
		}
	}

	// The groups of the organization, several chosen at once
	property FilterableSelectGqlDataProvider groupsMultiSelectDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "Groups"
		followsCollectionChanges: true
		pageSize: 50
		multiSelect: true
		excludeSelected: false
	}

	property FilterableSelectGqlDataProvider invitableUsersListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "UsersForInvitation"
		multiSelect: true
	}

	property RemoteCollectionChangeListener __invitableUsersCollectionChangeListener: RemoteCollectionChangeListener {
		collectionId: "UsersForInvitation"
		currentUserId: AuthorizationController.getUserId()

		function getHeaders() {
			if (root.invitableUsersListDataProvider && root.invitableUsersListDataProvider.getHeaders)
				return root.invitableUsersListDataProvider.getHeaders()
			return {}
		}

		onChanged: {
			if (changeInfo && changeInfo.valid)
				root.__refreshDataProvider(root.invitableUsersListDataProvider)
		}
	}

	property FilterableSelectGqlDataProvider tenantsListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "Tenants"
		pageSize: 50
	}

	property FilterableSelectGqlDataProvider connectionsDataProvider: FilterableSelectGqlDataProvider{
		collectionId: "TenantConnections"
		multiSelect: false
	}
}
