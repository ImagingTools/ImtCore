// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractSignDialog
 *
 * The signature of the user for the organization, with the position of the user.
 * The signature binds the organization to the revision of the terms the user sees.
 *
 * The result is read in onFinished from contentItem: position.
 */
Dialog {
	id: signDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	property int revision: 0

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("Sign"), true, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	contentComp: Component {
		Item {
			id: signContent

			width: signDialog.width
			height: signColumn.height + 2 * Style.marginXL

			readonly property string position: positionInput.text.trim()

			Column {
				id: signColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("You sign revision %1 of the terms for your organization. A change of the terms asks for the signatures again.").arg(signDialog.revision)
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				GroupElementView {
					width: parent.width

					TextInputElementView {
						id: positionInput
						name: qsTr("Position")
						placeHolderText: qsTr("e.g. Director")
					}
				}
			}
		}
	}
}
