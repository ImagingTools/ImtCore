// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import QtQuick.Controls
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantSimpleCollectionPage
 *
 * Generic CRUD page used by the TenantEditor tabs that only need a flat
 * list of items with checkbox-multiselect, a search filter, Create / Edit
 * / Remove header buttons and create/edit sub-pages driven by a
 * `CollectionDocumentManager`.
 *
 * Currently consumed by `TenantRolesPage` and `TenantGroupsPage`; the
 * Members tab does not use it because it has its own delegate (avatars,
 * invitations, role menu).
 *
 * The page is parameterised by:
 *   - `apiClient` / `stateManager` from the parent
 *   - the entity name and i18n strings (header / button labels, confirm
 *     dialog text, empty-state text, filter placeholder)
 *   - `documentManager` / `objectTypeId` / `dataProviderComp` taken from
 *     the apiClient for the concrete entity type
 *   - `removeItem(id)` — override to handle Remove buttons and
 *     the per-item Delete menu
 *
 * Parents that need to react to apiClient-level removal signals (e.g.
 * `onRoleRemoved` / `onGroupRemoved`) should call `refresh()` from a
 * `Connections` block they set up themselves.
 */
ViewBase {
	id: collectionPage

	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: collectionPage.model
	property var stateManager: null
	property var apiClient: null

	// --- entity-specific configuration ---
	property string entityName: "Item"                // e.g. "Role"
	property string entityNamePlural: "Items"         // e.g. "Roles"
	property string listHeaderTitle: ""               // defaults to entityNamePlural
	property string descriptionText: ""
	property string createButtonText: ""              // defaults to "+ Create <entityName>"
	property string emptyText: ""                     // defaults to "No <entityNamePlural> found."
	property string filterPlaceholder: ""             // defaults to "Filter <entityNamePlural>..."
	property string deleteSingleTitle: ""             // defaults to "Delete <entityName>"
	property string deleteMultipleTitle: ""           // defaults to "Delete <entityNamePlural>"

	// --- backend configuration ---
	property var documentManager: null
	property string objectTypeId: ""
	property var listModel: null                       // alternative to dataProviderComp: direct model for the list
	property Component delegateComponent: null         // custom delegate (receives modelData, selectionManager, collectionPage)
	property Component headerButtonsComponent: null    // custom header buttons placed at right of stackViewHeader
	property Component customEditorComponent: null     // custom create/edit form used when documentManager is null
	function removeItem(id) {}                        // override in subcomponents

	function updateGui() {}
	function updateModel() {}

	/** Re-fetch the list. Parents call this on external removal signals. */
	function refresh() {
		if (dataProvider)
			dataProvider.fetch(__lastFilterText)
	}

	function resolveDocumentName(documentId) {
		var view = collectionPage.documentManager.getDocumentViewInstance(documentId, "")
		if (view && view.model && view.model.m_name)
			return view.model.m_name
		return ""
	}

	/** Navigate back from a custom editor to the list. */
	function popEditor() {
		stackViewHeader.popHeader()
		collectionStackView.previous()
		while (collectionStackView.count > 1)
			collectionStackView.removePage(collectionStackView.count - 1)
		collectionPage.__activeShellView = null
	}

	property string __editItemId: ""
	property bool __isCreating: false
	property var __activeShellView: null

	readonly property bool __canManage: collectionPage.stateManager ? collectionPage.stateManager.canManageMembers : false

	property var __selectionManager: null
	property string __lastFilterText: ""
	property var __listItems: []

	// Public accessors for subcomponents with custom header buttons
	readonly property var selectionManager: __selectionManager
	property var dataProvider: null
	onDataProviderChanged: {
		if (dataProvider)
			dataProvider.fetch(__lastFilterText)
	}
	readonly property string filterText: __lastFilterText

	Connections {
		target: collectionPage.dataProvider
		function onDataChanged() {
			collectionPage.__listItems = collectionPage.dataProvider.items
		}
	}

	function openCreate() {
		while (collectionStackView.count > 1)
			collectionStackView.removePage(collectionStackView.count - 1)
		collectionPage.__isCreating = true
		stackViewHeader.addHeader("create", qsTr("Create New %1").arg(collectionPage.entityName))
		if (collectionPage.documentManager) {
			collectionStackView.addPage(createEditorView)
		} else if (collectionPage.customEditorComponent) {
			collectionStackView.addPage(collectionPage.customEditorComponent)
		}
		collectionStackView.next()
	}

	function openEdit(itemId, itemName, itemDescription) {
		__openEdit(itemId, itemName, itemDescription)
	}

	readonly property string __listTitle: listHeaderTitle.length > 0 ? listHeaderTitle : entityNamePlural
	readonly property string __createBtnText: createButtonText.length > 0 ? createButtonText : ("+ " + qsTr("Create %1").arg(entityName))
	readonly property string __emptyText: emptyText.length > 0 ? emptyText : qsTr("No %1 found.").arg(entityNamePlural.toLowerCase())
	readonly property string __filterPlaceholder: filterPlaceholder.length > 0 ? filterPlaceholder : qsTr("Filter %1...").arg(entityNamePlural.toLowerCase())
	readonly property string __deleteSingleTitle: deleteSingleTitle.length > 0 ? deleteSingleTitle : qsTr("Delete %1").arg(entityName)
	readonly property string __deleteMultipleTitle: deleteMultipleTitle.length > 0 ? deleteMultipleTitle : qsTr("Delete %1").arg(entityNamePlural)

	Connections {
		target: collectionPage.documentManager
		function onDocumentSaved(documentId) {
			collectionPage.refresh()
		}
		function onDocumentNameChanged(documentId, oldName, newName) {
			stackViewHeader.setHeaderName("edit", newName)
			stackViewHeader.setHeaderName("create", newName)
		}
	}

	StackViewHeader {
		id: stackViewHeader
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		height: Style.controlHeightL
		initialItemTitleVisible: true

		onCloseClicked: {
			if (collectionPage.__activeShellView
					&& collectionPage.__activeShellView.state === "content") {
				collectionPage.__activeShellView.closeDocument()
			}
			else {
				collectionStackView.previous()
				stackViewHeader.popHeader()
			}
		}

		onHeaderItemClicked: {
			if (collectionStackView.currentIndex <= index)
				return
			if (collectionPage.__activeShellView
					&& collectionPage.__activeShellView.state === "content") {
				// Delegate to the document close flow so the standard
				// "Save changes?" dialog is shown for dirty documents.
				// The editor's onClosed handler will pop the header /
				// stack back to the list page.
				collectionPage.__activeShellView.closeDocument()
				return
			}
			while (collectionStackView.currentIndex > index) {
				collectionStackView.previous()
				stackViewHeader.popHeader()
			}
		}

		Component.onCompleted: {
			stackViewHeader.addHeader("list", collectionPage.__listTitle)
		}
	}

	Text {
		id: createBtn
		visible: collectionPage.__canManage && collectionStackView.currentIndex === 0 && !collectionPage.headerButtonsComponent
		anchors.right: stackViewHeader.right
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: collectionPage.__createBtnText
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: Style.linkColor

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: collectionPage.openCreate()
		}
	}

	Text {
		id: editBtn
		visible: collectionPage.__canManage && collectionStackView.currentIndex === 0 && !collectionPage.headerButtonsComponent
		anchors.right: createBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: qsTr("Edit")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length === 1 ? Style.linkColor : Style.inactiveTextColor
		opacity: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length === 1 ? 1.0 : 0.5

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length === 1 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length === 1
			onClicked: {
				var selId = collectionPage.__selectionManager.selectedIds[0]
				var items = collectionPage.__listItems
				for (var i = 0; i < items.length; i++) {
					if (items[i] && items[i].id === selId) {
						collectionPage.__openEdit(selId, items[i].title || items[i].id || "", items[i].description || "")
						break
					}
				}
			}
		}
	}

	Text {
		id: removeBtn
		visible: collectionPage.__canManage && collectionStackView.currentIndex === 0 && !collectionPage.headerButtonsComponent
		anchors.right: editBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: qsTr("Remove")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length > 0 ? Style.errorColor : Style.inactiveTextColor
		opacity: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length > 0 ? 1.0 : 0.5

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: collectionPage.__selectionManager && collectionPage.__selectionManager.selectedIds.length > 0
			onClicked: {
				var count = collectionPage.__selectionManager.selectedIds.length
				ModalDialogManager.showConfirmationDialog(
							collectionPage.__deleteMultipleTitle,
							qsTr("Are you sure you want to delete %1 selected item(s)? This action cannot be undone.").arg(count),
							function(result) {
								if (result === Enums.yes) {
									var ids = collectionPage.__selectionManager.selectedIds.slice()
									for (var i = 0; i < ids.length; i++)
										collectionPage.removeItem(ids[i])
									collectionPage.__selectionManager.clear()
								}
							}
							)
			}
		}
	}

	Loader {
		id: customHeaderButtonsLoader
		visible: collectionPage.headerButtonsComponent && collectionStackView.currentIndex === 0
		anchors.right: stackViewHeader.right
		anchors.verticalCenter: stackViewHeader.verticalCenter
		sourceComponent: collectionPage.headerButtonsComponent
		property var page: collectionPage
	}

	BaseText {
		id: descriptionLabel
		anchors.top: stackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		visible: collectionStackView.currentIndex === 0 && collectionPage.descriptionText.length > 0
		text: collectionPage.descriptionText
		font.pixelSize: Style.fontSizeM
		color: Style.inactiveTextColor
	}

	StackView {
		id: collectionStackView
		anchors.top: descriptionLabel.visible ? descriptionLabel.bottom : stackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		Component.onCompleted: {
			collectionStackView.addPage(listView)
		}
	}

	Component {
		id: listView

		Item {
			id: listViewItem
			readonly property var effectiveModel: collectionPage.listModel ? collectionPage.listModel : collectionPage.__listItems

			SearchTextInput {
				id: filterInput
				anchors.top: parent.top
				anchors.topMargin: Style.marginM
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
				placeHolderText: collectionPage.__filterPlaceholder
				onTextChanged: {
					selectionManager.clear()
					collectionPage.__lastFilterText = text
					filterDebounce.restart()
				}
			}

			Timer {
				id: filterDebounce
				interval: 300
				repeat: false
				onTriggered: {
					if (collectionPage.dataProvider)
						collectionPage.dataProvider.fetch(filterInput.text)
				}
			}

			TenantTableContainer {
				anchors.top: filterInput.bottom
				anchors.topMargin: Style.marginM
				height: Math.min(tableHeader.height + emptyState.height + itemListView.contentHeight + 2,
								 parent.height - filterInput.height - filterInput.anchors.topMargin - Style.marginM - Style.marginL)

				IdSelectionManager {
					id: selectionManager
					multiSelect: true
					Component.onCompleted: collectionPage.__selectionManager = selectionManager
					Component.onDestruction: {
						if (collectionPage)
							collectionPage.__selectionManager = null
					}
				}

				TenantTableHeader {
					id: tableHeader
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					selectedCount: selectionManager.selectedIds.length
					totalCount: itemListView.count
					checkState: selectionManager.selectedIds.length === 0
								? Qt.Unchecked
								: (selectionManager.selectedIds.length === itemListView.count
								   ? Qt.Checked : Qt.PartiallyChecked)

					onSelectAllToggled: {
						if (checkState === Qt.Checked) {
							selectionManager.clear()
						} else {
							var allIds = []
							var items = listViewItem.effectiveModel
							for (var i = 0; i < items.length; i++) {
								if (items[i] && items[i].id)
									allIds.push(items[i].id)
							}
							selectionManager.selectMultiple(allIds)
						}
					}
				}

				Item {
					id: emptyState
					visible: !listViewItem.effectiveModel || listViewItem.effectiveModel.length === 0
					anchors.top: tableHeader.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					height: visible ? Style.controlHeightL + Style.marginL : 0

					BaseText {
						anchors.centerIn: parent
						text: collectionPage.__emptyText
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
					}
				}

				ListView {
					id: itemListView
					anchors.top: emptyState.visible ? emptyState.bottom : tableHeader.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
					clip: true
					boundsBehavior: Flickable.StopAtBounds
					model: listViewItem.effectiveModel

					delegate: collectionPage.delegateComponent ? collectionPage.delegateComponent : defaultDelegateComp
				}

				Component {
					id: defaultDelegateComp

					Rectangle {
						id: itemDelegateRoot
						width: itemListView.width
						height: Style.controlHeightL + Style.marginL

						property string itemId: modelData.id || ""
						property string itemTitle: modelData.title || modelData.id || ""
						property string itemDescription: modelData.description || ""
						property bool isSelected: selectionManager.isSelected(itemId)

						color: isSelected ? Style.selectedColor
										  : itemMouseArea.containsMouse ? Style.buttonHoverColor
																		: "transparent"
						MouseArea {
							id: itemMouseArea
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onDoubleClicked: {
								if (collectionPage.__canManage)
									collectionPage.__openEdit(itemDelegateRoot.itemId, itemDelegateRoot.itemTitle, itemDelegateRoot.itemDescription)
							}
						}

						Row {
							anchors.left: parent.left
							anchors.right: moreButton.left
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginM

							CheckBox {
								anchors.verticalCenter: parent.verticalCenter
								height: Style.itemSizeS
								width: Style.itemSizeS
								checkState: itemDelegateRoot.isSelected ? Qt.Checked : Qt.Unchecked
								onCheckStateChanged: {
									var shouldBeSelected = (checkState === Qt.Checked)
									var currentlySelected = selectionManager.isSelected(itemDelegateRoot.itemId)
									if (shouldBeSelected !== currentlySelected)
										selectionManager.toggleSelect(itemDelegateRoot.itemId)
								}
							}

							Column {
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.marginXS
								width: parent.width - Style.itemSizeS - parent.spacing

								BaseText {
									text: itemDelegateRoot.itemTitle
									font.pixelSize: Style.fontSizeL
									font.bold: true
									color: Style.textColor
								}

								BaseText {
									visible: itemDelegateRoot.itemDescription !== ""
									text: itemDelegateRoot.itemDescription
									font.pixelSize: Style.fontSizeM
									color: Style.inactiveTextColor
									elide: Text.ElideRight
									width: parent.width
								}
							}
						}

						Rectangle {
							id: moreButton
							anchors.right: parent.right
							anchors.rightMargin: Style.marginM
							anchors.verticalCenter: parent.verticalCenter
							width: Style.controlHeightM
							height: Style.controlHeightM
							radius: Style.controlHeightM / 2
							color: moreButtonMA.containsMouse ? Style.buttonHoverColor : "transparent"
							visible: collectionPage.__canManage && (itemMouseArea.containsMouse || itemDelegateRoot.isSelected || moreButtonMA.containsMouse)

							Text {
								anchors.centerIn: parent
								text: "\u2026"
								font.pixelSize: Style.fontSizeL
								color: Style.textColor
								horizontalAlignment: Text.AlignHCenter
								verticalAlignment: Text.AlignVCenter
							}

							MouseArea {
								id: moreButtonMA
								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: itemMenu.popup()
							}
						}

						Menu {
							id: itemMenu
							MenuItem {
								text: qsTr("Edit")
								enabled: collectionPage.__canManage
								onTriggered: collectionPage.__openEdit(itemDelegateRoot.itemId, itemDelegateRoot.itemTitle, itemDelegateRoot.itemDescription)
							}
							MenuItem {
								text: qsTr("Delete")
								enabled: collectionPage.__canManage
								onTriggered: {
									ModalDialogManager.showConfirmationDialog(
												collectionPage.__deleteSingleTitle,
												qsTr("Are you sure you want to delete \"%1\"? This action cannot be undone.").arg(itemDelegateRoot.itemTitle),
												function(result) {
													if (result === Enums.yes)
														collectionPage.removeItem(itemDelegateRoot.itemId)
												}
												)
								}
							}
						}

						Rectangle {
							anchors.bottom: parent.bottom
							anchors.left: parent.left
							anchors.right: parent.right
							height: 1
							color: Style.borderColor
							opacity: 0.5
						}
					}
				}

				CustomScrollbar {
					anchors.right: parent.right
					anchors.top: tableHeader.bottom
					anchors.bottom: parent.bottom
					targetItem: itemListView
					secondSize: 8
				}
			}
		}
	}

	function __openEdit(itemId, itemName, itemDescription) {
		collectionPage.__editItemId = itemId
		collectionPage.__isCreating = false
		while (collectionStackView.count > 1)
			collectionStackView.removePage(collectionStackView.count - 1)
		stackViewHeader.addHeader("edit", itemName || qsTr("Edit %1").arg(collectionPage.entityName))
		if (collectionPage.documentManager) {
			collectionStackView.addPage(editEditorView)
		} else if (collectionPage.customEditorComponent) {
			collectionStackView.addPage(collectionPage.customEditorComponent)
		}
		collectionStackView.next()
	}

	Component {
		id: createEditorView

		TenantDocumentEditorShell {
			documentManager: collectionPage.documentManager
			objectTypeId: collectionPage.objectTypeId
			createNew: true
			generateNewId: true
			activeShellTarget: collectionPage
			documentNameResolver: collectionPage.resolveDocumentName

			onClosed: {
				stackViewHeader.popHeader()
				collectionStackView.previous()
				while (collectionStackView.count > 1)
					collectionStackView.removePage(collectionStackView.count - 1)
			}
		}
	}

	Component {
		id: editEditorView

		TenantDocumentEditorShell {
			documentManager: collectionPage.documentManager
			objectTypeId: collectionPage.objectTypeId
			objectId: collectionPage.__editItemId
			createNew: false
			activeShellTarget: collectionPage
			documentNameResolver: collectionPage.resolveDocumentName

			onClosed: {
				stackViewHeader.popHeader()
				collectionStackView.previous()
				while (collectionStackView.count > 1)
					collectionStackView.removePage(collectionStackView.count - 1)
			}
		}
	}
}
