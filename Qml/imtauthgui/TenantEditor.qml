// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtauthTenantsSdl 1.0
import imtauthRolesSdl 1.0
import imtauthGroupsSdl 1.0

/**
 * TenantEditor
 *
 * Thin orchestrator that composes:
 *   - TenantEditorStateManager   — local UI state + pure logic
 *   - an injected `apiClient` (abstract TenantMembershipApiClient) — transport
 *   - the page components (General / Members / Roles / Groups / Permissions)
 *
 * The editor itself does NOT depend on any concrete transport (no GQL/SDL transport
 * imports). The concrete client (e.g. GqlBasedTenantMembershipApiClient from
 * imtguigql) is supplied by the embedding view (e.g. TenantCollectionView).
 *
 * SDL imports here are limited to:
 *   - imtauthTenantsSdl (TenantData type of the model)
 *   - imtauthRolesSdl / imtauthGroupsSdl (data factories for new role/group editors)
 */
DocumentViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property TenantData tenantData: model

	/**
	 * Injected transport implementing the TenantMembershipApiClient contract.
	 * Must be set by the embedding view before the editor becomes active.
	 */
	property var apiClient: null

	// --- Composition root ---
	TenantEditorStateManager {
		id: stateManager
		tenantData: container.tenantData
		apiClient: container.apiClient
	}

	// --- Convenience: data factories the pages cannot create themselves
	// (they can't import the SDL modules).
	function createRoleData() {
		var comp = Qt.createComponent("qrc:/imtauthRolesSdl/RoleData.qml")
		if (comp.status === Component.Ready)
			return comp.createObject(container)
		return null
	}

	function createGroupData() {
		var comp = Qt.createComponent("qrc:/imtauthGroupsSdl/GroupData.qml")
		if (comp.status === Component.Ready)
			return comp.createObject(container)
		return null
	}

	function updateGui() {
		stateManager.loadMembersFromModel()
		stateManager.loadInvitationsFromModel()
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
			stateManager.syncMembersToModel()
			var permissionsPage = multiPageView.getPageById("Permissions")
			if (permissionsPage)
				permissionsPage.doUpdateModel()
		}
	}

	// --- Multi-page navigation ---
	MultiPageView {
		id: multiPageView
		anchors.fill: parent

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			if (!stateManager.isNewTenant) {
				multiPageView.addPage("Members", qsTr("Members"), membersPageComp, "Icons/MultipleUser")
				if (stateManager.canManageMembers) {
					multiPageView.addPage("Roles", qsTr("Roles"), rolesPageComp, "Icons/Role")
					multiPageView.addPage("Groups", qsTr("Groups"), groupsPageComp, "Icons/MultipleUser")
				}
				if (stateManager.isCreator) {
					multiPageView.addPage("Permissions", qsTr("Permissions"), permissionsPageComp, "Icons/Role")
				}
			}
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: multiPageView.updatePages()
	}

	// Re-build pages when role / ownership state flips.
	Connections {
		target: stateManager
		function onIsNewTenantChanged() {
			multiPageView.updatePages()
			if (!stateManager.isNewTenant) {
				stateManager.loadMembersFromModel()
				stateManager.loadInvitationsFromModel()
			}
		}
		function onIsOwnerChanged() {
			if (!stateManager.isNewTenant) multiPageView.updatePages()
		}
		function onIsCreatorChanged() {
			if (!stateManager.isNewTenant) multiPageView.updatePages()
		}
		function onCanManageMembersChanged() {
			if (!stateManager.isNewTenant) multiPageView.updatePages()
		}
	}

	// --- Refresh the document when the server confirms membership changes ---
	Connections {
		target: container.apiClient
		function onInvitationCreated() {
			if (container.representationController)
				container.representationController.updateRepresentationFromDocument()
		}
		function onOwnershipTransferred() {
			if (container.representationController)
				container.representationController.updateRepresentationFromDocument()
		}
	}

	// --- Subscription: real-time membership notifications ---
	TenantMembershipSubscriptionClient {
		id: membershipSubscription

		onInvitationAccepted: {
			if (!container.tenantData || stateManager.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}

		onInvitationRejected: {
			if (!container.tenantData || stateManager.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
				stateManager.removePendingInvitation(notification.membershipId)
				if (container.representationController)
					container.representationController.updateRepresentationFromDocument()
			}
		}

		onOwnershipTransferred: {
			if (!container.tenantData || stateManager.isNewTenant)
				return
			if (notification.tenantId === container.tenantData.m_id) {
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
			stateManager: stateManager
		}
	}

	Component {
		id: membersPageComp

		TenantMembersPage {
			model: container.tenantData
			stateManager: stateManager
			apiClient: container.apiClient
		}
	}

	Component {
		id: rolesPageComp

		TenantRolesPage {
			model: container.tenantData
			stateManager: stateManager
			apiClient: container.apiClient
			roleDataFactory: container.createRoleData
		}
	}

	Component {
		id: groupsPageComp

		TenantGroupsPage {
			model: container.tenantData
			stateManager: stateManager
			apiClient: container.apiClient
			groupDataFactory: container.createGroupData
		}
	}

	Component {
		id: permissionsPageComp

		TenantPermissionsPage {
			model: container.tenantData
		}
	}
}
