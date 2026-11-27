// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
	AssistantPreviewCard

	Prepared change waiting for the decision of the user: target, changes and impact.
	Only an explicit click on Confirm executes the change; the card cannot be confirmed
	twice. The outcome is shown by the following server message, not by this card.
*/
Rectangle {
	id: previewCard
	objectName: "AssistantPreviewCard"

	color: Style.alternateBaseColor
	border.color: Style.imagingToolsAccentColor
	radius: Style.radiusS
	height: cardColumn.height + 2 * Style.marginM

	property string previewId: ""
	property string summary: ""
	property string targetLabel: ""
	property var changes: []
	property string impact: ""
	property string expiresAt: ""

	/**
		"" (open), "confirming", "rejecting" or "decided".
	*/
	property string decisionState: ""

	signal confirmClicked(string previewId)
	signal rejectClicked(string previewId)

	function changeText(change){
		let field = typeof change.field === "string" ? change.field : ""
		let oldValue = change.oldValue === undefined ? "" : String(change.oldValue)
		let newValue = change.newValue === undefined ? "" : String(change.newValue)

		// A change without a former value, e.g. a step of a plan with the data it sends
		if (change.oldValue === undefined){
			return qsTr("%1: %2").arg(field).arg(newValue)
		}

		return qsTr("%1: %2 → %3").arg(field).arg(oldValue).arg(newValue)
	}

	function stateText(){
		if (previewCard.decisionState === "confirming"){
			return qsTr("Executing…")
		}

		if (previewCard.decisionState === "rejecting"){
			return qsTr("Rejecting…")
		}

		if (previewCard.decisionState === "decided"){
			return qsTr("Decided")
		}

		return ""
	}

	Column {
		id: cardColumn
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Style.marginM
		spacing: Style.spacingS

		BaseText {
			width: parent.width
			text: qsTr("Proposed change")
			font.bold: true
			color: Style.imagingToolsAccentColor
		}

		BaseText {
			width: parent.width
			text: previewCard.summary
			wrapMode: Text.WordWrap
		}

		BaseText {
			width: parent.width
			visible: previewCard.targetLabel !== ""
			text: qsTr("Object: %1").arg(previewCard.targetLabel)
			wrapMode: Text.WordWrap
		}

		Repeater {
			model: previewCard.changes

			BaseText {
				width: cardColumn.width
				text: previewCard.changeText(modelData)
				wrapMode: Text.WordWrap
			}
		}

		BaseText {
			width: parent.width
			visible: previewCard.impact !== ""
			text: qsTr("Impact: %1").arg(previewCard.impact)
			wrapMode: Text.WordWrap
			color: Style.inactiveTextColor
		}

		Row {
			spacing: Style.spacingM
			visible: previewCard.decisionState === ""

			Button {
				id: confirmButton
				objectName: "AssistantConfirmButton"
				variant: "primary"
				text: qsTr("Confirm")

				onClicked: {
					previewCard.confirmClicked(previewCard.previewId)
				}
			}

			Button {
				id: rejectButton
				objectName: "AssistantRejectButton"
				text: qsTr("Reject")

				onClicked: {
					previewCard.rejectClicked(previewCard.previewId)
				}
			}
		}

		BaseText {
			width: parent.width
			visible: previewCard.decisionState !== ""
			text: previewCard.stateText()
			color: Style.inactiveTextColor
		}
	}
}
