// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * The permissions a server token holds at its receiving server; the ones every token holds are checked and locked.
 */
Rectangle {
	id: tokenPermissionsView

	color: Style.baseColor
	border.width: 1
	border.color: Style.borderColor
	radius: Style.radiusM
	clip: true

	property string tableObjectName: ""
	property bool hasPermissions: false

	// groups as PermissionsTableView.rebuildFromFlatArray takes them
	function setPermissions(groups, requiredPermissions, scopes){
		tokenPermissionsView.hasPermissions = groups.length > 0

		permissionsTable.rebuildFromFlatArray(groups)
		permissionsTable.applySelection(scopes)

		let locked = {}
		for (let i = 0; i < requiredPermissions.length; ++i){
			locked[requiredPermissions[i]] = qsTr("Every token for this server holds it")
		}
		permissionsTable.setLockedPermissions(locked)
	}

	function getCheckedIds(){
		return permissionsTable.getCheckedIds(false)
	}

	PermissionsTableView {
		id: permissionsTable
		objectName: tokenPermissionsView.tableObjectName
		anchors.fill: parent
		anchors.leftMargin: Style.marginM
		anchors.rightMargin: Style.marginM
		anchors.topMargin: Style.marginXS
		anchors.bottomMargin: Style.marginS
		controlPanelTopMargin: Style.marginS
		treeTopMargin: Style.marginXS
		treeBottomMargin: Style.marginS
		visible: tokenPermissionsView.hasPermissions
	}

	BaseText {
		anchors.centerIn: parent
		width: parent.width - 2 * Style.marginXL
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
		text: qsTr("The server has no permissions to give to this token.")
		color: Style.inactiveTextColor
		visible: !tokenPermissionsView.hasPermissions
	}
}
