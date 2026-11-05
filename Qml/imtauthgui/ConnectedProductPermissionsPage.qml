// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * The permission tree a product published, and when it did.
 */
Item {
	id: permissionsPage

	anchors.fill: parent

	property string productId: ""
	property var statusProvider: null
	property var permissionsProvider: null

	property string statusText: ""
	property bool isPublished: false

	Component.onCompleted: {
		if (permissionsPage.statusProvider){
			permissionsPage.statusProvider.requestStatus(permissionsPage.productId)
		}
		if (permissionsPage.permissionsProvider){
			permissionsPage.permissionsProvider.productId = permissionsPage.productId
			permissionsPage.permissionsProvider.requestAllPermissions()
		}
	}

	Connections {
		target: permissionsPage.statusProvider

		function onStatusReceived(isPublished, version, publishedAt){
			permissionsPage.isPublished = isPublished
			permissionsPage.statusText = isPublished
						? qsTr("Version %1, published %2").arg(version).arg(publishedAt)
						: qsTr("The product has not published its permissions yet: its server needs a token with the permission to publish.")
		}

		function onRequestFailed(message){
			permissionsPage.statusText = message
		}
	}

	Connections {
		target: permissionsPage.permissionsProvider

		function onAllPermissionsReceived(){
			permissionsTable.rebuildFromFlatArray(permissionsPage.permissionsProvider.allPermissions)
		}
	}

	BaseText {
		id: statusLabel
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Style.marginXL
		text: permissionsPage.statusText
		wrapMode: Text.WordWrap
	}

	PermissionsTableView {
		id: permissionsTable
		anchors.top: statusLabel.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.leftMargin: Style.marginL
		anchors.rightMargin: Style.marginL
		visible: permissionsPage.isPublished
		readOnly: true
		showControlPanel: false
	}
}
