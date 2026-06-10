// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtauthTenantsSdl 1.0

/**
 * TenantEditor
 *
 * Thin orchestrator that composes:
 *   - TenantEditorStateManager  — local UI state + pure logic
 *   - an injected `apiClient` (abstract TenantManagementApiClient) — transport
 *   - the page components (General / Members / Roles / Groups / Permissions)
 *
 * The editor itself does NOT depend on any concrete transport (no GQL/SDL transport
 * imports). The concrete client (e.g. GqlBasedTenantManagementApiClient from
 * imtauthgui) is supplied by the embedding view (e.g. TenantCollectionView).
 *
 * SDL imports here are limited to imtauthTenantsSdl (TenantData type of the model).
 */
DocumentViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	// GitHub-like typography applied locally within TenantEditor scope
	// font.family: "Segoe UI, Helvetica, Arial, sans-serif"

	property TenantData tenantData: model

	/**
	 * Injected transport implementing the TenantManagementApiClient contract.
	 * Must be set by the embedding view before the editor becomes active.
	 */
	property var apiClient: null

	/**
	 * Exposed so that TenantCollectionView can bind commandsPanelVisible: isNewTenant.
	 * After save, m_id is populated → isNewTenant becomes false → panel hides.
	 */
	readonly property bool isNewTenant: stateManager_.isNewTenant

	// --- Composition root ---
	TenantEditorStateManager {
		id: stateManager_
		tenantData: container.tenantData
		apiClient: container.apiClient
	}

	function updateGui() {
		stateManager_.loadMembersFromModel()
		stateManager_.loadInvitationsFromModel()
		var generalPage = multiPageView.getPageByIndex(0)
		if (generalPage)
			generalPage.doUpdateGui()
		var permissionsPage = multiPageView.getPageById("Permissions")
		if (permissionsPage)
			permissionsPage.doUpdateGui()
	}

	function updateModel() {
		var generalPage = multiPageView.getPageByIndex(0)
		if (generalPage)
			generalPage.doUpdateModel()
		if (container.tenantData) {
			stateManager_.syncMembersToModel()
			var permissionsPage = multiPageView.getPageById("Permissions")
			if (permissionsPage)
				permissionsPage.doUpdateModel()
		}
	}

	// --- Multi-page navigation ---
	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			if (!stateManager_.isNewTenant) {
				multiPageView.addPage("Members", qsTr("Members"), membersPageComp, "Icons/MultipleUser")
				if (stateManager_.canManageMembers) {
					multiPageView.addPage("Roles", qsTr("Roles"), rolesPageComp, "Icons/Role")
					multiPageView.addPage("Groups", qsTr("Groups"), groupsPageComp, "Icons/Organization")
				}
				if (stateManager_.isCreator) {
					multiPageView.addPage("Permissions", qsTr("Permissions"), permissionsPageComp, "Icons/Key")
					multiPageView.addPage("CrossOrgGrants", qsTr("Grants"), crossOrgGrantsPageComp, "Icons/Crown")
					// multiPageView.addPage("Contracts", qsTr("Contracts"), contractsPageComp, "Icons/Assignment")
					multiPageView.addPage("Relationships", qsTr("Relationships"), null, "Icons/Participant")
					multiPageView.addSubPage("Relationships", "ConnectionCode", qsTr("Connection Code"), connectionCodePageComp)
					multiPageView.addSubPage("Relationships", "ConnectOrganization", qsTr("Connect Organization"), connectOrganizationPageComp)
					multiPageView.addSubPage("Relationships", "MyRelationships", qsTr("My Relationships"), myConnectionsPageComp)
					// multiPageView.addPage("Messages", qsTr("Messages"), messagesPageComp, "Icons/Message")
				}
			}
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	// Re-build pages when role / ownership state flips.
	Connections {
		target: stateManager_
		function onIsNewTenantChanged() {
			multiPageView.updatePages()
			if (!stateManager_.isNewTenant) {
				stateManager_.loadMembersFromModel()
				stateManager_.loadInvitationsFromModel()
			}
		}
		function onIsOwnerChanged() {
			if (!stateManager_.isNewTenant) multiPageView.updatePages()
		}
		function onIsCreatorChanged() {
			if (!stateManager_.isNewTenant) multiPageView.updatePages()
		}
		function onCanManageMembersChanged() {
			if (!stateManager_.isNewTenant) multiPageView.updatePages()
		}
	}

	// --- Refresh the document when the server confirms membership changes ---
	Connections {
		target: container.apiClient
		function onInvitationCreated() {
			PopupManager.addSuccessMessage(qsTr("Invitation created successfully"), true)
			if (container.representationController)
				container.representationController.updateRepresentationFromDocument()
		}
		function onOwnershipTransferred() {
			PopupManager.addSuccessMessage(qsTr("Ownership transferred successfully"), true)
			if (container.representationController)
				container.representationController.updateRepresentationFromDocument()
		}
		function onRoleCreated() {
			PopupManager.addSuccessMessage(qsTr("Role created successfully"), true)
		}
		function onGroupCreated() {
			PopupManager.addSuccessMessage(qsTr("Group created successfully"), true)
		}
		function onUserCreated() {
			PopupManager.addSuccessMessage(qsTr("User created successfully"), true)
			// Refresh the tenant document so the newly added user (auto-joined
			// as Member via AddMembership in the api client) appears in the
			// TenantMembersPage list.
			if (!stateManager_.isNewTenant && container.representationController)
				container.representationController.updateRepresentationFromDocument()
		}
		function onMemberRemoved(userId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			// If the current user left/was removed, force-close the editor
			var currentUserId = container.tenantData.m_currentUserId || ""
			if (currentUserId && userId === currentUserId) {
				PopupManager.addInfoMessage(qsTr("You have left this organization. Closing editor."), true)
				if (container.documentManager && container.documentId)
					container.documentManager.closeDocument(container.documentId)
			}
		}
		function onSubscriptionInvitationAccepted(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				PopupManager.addSuccessMessage(qsTr("Invitation accepted"), true)
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
		function onSubscriptionInvitationRejected(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				PopupManager.addInfoMessage(qsTr("Invitation rejected"), true)
				stateManager_.removePendingInvitation(notification.membershipId)
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
		function onSubscriptionOwnershipTransferred(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				PopupManager.addInfoMessage(qsTr("Ownership transferred"), true)
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
		function onSubscriptionMembershipRoleChanged(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
		function onSubscriptionMembershipRemoved(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				// If the removed member is the current user, force-close the document
				var currentUserId = container.tenantData.m_currentUserId || ""
				if (currentUserId && notification.userId === currentUserId) {
					PopupManager.addInfoMessage(qsTr("You have been removed from this organization. Closing editor."), true)
					if (container.documentManager && container.documentId)
						container.documentManager.closeDocument(container.documentId)
					return
				}
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
	}

	// --- Listen to globally-broadcast tenant membership events so the editor
	// reloads even when the change was performed by the local user via another
	// view (e.g. accepting an invitation in TenantCollectionView) — the server
	// does not re-deliver a subscription notification to the actor itself.
	Connections {
		target: AuthorizationController

		function onTenantInvitationAccepted(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification && notification.tenantId === container.tenantData.m_id) {
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
		function onTenantInvitationRejected(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification && notification.tenantId === container.tenantData.m_id) {
				if (notification.membershipId)
					stateManager_.removePendingInvitation(notification.membershipId)
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
		function onTenantOwnershipTransferred(notification) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (notification && notification.tenantId === container.tenantData.m_id) {
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}
	}

	// =========================================================================
	// Page factories — each delegates to a dedicated component file.
	// =========================================================================
	Component {
		id: generalPageComp

		TenantGeneralPage {
			model: container.tenantData
			stateManager: stateManager_
		}
	}

	Component {
		id: membersPageComp

		TenantMembersPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: rolesPageComp

		TenantRolesPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: groupsPageComp

		TenantGroupsPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: permissionsPageComp

		TenantPermissionsPage {
			model: container.tenantData
		}
	}

	Component {
		id: crossOrgGrantsPageComp

		TenantCrossOrgGrantsPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: contractsPageComp

		TenantContractsPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: connectionCodePageComp

		TenantConnectionCodePage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: connectOrganizationPageComp

		TenantConnectOrganizationPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: myConnectionsPageComp

		TenantMyConnectionsPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}

	Component {
		id: messagesPageComp

		TenantMessagesPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}
}
