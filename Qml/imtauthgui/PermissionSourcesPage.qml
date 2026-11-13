import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

// Read-only page of granted permissions and where each one comes from.
Item {
	id: permissionsPage

	// SDL list of Assignment.
	property var permissions: null
	// SDL list of AccessWarning.
	property var warnings: null
	// Permission tree of the product [{groupId, groupName, entries: [{permissionId, displayName}]}].
	property var permissionGroups: []
	property string emptyText: qsTr("No permissions are granted.")
	// The inherited permissions are being recalculated.
	property bool updating: false

	property var __nameMap: null

	onPermissionGroupsChanged: {
		permissionsPage.updateNameMap()
	}

	onPermissionsChanged: {
		permissionsTable.loadAssignments(permissionsPage.permissions)
	}

	Component.onCompleted: {
		permissionsPage.updateNameMap()
		permissionsTable.loadAssignments(permissionsPage.permissions)
	}

	// The lists are refilled in place, so their properties do not change.
	function updateGui(){
		permissionsTable.loadAssignments(permissionsPage.permissions)
		permissionsTable.updateWarnings()
	}

	function updateNameMap(){
		let nameMap = ({})
		let groups = permissionsPage.permissionGroups ? permissionsPage.permissionGroups : []
		for (let i = 0; i < groups.length; i++){
			let entries = groups[i].entries ? groups[i].entries : []
			for (let k = 0; k < entries.length; k++){
				if (entries[k].displayName){
					nameMap[entries[k].permissionId] = entries[k].displayName
				}
			}
		}
		permissionsPage.__nameMap = nameMap
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
			title: qsTr("Permissions") + " (" + permissionsTable.itemsCount + ")" + (permissionsPage.updating ? "   " + qsTr("Updating...") : "")
		}

		Text {
			width: parent.width
			text: qsTr("Permissions come from roles: assigned directly, through groups and through parent roles. To change them, edit the roles or the group membership.")
			color: Style.inactiveTextColor
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeM
			wrapMode: Text.WordWrap
		}
	}

	AssignmentsTable {
		id: permissionsTable
		anchors.top: permissionsHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		x: permissionsHeader.x
		width: permissionsHeader.width
		warnings: permissionsPage.warnings
		nameMap: permissionsPage.__nameMap
		nameTitle: qsTr("Permission")
		emptyText: permissionsPage.emptyText
		filterPlaceholder: qsTr("Filter permissions...")
	}
}
