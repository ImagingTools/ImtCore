// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtassistgui 1.0

/**
	Button of the top panel that opens and closes the assistant of the application.
	Visible only while the assistant is enabled for the organization of the session.
*/
Item {
	id: assistantButton

	width: Style.buttonWidthM
	height: width

	visible: AssistantCenter.active

	ToolButton {
		id: toolButton
		objectName: "AssistantButton"

		anchors.fill: parent

		tooltipText: AssistantCenter.panelVisible ? qsTr("Close the assistant") : qsTr("Assistant")
		iconSource: "qrc:/" + Style.getIconPath("Icons/Magic", Icon.State.On, Icon.Mode.Normal)

		onClicked: {
			AssistantCenter.toggle()
		}
	}
}
