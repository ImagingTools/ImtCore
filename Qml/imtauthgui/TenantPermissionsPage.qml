// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * TenantPermissionsPage
 *
 * Permissions tab of the TenantEditor — tree of product permissions and the
 * selected permissions for this tenant.
 *
 * No SDL imports: the tenantData model is provided by the orchestrator via the
 * inherited ViewBase `model` property; updates go through doUpdateGui/doUpdateModel.
 */
ViewBase {
	id: permissionsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: model
	property var stateManager: null
	readonly property bool __canEditPermissions: stateManager ? stateManager.canViewOrganizationPermissions : false

	function updateGui() {
		if (!permissionsPage.tenantData)
			return

		var allPermsModel = permissionsPage.tenantData.m_allProductPermissions
		var nodes = TreeModelBuilder.fromListModel(
			allPermsModel,
			function(item, index) {
				var perm = item && item.item ? item.item : item
				return {
					key: perm ? (perm.m_id || "") : "",
					text: perm ? (perm.m_name || "") : "",
					checkable: true,
					expanded: true,
					data: {
						id: perm ? (perm.m_id || "") : "",
						name: perm ? (perm.m_name || "") : "",
						description: perm ? (perm.m_description || "") : ""
					}
				}
			},
			function(item, index) {
				var perm = item && item.item ? item.item : item
				var children = perm ? perm.m_children : null
				return (children && (children.count || 0) > 0) ? children : null
			}
		)

		tenantPermissionsTreeView.model = nodes

		var selectedPermissionsIds = []
		var permissionsArray = permissionsPage.tenantData.m_tenantPermissions
		if (permissionsArray) {
			var permCount = permissionsArray.length || 0
			for (var pi2 = 0; pi2 < permCount; pi2++)
				selectedPermissionsIds.push(permissionsArray[pi2])
		}

		tenantPermissionsTreeView.uncheckAll()

		var allNodesList = tenantPermissionsTreeView.allNodes()
		for (var i = 0; i < allNodesList.length; i++) {
			var nodeObj = allNodesList[i]
			var nodeChildren = nodeObj.children || []
			if (nodeChildren.length === 0) {
				var nodeData = nodeObj.data || {}
				var id = nodeData.id
				if (selectedPermissionsIds.includes(id))
					tenantPermissionsTreeView.checkItem(nodeObj.key)
			}
		}
	}

	function updateModel() {
		if (!permissionsPage.tenantData)
			return
		var selectedPermissionIds = []
		var checkedNodes = tenantPermissionsTreeView.getCheckedNodes()
		for (var j = 0; j < checkedNodes.length; j++) {
			var nodeObj = checkedNodes[j]
			var nodeChildren = nodeObj.children || []
			if (nodeChildren.length === 0) {
				var nodeData = nodeObj.data || {}
				var id = nodeData.id
				selectedPermissionIds.push(id)
			}
		}

		selectedPermissionIds.sort()

		if (!permissionsPage.tenantData.hasTenantPermissions())
			permissionsPage.tenantData.emplaceTenantPermissions()
		permissionsPage.tenantData.m_tenantPermissions = []
		for (var k = 0; k < selectedPermissionIds.length; k++)
			permissionsPage.tenantData.m_tenantPermissions.push(selectedPermissionIds[k])
	}

	// Centered fixed-width container
	property int maxTableWidth: 1000

	Item {
		id: centeredContainer
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, permissionsPage.maxTableWidth)
	}

	// --- Fixed header area ---
	Column {
		id: permissionsHeader
		anchors.top: centeredContainer.top
		anchors.topMargin: Style.marginXL
		anchors.left: centeredContainer.left
		anchors.right: centeredContainer.right
		spacing: Style.marginM

		Row {
			width: parent.width
			spacing: Style.marginM

			Column {
				anchors.verticalCenter: parent.verticalCenter
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

			Item {
				width: parent.width
					- parent.children[0].width
					- permCheckAllBtn.width
					- permUncheckAllBtn.width
					- permExpandBtn.width
					- permCollapseBtn.width
					- parent.spacing * 5
				height: 1
			}

			Text {
				id: permCheckAllBtn
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Check All")
				font.pixelSize: Style.fontSizeM
				color: permissionsPage.__canEditPermissions ? Style.linkColor : Style.inactiveTextColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: permissionsPage.__canEditPermissions ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: permissionsPage.__canEditPermissions
					onClicked: tenantPermissionsTreeView.checkAll()
				}
			}

			Text {
				id: permUncheckAllBtn
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Uncheck All")
				font.pixelSize: Style.fontSizeM
				color: permissionsPage.__canEditPermissions ? Style.linkColor : Style.inactiveTextColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: permissionsPage.__canEditPermissions ? Qt.PointingHandCursor : Qt.ArrowCursor
					enabled: permissionsPage.__canEditPermissions
					onClicked: tenantPermissionsTreeView.uncheckAll()
				}
			}

			Text {
				id: permExpandBtn
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Expand All")
				font.pixelSize: Style.fontSizeM
				color: Style.linkColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: tenantPermissionsTreeView.expandAll()
				}
			}

			Text {
				id: permCollapseBtn
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Collapse All")
				font.pixelSize: Style.fontSizeM
				color: Style.linkColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: tenantPermissionsTreeView.collapseAll()
				}
			}
		}

		SearchTextInput {
			id: permissionsFilterInput
			width: parent.width
			placeHolderText: qsTr("Filter permissions...")
			onTextChanged: tenantPermissionsTreeView.filterText = text
		}
	}

	CustomScrollbar {
		id: permissionsScrollbar
		z: parent.z + 1
		anchors.right: centeredContainer.right
		anchors.top: tenantPermissionsTreeView.top
		anchors.bottom: tenantPermissionsTreeView.bottom
		secondSize: Style.marginM
		targetItem: tenantPermissionsTreeView.contentListView
	}

	BasicTreeView {
		id: tenantPermissionsTreeView
		anchors.top: permissionsHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: centeredContainer.left
		anchors.right: permissionsScrollbar.left
		anchors.rightMargin: Style.marginXL
		showHeader: true
		columns: [
			{ id: "name", name: qsTr("Permission"), tree: true },
			{ id: "description", name: qsTr("Description"), tree: false }
		]
		filterRole: ["name", "description"]

		onCheckedItemsChanged: {
			if (permissionsPage.__canEditPermissions)
				permissionsPage.doUpdateModel()
		}
	}
}
