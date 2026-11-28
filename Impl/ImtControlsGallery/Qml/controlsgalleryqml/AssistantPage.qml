import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtassistgui 1.0


/**
	Example of the AI assistant (issue #1206): search, registered selectable list,
	selection, and a change that is executed only after confirmation.
	The server side uses the deterministic fake provider and in-memory demo tasks.
*/
Rectangle {
	id: assistantPage

	anchors.fill: parent
	clip: true

	color: Style.baseColor

	GqlBasedAssistantController {
		id: assistantController
	}

	Column {
		id: introColumn
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.margins: Style.marginL
		width: parent.width * 0.35
		spacing: Style.spacingM

		BaseText {
			width: parent.width
			text: qsTr("AI assistant")
			font.bold: true
			font.pixelSize: Style.fontSizeL
		}

		BaseText {
			width: parent.width
			wrapMode: Text.WordWrap
			text: qsTr("This example uses a deterministic test provider and example tasks kept in memory. No data leaves the server.")
		}

		BaseText {
			width: parent.width
			wrapMode: Text.WordWrap
			text: qsTr("1. Write \"Show open tasks\".\n2. Select a task in the list and click \"Hand over selection\".\n3. Write \"Mark it as done\".\n4. Check the proposed change and confirm or reject it.\n5. Write \"Show tasks\" to see the result.")
		}

		BaseText {
			width: parent.width
			wrapMode: Text.WordWrap
			color: Style.inactiveTextColor
			text: qsTr("Without a handed over task, the assistant asks which task you mean.")
		}
	}

	AssistantPanel {
		id: assistantPanel
		anchors.left: introColumn.right
		anchors.leftMargin: Style.marginL
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		controller: assistantController
	}
}
