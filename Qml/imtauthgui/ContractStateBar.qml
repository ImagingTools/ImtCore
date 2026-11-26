// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractStateBar
 *
 * Where a contract is: its stages (draft, negotiation, signing, in force, end) with the current one marked,
 * and every party with its own state. A part of ContractView.
 */
Column {
	id: stateBar

	spacing: Style.marginM

	// The state of the contract ("" for a new one) and its parties: [{tenantId, tenantName, state}]
	property string contractState: ""
	property var parties: []
	property string ownTenantId: ""
	// False for a contract that ended without being concluded: it skipped the signing and the force
	property bool wasConcluded: false

	readonly property int currentStage: stateBar.__stageOf(stateBar.contractState)
	// "normal", "attention" (the offer is stuck) or "ended"
	readonly property string tone: stateBar.__toneOf(stateBar.contractState)

	readonly property var stages: [qsTr("Draft"), stateBar.tone === "attention" ? ContractTexts.contractStateText(stateBar.contractState) : qsTr("Negotiation"), qsTr("Signing"), qsTr("In force"),
		stateBar.currentStage === 4 ? ContractTexts.contractStateText(stateBar.contractState) : qsTr("Ended")]

	function __stageOf(state) {
		if (state === "" || state === "Draft")
			return 0
		if (state === "Negotiation" || state === "Rejected")
			return 1
		if (state === "Signing")
			return 2
		if (state === "Signed" || state === "Active")
			return 3
		return 4
	}

	function __toneOf(state) {
		if (state === "Rejected")
			return "attention"
		if (state === "Cancelled" || state === "Terminated")
			return "danger"
		return "normal"
	}

	function __isSkipped(index) {
		return stateBar.currentStage === 4 && !stateBar.wasConcluded && (index === 2 || index === 3)
	}

	function __stageColor(index) {
		if (stateBar.__isSkipped(index))
			return Style.neutralSubtleColor
		if (index < stateBar.currentStage)
			return Style.successColor
		if (index > stateBar.currentStage)
			return Style.neutralSubtleColor
		if (stateBar.tone === "attention")
			return Style.attentionColor
		if (stateBar.tone === "danger")
			return Style.dangerColor
		return index === 4 ? Style.neutralEmphasisColor : Style.imagingToolsAccentColor
	}

	function __partyColor(state) {
		if (state === "Signed")
			return Style.successColor
		if (state === "Declined" || state === "Withdrawn")
			return Style.dangerColor
		if (state === "ChangesRequested" || state === "NoticeGiven")
			return Style.attentionColor
		if (state === "Approved")
			return Style.imagingToolsAccentColor
		return Style.neutralEmphasisColor
	}

	// The stages
	Row {
		id: stagesRow

		width: parent.width
		spacing: 0

		Repeater {
			model: stateBar.stages.length

			delegate: Item {
				id: stageItem

				width: stagesRow.width / stateBar.stages.length
				height: stageCircle.height + Style.marginXS + stageText.height

				readonly property color stageColor: stateBar.__stageColor(index)
				readonly property bool isCurrent: index === stateBar.currentStage

				// The line to the next stage
				Rectangle {
					visible: index < stateBar.stages.length - 1
					anchors.left: stageCircle.horizontalCenter
					anchors.verticalCenter: stageCircle.verticalCenter
					width: stageItem.width
					height: 2
					color: index < stateBar.currentStage && !stateBar.__isSkipped(index) && !stateBar.__isSkipped(index + 1) ? Style.successColor : Style.neutralSubtleColor
				}

				Rectangle {
					id: stageCircle

					anchors.horizontalCenter: parent.horizontalCenter
					width: stageItem.isCurrent ? Style.marginL : Style.marginM
					height: width
					radius: width / 2
					color: index <= stateBar.currentStage && !stateBar.__isSkipped(index) ? stageItem.stageColor : Style.baseColor
					border.width: 2
					border.color: stageItem.stageColor
				}

				BaseText {
					id: stageText

					anchors.top: stageCircle.bottom
					anchors.topMargin: Style.marginXS
					anchors.horizontalCenter: parent.horizontalCenter
					width: stageItem.width - Style.marginS
					horizontalAlignment: Text.AlignHCenter
					elide: Text.ElideRight
					text: stateBar.stages[index]
					font.pixelSize: Style.fontSizeS
					font.bold: stageItem.isCurrent
					color: index <= stateBar.currentStage && !stateBar.__isSkipped(index) ? Style.textColor : Style.inactiveTextColor
				}
			}
		}
	}

	// The parties and their states
	Flow {
		width: parent.width
		spacing: Style.marginS
		visible: stateBar.parties.length > 0 && stateBar.currentStage > 0 && !(stateBar.currentStage === 4 && !stateBar.wasConcluded)

		Repeater {
			model: stateBar.parties.length

			delegate: Rectangle {
				id: partyChip

				readonly property var party: stateBar.parties[index]
				readonly property color partyColor: stateBar.__partyColor(partyChip.party.state)

				width: chipText.width + 2 * Style.marginM + chipDot.width + Style.marginS
				height: Style.controlHeightS
				radius: height / 2
				color: Style.baseColor
				border.width: 1
				border.color: partyChip.partyColor

				Rectangle {
					id: chipDot
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					width: Style.marginS
					height: width
					radius: width / 2
					color: partyChip.partyColor
				}

				BaseText {
					id: chipText
					anchors.left: chipDot.right
					anchors.leftMargin: Style.marginS
					anchors.verticalCenter: parent.verticalCenter
					text: (partyChip.party.tenantName || partyChip.party.tenantId)
						+ (partyChip.party.tenantId === stateBar.ownTenantId ? " " + qsTr("(you)") : "")
						+ ": " + ContractTexts.partyStateText(partyChip.party.state)
					font.pixelSize: Style.fontSizeS
					color: Style.textColor
				}
			}
		}
	}
}
