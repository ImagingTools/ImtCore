// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtnotifygui 1.0

// Filter / sort bar for the Events page. All selections drive the server-side
// query through ActivityController (which forwards them to the transport).
Item {
	id: filterBar

	implicitHeight: Style.controlHeightL

	// Maps the severity segment index to the service severity value
	// (-1 = any; Info=0, Success=1, Warning=2, Critical=3).
	function severityForIndex(index){
		if (index <= 0){
			return -1
		}
		return index - 1
	}

	SegmentedButton {
		id: statusFilter

		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		height: Style.controlHeightM
		isExclusive: true

		onClicked: ActivityController.setUnreadOnly(index === 1)

		Button {
			id: allButton

			anchors.verticalCenter: parent.verticalCenter
			checkable: true
			checked: true
			text: qsTr("All")
			widthFromDecorator: true
			heightFromDecorator: true
		}

		Button {
			id: unreadButton

			anchors.verticalCenter: parent.verticalCenter
			checkable: true
			text: qsTr("Unread")
			widthFromDecorator: true
			heightFromDecorator: true
		}
	}

	SegmentedButton {
		id: severityFilter

		anchors.left: statusFilter.right
		anchors.leftMargin: Style.marginL
		anchors.verticalCenter: parent.verticalCenter
		height: Style.controlHeightM
		isExclusive: true

		onClicked: ActivityController.setSeverity(filterBar.severityForIndex(index))

		Button {
			id: severityAll

			anchors.verticalCenter: parent.verticalCenter
			checkable: true
			checked: true
			text: qsTr("Any")
			widthFromDecorator: true
			heightFromDecorator: true
		}

		Button {
			id: severityInfo

			anchors.verticalCenter: parent.verticalCenter
			checkable: true
			text: qsTr("Info")
			widthFromDecorator: true
			heightFromDecorator: true
		}

		Button {
			id: severitySuccess

			anchors.verticalCenter: parent.verticalCenter
			checkable: true
			text: qsTr("Success")
			widthFromDecorator: true
			heightFromDecorator: true
		}

		Button {
			id: severityWarning

			anchors.verticalCenter: parent.verticalCenter
			checkable: true
			text: qsTr("Warning")
			widthFromDecorator: true
			heightFromDecorator: true
		}

		Button {
			id: severityCritical

			anchors.verticalCenter: parent.verticalCenter
			checkable: true
			text: qsTr("Critical")
			widthFromDecorator: true
			heightFromDecorator: true
		}
	}

	ToolButton {
		id: sortButton

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		width: Style.controlHeightM
		height: Style.controlHeightM
		tooltipText: ActivityController.sortOrder === 0 ? qsTr("Newest first") : qsTr("Oldest first")
		iconSource: ActivityController.sortOrder === 0
					? Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
					: Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal)
		onClicked: ActivityController.setSortOrder(ActivityController.sortOrder === 0 ? 1 : 0)
	}
}
