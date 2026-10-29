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
 * Permission scope of the organization: the product permissions its roles and owner may hold.
 * Only the creator edits it; a sub-organization chooses within the scope of its parent.
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
	readonly property bool __canEditPermissions: stateManager ? stateManager.canEditTenantPermissions : false
	readonly property bool __isSubOrganization: stateManager ? stateManager.isSubOrganization : false
	readonly property bool __isOwner: stateManager ? stateManager.isOwner : false
	readonly property bool __isNewTenant: stateManager ? stateManager.isNewTenant : true
	property bool __showSelectedOnly: false
	property int __availableCount: 0
	// The scope last applied, restored when narrowing it is cancelled
	property var __appliedPermissionIds: []

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
		permissionsTableView.rebuildFromFlatArray(permissionsPage.__availableGroups(permissionsPage.apiClient.allPermissions))
		__applyTenantSelection()
	}

	// Read-only the tree shows the scope itself; a sub-organization chooses within the scope of its parent
	function __availableGroups(groups) {
		var retVal = groups ? groups : []
		var parentPermissions = permissionsPage.tenantData ? permissionsPage.tenantData.m_parentTenantPermissions : null
		if (permissionsPage.__canEditPermissions && permissionsPage.__isSubOrganization && parentPermissions)
			retVal = permissionsPage.__filterGroups(retVal, parentPermissions)

		// The scope is never hidden: a chosen permission the product tree lacks is listed apart
		var listed = {}
		var count = 0
		for (var i = 0; i < retVal.length; i++) {
			var entries = retVal[i].entries || []
			for (var e = 0; e < entries.length; e++)
				listed[String(entries[e].permissionId)] = true
			count += entries.length
		}
		var scope = permissionsPage.tenantData ? (permissionsPage.tenantData.m_tenantPermissions || []) : []
		var otherEntries = []
		for (var s = 0; s < scope.length; s++) {
			var permissionId = String(scope[s])
			if (!listed[permissionId])
				otherEntries.push({permissionId: permissionId, displayName: permissionId, description: ""})
		}
		if (otherEntries.length > 0) {
			retVal = retVal.concat([{groupId: "OtherPermissions", groupName: qsTr("Other permissions"), entries: otherEntries}])
			count += otherEntries.length
		}

		permissionsPage.__availableCount = count
		return retVal
	}

	function __filterGroups(groups, permissionIds) {
		var allowed = {}
		for (var i = 0; i < permissionIds.length; i++)
			allowed[String(permissionIds[i])] = true
		var retVal = []
		for (var gi = 0; gi < groups.length; gi++) {
			var entries = groups[gi].entries || []
			var allowedEntries = []
			for (var ei = 0; ei < entries.length; ei++) {
				if (allowed[String(entries[ei].permissionId)])
					allowedEntries.push(entries[ei])
			}
			if (allowedEntries.length > 0)
				retVal.push({groupId: groups[gi].groupId, groupName: groups[gi].groupName, entries: allowedEntries})
		}
		return retVal
	}

	function __applyTenantSelection() {
		if (!permissionsPage.tenantData)
			return
		var selectedPermissionsIds = []
		var permissionsArray = permissionsPage.tenantData.m_tenantPermissions || []
		for (var i = 0; i < permissionsArray.length; i++)
			selectedPermissionsIds.push(String(permissionsArray[i]))
		permissionsPage.__appliedPermissionIds = selectedPermissionsIds
		permissionsTableView.applySelection(selectedPermissionsIds)
	}

	function updateGui() {
		if (!permissionsPage.tenantData)
			return

		if (permissionsPage.apiClient && permissionsPage.apiClient.allPermissions
				&& permissionsPage.apiClient.allPermissions.length > 0) {
			permissionsTableView.rebuildFromFlatArray(permissionsPage.__availableGroups(permissionsPage.apiClient.allPermissions))
		}

		__applyTenantSelection()
	}

	// Removing permissions of a stored organization takes them from its owner, roles and sub-organizations
	function __onSelectionChanged() {
		var checkedIds = permissionsTableView.getCheckedIds()
		var removedCount = 0
		for (var i = 0; i < permissionsPage.__appliedPermissionIds.length; i++) {
			if (checkedIds.indexOf(permissionsPage.__appliedPermissionIds[i]) < 0)
				removedCount++
		}

		if (removedCount === 0 || permissionsPage.__isNewTenant) {
			permissionsPage.__appliedPermissionIds = checkedIds
			permissionsPage.doUpdateModel()
			return
		}

		ModalDialogManager.showConfirmationDialog(
			qsTr("Remove permissions"),
			qsTr("%1 permission(s) will be taken from the owner, the roles and the sub-organizations of this organization. Continue?").arg(removedCount),
			function(result) {
				if (result === Enums.yes) {
					permissionsPage.__appliedPermissionIds = checkedIds
					permissionsPage.doUpdateModel()
				}
				else {
					permissionsTableView.applySelection(permissionsPage.__appliedPermissionIds)
				}
			}
		)
	}

	function __infoText() {
		if (!permissionsPage.__canEditPermissions) {
			var creatorName = permissionsPage.tenantData ? permissionsPage.tenantData.m_creatorName : ""
			var chosenBy = creatorName !== ""
				? qsTr("The permissions of this organization are chosen by its creator %1.").arg(creatorName)
				: qsTr("The permissions of this organization are chosen by its creator.")
			var holder = permissionsPage.__isOwner
				? qsTr("You are the owner: all of them are yours already.")
				: qsTr("The owner holds all of them.")
			return chosenBy + " " + holder + " " + qsTr("Roles and members can be given only these permissions.")
		}

		var text = qsTr("Choose the product features available to this organization. The owner holds every chosen permission; roles and members can be given only them.")
		if (permissionsPage.__isSubOrganization)
			text += " " + qsTr("Only the permissions of the parent organization can be chosen.")
		if (!permissionsPage.__isNewTenant)
			text += " " + qsTr("A removed permission is taken from the owner, the roles and the sub-organizations.")
		return text
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

	Column {
		id: permissionsHeader
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		x: Math.max(0, (permissionsPage.width - width) / 2)
		width: Math.max(0, Math.min(Style.contentWidthMax, permissionsPage.width - 2 * Style.marginXL))
		spacing: Style.marginM

		GroupHeaderView {
			width: parent.width
			title: permissionsPage.__canEditPermissions
				? qsTr("Available permissions (%1 of %2)").arg(permissionsTableView.checkedCount).arg(permissionsPage.__availableCount)
				: qsTr("Available permissions (%1)").arg(permissionsTableView.checkedCount)
			controlComp: Component {
				CheckBox {
					anchors.verticalCenter: parent.verticalCenter
					visible: permissionsPage.__canEditPermissions
					text: qsTr("Selected only")

					onCheckStateChanged: {
						permissionsPage.__showSelectedOnly = checkState === Qt.Checked
					}
				}
			}
		}

		Rectangle {
			width: parent.width
			height: infoText.height + 2 * Style.marginM
			radius: Style.marginXS
			color: Style.alternateBaseColor
			border.width: 1
			border.color: Style.borderColor

			BaseText {
				id: infoText
				x: Style.marginL
				y: Style.marginM
				width: parent.width - 2 * Style.marginL
				text: permissionsPage.__infoText()
				font.pixelSize: Style.fontSizeS
				color: Style.textColor
				wrapMode: Text.WordWrap
			}
		}
	}

	// The tree gets the page's height and scrolls itself, so only its visible rows are created.
	Rectangle {
		id: permissionsFrame
		anchors.top: permissionsHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		x: permissionsHeader.x
		width: permissionsHeader.width
		color: Style.baseColor
		border.width: 1
		border.color: Style.borderColor
		radius: Style.marginXS

		PermissionsTableView {
			id: permissionsTableView
			anchors.fill: parent
			anchors.leftMargin: Style.marginL
			anchors.rightMargin: Style.marginL
			readOnly: !permissionsPage.__canEditPermissions
			// The owner and the members read the scope as a table of the chosen permissions
			selectedOnly: !permissionsPage.__canEditPermissions || permissionsPage.__showSelectedOnly
			showControlPanel: true
			treeToScrollbarSpacing: 0
			controlPanelTopMargin: Style.marginL
			treeTopMargin: Style.marginL
			treeBottomMargin: Style.marginL

			onSelectionChanged: {
				permissionsPage.__onSelectionChanged()
			}
		}
	}
}
