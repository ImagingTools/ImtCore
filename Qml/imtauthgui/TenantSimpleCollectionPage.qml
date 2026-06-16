// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantSimpleCollectionPage
 *
 * Generic CRUD page for TenantEditor tabs that need a flat list with
 * checkbox-multiselect, search filter, Create/Edit/Remove header buttons
 * and create/edit sub-pages driven by a CollectionDocumentManager.
 *
 * Consumed by TenantRolesPage, TenantGroupsPage; the Members tab has its
 * own delegate and does not use this component.
 */
ViewBase {
	id: collectionPage

	commandsPanelVisible: false
	contentColor: Style.baseColor
	readonly property var tenantData: collectionPage.model
	property var stateManager: null
	property var apiClient: null

	// --- entity-specific configuration ---
	property string entityName: "Item"
	property string entityNamePlural: "Items"
	property string listHeaderTitle: ""
	property string descriptionText: ""
	property string createButtonText: ""
	property string emptyText: ""
	property string filterPlaceholder: ""
	property string deleteSingleTitle: ""
	property string deleteMultipleTitle: ""

	// --- backend configuration ---
	property var documentManager: null
	property string objectTypeId: ""
	property var listModel: null
	property Component delegateComponent: null
	property Component headerButtonsComponent: null
	property Component customEditorComponent: null
	property bool showCreateButton: true
	function removeItems(ids) {}

	function updateGui() {}
	function updateModel() {}

	function refresh() {
		if (dataProvider)
			dataProvider.fetch(__lastFilterText)
	}

	// Override in subcomponents to provide entity-specific document name resolution
	property var documentNameFields: ["m_name"]

	function resolveDocumentName(documentId) {
		var view = collectionPage.documentManager.getDocumentViewInstance(documentId, "")
		if (view && view.model) {
			for (var i = 0; i < collectionPage.documentNameFields.length; i++) {
				var field = collectionPage.documentNameFields[i]
				if (view.model[field])
					return view.model[field]
			}
		}
		return ""
	}

	function popEditor() {
		__resetStackToList()
		collectionPage.__activeShellView = null
	}

	// --- internal state ---
	property string __editItemId: ""
	property bool __isCreating: false
	property var __activeShellView: null

	readonly property bool __canManage: stateManager ? stateManager.canManageMembers : false
	readonly property int __selectedCount: selectionManager ? selectionManager.selectedIds.length : 0
	readonly property bool __onListPage: collectionStackView.currentIndex === 0
	readonly property bool __useDefaultButtons: !headerButtonsComponent

	property var selectionManager: null
	property string __lastFilterText: ""
	property var __listItems: []

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

	// --- derived display strings ---
	readonly property string __listTitle: listHeaderTitle.length > 0 ? listHeaderTitle : entityNamePlural
	readonly property string __createBtnText: createButtonText.length > 0 ? createButtonText : ("+ " + qsTr("Create %1").arg(entityName))
	readonly property string __emptyText: emptyText.length > 0 ? emptyText : qsTr("No %1 found.").arg(entityNamePlural.toLowerCase())
	readonly property string __filterPlaceholder: filterPlaceholder.length > 0 ? filterPlaceholder : qsTr("Filter %1...").arg(entityNamePlural.toLowerCase())
	readonly property string __deleteSingleTitle: deleteSingleTitle.length > 0 ? deleteSingleTitle : qsTr("Delete %1").arg(entityName)
	readonly property string __deleteMultipleTitle: deleteMultipleTitle.length > 0 ? deleteMultipleTitle : qsTr("Delete %1").arg(entityNamePlural)

	// --- stack navigation helpers ---
	function __resetStackToList() {
		stackViewHeader.popHeader()
		collectionStackView.previous()
		while (collectionStackView.count > 1)
			collectionStackView.removePage(collectionStackView.count - 1)
	}

	function __clearExtraPages() {
		while (collectionStackView.count > 1)
			collectionStackView.removePage(collectionStackView.count - 1)
	}

	function openCreate() {
		__clearExtraPages()
		collectionPage.__isCreating = true
		stackViewHeader.addHeader("create", qsTr("Create New %1").arg(collectionPage.entityName))
		if (collectionPage.documentManager) {
			collectionStackView.addPage(editorViewComponent)
		} else if (collectionPage.customEditorComponent) {
			collectionStackView.addPage(collectionPage.customEditorComponent)
		}
		collectionStackView.next()
	}

	function openEdit(itemId, itemName, itemDescription) {
		__openEdit(itemId, itemName, itemDescription)
	}

	function __openEdit(itemId, itemName, itemDescription) {
		collectionPage.__editItemId = itemId
		collectionPage.__isCreating = false
		__clearExtraPages()
		stackViewHeader.addHeader("edit", itemName || qsTr("Edit %1").arg(collectionPage.entityName))
		if (collectionPage.documentManager) {
			collectionStackView.addPage(editorViewComponent)
		} else if (collectionPage.customEditorComponent) {
			collectionStackView.addPage(collectionPage.customEditorComponent)
		}
		collectionStackView.next()
	}

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

	// --- UI ---
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
			} else {
				collectionStackView.previous()
				stackViewHeader.popHeader()
			}
		}

		onHeaderItemClicked: {
			if (collectionStackView.currentIndex <= index)
				return
			if (collectionPage.__activeShellView
					&& collectionPage.__activeShellView.state === "content") {
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
		visible: collectionPage.showCreateButton && collectionPage.__canManage && collectionPage.__onListPage && collectionPage.__useDefaultButtons
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
		visible: collectionPage.__canManage && collectionPage.__onListPage && collectionPage.__useDefaultButtons
		anchors.right: createBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: qsTr("Edit")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: collectionPage.__selectedCount === 1 ? Style.linkColor : Style.inactiveTextColor
		opacity: collectionPage.__selectedCount === 1 ? 1.0 : 0.5

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: collectionPage.__selectedCount === 1 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: collectionPage.__selectedCount === 1
			onClicked: {
				var selId = collectionPage.selectionManager.selectedIds[0]
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
		visible: collectionPage.__canManage && collectionPage.__onListPage && collectionPage.__useDefaultButtons
		anchors.right: editBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: qsTr("Remove")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: collectionPage.__selectedCount > 0 ? Style.errorColor : Style.inactiveTextColor
		opacity: collectionPage.__selectedCount > 0 ? 1.0 : 0.5

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: collectionPage.__selectedCount > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: collectionPage.__selectedCount > 0
			onClicked: {
				ModalDialogManager.showConfirmationDialog(
							collectionPage.__deleteMultipleTitle,
							qsTr("Are you sure you want to delete %1 selected item(s)? This action cannot be undone.").arg(collectionPage.__selectedCount),
							function(result) {
								if (result === Enums.yes) {
									var ids = collectionPage.selectionManager.selectedIds.slice()
									collectionPage.removeItems(ids)
									collectionPage.selectionManager.clear()
								}
							}
							)
			}
		}
	}

	Loader {
		id: customHeaderButtonsLoader
		visible: collectionPage.headerButtonsComponent && collectionPage.__onListPage
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
		visible: collectionPage.__onListPage && collectionPage.descriptionText.length > 0
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
			readonly property int effectiveCount: effectiveModel ? (effectiveModel.count || effectiveModel.length || 0) : 0

			SearchTextInput {
				id: filterInput
				anchors.top: parent.top
				anchors.topMargin: Style.marginM
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - Style.marginXL * 2, 1000)
				placeHolderText: collectionPage.__filterPlaceholder
				onTextChanged: {
					selectionManager_.clear()
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
					id: selectionManager_
					multiSelect: true
					Component.onCompleted: {
						collectionPage.selectionManager = selectionManager_
					}
					Component.onDestruction: {
						if (collectionPage)
							collectionPage.selectionManager = null
					}
				}

				TenantTableHeader {
					id: tableHeader
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					selectedCount: selectionManager_.selectedIds.length
					totalCount: itemListView.count
					checkState: selectionManager_.selectedIds.length === 0
								? Qt.Unchecked
								: (selectionManager_.selectedIds.length === itemListView.count
								   ? Qt.Checked : Qt.PartiallyChecked)

					onSelectAllToggled: {
						if (checkState === Qt.Checked) {
							selectionManager_.clear()
						} else {
							var allIds = []
							var items = listViewItem.effectiveModel
							var itemCount = itemListView.count
							for (var i = 0; i < itemCount; i++) {
								var item = items && items.get ? items.get(i) : items[i]
								if (item && item.id)
									allIds.push(item.id)
							}
							selectionManager_.selectMultiple(allIds)
						}
					}
				}

				Item {
					id: emptyState
					visible: listViewItem.effectiveCount === 0
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

					TenantCollectionItemDelegateBase {
						id: defaultDelegate
						selectionManager: selectionManager_
						collectionPage: collectionPage

						BaseText {
							text: defaultDelegate.itemTitle
							font.pixelSize: Style.fontSizeL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							visible: defaultDelegate.itemDescription !== ""
							text: defaultDelegate.itemDescription
							font.pixelSize: Style.fontSizeM
							color: Style.inactiveTextColor
							elide: Text.ElideRight
							width: parent.width
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

	Component {
		id: editorViewComponent

		TenantDocumentEditorShell {
			documentManager: collectionPage.documentManager
			objectTypeId: collectionPage.objectTypeId
			objectId: collectionPage.__isCreating ? "" : collectionPage.__editItemId
			createNew: collectionPage.__isCreating
			generateNewId: collectionPage.__isCreating
			activeShellTarget: collectionPage
			documentNameResolver: collectionPage.resolveDocumentName

			onClosed: collectionPage.__resetStackToList()
		}
	}
}
