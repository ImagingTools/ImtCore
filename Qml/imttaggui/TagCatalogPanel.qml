// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseImtCollectionSdl 1.0

/*!
	\qmltype TagCatalogPanel
	\inqmlmodule imttaggui
	\brief Page for managing tags: the organization's tags and the system tags.

	Lists the catalog through the same provider as the tag pickers, with the text
	filter and paging. Users with ManageTags create, change and delete the tags of
	their organization; system tags are read-only for everybody but the superuser.
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
		tagList.started()
	}

	function canEdit(isSystem){
		return isSystem ? tagCatalogPanelRoot.isSuperuser : tagCatalogPanelRoot.canManage
	}

	function openEditor(tag){
		if (tag){
			ModalDialogManager.openDialog(tagEditorComp, {
				tagId: String(tag.id),
				tagName: String(tag.title),
				tagDescription: String(tag.description),
				tagColor: tagSelectDataProvider.getColor(tag),
				isSystem: tagSelectDataProvider.isSystemTag(tag)
			})
		}
		else{
			ModalDialogManager.openDialog(tagEditorComp, {})
		}
	}

	function removeTag(tag){
		tagCatalogPanelRoot.pendingRemoveTagId = String(tag.id)

		var usageCount = tagSelectDataProvider.getUsageCount(tag)
		var message = usageCount > 0
			? qsTr("Delete the tag '%1'? It disappears from %2 objects.").arg(tag.title).arg(usageCount)
			: qsTr("Delete the tag '%1'?").arg(tag.title)

		ModalDialogManager.openDialog(removeConfirmComp, {message: message})
	}

	Row {
		id: headerRow
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.margins: Style.marginL
		spacing: Style.spacingM

		BaseText {
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Tags")
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

	Item {
		id: listArea
		anchors.top: headerRow.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.leftMargin: Style.marginL
		anchors.rightMargin: Style.marginL
		anchors.bottomMargin: Style.marginL

		FilterableSelectPopup {
			id: tagList
			objectName: "TagCatalogList"

			embedded: true
			itemWidth: listArea.width
			maxVisibleItems: Math.max(1, Math.floor((listArea.height - Style.controlHeightM - Style.marginM) / tagList.itemHeight))
			filterPlaceholder: qsTr("Filter tags...")

			dataProvider: TagSelectDataProvider {
				id: tagSelectDataProvider
				context: tagCatalogPanelRoot.context
				pageSize: 50
			}

			delegate: Component {
				Item {
					id: tagRow
					objectName: "TagCatalogItem_" + model.index
					width: tagList.itemWidth
					height: tagList.itemHeight

					readonly property var tag: tagList.getItem(model.index)
					readonly property bool isSystem: tagSelectDataProvider.isSystemTag(tagRow.tag)
					readonly property bool isEditable: tagCatalogPanelRoot.canEdit(tagRow.isSystem)

					Rectangle {
						anchors.fill: parent
						color: tagList.rowBackgroundColor(model.index, tagRowMouseArea.containsMouse, false)
					}

					MouseArea {
						id: tagRowMouseArea
						anchors.fill: parent
						hoverEnabled: true
						cursorShape: tagRow.isEditable ? Qt.PointingHandCursor : Qt.ArrowCursor

						onClicked: {
							if (tagRow.isEditable){
								tagCatalogPanelRoot.openEditor(tagRow.tag)
							}
						}
					}

					Item {
						id: chipCell
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						width: tagRow.width * 0.25
						height: tagChip.height

						TagChip {
							id: tagChip
							tagName: tagRow.tag ? String(tagRow.tag.title) : ""
							tagColor: tagSelectDataProvider.getColor(tagRow.tag)
							isSystem: tagRow.isSystem
						}
					}

					BaseText {
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: chipCell.right
						anchors.leftMargin: Style.marginM
						anchors.right: infoText.left
						anchors.rightMargin: Style.marginM
						text: tagRow.tag ? String(tagRow.tag.description) : ""
						color: Style.inactiveTextColor
						elide: Text.ElideRight
					}

					BaseText {
						id: infoText
						anchors.verticalCenter: parent.verticalCenter
						anchors.right: rowActions.left
						anchors.rightMargin: Style.marginL
						text: tagRow.isSystem
							? qsTr("System · %1 objects").arg(tagSelectDataProvider.getUsageCount(tagRow.tag))
							: qsTr("%1 objects").arg(tagSelectDataProvider.getUsageCount(tagRow.tag))
						color: Style.inactiveTextColor
						font.pixelSize: Style.fontSizeS
					}

					Row {
						id: rowActions
						anchors.verticalCenter: parent.verticalCenter
						anchors.right: parent.right
						anchors.rightMargin: Style.marginM
						spacing: Style.spacingS
						visible: tagRow.isEditable

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
								tagCatalogPanelRoot.removeTag(tagRow.tag)
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
		}
	}

	Component {
		id: tagEditorComp

		TagEditorDialog {
			context: tagCatalogPanelRoot.context

			onSaved: {
				tagSelectDataProvider.refetch()
			}
		}
	}

	Component {
		id: removeConfirmComp

		MessageDialog {
			width: Style.sizeHintM
			title: qsTr("Delete tag")

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
					tagSelectDataProvider.refetch()
				}
			}
		}

		function onError(message, type){
			PopupManager.addErrorMessage(message)
		}
	}
}
