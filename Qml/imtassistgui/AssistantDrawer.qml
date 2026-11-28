// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtassistgui 1.0

/**
	Side panel of the application with the assistant of AssistantCenter.
	Shown beside the pages, so that the user sees the data while the assistant prepares a change.
*/
Item {
	id: assistantDrawer
	objectName: "AssistantDrawer"

	visible: AssistantCenter.panelVisible

	Rectangle {
		id: separator

		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		width: 1
		color: Style.borderColor
	}

	AssistantPanel {
		id: panel

		anchors.left: separator.right
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		controller: AssistantCenter.controller
		closable: true

		onCloseRequested: {
			AssistantCenter.close()
		}
	}

	onVisibleChanged: {
		if (assistantDrawer.visible){
			panel.focusInput()
		}
	}
}
