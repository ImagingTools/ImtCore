// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseImtCollectionSdl 1.0

/*!
	\qmltype TagCatalogPanel
	\inqmlmodule imttaggui
	\brief Page for managing tags: the organization's tags and the system tags.

	Built like the Support and History panels: a SimpleCollectionTable with search and
	paging over TagSelectDataProvider. Users with ManageTags create, change and delete the
	tags of their organization; system tags are read-only for everybody but the superuser.

	Also the \c actionHandler of its rows (canManage, canEdit, canDelete, openEdit(),
	confirmRemoveItems()), which drives the rows' Edit/Delete menu.
*/
Item {
	id: tagCatalogPanelRoot
	objectName: "TagCatalogPanel"

	anchors.fill: parent

	property string context: ""

	readonly property bool canManage: PermissionsController.checkPermission("ManageTags") || tagCatalogPanelRoot.isSuperuser
	readonly property bool canEdit: tagCatalogPanelRoot.canManage
	readonly property bool canDelete: tagCatalogPanelRoot.canManage
	readonly property bool isSuperuser: AuthorizationController.loggedUserIsSuperuser()

	property string pendingRemoveTagId: ""

	Component.onCompleted: {
		tagSelectDataProvider.fetch("")
	}

	function canEditTag(isSystem){
		return isSystem ? tagCatalogPanelRoot.isSuperuser : tagCatalogPanelRoot.canManage
	}

	function findTag(tagId){
		var items = tagSelectDataProvider.items
		for (var i = 0; i < items.length; i++){
			if (String(items[i].id) === String(tagId)){
				return items[i]
			}
		}

		return null
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

	// actionHandler interface of SimpleCollectionItemDelegateBase
	function openEdit(itemId, itemTitle, itemDescription){
		tagCatalogPanelRoot.openEditor(tagCatalogPanelRoot.findTag(itemId))
	}

	function confirmRemoveItems(ids, itemTitle){
		var tag = tagCatalogPanelRoot.findTag(ids[0])
		if (!tag){
			return
		}

		tagCatalogPanelRoot.pendingRemoveTagId = String(tag.id)

		var usageCount = tagSelectDataProvider.getUsageCount(tag)
		var message = usageCount > 0
			? qsTr("Delete the tag '%1'? It disappears from %2 objects.").arg(tag.title).arg(usageCount)
			: qsTr("Delete the tag '%1'?").arg(tag.title)

		ModalDialogManager.openDialog(removeConfirmComp, {message: message})
	}

	TagSelectDataProvider {
		id: tagSelectDataProvider
		context: tagCatalogPanelRoot.context
		pageSize: 50

		onDataChanged: {
			tagsTable.model = tagSelectDataProvider.items
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Style.baseColor
	}

	Item {
		id: pageContent
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		x: Math.max(0, (tagCatalogPanelRoot.width - width) / 2)
		width: Math.min(tagCatalogPanelRoot.width, Style.contentWidthMax)

		Item {
			id: pageHeader
			anchors.top: parent.top
			anchors.topMargin: Style.marginL
			anchors.left: parent.left
			anchors.leftMargin: Style.marginL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginL
			height: headerActions.height

			Row {
				anchors.left: parent.left
				anchors.right: headerActions.left
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.spacingS
				clip: true

				Text {
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Tags")
					font.pixelSize: Style.fontSizeXL
					font.family: Style.fontFamilyBold
					color: Style.textColor
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					visible: !tagSelectDataProvider.hasMore && tagSelectDataProvider.items.length > 0 && tagsTable.filterText === ""
					text: "(" + tagSelectDataProvider.items.length + ")"
					font.pixelSize: Style.fontSizeXL
					font.family: Style.fontFamilyBold
					color: Style.imaginToolsAccentColor
				}
			}

			Row {
				id: headerActions
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.spacingS

				ToolButton {
					id: reloadButton
					objectName: "TagsReloadButton"
					anchors.verticalCenter: parent.verticalCenter
					height: Style.buttonHeightS
					width: height
					enabled: !tagSelectDataProvider.isInitialLoading
					tooltipText: qsTr("Reload tags")
					iconSource: "qrc:/" + Style.getIconPath("Icons/AutoUpdate", Icon.State.On, enabled ? Icon.Mode.Normal : Icon.Mode.Disabled)
					onClicked: {
						tagSelectDataProvider.fetch(tagsTable.filterText)
					}
				}

				Button {
					id: newTagButton
					objectName: "NewTagButton"
					visible: tagCatalogPanelRoot.canManage
					text: qsTr("New tag")
					tooltipText: qsTr("New tag (Alt+N)")
					decorator: Component {
						ButtonDecorator {
							color: Style.imaginToolsAccentColor
							textColor: "white"
							opacity: newTagButton.hovered ? 0.85 : 1
						}
					}
					onClicked: {
						tagCatalogPanelRoot.openEditor(null)
					}
				}
			}
		}

		Shortcut {
			sequence: "Alt+N"
			enabled: tagCatalogPanelRoot.visible && tagCatalogPanelRoot.canManage
			onActivated: {
				tagCatalogPanelRoot.openEditor(null)
			}
		}

		Component {
			id: tagColumnHeaderComp

			Item {
				height: Style.controlHeightL + Style.marginM

				Rectangle {
					anchors.fill: parent
					color: Style.backgroundColor2
				}

				Row {
					id: tagHeaderRow
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.leftMargin: Style.marginL
					anchors.rightMargin: Style.marginL + Style.controlHeightM + Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginL

					readonly property int columnCount: 4
					readonly property real columnsWidth: width - spacing * (columnCount - 1)

					BaseText {
						width: tagHeaderRow.columnsWidth * 0.28
						text: qsTr("Tag")
						font.bold: true
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					BaseText {
						width: tagHeaderRow.columnsWidth * 0.46
						text: qsTr("Description")
						font.bold: true
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					BaseText {
						width: tagHeaderRow.columnsWidth * 0.12
						text: qsTr("Type")
						font.bold: true
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					BaseText {
						width: tagHeaderRow.columnsWidth * 0.14
						text: qsTr("Objects")
						font.bold: true
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
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
			id: tagRowDelegateComp

			SimpleCollectionItemDelegateBase {
				id: tagDelegate
				objectName: "TagCatalogItem_" + index
				showCheckBox: false
				actionHandler: tagCatalogPanelRoot
				enableDefaultDoubleClickEdit: false
				canEdit: tagCatalogPanelRoot.canEditTag(tagDelegate.isSystemTag)
				canDelete: tagDelegate.canEdit

				readonly property bool isSystemTag: tagSelectDataProvider.isSystemTag(tagDelegate.modelItem)

				onItemClicked: {
					if (tagDelegate.canEdit){
						tagCatalogPanelRoot.openEditor(tagDelegate.modelItem)
					}
				}

				Row {
					id: tagDataRow
					width: parent.width
					spacing: Style.marginL

					readonly property int columnCount: 4
					readonly property real columnsWidth: width - spacing * (columnCount - 1)

					Item {
						anchors.verticalCenter: parent.verticalCenter
						width: tagDataRow.columnsWidth * 0.28
						height: tagChip.height

						TagChip {
							id: tagChip
							tagName: tagDelegate.itemTitle
							tagColor: tagSelectDataProvider.getColor(tagDelegate.modelItem)
							isSystem: tagDelegate.isSystemTag
						}
					}

					Text {
						anchors.verticalCenter: parent.verticalCenter
						width: tagDataRow.columnsWidth * 0.46
						text: tagDelegate.itemDescription
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
						elide: Text.ElideRight
					}

					Text {
						anchors.verticalCenter: parent.verticalCenter
						width: tagDataRow.columnsWidth * 0.12
						text: tagDelegate.isSystemTag ? qsTr("System") : qsTr("Organization")
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
						elide: Text.ElideRight
					}

					Text {
						anchors.verticalCenter: parent.verticalCenter
						width: tagDataRow.columnsWidth * 0.14
						text: tagSelectDataProvider.getUsageCount(tagDelegate.modelItem)
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
						elide: Text.ElideRight
					}
				}
			}
		}

		SimpleCollectionTable {
			id: tagsTable
			anchors.top: pageHeader.bottom
			anchors.topMargin: Style.marginM
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			selectionEnabled: false
			maximumWidth: pageContent.width
			horizontalMargin: Style.marginL
			emptyText: qsTr("No tags yet")
			filterPlaceholder: qsTr("Filter tags...")
			columnHeaderComponent: tagColumnHeaderComp
			delegateComponent: tagRowDelegateComp

			initialLoading: tagSelectDataProvider.isInitialLoading
			loadingMore: tagSelectDataProvider.isPageLoading
			errorMessage: tagSelectDataProvider.error ? (tagSelectDataProvider.error.message || qsTr("Error loading tags")) : ""

			onFilterRequested: {
				tagSelectDataProvider.fetch(text)
			}

			onLoadMoreRequested: {
				tagSelectDataProvider.fetchMore()
			}

			onRetryRequested: {
				tagSelectDataProvider.retry()
			}
		}
	}

	Component {
		id: tagEditorComp

		TagEditorDialog {
			context: tagCatalogPanelRoot.context

			onSaved: {
				tagSelectDataProvider.fetch(tagsTable.filterText)
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
					tagSelectDataProvider.fetch(tagsTable.filterText)
				}
			}
		}

		function onError(message, type){
			PopupManager.addErrorMessage(message)
		}
	}
}
