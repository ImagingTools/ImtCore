// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0

Rectangle {
	id: historyPanel

	z: parent.z + 1
	anchors.top: parent.top
	anchors.right: parent.right
	anchors.bottom: parent.bottom

	width: contentWidth
	color: Style.baseColor

	property int contentWidth: Style.sizeHintL
	property string documentId: ""
	property alias collectionId: historyView.collectionId
	property alias contentHeight: historyView.contentHeight

	Component.onCompleted: {
		historyPanel.visible = PermissionsController.checkPermission("ViewRevisions")
	}

	Rectangle {
		id: separator
		anchors.top: parent.top
		anchors.left: parent.left
		width: 1
		height: parent.height
		color: Style.borderColor
	}

	GroupHeaderView {
		id: historyHeader
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		height: titleText.height
		title: qsTr("History") + " (" + historyView.revisionsCount + ")"
	}

	DocumentHistoryView {
		id: historyView
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.top: historyHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		documentId: historyPanel.documentId

		function getHeaders(){
			return historyPanel.getHeaders()
		}
	}

	function getHeaders(){
		return {}
	}
}
