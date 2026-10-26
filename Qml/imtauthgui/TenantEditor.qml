// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
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
		// The role, group and user editors of the organization offer only its permission scope
		if (container.apiClient && container.tenantData && container.tenantData.m_id !== "")
			container.apiClient.rolePermissionsTenantId = container.tenantData.m_id
		stateManager_.loadMembersFromModel()
		stateManager_.loadInvitationsFromModel()
		var generalPage = multiPageView.getPageByIndex(0)
		if (generalPage)
			generalPage.doUpdateGui()
		var permissionsPage = multiPageView.getPageById("Permissions")
		if (permissionsPage)
			permissionsPage.doUpdateGui()
	}

	// Runs inside this editor's change transaction on the shared model, so the pages must not open their own.
	function updateModel() {
		var generalPage = multiPageView.getPageByIndex(0)
		if (generalPage)
			generalPage.updateModel()
		if (container.tenantData) {
			stateManager_.syncMembersToModel()
			var permissionsPage = multiPageView.getPageById("Permissions")
			if (permissionsPage)
				permissionsPage.updateModel()
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
			// The creator picks the permission scope while creating the organization
			if (stateManager_.isNewTenant)
				multiPageView.addPage("Permissions", qsTr("Available permissions"), permissionsPageComp, "Icons/Key")
			if (!stateManager_.isNewTenant) {
				if (stateManager_.canViewOrganizationMembers)
					multiPageView.addPage("Members", qsTr("Members"), membersPageComp, "Icons/MultipleUser")
				if (stateManager_.canViewOrganizationRoles) {
					multiPageView.addPage("Roles", qsTr("Roles"), rolesPageComp, "Icons/Role")
				}
				if (stateManager_.canViewOrganizationGroups) {
					multiPageView.addPage("Groups", qsTr("Groups"), groupsPageComp, "Icons/Organization")
				}
				if (stateManager_.canViewOrganizationPermissions) {
					multiPageView.addPage("Permissions", qsTr("Available permissions"), permissionsPageComp, "Icons/Key")
				}
				if (stateManager_.isCreator) {
					multiPageView.addPage("CrossOrgGrants", qsTr("Grants"), crossOrgGrantsPageComp, "Icons/Crown")
					// multiPageView.addPage("Contracts", qsTr("Contracts"), contractsPageComp, "Icons/Assignment")
				}
				if (stateManager_.canViewOrganizationConnections || stateManager_.canViewOrganizationConnectionCode || stateManager_.canConnectOrganization) {
					multiPageView.addPage("Relationships", qsTr("Relationships"), null, "Icons/Participant")
					if (stateManager_.canViewOrganizationConnectionCode)
						multiPageView.addSubPage("Relationships", "ConnectionCode", qsTr("Connection Code"), connectionCodePageComp)
					if (stateManager_.canConnectOrganization || stateManager_.canViewOrganizationConnections)
						multiPageView.addSubPage("Relationships", "ConnectOrganization", qsTr("Connect Organization"), connectOrganizationPageComp)
					if (stateManager_.canViewOrganizationConnections)
						multiPageView.addSubPage("Relationships", "MyRelationships", qsTr("My Relationships"), myConnectionsPageComp)
					if (stateManager_.canViewOrganizationConnections)
						multiPageView.addSubPage("Relationships", "RelationshipProposals", qsTr("Proposals"), relationshipProposalsPageComp)
					// multiPageView.addPage("Messages", qsTr("Messages"), messagesPageComp, "Icons/Message")
				}
				// The shares belong to the organization of the session, so only its own editor shows them
				if (ApplicationInfoProvider.documentSharingEnabled
							&& (stateManager_.isAdmin || stateManager_.canViewOrganizationConnections)
							&& container.tenantData && String(container.tenantData.m_id) === AuthorizationController.getTenantId()) {
					multiPageView.addPage("DocumentShares", qsTr("Shared Documents"), documentSharesPageComp, "Icons/Share")
				}
			}
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			container.__builtPagesKey = container.__pagesKey()
			multiPageView.updatePages()
		}
	}

	// The page list follows what the pages depend on, including the organization of the session for the shares
	function __pagesKey() {
		var sessionTenant = container.tenantData && String(container.tenantData.m_id) === AuthorizationController.getTenantId()
		return stateManager_.pagesConfigKey + (sessionTenant ? "S" : "")
	}

	property string __builtPagesKey: ""

	// A representation fills the model field by field and every field flips some of the flags: the pages are rebuilt
	// once, after the model is filled, and only when the list really changes (a rebuild recreates the shown page).
	Timer {
		id: pagesUpdateTimer
		interval: 0
		onTriggered: {
			var key = container.__pagesKey()
			if (key === container.__builtPagesKey)
				return
			container.__builtPagesKey = key
			multiPageView.updatePages()
		}
	}

	Connections {
		target: stateManager_
		function onIsNewTenantChanged() {
			pagesUpdateTimer.restart()
			if (!stateManager_.isNewTenant) {
				stateManager_.loadMembersFromModel()
				stateManager_.loadInvitationsFromModel()
			}
		}
		function onPagesConfigKeyChanged() {
			pagesUpdateTimer.restart()
		}
	}

	Connections {
		target: container.tenantData
		function onM_idChanged() {
			pagesUpdateTimer.restart()
		}
	}

	// The reasons to read the organization again come in bursts: an own change and its notification,
	// a notification relayed by the API client and by AuthorizationController. One reading serves them all.
	function __refreshRepresentation() {
		if (!stateManager_.isNewTenant && container.representationController)
			representationRefreshTimer.restart()
	}

	Timer {
		id: representationRefreshTimer
		interval: 100
		onTriggered: {
			if (!stateManager_.isNewTenant && container.representationController)
				container.representationController.updateRepresentationFromDocument()
		}
	}

	// Members invited by another manager are not notified: the members are read again when their page is shown again
	Connections {
		target: stateManager_
		function onMembersRefreshRequested() {
			container.__refreshRepresentation()
		}
	}

	// --- Refresh the document when the server confirms membership changes ---
	Connections {
		target: container.apiClient ? container.apiClient.userDocumentManager : null
		function onDocumentSaved(documentId) {
			container.__refreshRepresentation()
		}
	}

	Connections {
		target: container.apiClient
		function onInvitationCreated() {
			PopupManager.addSuccessMessage(qsTr("Invitation created successfully"), true)
			container.__refreshRepresentation()
		}
		function onOwnershipTransferred() {
			PopupManager.addSuccessMessage(qsTr("Ownership transferred successfully"), true)
			container.__refreshRepresentation()
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
			container.__refreshRepresentation()
		}
		function onMemberRemoved(userId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			// If the current user left/was removed, force-close the editor
			var currentUserId = container.tenantData.m_currentUserId || ""
			if (currentUserId && userId === currentUserId) {
				PopupManager.addInfoMessage(qsTr("You have left this organization. Closing editor."), true)
				if (AuthorizationController.currentTenantId === container.tenantData.m_id)
					AuthorizationController.selectTenant("")
				if (container.documentManager && container.documentId)
					container.documentManager.closeDocument(container.documentId)
			}
		}
		function onSubscriptionInvitationAccepted(tenantId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				container.__refreshRepresentation()
			}
		}
		function onSubscriptionInvitationRejected(tenantId, membershipId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				stateManager_.removePendingInvitation(membershipId)
				container.__refreshRepresentation()
			}
		}
		function onSubscriptionOwnershipTransferred(tenantId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				container.__refreshRepresentation()
			}
		}
		function onSubscriptionMembershipRoleChanged(tenantId, userId, role) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				container.__refreshRepresentation()
			}
		}
		function onSubscriptionMembershipRemoved(tenantId, userId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				// If the removed member is the current user, force-close the document
				var currentUserId = container.tenantData.m_currentUserId || ""
				if (currentUserId && userId === currentUserId) {
					PopupManager.addInfoMessage(qsTr("You have been removed from this organization. Closing editor."), true)
					if (AuthorizationController.currentTenantId === container.tenantData.m_id)
						AuthorizationController.selectTenant("")
					if (container.documentManager && container.documentId)
						container.documentManager.closeDocument(container.documentId)
					return
				}
				container.__refreshRepresentation()
			}
		}
	}

	// --- Listen to globally-broadcast tenant membership events so the editor
	// reloads even when the change was performed by the local user via another
	// view (e.g. accepting an invitation in TenantCollectionView) — the server
	// does not re-deliver a subscription notification to the actor itself.
	Connections {
		target: AuthorizationController

		function onTenantInvitationAccepted(tenantId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				container.__refreshRepresentation()
			}
		}
		function onTenantInvitationRejected(tenantId, membershipId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				if (membershipId)
					stateManager_.removePendingInvitation(membershipId)
				container.__refreshRepresentation()
			}
		}
		function onTenantOwnershipTransferred(tenantId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				container.__refreshRepresentation()
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
			stateManager: stateManager_
			apiClient: container.apiClient
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
		id: documentSharesPageComp

		TenantDocumentSharesPage {
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
		id: relationshipProposalsPageComp

		TenantRelationshipProposalsPage {
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
