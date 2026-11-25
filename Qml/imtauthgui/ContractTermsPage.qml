// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ContractTermsPage
 *
 * The terms of a contract: its name, its type and the text agreed.
 * A page of ContractView: it shows the working copy of the view and writes it back.
 */
Item {
	id: termsPage

	anchors.fill: parent

	// ContractView
	property var editor: null

	readonly property bool __isEditable: termsPage.editor ? termsPage.editor.isEditable : false

	property bool __filling: false

	function updateGui() {
		if (!termsPage.editor)
			return
		termsPage.__filling = true
		var draft = termsPage.editor.draft
		nameInput.text = draft.name || ""
		typeCB.currentIndex = typeModel.fill(termsPage.editor.availableTypes(), "typeId", "name", draft.typeId || "")
		descriptionInput.text = draft.description || ""
		termsInput.text = draft.terms || ""
		termsPage.__filling = false
	}

	// The required and checked fields hold valid texts
	function isValid() {
		return nameInput.acceptableInput
	}

	function revealErrors() {
		nameInput.revealError()
	}

	function updateModel() {
		if (!termsPage.editor)
			return
		var draft = termsPage.editor.draft
		draft.name = nameInput.text
		draft.description = descriptionInput.text
		draft.terms = termsInput.text
	}

	ChoiceModel {
		id: typeModel
	}

	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
	}

	Flickable {
		id: flickable

		anchors.fill: parent
		anchors.margins: Style.marginXL
		contentHeight: termsColumn.height
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: termsColumn

			width: Math.min(flickable.width - Style.marginM, Style.contentWidthMax)
			spacing: Style.marginL

			RegularExpressionValidator {
				id: nameValidator
				regularExpression: /^(?!\s*$).+/
			}

			GroupElementView {
				width: parent.width

				TextInputElementView {
					id: nameInput
					isRequired: true
					showErrorWhenInvalid: true
					deferErrorUntilEdited: true
					errorText: qsTr("Enter the name of the contract")
					textInputValidator: nameValidator
					name: qsTr("Name")
					readOnly: !termsPage.__isEditable
					placeHolderText: qsTr("e.g. Supply agreement")
				}

				ClearableComboBoxElementView {
					id: typeCB
					name: qsTr("Type")
					description: qsTr("A type offers its periods, its roles and the links of the parties")
					model: typeModel
					changeable: termsPage.__isEditable

					onCurrentIndexChanged: {
						if (!termsPage.__filling && termsPage.editor && typeModel.idAt(typeCB.currentIndex) !== termsPage.editor.draft.typeId)
							termsPage.editor.applyType(typeModel.idAt(typeCB.currentIndex))
					}
				}

				TextInputElementView {
					id: descriptionInput
					placeHolderText: qsTr("Optional: what the contract is about")
					name: qsTr("Description")
					readOnly: !termsPage.__isEditable
				}

				TextInputElementView {
					id: termsInput
					name: qsTr("Terms")
					readOnly: !termsPage.__isEditable
					placeHolderText: qsTr("The terms agreed, or attach the main text")
				}
			}
		}
	}
}
