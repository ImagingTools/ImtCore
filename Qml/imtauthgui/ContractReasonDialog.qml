// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractReasonDialog
 *
 * A step of a contract that takes a reason: a refusal, a request of changes, a notice or a proposed termination.
 * The notice can be given at once (showImmediate), the termination is proposed for a day (showDate).
 *
 * The result is read in onFinished from contentItem: reason, dateIso(), isImmediate.
 */
Dialog {
	id: reasonDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	property string message: ""
	// The message while the organization leaves at once
	property string immediateMessage: ""
	property string okText: qsTr("OK")
	property string reasonName: qsTr("Reason")
	property bool isReasonRequired: true
	property bool showDate: false
	property bool showImmediate: false

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, reasonDialog.okText, !reasonDialog.isReasonRequired, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	function __updateOk(content) {
		if (!reasonDialog.buttons)
			return
		var reasonGiven = content.reason !== "" || (!reasonDialog.isReasonRequired && !content.isImmediate)
		reasonDialog.buttons.setButtonState(Enums.ok, reasonGiven)
	}

	contentComp: Component {
		Item {
			id: reasonContent

			width: reasonDialog.width
			height: reasonColumn.height + 2 * Style.marginXL

			readonly property string reason: reasonInput.text.trim()
			readonly property bool isImmediate: immediateSwitch.checked

			// The time of the API of the chosen day, empty for today
			function dateIso() {
				return ContractTexts.pickerIso(dateInput)
			}

			onReasonChanged: {
				reasonDialog.__updateOk(reasonContent)
			}

			onIsImmediateChanged: {
				reasonDialog.__updateOk(reasonContent)
			}

			Column {
				id: reasonColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				BaseText {
					width: parent.width
					visible: text !== ""
					wrapMode: Text.WordWrap
					text: immediateSwitch.checked && reasonDialog.immediateMessage !== "" ? reasonDialog.immediateMessage : reasonDialog.message
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				GroupElementView {
					width: parent.width

					TextInputElementView {
						id: reasonInput
						// Leaving at once gives the breach as the reason
						isRequired: reasonDialog.isReasonRequired || immediateSwitch.checked
						name: reasonDialog.reasonName
						placeHolderText: reasonInput.isRequired ? qsTr("Required") : qsTr("Optional")
					}

					DateTimePickerElementView {
						id: dateInput
						visible: reasonDialog.showDate
						name: qsTr("Terminate on")
						description: qsTr("Empty for today")
						canShowCurrentDate: false
					}

					SwitchElementView {
						id: immediateSwitch
						visible: reasonDialog.showImmediate
						name: qsTr("Leave at once because of a breach")
						description: qsTr("Without the notice period. The breach is the reason.")
					}
				}
			}
		}
	}
}
