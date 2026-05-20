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

/**
 * GqlBasedTenantMembershipApiClient
 *
 * GQL/SDL implementation of the abstract TenantMembershipApiClient contract.
 * This is the ONLY place that imports the membership / roles / groups SDL modules
 * and that owns GqlSdlRequestSender instances for these operations.
 *
 * Pages depend only on the abstract contract; the orchestrator (TenantEditor)
 * injects this concrete client.
 */
QtObject {
	id: root

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

	// --- Roles (ImtCollection.sdl) ---
	property InsertNewObjectInput __insertRoleInput: InsertNewObjectInput {}
	property GqlSdlRequestSender __insertRoleSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_insertNewObject

		sdlObjectComp: Component {
			InsertNewObjectPayload {
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

	property SetObjectDataInput __setRoleDataInput: SetObjectDataInput {}
	property string __pendingSetRoleId: ""
	property GqlSdlRequestSender __setRoleDataSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObjectData

		sdlObjectComp: Component {
			SetObjectDataPayload {
				onFinished: { root.roleUpdated(root.__pendingSetRoleId) }
			}
		}
	}

	property GetObjectDataInput __getRoleDataInput: GetObjectDataInput {}
	property GqlSdlRequestSender __getRoleDataSender: GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectData

		sdlObjectComp: Component {
			GetObjectDataPayload {
				onFinished: { root.__handleRoleDataReceived(m_objectData) }
			}
		}
	}

	// --- Groups (ImtCollection.sdl) ---
	property InsertNewObjectInput __insertGroupInput: InsertNewObjectInput {}
	property GqlSdlRequestSender __insertGroupSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_insertNewObject

		sdlObjectComp: Component {
			InsertNewObjectPayload {
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

	property SetObjectDataInput __setGroupDataInput: SetObjectDataInput {}
	property string __pendingSetGroupId: ""
	property GqlSdlRequestSender __setGroupDataSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObjectData

		sdlObjectComp: Component {
			SetObjectDataPayload {
				onFinished: { root.groupUpdated(root.__pendingSetGroupId) }
			}
		}
	}

	property GetObjectDataInput __getGroupDataInput: GetObjectDataInput {}
	property GqlSdlRequestSender __getGroupDataSender: GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectData

		sdlObjectComp: Component {
			GetObjectDataPayload {
				onFinished: { root.__handleGroupDataReceived(m_objectData) }
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
		// Not yet wired to a GQL endpoint server-side; emit signal so UI can react if needed.
		root.memberRoleChanged(userId, role)
	}

	function removeMember(tenantId, userId) {
		// Local removal is handled in the state manager; this hook exists for future
		// server-side membership removal endpoints.
		root.memberRemoved(userId)
	}

	function insertRole(name, description) {
		root.__insertRoleInput.m_collectionId = "Roles"
		root.__insertRoleInput.m_typeId = "Role"
		root.__insertRoleInput.m_name = name || ""
		root.__insertRoleInput.m_description = description || ""
		root.__insertRoleSender.send(root.__insertRoleInput)
	}

	function removeRole(roleId) {
		root.__pendingRemoveRoleId = roleId || ""
		root.__removeRoleInput.m_collectionId = "Roles"
		root.__removeRoleInput.m_elementIds = [roleId]
		root.__removeRoleSender.send(root.__removeRoleInput)
	}

	function setRoleData(roleId, name, description) {
		root.__pendingSetRoleId = roleId || ""
		root.__setRoleDataInput.m_collectionId = "Roles"
		root.__setRoleDataInput.m_objectId = roleId || ""
		root.__setRoleDataInput.m_objectData = JSON.stringify({
			name: name || "",
			description: description || ""
		})
		root.__setRoleDataSender.send(root.__setRoleDataInput)
	}

	function getRoleData(roleId) {
		root.__getRoleDataInput.m_collectionId = "Roles"
		root.__getRoleDataInput.m_objectId = roleId || ""
		root.__getRoleDataSender.send(root.__getRoleDataInput)
	}

	function insertGroup(name, description) {
		root.__insertGroupInput.m_collectionId = "Groups"
		root.__insertGroupInput.m_typeId = "Group"
		root.__insertGroupInput.m_name = name || ""
		root.__insertGroupInput.m_description = description || ""
		root.__insertGroupSender.send(root.__insertGroupInput)
	}

	function removeGroup(groupId) {
		root.__pendingRemoveGroupId = groupId || ""
		root.__removeGroupInput.m_collectionId = "Groups"
		root.__removeGroupInput.m_elementIds = [groupId]
		root.__removeGroupSender.send(root.__removeGroupInput)
	}

	function setGroupData(groupId, name, description) {
		root.__pendingSetGroupId = groupId || ""
		root.__setGroupDataInput.m_collectionId = "Groups"
		root.__setGroupDataInput.m_objectId = groupId || ""
		root.__setGroupDataInput.m_objectData = JSON.stringify({
			name: name || "",
			description: description || ""
		})
		root.__setGroupDataSender.send(root.__setGroupDataInput)
	}

	function getGroupData(groupId) {
		root.__getGroupDataInput.m_collectionId = "Groups"
		root.__getGroupDataInput.m_objectId = groupId || ""
		root.__getGroupDataSender.send(root.__getGroupDataInput)
	}

	// --- Internal parse helpers ---
	function __handleRoleDataReceived(objectDataJson) {
		if (!objectDataJson)
			return
		try {
			var data = JSON.parse(objectDataJson)
			root.roleDataReceived(data)
		} catch (e) {
			console.warn("Failed to parse role object data:", e)
		}
	}

	function __handleGroupDataReceived(objectDataJson) {
		if (!objectDataJson)
			return
		try {
			var data = JSON.parse(objectDataJson)
			root.groupDataReceived(data)
		} catch (e) {
			console.warn("Failed to parse group object data:", e)
		}
	}
}
