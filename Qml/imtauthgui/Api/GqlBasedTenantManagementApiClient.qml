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
import imtauthRelationshipCollectionDocumentServiceSdl 1.0
import imtauthCrossOrgGrantCollectionDocumentServiceSdl 1.0
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
	property string crossOrgGrantObjectTypeId: "CrossOrgGrant"
	property string contractObjectTypeId: "Contract"
	property string relationshipObjectTypeId: "TenantRelationship"
	property string crossTenantMessageObjectTypeId: "CrossTenantMessage"

	readonly property alias roleDocumentManager: __userAdminObj.roleDocumentManager
	readonly property alias groupDocumentManager: __userAdminObj.groupDocumentManager
	readonly property alias userDocumentManager: __userAdminObj.userDocumentManager
	readonly property var relationshipDocumentManager: __relationshipDocumentService
	readonly property var crossOrgGrantDocumentManager: __crossOrgGrantDocumentService
	readonly property alias permissionsProvider: __userAdminObj.permissionsProvider

	signal invitationCreated()
	signal invitationRevoked(string invitationId)
	signal invitationResent(string invitationId)

	signal ownershipTransferred()
	signal memberRemoved(string userId)

	signal roleCreated()
	signal rolesRemoved()
	signal crossOrgGrantsRemoved()
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

	signal contractCreated(string contractId)
	signal contractStatusUpdated(string contractId)
	signal contractTerminated(string contractId)
	signal contractsReceived(var contracts)

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
	}

	// --- Real-time cross-tenant message subscription notifications ---
	signal subscriptionCrossTenantMessageReceived(var notification)
	signal subscriptionCrossTenantMessageStatusChanged(var notification)
	signal subscriptionConnectionCodesChanged(var notification)

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

	property SubscriptionClient __connectionCodesSubscription: SubscriptionClient {
		gqlCommandId: "OnConnectionCodesNotification"

		function getHeaders() { return {} }

		onMessageReceived: {
			if (!data) return
			var notification = {
				"notificationType": data.containsKey("notificationType") ? data.getData("notificationType") : "",
				"tenantId": data.containsKey("tenantId") ? data.getData("tenantId") : "",
				"requestId": data.containsKey("requestId") ? data.getData("requestId") : ""
			}
			root.subscriptionConnectionCodesChanged(notification)
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
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_addMembership

		sdlObjectComp: Component {
			AddMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					}
					root.userCreated()
				}
			}
		}
	}

	property RemoveElementsInput __removeUserInput: RemoveElementsInput {}
	property GqlSdlRequestSender __removeUserSender: GqlSdlRequestSender {
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

	function removeMember(tenantId, userId) {
		// First find the membership to get its ID, then remove it
		root.__findMembershipForRemoveInput.m_userId = userId
		root.__findMembershipForRemoveInput.m_tenantId = tenantId
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
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_findMembership

		sdlObjectComp: Component {
			FindMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else if (m_membership && m_membership.m_id) {
						root.__removeMembershipInput.m_membershipId = m_membership.m_id
						root.__removeMembershipSender.send(root.__removeMembershipInput)
					}
				}
			}
		}
	}

	property FindMembershipInput __findMembershipForOrgPermsInput: FindMembershipInput {}
	property GqlSdlRequestSender __findMembershipForOrgPermsSender: GqlSdlRequestSender {
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
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_updateMembershipPermissions

		sdlObjectComp: Component {
			UpdateMembershipPermissionsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
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
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_removeMembership

		sdlObjectComp: Component {
			RemoveMembershipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.memberRemoved(root.__findMembershipForRemoveInput.m_userId)
					}
				}
			}
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

	function fetchOrganizationPermissions(tenantId, userId) {
		__userAdmin.fetchOrganizationPermissions(tenantId, userId)
	}

	// =========================================================================
	// Cross-org grants
	//
	// Creation and editing go through the document service
	// (crossOrgGrantDocumentManager); the list is provided by
	// crossOrgGrantsListDataProvider. Only revoke is still handled here via the
	// RevokeCrossOrgGrant mutation (Tenants.sdl).
	// =========================================================================

	property RevokeCrossOrgGrantInput __revokeCrossOrgGrantInput: RevokeCrossOrgGrantInput {}
	property GqlSdlRequestSender __revokeCrossOrgGrantSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_revokeCrossOrgGrant

		sdlObjectComp: Component {
			RevokeCrossOrgGrantPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.crossOrgGrantsRemoved()
					}
				}
			}
		}
	}

	property RemoveCrossOrgGrantsInput __removeCrossOrgGrantsInput: RemoveCrossOrgGrantsInput {}
	property GqlSdlRequestSender __removeCrossOrgGrantsSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_removeCrossOrgGrants

		sdlObjectComp: Component {
			RemoveCrossOrgGrantsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.crossOrgGrantsRemoved()
					}
				}
			}
		}
	}

	function revokeCrossOrgGrant(grantId) {
		root.__revokeCrossOrgGrantInput.m_grantId = grantId || ""
		root.__revokeCrossOrgGrantSender.send(root.__revokeCrossOrgGrantInput)
	}

	function removeCrossOrgGrants(grantIds) {
		var ids = []
		if (Array.isArray(grantIds)) {
			ids = grantIds
		} else if (grantIds && grantIds !== "") {
			ids = [grantIds]
		}
		root.__removeCrossOrgGrantsInput.m_grantIds = ids
		root.__removeCrossOrgGrantsSender.send(root.__removeCrossOrgGrantsInput)
	}

	// =========================================================================
	// Cooperation contracts (Tenants.sdl: CreateContract / UpdateContractStatus
	// / TerminateContract / GetContracts)
	// =========================================================================

	property ListModel contractsModel: ListModel {}

	property CreateContractInput __createContractInput: CreateContractInput {}
	property GqlSdlRequestSender __createContractSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_createContract

		sdlObjectComp: Component {
			CreateContractPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.contractCreated(m_contractId || "")
					}
				}
			}
		}
	}

	property UpdateContractStatusInput __updateContractStatusInput: UpdateContractStatusInput {}
	property string __pendingUpdateContractId: ""
	property GqlSdlRequestSender __updateContractStatusSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_updateContractStatus

		sdlObjectComp: Component {
			UpdateContractStatusPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.contractStatusUpdated(root.__pendingUpdateContractId)
					}
				}
			}
		}
	}

	property TerminateContractInput __terminateContractInput: TerminateContractInput {}
	property string __pendingTerminateContractId: ""
	property GqlSdlRequestSender __terminateContractSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_terminateContract

		sdlObjectComp: Component {
			TerminateContractPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.contractTerminated(root.__pendingTerminateContractId)
					}
				}
			}
		}
	}

	property GetContractsInput __getContractsInput: GetContractsInput {}
	property GqlSdlRequestSender __getContractsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getContracts

		sdlObjectComp: Component {
			GetContractsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						PopupManager.addErrorMessage(m_errorMessage, true)
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateContractsModel(m_contracts)
					}
				}
			}
		}
	}

	function __populateContractsModel(contracts) {
		root.contractsModel.clear()
		if (contracts) {
			for (var i = 0; i < contracts.count; ++i) {
				var contract = contracts.get(i).item
				if (!contract)
					continue
				root.contractsModel.append({
					"id": contract.m_id || "",
					"contractId": contract.m_id || "",
					"relationshipId": contract.m_relationshipId || "",
					"sourceTenantId": contract.m_sourceTenantId || "",
					"targetTenantId": contract.m_targetTenantId || "",
					"status": contract.m_status || ContractStatusEnum.s_draft,
					"scope": contract.m_scope || "",
					"validFrom": contract.m_validFrom || "",
					"validUntil": contract.m_validUntil || "",
					"description": contract.m_description || "",
					"terms": contract.m_terms || "",
					"createdAt": contract.m_createdAt || "",
					"updatedAt": contract.m_updatedAt || ""
				})
			}
		}
		root.contractsReceived(contracts || [])
	}

	function fetchContracts(tenantId) {
		root.__getContractsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getContractsSender.send(root.__getContractsInput)
	}

	function createContract(relationshipId, sourceTenantId, targetTenantId, scope, validFrom, validUntil, description, terms) {
		root.__createContractInput.m_relationshipId = relationshipId || ""
		root.__createContractInput.m_sourceTenantId = sourceTenantId || ""
		root.__createContractInput.m_targetTenantId = targetTenantId || ""
		root.__createContractInput.m_scope = scope || ""
		root.__createContractInput.m_validFrom = validFrom || ""
		root.__createContractInput.m_validUntil = validUntil || ""
		root.__createContractInput.m_description = description || ""
		root.__createContractInput.m_terms = terms || ""
		root.__createContractSender.send(root.__createContractInput)
	}

	function updateContractStatus(contractId, status) {
		root.__pendingUpdateContractId = contractId || ""
		root.__updateContractStatusInput.m_contractId = contractId || ""
		root.__updateContractStatusInput.m_status = status || ContractStatusEnum.s_active
		root.__updateContractStatusSender.send(root.__updateContractStatusInput)
	}

	function terminateContract(contractId) {
		root.__pendingTerminateContractId = contractId || ""
		root.__terminateContractInput.m_contractId = contractId || ""
		root.__terminateContractSender.send(root.__terminateContractInput)
	}

	// =========================================================================
	// Connection Code, Connection Requests, Connections, Relationships,
	// Relationship Proposals (new corporate model)
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
	signal tenantRelationshipRemoved()
	signal tenantRelationshipsReceived()
	signal relationshipProposalCreated()
	signal relationshipProposalApproved(string relationshipId)
	signal relationshipProposalRejected()
	signal relationshipProposalCanceled()
	signal relationshipProposalsReceived()

	// --- Models ---
	property ListModel tenantRelationshipsModel: ListModel {}
	property ListModel connectionRequestsModel: ListModel {}
	property ListModel connectionsModel: ListModel {}
	property ListModel relationshipProposalsModel: ListModel {}

	// --- Connection Code ---
	property GetConnectionCodeInput __getConnectionCodeInput: GetConnectionCodeInput {}
	property GqlSdlRequestSender __getConnectionCodeSender: GqlSdlRequestSender {
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

	function createConnectionRequest(sourceTenantId, connectionCode, message) {
		root.__createConnectionRequestInput.m_sourceTenantId = sourceTenantId || root.tenantId || ""
		root.__createConnectionRequestInput.m_connectionCode = connectionCode || ""
		root.__createConnectionRequestInput.m_message = message || ""
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

	// --- Tenant Relationships ---
	property GetTenantRelationshipsInput __getTenantRelationshipsInput: GetTenantRelationshipsInput {}
	property GqlSdlRequestSender __getTenantRelationshipsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getTenantRelationships
		sdlObjectComp: Component {
			GetTenantRelationshipsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateTenantRelationshipsModel(m_relationships)
					}
				}
			}
		}
	}

	property RemoveTenantRelationshipInput __removeTenantRelationshipInput: RemoveTenantRelationshipInput {}
	property GqlSdlRequestSender __removeTenantRelationshipSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_removeTenantRelationship
		sdlObjectComp: Component {
			RemoveTenantRelationshipPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.tenantRelationshipRemoved()
					}
				}
			}
		}
	}

	function __populateTenantRelationshipsModel(relationships) {
		root.tenantRelationshipsModel.clear()
		if (relationships) {
			for (var i = 0; i < relationships.count; ++i) {
				var rel = relationships.get(i).item
				if (!rel) {
					continue
				}
				root.tenantRelationshipsModel.append({
					"id": rel.m_id || "",
					"connectionId": rel.m_connectionId || "",
					"sourceTenantId": rel.m_sourceTenantId || "",
					"targetTenantId": rel.m_targetTenantId || "",
					"sourceRole": rel.m_sourceRole || TenantRelationshipRoleEnum.s_partner,
					"targetRole": rel.m_targetRole || TenantRelationshipRoleEnum.s_partner,
					"scope": rel.m_scope || "",
					"validFrom": rel.m_validFrom || "",
					"validUntil": rel.m_validUntil || "",
					"status": rel.m_status || RelationshipStatusEnum.s_active,
					"description": rel.m_description || "",
					"createdAt": rel.m_createdAt || "",
					"updatedAt": rel.m_updatedAt || ""
				})
			}
		}
		root.tenantRelationshipsReceived()
	}

	function fetchTenantRelationships(tenantId) {
		root.__getTenantRelationshipsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getTenantRelationshipsSender.send(root.__getTenantRelationshipsInput)
	}

	function removeTenantRelationship(tenantId, relationshipId) {
		root.__removeTenantRelationshipInput.m_tenantId = tenantId || root.tenantId || ""
		root.__removeTenantRelationshipInput.m_relationshipId = relationshipId || ""
		root.__removeTenantRelationshipSender.send(root.__removeTenantRelationshipInput)
	}

	// --- Relationship Proposals ---
	property GetRelationshipProposalsInput __getRelationshipProposalsInput: GetRelationshipProposalsInput {}
	property GqlSdlRequestSender __getRelationshipProposalsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getRelationshipProposals
		sdlObjectComp: Component {
			GetRelationshipProposalsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.__populateRelationshipProposalsModel(m_proposals)
					}
				}
			}
		}
	}

	property CreateRelationshipProposalInput __createRelationshipProposalInput: CreateRelationshipProposalInput {}
	property RelationshipProposal __createRelationshipProposalData: RelationshipProposal {}
	property GqlSdlRequestSender __createRelationshipProposalSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_createRelationshipProposal
		sdlObjectComp: Component {
			CreateRelationshipProposalPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.relationshipProposalCreated()
					}
				}
			}
		}
	}

	property ApproveRelationshipProposalInput __approveRelationshipProposalInput: ApproveRelationshipProposalInput {}
	property GqlSdlRequestSender __approveRelationshipProposalSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_approveRelationshipProposal
		sdlObjectComp: Component {
			ApproveRelationshipProposalPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.relationshipProposalApproved(m_relationshipId || "")
					}
				}
			}
		}
	}

	property RejectRelationshipProposalInput __rejectRelationshipProposalInput: RejectRelationshipProposalInput {}
	property GqlSdlRequestSender __rejectRelationshipProposalSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_rejectRelationshipProposal
		sdlObjectComp: Component {
			RejectRelationshipProposalPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.relationshipProposalRejected()
					}
				}
			}
		}
	}

	property CancelRelationshipProposalInput __cancelRelationshipProposalInput: CancelRelationshipProposalInput {}
	property GqlSdlRequestSender __cancelRelationshipProposalSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_cancelRelationshipProposal
		sdlObjectComp: Component {
			CancelRelationshipProposalPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						root.requestFailed(m_errorMessage)
					} else {
						root.relationshipProposalCanceled()
					}
				}
			}
		}
	}

	function __populateRelationshipProposalsModel(proposals) {
		root.relationshipProposalsModel.clear()
		if (proposals) {
			for (var i = 0; i < proposals.count; ++i) {
				var p = proposals.get(i).item
				if (!p) {
					continue
				}
				root.relationshipProposalsModel.append({
					"id": p.m_id || "",
					"connectionId": p.m_connectionId || "",
					"existingRelationshipId": p.m_existingRelationshipId || "",
					"proposalType": p.m_proposalType || RelationshipProposalTypeEnum.s_create,
					"initiatorTenantId": p.m_initiatorTenantId || "",
					"counterpartyTenantId": p.m_counterpartyTenantId || "",
					"proposedSourceRole": p.m_proposedSourceRole || TenantRelationshipRoleEnum.s_partner,
					"proposedTargetRole": p.m_proposedTargetRole || TenantRelationshipRoleEnum.s_partner,
					"proposedScope": p.m_proposedScope || "",
					"proposedDescription": p.m_proposedDescription || "",
					"message": p.m_message || "",
					"status": p.m_status || RelationshipProposalStatusEnum.s_pending,
					"createdAt": p.m_createdAt || "",
					"updatedAt": p.m_updatedAt || ""
				})
			}
		}
		root.relationshipProposalsReceived()
	}

	function fetchRelationshipProposals(tenantId) {
		root.__getRelationshipProposalsInput.m_tenantId = tenantId || root.tenantId || ""
		root.__getRelationshipProposalsSender.send(root.__getRelationshipProposalsInput)
	}

	function createRelationshipProposal(connectionId, initiatorTenantId, proposalType, proposedSourceRole, proposedTargetRole, proposedScope, proposedDescription, message) {
		root.__createRelationshipProposalData.m_connectionId = connectionId || ""
		root.__createRelationshipProposalData.m_initiatorTenantId = initiatorTenantId || root.tenantId || ""
		root.__createRelationshipProposalData.m_proposalType = proposalType || RelationshipProposalTypeEnum.s_create
		root.__createRelationshipProposalData.m_proposedSourceRole = proposedSourceRole || TenantRelationshipRoleEnum.s_partner
		root.__createRelationshipProposalData.m_proposedTargetRole = proposedTargetRole || TenantRelationshipRoleEnum.s_partner
		root.__createRelationshipProposalData.m_proposedScope = proposedScope || ""
		root.__createRelationshipProposalData.m_proposedDescription = proposedDescription || ""
		root.__createRelationshipProposalData.m_message = message || ""
		root.__createRelationshipProposalInput.m_proposal = root.__createRelationshipProposalData
		root.__createRelationshipProposalSender.send(root.__createRelationshipProposalInput)
	}

	function approveRelationshipProposal(proposalId, tenantId) {
		root.__approveRelationshipProposalInput.m_proposalId = proposalId || ""
		root.__approveRelationshipProposalInput.m_tenantId = tenantId || root.tenantId || ""
		root.__approveRelationshipProposalSender.send(root.__approveRelationshipProposalInput)
	}

	function rejectRelationshipProposal(proposalId, tenantId) {
		root.__rejectRelationshipProposalInput.m_proposalId = proposalId || ""
		root.__rejectRelationshipProposalInput.m_tenantId = tenantId || root.tenantId || ""
		root.__rejectRelationshipProposalSender.send(root.__rejectRelationshipProposalInput)
	}

	function cancelRelationshipProposal(proposalId, tenantId) {
		root.__cancelRelationshipProposalInput.m_proposalId = proposalId || ""
		root.__cancelRelationshipProposalInput.m_tenantId = tenantId || root.tenantId || ""
		root.__cancelRelationshipProposalSender.send(root.__cancelRelationshipProposalInput)
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
					"relationshipId": msg.m_relationshipId || "",
					"contractId": msg.m_contractId || "",
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
					"relationshipId": ord.m_relationshipId || "",
					"contractId": ord.m_contractId || "",
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
	// List data providers (Roles / Groups / invitable Users / Tenants / TenantRelationships)
	// =========================================================================

	property FilterableSelectGqlDataProvider roleListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "Roles"
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

	property FilterableSelectGqlDataProvider tenantRelationshipsListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "TenantRelationships"
		pageSize: 50
	}

	property RemoteCollectionChangeListener __tenantRelationshipsCollectionChangeListener: RemoteCollectionChangeListener {
		collectionId: "TenantRelationships"
		currentUserId: AuthorizationController.getUserId()

		function getHeaders() {
			if (root.tenantRelationshipsListDataProvider && root.tenantRelationshipsListDataProvider.getHeaders)
				return root.tenantRelationshipsListDataProvider.getHeaders()
			return {}
		}

		onChanged: {
			if (changeInfo && changeInfo.valid)
				root.__refreshDataProvider(root.tenantRelationshipsListDataProvider)
		}
	}

	property GqlBasedCollectionDocumentService __relationshipDocumentService: GqlBasedCollectionDocumentService {
		collectionId: "TenantRelationships"
	}

	property GqlBasedCollectionDocumentService __crossOrgGrantDocumentService: GqlBasedCollectionDocumentService {
		collectionId: "CrossOrgGrants"
	}

	property FilterableSelectGqlDataProvider crossOrgGrantsListDataProvider: FilterableSelectGqlDataProvider {
		collectionId: "CrossOrgGrants"
	}

	property RemoteCollectionChangeListener __crossOrgGrantsCollectionChangeListener: RemoteCollectionChangeListener {
		collectionId: "CrossOrgGrants"
		currentUserId: AuthorizationController.getUserId()

		function getHeaders() {
			if (root.crossOrgGrantsListDataProvider && root.crossOrgGrantsListDataProvider.getHeaders)
				return root.crossOrgGrantsListDataProvider.getHeaders()
			return {}
		}

		onChanged: {
			if (changeInfo && changeInfo.valid)
				root.__refreshDataProvider(root.crossOrgGrantsListDataProvider)
		}
	}

	// --- Relationship editor + representation controller ---
	property Component __relationshipEditorComp: Component {
		RelationshipView {
			apiClient: root
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: root.relationshipObjectTypeId
				}
			}
		}
	}

	property FilterableSelectGqlDataProvider connectionsDataProvider: FilterableSelectGqlDataProvider{
		collectionId: "TenantConnections"
		multiSelect: false
	}

	property Component __relationshipControllerComp: Component {
		DocumentRepresentationController {
			id: relationshipReprController

			representationModel: TenantRelationship {
				m_id: UuidGenerator.generateUUID()
				m_status: "Active"
				m_sourceRole: "Partner"
				m_targetRole: "Partner"
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				getRelationshipInput.m_id = documentId
				getRelationshipInput.m_collectionId = "TenantRelationships"
				getRelationshipRequest.send(getRelationshipInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateRelationshipInput.m_documentId = documentId
				updateRelationshipInput.m_relationship = representationModel
				updateRelationshipRequest.send(updateRelationshipInput)
			}

			property DocumentId getRelationshipInput: DocumentId {}
			property UpdateRelationshipFromRepresentationInput updateRelationshipInput: UpdateRelationshipFromRepresentationInput {}

			property GqlSdlRequestSender getRelationshipRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthRelationshipCollectionDocumentServiceSdlCommandIds.s_getRelationshipRepresentation
				sdlObjectComp: Component {
					TenantRelationship {
						onFinished: {
							relationshipReprController.representationModel.copyFrom(this)
							relationshipReprController.representationUpdated(
								relationshipReprController.documentId,
								relationshipReprController.representationModel)
						}
					}
				}

				function onError(message, type){
					relationshipReprController.updateRepresentationFailed(relationshipReprController.documentId, message)
				}
			}

			property GqlSdlRequestSender updateRelationshipRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthRelationshipCollectionDocumentServiceSdlCommandIds.s_updateRelationshipFromRepresentation
				requestType: 1
				sdlObjectComp: Component {
					DocumentOperationStatus {
						onFinished: {
							if (m_status === "Success"){
								relationshipReprController.documentUpdated(relationshipReprController.documentId)
							}
						}
					}
				}

				function onError(message, type){
					relationshipReprController.updateDocumentFailed(relationshipReprController.documentId, message)
				}
			}
		}
	}

	// --- CrossOrgGrant editor + representation controller ---
	property Component __crossOrgGrantEditorComp: Component {
		CrossOrgGrantView {
			apiClient: root
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: root.crossOrgGrantObjectTypeId
				}
			}
		}
	}

	property Component __crossOrgGrantControllerComp: Component {
		DocumentRepresentationController {
			id: crossOrgGrantReprController

			representationModel: CrossOrgGrant {
				m_id: UuidGenerator.generateUUID()
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				getCrossOrgGrantInput.m_id = documentId
				getCrossOrgGrantInput.m_collectionId = "CrossOrgGrants"
				getCrossOrgGrantInputRequest.send(getCrossOrgGrantInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateCrossOrgGrantInputInput.m_documentId = documentId
				updateCrossOrgGrantInputInput.m_grant = representationModel
				updateCrossOrgGrantRequest.send(updateCrossOrgGrantInputInput)
			}

			property DocumentId getCrossOrgGrantInput: DocumentId {}
			property UpdateGrantFromRepresentationInput updateCrossOrgGrantInputInput: UpdateGrantFromRepresentationInput {}

			property GqlSdlRequestSender getCrossOrgGrantInputRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthCrossOrgGrantCollectionDocumentServiceSdlCommandIds.s_getGrantRepresentation
				sdlObjectComp: Component {
					CrossOrgGrant {
						onFinished: {
							crossOrgGrantReprController.representationModel.copyFrom(this)
							crossOrgGrantReprController.representationUpdated(
								crossOrgGrantReprController.documentId,
								crossOrgGrantReprController.representationModel)
						}
					}
				}

				function onError(message, type){
					crossOrgGrantReprController.updateRepresentationFailed(crossOrgGrantReprController.documentId, message)
				}
			}

			property GqlSdlRequestSender updateCrossOrgGrantRequest: GqlSdlRequestSender {
				gqlCommandId: ImtauthCrossOrgGrantCollectionDocumentServiceSdlCommandIds.s_updateGrantFromRepresentation
				requestType: 1
				sdlObjectComp: Component {
					DocumentOperationStatus {
						onFinished: {
							if (m_status === "Success"){
								crossOrgGrantReprController.documentUpdated(crossOrgGrantReprController.documentId)
							}
						}
					}
				}

				function onError(message, type){
					crossOrgGrantReprController.updateDocumentFailed(crossOrgGrantReprController.documentId, message)
				}
			}
		}
	}

	Component.onCompleted: {
		root.__crossOrgGrantDocumentService.registerDocumentViewData(
			root.crossOrgGrantObjectTypeId, "Editor", root.__crossOrgGrantEditorComp, root.__crossOrgGrantControllerComp)
		root.__relationshipDocumentService.registerDocumentViewData(
			root.relationshipObjectTypeId, "Editor", root.__relationshipEditorComp, root.__relationshipControllerComp)
	}
}
