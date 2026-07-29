// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * SimpleCollectionPage
 *
 * Generic CRUD page for a flat list: checkbox multiselect, search filter,
 * Create/Edit/Remove header buttons and create/edit sub-pages driven by a
 * CollectionDocumentManager.
 *
 * A subclass supplies the entity-specific wiring and nothing else - see
 * TenantRolesPage / TenantGroupsPage for the minimal shape, TenantMembersPage
 * for one that also replaces the delegate and the header buttons.
 *
 * Rows come from exactly one of:
 *   - `dataProvider` - a FilterableSelectDataProvider; this page drives
 *     fetch/fetchMore/retry and reflects its loading and error state.
 *   - `listModel` - a model the subclass builds and refreshes itself. Note
 *     that `refresh()` does nothing in this mode; the subclass owns reloading.
 *
 * It also implements the `actionHandler` interface expected by
 * SimpleCollectionItemDelegateBase (canManage/canEdit/canDelete, openEdit,
 * confirmRemoveItems).
 */
ViewBase {
	id: root

	commandsPanelVisible: false
	contentColor: Style.baseColor
	property var stateManager: null
	// Not used by this page; declared here because the whole page family is
	// wired the same way by its host and every subclass reads it.
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
	property var managePermissionIds: []
	property var createPermissionIds: []
	property var editPermissionIds: []
	property var deletePermissionIds: []

	// Width of the centered content column; the table matches it.
	readonly property int contentWidth: Math.min(root.width - Style.marginXL * 2, Style.contentWidthMax)
	readonly property int contentLeftMargin: Math.max((root.width - root.contentWidth) / 2, Style.marginXL)

	function removeItems(ids) {
		console.warn("SimpleCollectionPage.removeItems() is not implemented; override it in the consumer.")
	}

	function confirmRemoveItems(ids, itemTitle) {
		var singleItem = ids.length === 1 && itemTitle && itemTitle !== ""
		var title = singleItem ? root.__deleteSingleTitle : root.__deleteMultipleTitle
		var message = singleItem
			? qsTr("Are you sure you want to delete \"%1\"? This action cannot be undone.").arg(itemTitle)
			: qsTr("Are you sure you want to delete %1 selected item(s)? This action cannot be undone.").arg(ids.length)
		ModalDialogManager.showConfirmationDialog(title, message, function(result) {
			if (result === Enums.yes) {
				root.removeItems(ids)
				if (root.selectionManager)
					root.selectionManager.clear()
			}
		})
	}

	function updateGui() {}
	function updateModel() {}

	// Reloads through `dataProvider`. A `listModel`-driven subclass reloads
	// itself and can leave this alone.
	function refresh() {
		if (!root.visible) {
			root.__refreshPending = true
			return
		}
		if (dataProvider)
			dataProvider.fetch(__lastFilterText)
	}

	// Override in subcomponents to provide entity-specific document name resolution
	property var documentNameFields: ["m_name"]

	function resolveDocumentName(documentId) {
		var view = root.documentManager.getDocumentViewInstance(documentId, "")
		if (view && view.model) {
			for (var i = 0; i < root.documentNameFields.length; i++) {
				var field = root.documentNameFields[i]
				if (view.model[field])
					return view.model[field]
			}
		}
		return ""
	}

	function popEditor() {
		__resetStackToList()
		root.__activeShellView = null
	}

	// --- internal state ---
	property string __editItemId: ""
	property bool __isCreating: false
	property var __activeShellView: null

	// Public - read by actionHandler consumers such as SimpleCollectionItemDelegateBase.
	readonly property bool canManage: stateManager
		? (managePermissionIds.length > 0
			? stateManager.hasAnyPermission(managePermissionIds)
			: stateManager.canManageMembers)
		: false

	function __resolveCommandPermission(permissionIds, fallbackValue) {
		if (!stateManager)
			return false
		if (permissionIds && permissionIds.length > 0)
			return stateManager.hasAnyPermission(permissionIds)
		return fallbackValue
	}

	readonly property bool canCreate: __resolveCommandPermission(createPermissionIds, canManage)
	readonly property bool canEdit: __resolveCommandPermission(editPermissionIds, canManage)
	readonly property bool canDelete: __resolveCommandPermission(deletePermissionIds, canManage)

	readonly property int __selectedCount: selectionManager ? selectionManager.selectedIds.length : 0
	readonly property bool __onListPage: collectionStackView.currentIndex === 0
	readonly property bool __useDefaultButtons: !headerButtonsComponent

	property var selectionManager: null
	property string __lastFilterText: ""
	property var __listItems: []
	property bool __refreshPending: false

	// Exactly one of listModel/dataProvider should be set; listModel wins if both are (see __warnIfMisconfigured).
	readonly property var effectiveModel: root.listModel ? root.listModel : root.__listItems

	function __findItemById(id) {
		var items = root.effectiveModel
		if (!items)
			return null
		var count = items.count !== undefined ? items.count : items.length
		for (var i = 0; i < count; i++) {
			var item = items.get ? items.get(i) : items[i]
			if (item && item.id === id)
				return item
		}
		return null
	}

	function __warnIfMisconfigured() {
		if (root.listModel && root.dataProvider)
			console.warn("SimpleCollectionPage: both listModel and dataProvider are set - listModel wins and dataProvider results are ignored.")
	}

	property var dataProvider: null
	onListModelChanged: __warnIfMisconfigured()
	onDataProviderChanged: {
		__warnIfMisconfigured()
		if (dataProvider && root.visible)
			dataProvider.fetch(__lastFilterText)
		else if (dataProvider)
			root.__refreshPending = true
	}
	onVisibleChanged: {
		if (root.visible && root.__refreshPending) {
			root.__refreshPending = false
			root.refresh()
		}
	}
	readonly property string filterText: __lastFilterText

	Connections {
		target: root.dataProvider
		function onDataChanged() {
			if (root.visible)
				root.__listItems = root.dataProvider.items
			else
				root.__refreshPending = true
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

	function __pushEditorPage() {
		if (root.documentManager)
			collectionStackView.addPage(editorViewComponent)
		else if (root.customEditorComponent)
			collectionStackView.addPage(root.customEditorComponent)
		collectionStackView.next()
	}

	function openCreate() {
		__clearExtraPages()
		root.__isCreating = true
		stackViewHeader.addHeader("create", qsTr("Create New %1").arg(root.entityName))
		__pushEditorPage()
	}

	function openEdit(itemId, itemName, itemDescription) {
		__openEdit(itemId, itemName, itemDescription)
	}

	function __openEdit(itemId, itemName, itemDescription) {
		root.__editItemId = itemId
		root.__isCreating = false
		__clearExtraPages()
		stackViewHeader.addHeader("edit", itemName || qsTr("Edit %1").arg(root.entityName))
		__pushEditorPage()
	}

	Connections {
		target: root.documentManager
		function onDocumentSaved(documentId) {
			root.refresh()
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
		anchors.leftMargin: root.contentLeftMargin
		width: root.contentWidth
		height: Style.controlHeightL
		initialItemTitleVisible: true

		onCloseClicked: {
			if (root.__activeShellView
					&& root.__activeShellView.state === "content") {
				root.__activeShellView.closeDocument()
			} else {
				collectionStackView.previous()
				stackViewHeader.popHeader()
			}
		}

		onHeaderItemClicked: {
			if (collectionStackView.currentIndex <= index)
				return
			if (root.__activeShellView
					&& root.__activeShellView.state === "content") {
				root.__activeShellView.closeDocument()
				return
			}
			while (collectionStackView.currentIndex > index) {
				collectionStackView.previous()
				stackViewHeader.popHeader()
			}
		}

		Component.onCompleted: {
			stackViewHeader.addHeader("list", root.__listTitle)
		}
	}

	Text {
		id: createBtn
		visible: root.showCreateButton && root.canCreate && root.__onListPage && root.__useDefaultButtons
		anchors.right: stackViewHeader.right
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: root.__createBtnText
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: Style.linkColor

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: {
				if (root.canCreate)
					root.openCreate()
			}
		}
	}

	Text {
		id: editBtn
		visible: root.canEdit && root.__onListPage && root.__useDefaultButtons
		anchors.right: createBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: qsTr("Edit")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: root.__selectedCount === 1 ? Style.linkColor : Style.inactiveTextColor
		opacity: root.__selectedCount === 1 ? 1.0 : 0.5

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: root.__selectedCount === 1 && root.canEdit ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: root.__selectedCount === 1 && root.canEdit
			onClicked: {
				if (!root.canEdit)
					return

				var selId = root.selectionManager.selectedIds[0]
				var item = root.__findItemById(selId)
				if (item)
					root.__openEdit(selId, item.title || item.id || "", item.description || "")
			}
		}
	}

	Text {
		id: removeBtn
		visible: root.canDelete && root.__onListPage && root.__useDefaultButtons
		anchors.right: editBtn.left
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: stackViewHeader.verticalCenter
		text: qsTr("Remove")
		font.pixelSize: Style.fontSizeM
		font.bold: true
		color: root.__selectedCount > 0 ? Style.errorColor : Style.inactiveTextColor
		opacity: root.__selectedCount > 0 ? 1.0 : 0.5

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: root.__selectedCount > 0 && root.canDelete ? Qt.PointingHandCursor : Qt.ArrowCursor
			enabled: root.__selectedCount > 0 && root.canDelete
			onClicked: {
				if (!root.canDelete)
					return

				root.confirmRemoveItems(root.selectionManager.selectedIds.slice(), "")
			}
		}
	}

	Loader {
		id: customHeaderButtonsLoader
		visible: !!root.headerButtonsComponent && root.__onListPage
		anchors.right: stackViewHeader.right
		anchors.verticalCenter: stackViewHeader.verticalCenter
		sourceComponent: root.headerButtonsComponent
		property var page: root
	}

	BaseText {
		id: descriptionLabel
		anchors.top: stackViewHeader.bottom
		anchors.topMargin: Style.marginS
		anchors.horizontalCenter: parent.horizontalCenter
		width: root.contentWidth
		visible: root.__onListPage && root.descriptionText.length > 0
		text: root.descriptionText
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

		SimpleCollectionTable {
			id: collectionTable
			model: root.effectiveModel
			delegateComponent: root.delegateComponent
			actionHandler: root
			emptyText: root.__emptyText
			filterPlaceholder: root.__filterPlaceholder
			initialLoading: root.dataProvider ? root.dataProvider.isInitialLoading : false
			loadingMore: root.dataProvider ? root.dataProvider.isPageLoading : false
			errorMessage: root.dataProvider && root.dataProvider.error
				? (root.dataProvider.error.message || qsTr("Error loading items"))
				: ""

			onFilterChanged: root.__lastFilterText = text
			onFilterRequested: {
				if (root.dataProvider)
					root.dataProvider.fetch(text)
			}
			onLoadMoreRequested: {
				if (root.dataProvider)
					root.dataProvider.fetchMore()
			}
			onRetryRequested: {
				if (root.dataProvider)
					root.dataProvider.retry()
			}

			Component.onCompleted: root.selectionManager = collectionTable.selectionManager
			Component.onDestruction: {
				if (root)
					root.selectionManager = null
			}
		}
	}

	Component {
		id: editorViewComponent

		SimpleDocumentEditorShell {
			documentManager: root.documentManager
			objectTypeId: root.objectTypeId
			objectId: root.__isCreating ? "" : root.__editItemId
			createNew: root.__isCreating
			generateNewId: root.__isCreating
			activeShellTarget: root
			documentNameResolver: root.resolveDocumentName

			onClosed: root.__resetStackToList()
		}
	}
}
