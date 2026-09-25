// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseImtCollectionSdl 1.0
import imttagTagsSdl 1.0

/*!
	\qmltype TagEditorDialog
	\inqmlmodule imttaggui
	\brief Creates a tag or changes its name, description and color.

	Leave \c tagId empty to create a tag. Only the superuser can create a
	system tag; the system flag cannot be changed later.
*/
Dialog {
	id: tagEditorDialog

	property string context: ""
	property string tagId: ""
	property string tagName: ""
	property string tagDescription: ""
	property string tagColor: "ededed"
	property bool isSystem: false

	readonly property bool isNew: tagEditorDialog.tagId === ""
	readonly property bool canCreateSystemTag: AuthorizationController.loggedUserIsSuperuser()
	readonly property bool isInputValid: tagEditorDialog.tagName.trim() !== "" && tagEditorDialog.contentItem && tagEditorDialog.contentItem.isColorValid

	signal saved(string tagId)

	title: tagEditorDialog.isNew ? qsTr("New tag") : qsTr("Edit tag")
	width: Math.max(Style.sizeHintL, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintXL))
	notClosingButtons: Enums.save

	Component.onCompleted: {
		addButton(Enums.save, qsTr("Save"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	onIsInputValidChanged: {
		if (tagEditorDialog.buttons){
			tagEditorDialog.buttons.setButtonState(Enums.save, tagEditorDialog.isInputValid)
		}
	}

	onFinished: {
		if (buttonId == Enums.save){
			tagEditorDialog.save()
		}
	}

	function trySave(){
		if (tagEditorDialog.isInputValid){
			tagEditorDialog.save()
		}
	}

	function save(){
		tagData.m_id = tagEditorDialog.tagId
		tagData.m_name = tagEditorDialog.tagName.trim()
		tagData.m_description = tagEditorDialog.tagDescription
		tagData.m_color = tagEditorDialog.tagColor
		tagData.m_isSystem = tagEditorDialog.isSystem

		tagDataInput.m_id = tagEditorDialog.tagId
		tagDataInput.m_typeId = "Tag"
		tagDataInput.m_name = tagData.m_name
		tagDataInput.m_description = tagData.m_description
		tagDataInput.m_item = tagData

		if (tagEditorDialog.isNew){
			tagAddRequest.send(tagDataInput)
		}
		else{
			tagUpdateRequest.send(tagDataInput)
		}
	}

	function onSaved(savedTagId){
		tagEditorDialog.saved(savedTagId)
		tagEditorDialog.root.closeDialog()
	}

	TagData {
		id: tagData
	}

	TagDataInput {
		id: tagDataInput
	}

	GqlSdlRequestSender {
		id: tagAddRequest
		context: tagEditorDialog.context
		requestType: 1
		gqlCommandId: ImttagTagsSdlCommandIds.s_tagAdd

		sdlObjectComp: Component {
			AddedNotificationPayload {
				onFinished: {
					tagEditorDialog.onSaved(m_id)
				}
			}
		}

		function onError(message, type){
			PopupManager.addErrorMessage(message)
		}
	}

	GqlSdlRequestSender {
		id: tagUpdateRequest
		context: tagEditorDialog.context
		requestType: 1
		gqlCommandId: ImttagTagsSdlCommandIds.s_tagUpdate

		sdlObjectComp: Component {
			UpdatedNotificationPayload {
				onFinished: {
					tagEditorDialog.onSaved(m_id)
				}
			}
		}

		function onError(message, type){
			PopupManager.addErrorMessage(message)
		}
	}

	// Tab order: name, description, color swatches (arrow keys pick), hex, "Random", system flag.
	// Enter in a text field saves, Esc cancels.
	contentComp: Component {
		Item {
			id: editorContent
			width: tagEditorDialog.width
			height: editorColumn.height + 2 * Style.marginXL

			property bool isColorValid: colorPalette.isValid

			Component.onCompleted: {
				focusDelay.start()
			}

			PauseAnimation {
				id: focusDelay
				duration: 50
				onFinished: {
					nameField.setFocus(true)
				}
			}

			Column {
				id: editorColumn
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXL
				spacing: Style.spacingM

				Row {
					spacing: Style.spacingM

					BaseText {
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("Preview")
					}

					TagChip {
						anchors.verticalCenter: parent.verticalCenter
						tagName: tagEditorDialog.tagName.trim() !== "" ? tagEditorDialog.tagName : qsTr("Tag")
						tagColor: colorPalette.isValid ? tagEditorDialog.tagColor : "ededed"
						isSystem: tagEditorDialog.isSystem
					}
				}

				BaseText {
					text: qsTr("Name")
				}

				TextField {
					id: nameField
					objectName: "TagNameField"
					width: parent.width
					placeHolderText: qsTr("Tag name")
					text: tagEditorDialog.tagName
					maximumLength: 50

					KeyNavigation.tab: descriptionField
					KeyNavigation.backtab: systemRow.visible ? systemRow : colorPalette.lastItem

					onTextEdited: {
						tagEditorDialog.tagName = nameField.text
					}

					onAccepted: {
						tagEditorDialog.trySave()
					}
				}

				BaseText {
					text: qsTr("Description")
				}

				TextField {
					id: descriptionField
					objectName: "TagDescriptionField"
					width: parent.width
					placeHolderText: qsTr("Optional description")
					text: tagEditorDialog.tagDescription
					maximumLength: 200

					KeyNavigation.tab: colorPalette.firstItem
					KeyNavigation.backtab: nameField

					onTextEdited: {
						tagEditorDialog.tagDescription = descriptionField.text
					}

					onAccepted: {
						tagEditorDialog.trySave()
					}
				}

				BaseText {
					text: qsTr("Color")
				}

				TagColorPalette {
					id: colorPalette
					width: parent.width
					color: tagEditorDialog.tagColor
					previousItem: descriptionField
					nextItem: systemRow.visible ? systemRow : nameField

					onColorEdited: {
						tagEditorDialog.tagColor = color
					}

					onAccepted: {
						tagEditorDialog.trySave()
					}
				}

				Row {
					id: systemRow
					objectName: "TagSystemFlag"
					visible: tagEditorDialog.isNew && tagEditorDialog.canCreateSystemTag
					spacing: Style.spacingS

					KeyNavigation.tab: nameField
					KeyNavigation.backtab: colorPalette.lastItem

					Keys.onSpacePressed: {
						tagEditorDialog.isSystem = !tagEditorDialog.isSystem
					}

					CheckBox {
						id: systemCheckBox
						anchors.verticalCenter: parent.verticalCenter
						checkState: tagEditorDialog.isSystem ? Qt.Checked : Qt.Unchecked
						borderColor: systemRow.activeFocus ? Style.textSelectedColor : Style.grayColor

						MouseArea {
							anchors.fill: parent
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								tagEditorDialog.isSystem = !tagEditorDialog.isSystem
								systemRow.forceActiveFocus()
							}
						}
					}

					BaseText {
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("System tag (visible to every organization)")
					}
				}
			}
		}
	}
}
