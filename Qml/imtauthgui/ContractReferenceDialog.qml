// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractReferenceDialog
 *
 * A reference of a contract to another sent contract of the organization: the one it replaces,
 * the framework contract it is concluded under, or a related one.
 *
 * The result is read in onFinished from contentItem: contractId, contractNumber, contractName, referenceType.
 */
Dialog {
	id: referenceDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))

	// [{id, number, name}]
	property var contracts: []

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		addButton(Enums.ok, qsTr("Add"), referenceDialog.contracts.length > 0, false, "primary")
		addButton(Enums.cancel, qsTr("Cancel"), true)
		if (referenceDialog.contentItem)
			referenceDialog.contentItem.init()
	}

	contentComp: Component {
		Item {
			id: referenceContent

			width: referenceDialog.width
			height: referenceColumn.height + 2 * Style.marginXL

			readonly property string contractId: contractModel.idAt(contractCB.currentIndex)
			readonly property var contract: referenceContent.__contractById(referenceContent.contractId)
			readonly property string contractNumber: referenceContent.contract ? (referenceContent.contract.number || "") : ""
			readonly property string contractName: referenceContent.contract ? (referenceContent.contract.name || "") : ""
			readonly property string referenceType: typeModel.idAt(typeCB.currentIndex) !== "" ? typeModel.idAt(typeCB.currentIndex) : "Related"

			function __contractById(contractId) {
				for (var i = 0; i < referenceDialog.contracts.length; i++) {
					if (referenceDialog.contracts[i].id === contractId)
						return referenceDialog.contracts[i]
				}
				return null
			}

			function init() {
				var contracts = []
				for (var i = 0; i < referenceDialog.contracts.length; i++) {
					var contract = referenceDialog.contracts[i]
					contracts.push({"id": contract.id, "name": (contract.number || "") + " " + (contract.name || "")})
				}
				contractCB.currentIndex = contractModel.fill(contracts, "id", "name", contracts.length > 0 ? contracts[0].id : "")
				typeCB.currentIndex = typeModel.fill(ContractTexts.referenceTypeItems(), "id", "name", "Related")
			}

			ChoiceModel {
				id: contractModel
			}

			ChoiceModel {
				id: typeModel
			}

			Column {
				id: referenceColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL

				spacing: Style.marginL

				BaseText {
					width: parent.width
					visible: referenceDialog.contracts.length === 0
					wrapMode: Text.WordWrap
					text: qsTr("The organization has no other sent contract to refer to.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				GroupElementView {
					width: parent.width
					visible: referenceDialog.contracts.length > 0

					ComboBoxElementView {
						id: contractCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose the contract")
						name: qsTr("Contract")
						model: contractModel
					}

					ComboBoxElementView {
						id: typeCB
						isRequired: true
						isSelectionRequired: true
						errorText: qsTr("Choose how the contracts are related")
						name: qsTr("Reference")
						model: typeModel
					}
				}
			}
		}
	}
}
