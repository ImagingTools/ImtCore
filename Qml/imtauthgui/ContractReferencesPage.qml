// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractReferencesPage
 *
 * The other contracts a contract refers to: the one it replaces, the framework contract it is concluded under,
 * a related one. A page of ContractView: the references are the list of the view, changed through the view.
 */
Item {
	id: referencesPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __isEditable: referencesPage.editor ? referencesPage.editor.isEditable : false

	signal addReferenceRequested()

	readonly property var referenceRows: referencesPage.__referenceRows(referencesPage.editor ? referencesPage.editor.references : [])

	function __referenceRows(references) {
		var rows = []
		for (var i = 0; i < references.length; i++) {
			var reference = references[i]
			rows.push({"id": reference.contractId,
					   "index": i,
					   "reference": ContractTexts.referenceTypeText(reference.referenceType),
					   "number": reference.contractNumber || "",
					   "name": reference.contractName || ""})
		}
		return rows
	}

	Item {
		id: headerItem

		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL

		height: Math.max(descriptionText.height, addButton.height)

		BaseText {
			id: descriptionText
			anchors.left: parent.left
			anchors.right: addButton.left
			anchors.rightMargin: Style.marginL
			anchors.verticalCenter: parent.verticalCenter
			wrapMode: Text.WordWrap
			text: qsTr("A contract replacing another one ends it once concluded; a contract under a framework contract gives no more than it.")
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
		}

		Button {
			id: addButton
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			visible: referencesPage.__isEditable
			text: qsTr("Refer to a contract")
			onClicked: {
				referencesPage.addReferenceRequested()
			}
		}
	}

	Table {
		id: referencesTable

		anchors.top: headerItem.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL

		visible: referencesPage.referenceRows.length > 0
		selectable: false
		elements: referencesPage.referenceRows
	}

	TableHeaders {
		id: referencesHeaders

		Component.onCompleted: {
			addHeader("reference", qsTr("Reference"), false, false)
			addHeader("number", qsTr("Number"), false, false)
			addHeader("name", qsTr("Name"), false, false)
			addHeader("actions", "", false, false)
			referencesTable.setColumnContentById("actions", referenceActionsComp)
			referencesTable.headers = referencesHeaders
		}
	}

	Component {
		id: referenceActionsComp

		TableCellDelegateBase {
			id: referenceActions

			readonly property var rowData: referenceActions.getRowData()

			Button {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				visible: referencesPage.__isEditable
				text: qsTr("Remove")
				onClicked: {
					referencesPage.editor.removeReference(referenceActions.rowData.index)
				}
			}
		}
	}
}
