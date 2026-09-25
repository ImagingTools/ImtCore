// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseImtCollectionSdl 1.0

/*!
	\qmltype TagCatalogPanel
	\inqmlmodule imttaggui
	\brief Page for managing tags: the organization's tags and the system tags.

	Users with ManageTags create, change and delete the tags of their organization;
	system tags are read-only for everybody but the superuser.
*/
Item {
	id: tagCatalogPanelRoot
	objectName: "TagCatalogPanel"

	anchors.fill: parent

	property string context: ""

	readonly property bool canManage: PermissionsController.checkPermission("ManageTags")
	readonly property bool isSuperuser: AuthorizationController.loggedUserIsSuperuser()

	property string pendingRemoveTagId: ""

	Component.onCompleted: {
		tagCatalogProvider.load()
	}

	function canEdit(isSystem){
		return isSystem ? tagCatalogPanelRoot.isSuperuser : tagCatalogPanelRoot.canManage
	}

	function openEditor(tag){
		if (tag){
			ModalDialogManager.openDialog(tagEditorComp, {
				tagId: tag.id,
				tagName: tag.name,
				tagDescription: tag.description,
				tagColor: tag.color,
				isSystem: tag.isSystem
			})
		}
		else{
			ModalDialogManager.openDialog(tagEditorComp, {})
		}
	}

	function removeTag(tagId){
		tagCatalogPanelRoot.pendingRemoveTagId = tagId
		ModalDialogManager.openDialog(removeConfirmComp, {})
	}

	TagCatalogProvider {
		id: tagCatalogProvider
		context: tagCatalogPanelRoot.context

		onFailed: {
			PopupManager.addErrorMessage(message)
		}
	}

	Row {
		id: headerRow
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.margins: Style.marginL
		spacing: Style.spacingM

		BaseText {
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Tags (%1)").arg(tagCatalogProvider.tags.length)
			font.pixelSize: Style.fontSizeL
			font.bold: true
		}

		Button {
			anchors.verticalCenter: parent.verticalCenter
			visible: tagCatalogPanelRoot.canManage || tagCatalogPanelRoot.isSuperuser
			variant: "primary"
			text: qsTr("New tag")
			onClicked: {
				tagCatalogPanelRoot.openEditor(null)
			}
		}
	}

	ListView {
		id: tagListView
		anchors.top: headerRow.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.leftMargin: Style.marginL
		anchors.rightMargin: Style.marginL
		clip: true
		spacing: 0
		model: tagCatalogProvider.tags

		delegate: Item {
			id: tagRow
			width: tagListView.width
			height: Style.controlHeightL

			readonly property var tag: modelData

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.right: rowActions.left
				anchors.rightMargin: Style.marginM
				spacing: Style.spacingM

				Item {
					anchors.verticalCenter: parent.verticalCenter
					width: tagListView.width * 0.25
					height: tagChip.height

					TagChip {
						id: tagChip
						tagName: String(tagRow.tag.name)
						tagColor: String(tagRow.tag.color)
						isSystem: tagRow.tag.isSystem === true
					}
				}

				BaseText {
					anchors.verticalCenter: parent.verticalCenter
					width: tagListView.width * 0.40
					text: String(tagRow.tag.description)
					color: Style.inactiveTextColor
				}

				BaseText {
					anchors.verticalCenter: parent.verticalCenter
					text: tagRow.tag.isSystem === true ? qsTr("System") : ""
					color: Style.inactiveTextColor
					font.pixelSize: Style.fontSizeS
				}

				BaseText {
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("%1 objects").arg(tagRow.tag.usageCount)
					color: Style.inactiveTextColor
					font.pixelSize: Style.fontSizeS
				}
			}

			Row {
				id: rowActions
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				spacing: Style.spacingS
				visible: tagCatalogPanelRoot.canEdit(tagRow.tag.isSystem === true)

				Button {
					text: qsTr("Edit")
					onClicked: {
						tagCatalogPanelRoot.openEditor(tagRow.tag)
					}
				}

				Button {
					variant: "danger"
					text: qsTr("Delete")
					onClicked: {
						tagCatalogPanelRoot.removeTag(String(tagRow.tag.id))
					}
				}
			}

			Rectangle {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				height: 1
				color: Style.borderColor
			}
		}
	}

	Component {
		id: tagEditorComp

		TagEditorDialog {
			context: tagCatalogPanelRoot.context

			onSaved: {
				tagCatalogProvider.load()
			}
		}
	}

	Component {
		id: removeConfirmComp

		MessageDialog {
			width: Style.sizeHintM
			title: qsTr("Delete tag")
			message: qsTr("Delete the tag? It disappears from all objects.")

			onFinished: {
				if (buttonId == Enums.yes){
					removeInput.m_collectionId = "Tags"
					removeInput.m_elementIds = [tagCatalogPanelRoot.pendingRemoveTagId]
					removeRequest.send(removeInput)
				}
			}
		}
	}

	RemoveElementsInput {
		id: removeInput
	}

	GqlSdlRequestSender {
		id: removeRequest
		context: tagCatalogPanelRoot.context
		requestType: 1
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements

		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: {
					tagCatalogProvider.load()
				}
			}
		}

		function onError(message, type){
			PopupManager.addErrorMessage(message)
		}
	}
}
