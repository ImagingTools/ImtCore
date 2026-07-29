// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantPermissionsPage
 *
 * Permissions page for TenantEditor — displays product permissions tree.
 * (Organization-specific permissions are assigned per-member via memberships, not here.)
 */
ViewBase {
	id: permissionsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: model
	property var stateManager: null
	property var apiClient: null
	property bool __permissionsRequested: false
	readonly property bool __canEditPermissions: stateManager ? stateManager.canViewOrganizationPermissions : false

	function __requestPermissionsOnce() {
		if (!permissionsPage.apiClient)
			return
		if (permissionsPage.__permissionsRequested)
			return
		permissionsPage.__permissionsRequested = true
		permissionsPage.apiClient.fetchAllPermissions()
	}

	// Fetch permissions tree when the page is first shown
	Component.onCompleted: {
		permissionsPage.__requestPermissionsOnce()
	}

	onApiClientChanged: {
		permissionsPage.__permissionsRequested = false
		permissionsPage.__requestPermissionsOnce()
	}

	Connections {
		target: permissionsPage.apiClient

		function onAllPermissionsReceived() {
			permissionsPage.__onPermissionsReceived()
		}
	}

	function __onPermissionsReceived() {
		if (!permissionsPage.apiClient)
			return
		permissionsTableView.rebuildFromFlatArray(permissionsPage.apiClient.allPermissions)
		__applyTenantSelection()
	}

	function __applyTenantSelection() {
		if (!permissionsPage.tenantData)
			return
		var selectedPermissionsIds = []
		var permissionsArray = permissionsPage.tenantData.m_tenantPermissions || []
		for (var i = 0; i < permissionsArray.length; i++)
			selectedPermissionsIds.push(permissionsArray[i])
		permissionsTableView.applySelection(selectedPermissionsIds)
	}

	function updateGui() {
		if (!permissionsPage.tenantData)
			return

		if (permissionsPage.apiClient && permissionsPage.apiClient.allPermissions
				&& permissionsPage.apiClient.allPermissions.length > 0) {
			permissionsTableView.rebuildFromFlatArray(permissionsPage.apiClient.allPermissions)
		}

		__applyTenantSelection()
	}

	function updateModel() {
		if (!permissionsPage.tenantData)
			return

		var selectedPermissionIds = permissionsTableView.getCheckedIds()

		if (!permissionsPage.tenantData.hasTenantPermissions())
			permissionsPage.tenantData.emplaceTenantPermissions()
		permissionsPage.tenantData.m_tenantPermissions = []
		for (var k = 0; k < selectedPermissionIds.length; k++)
			permissionsPage.tenantData.m_tenantPermissions.push(selectedPermissionIds[k])
	}

	Item {
		id: centeredContainer
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.8
	}

	Column {
		id: permissionsHeader
		anchors.top: centeredContainer.top
		anchors.topMargin: Style.marginXL
		anchors.left: centeredContainer.left
		anchors.right: centeredContainer.right
		spacing: Style.marginM

		Column {
			spacing: Style.marginXS

			BaseText {
				text: qsTr("Permissions")
				font.pixelSize: Style.fontSizeXL
				font.bold: true
				color: Style.textColor
			}

			BaseText {
				text: qsTr("Select which product features are available for this tenant.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}
		}
	}

	PermissionsTableView {
		id: permissionsTableView
		anchors.top: permissionsHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.bottom: parent.bottom
		anchors.left: centeredContainer.left
		anchors.right: centeredContainer.right
		readOnly: !permissionsPage.__canEditPermissions
		showControlPanel: true
		controlPanelTopMargin: 0
		treeToScrollbarSpacing: 0

		onSelectionChanged: {
			permissionsPage.doUpdateModel()
		}
	}
}
