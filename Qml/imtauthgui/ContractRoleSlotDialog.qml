// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractRoleSlotDialog
 *
 * How many parties of a role a type of contracts takes: at least and at most, 0 at most for any number.
 *
 * The result is read in onFinished from contentItem: roleId, roleName, minCount, maxCount.
 */
Dialog {
	id: slotDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintL))

	// The roles that can get a slot: [{roleId, name}]
	property var roles: []
	property string initialRoleId: ""
	property int initialMinCount: 1
	property int initialMaxCount: 0

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("OK"), slotDialog.initialRoleId !== "", false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
		if (slotDialog.contentItem)
			slotDialog.contentItem.init()
	}

	contentComp: Component {
		Item {
			id: slotContent

			width: slotDialog.width
			height: slotColumn.height + 2 * Style.marginXL

			readonly property string roleId: roleModel.idAt(roleCB.currentIndex)
			readonly property string roleName: roleModel.nameAt(roleCB.currentIndex)
			readonly property int minCount: minInput.value
			readonly property int maxCount: maxInput.value

			onRoleIdChanged: {
				if (slotDialog.buttons)
					slotDialog.buttons.setButtonState(Enums.ok, slotContent.roleId !== "")
			}

			function init() {
				roleCB.currentIndex = roleModel.fill(slotDialog.roles, "roleId", "name", slotDialog.initialRoleId)
				minInput.value = slotDialog.initialMinCount
				maxInput.value = slotDialog.initialMaxCount
			}

			ChoiceModel {
				id: roleModel
			}

			Column {
				id: slotColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				GroupElementView {
					width: parent.width

					ComboBoxElementView {
						id: roleCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose the role")
						name: qsTr("Role")
						model: roleModel
						changeable: slotDialog.initialRoleId === ""
					}

					SpinBoxElementView {
						id: minInput
						name: qsTr("At least")
						from: 0
						to: 99
					}

					SpinBoxElementView {
						id: maxInput
						name: qsTr("At most")
						description: qsTr("0: any number")
						from: 0
						to: 99
					}
				}
			}
		}
	}
}
