// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/**
 * SimpleCollectionTable
 *
 * Presentation-only list: search box + select-all header + ListView +
 * scrollbar. It does not fetch data itself - it renders `model` and reports
 * what the user did through signals.
 *
 * Item shape (default delegate): { id, title, description, parameters }.
 * Only `id` is required (used for selection); the rest is optional.
 * A custom `delegateComponent` can use any fields instead.
 *
 * The default delegate's row is a single click target: `itemClicked(id)`
 * fires on it, for a read-only list where clicking a row means "open it"
 * (an entity's tickets, a document's revision history, ...).
 *
 * A custom `delegateComponent` with its own columns can pair them with
 * `columnHeaderComponent` - a row of column titles shown above the list,
 * independent of the select-all bar (see EntityContextTicketsPanel /
 * DocumentHistoryView for a worked example).
 *
 * `model` accepts anything ListView accepts - a JS array, a ListModel or a
 * QAbstractItemModel. `SimpleCollectionItemDelegateBase` reads whichever of
 * `modelData`/`model` the runtime provides, so all three shapes work:
 *   1. A plain array/ListModel you own and refresh yourself:
 *        model: myListModel
 *        onFilterRequested: myListModel = filterArray(allItems, text)
 *   2. A FilterableSelectDataProvider's `items` (see imtcolgui/imtguigql):
 *        model: myDataProvider.items
 *        onFilterRequested: myDataProvider.fetch(text)
 *        onLoadMoreRequested: myDataProvider.fetchMore()
 *        onRetryRequested: myDataProvider.retry()
 *        initialLoading: myDataProvider.isInitialLoading
 *        loadingMore: myDataProvider.isPageLoading
 *        errorMessage: myDataProvider.error ? myDataProvider.error.message : ""
 *        Connections {
 *            target: myDataProvider
 *            function onDataChanged() { table.model = myDataProvider.items }
 *        }
 *
 * loading/error/pagination properties are optional and only relevant for
 * the paginated-provider case above; a plain array/ListModel just leaves
 * them at their defaults.
 */
Item {
	id: root

	property var model: null
	property Component delegateComponent: null
	property var actionHandler: null
	property string emptyText: qsTr("No items found.")
	property string filterPlaceholder: qsTr("Filter items...")
	// Upper bound for the centered content column (search box and table).
	property int maximumWidth: Style.contentWidthMax
	// Inset on each side of that column. Set to 0 when embedding the table in a
	// container that already applies its own margins.
	property int horizontalMargin: Style.marginXL
	// Chrome switches. Both default to on, so existing consumers are unaffected.
	// `filterEnabled` shows the search box; `selectionEnabled` shows the
	// select-all bar and puts a checkbox on each row of the default delegate.
	// A read-only list (document revisions, an entity's tickets) turns
	// selection off; a short list that never needs searching turns the filter off.
	property bool filterEnabled: true
	property bool selectionEnabled: true
	// Optional column-header row shown above the list, independent of the
	// select-all bar above. Pairs a custom `delegateComponent`'s columns with
	// matching titles; leave null (nothing shown) for the default delegate's
	// title/description rows. The component's root item must set an explicit
	// `height` (not `implicitHeight`) since it is sized through a Loader.
	property Component columnHeaderComponent: null
	// Gap between rows. Leave at 0 for flush rows separated by the delegate's
	// own divider; raise it for delegates drawn as detached cards.
	property int itemSpacing: 0
	// Optional pagination/loading state - see header comment above.
	property bool initialLoading: false
	property bool loadingMore: false
	property string errorMessage: ""

	property real __pagingContentY: 0
	property bool __restorePagingScroll: false

	onLoadingMoreChanged: {
		if (root.loadingMore) {
			root.__pagingContentY = itemListView.contentY
			root.__restorePagingScroll = true
		}
	}

	readonly property alias selectionManager: selectionManager_
	readonly property string filterText: filterInput.text
	readonly property int itemCount: itemListView.count
	// Single source of truth for the width of everything in this view, so the
	// search box and the table below it can never drift apart.
	readonly property int contentWidth: Math.min(root.width - root.horizontalMargin * 2, root.maximumWidth)
	// Height this view would like, if the surrounding page can spare it.
	readonly property real contentHeight: tableHeader.height + columnHeader.height + stateOverlay.height
		+ itemListView.contentHeight + loadingMoreRow.height + tableFrame.borderWidth * 2

	// Every keystroke, undebounced.
	signal filterChanged(string text)
	// Debounced by SearchTextInput - trigger fetch/refresh here.
	signal filterRequested(string text)
	// List scrolled to the bottom - fetch the next page here, if paginated.
	signal loadMoreRequested()
	// User clicked the error message - retry the failed fetch here.
	signal retryRequested()
	// A row was clicked, single click. Only fired by the default delegate - a
	// custom `delegateComponent` handles its own clicks and reports through
	// whatever mechanism makes sense for it (e.g. `actionHandler`).
	signal itemClicked(string id)

	// Reads one item out of `model`, hiding the difference between a JS array
	// (indexed), a ListModel (get()) and a QAbstractItemModel (data/roleNames).
	function itemAt(index) {
		if (!root.model)
			return null
		// ListModel path
		if (root.model.get)
			return root.model.get(index)
		// JS array path
		if (typeof root.model[index] !== "undefined")
			return root.model[index]
		// QAbstractItemModel path - build a role-keyed object from roleNames()
		if (root.model.roleNames && root.model.data && root.model.index) {
			var modelIndex = root.model.index(index, 0)
			var roles = root.model.roleNames()
			var item = {}
			for (var role in roles)
				item[roles[role]] = root.model.data(modelIndex, parseInt(role))
			return item
		}
		return null
	}

	function clearSelection() {
		selectionManager_.clear()
	}

	function selectAll() {
		var allIds = []
		for (var i = 0; i < itemListView.count; i++) {
			var item = root.itemAt(i)
			if (item && item.id)
				allIds.push(item.id)
		}
		selectionManager_.selectMultiple(allIds)
	}

	// --- layout metrics ------------------------------------------------- //
	readonly property int __filterTopMargin: root.filterEnabled ? Style.marginM : 0
	readonly property int __frameTopMargin: Style.marginM
	readonly property int __frameBottomMargin: Style.marginL
	readonly property real __availableFrameHeight: root.height - filterInput.height
		- root.__filterTopMargin - root.__frameTopMargin - root.__frameBottomMargin

	IdSelectionManager {
		id: selectionManager_
		multiSelect: true
	}

	SearchTextInput {
		id: filterInput
		visible: root.filterEnabled
		height: visible ? Style.controlHeightM : 0
		anchors.top: parent.top
		anchors.topMargin: root.__filterTopMargin
		anchors.horizontalCenter: parent.horizontalCenter
		width: root.contentWidth
		placeHolderText: root.filterPlaceholder

		onTextChanged: {
			selectionManager_.clear()
			root.filterChanged(filterInput.text)
		}

		onSearchChanged: {
			root.filterRequested(filterInput.text)
		}
	}

	SimpleCollectionTableFrame {
		id: tableFrame
		anchors.top: filterInput.bottom
		anchors.topMargin: root.__frameTopMargin
		anchors.horizontalCenter: parent.horizontalCenter
		width: root.contentWidth
		height: Math.min(root.contentHeight, root.__availableFrameHeight)

		SimpleCollectionTableHeader {
			id: tableHeader
			visible: root.selectionEnabled
			height: visible ? implicitHeight : 0
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			selectedCount: selectionManager_.selectedIds.length
			checkState: selectionManager_.selectedIds.length === 0
				? Qt.Unchecked
				: (selectionManager_.selectedIds.length === itemListView.count
					? Qt.Checked : Qt.PartiallyChecked)

			onSelectAllToggled: {
				if (checkState === Qt.Checked)
					root.clearSelection()
				else
					root.selectAll()
			}
		}

		Loader {
			id: columnHeader
			anchors.top: tableHeader.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			sourceComponent: root.columnHeaderComponent
		}

		SimpleCollectionStateOverlay {
			id: stateOverlay
			anchors.top: columnHeader.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			isEmpty: itemListView.count === 0
			initialLoading: root.initialLoading
			errorMessage: root.errorMessage
			emptyText: root.emptyText

			onRetryRequested: root.retryRequested()
		}

		ListView {
			id: itemListView
			anchors.top: stateOverlay.visible ? stateOverlay.bottom : columnHeader.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: loadingMoreRow.top
			clip: true
			spacing: root.itemSpacing
			boundsBehavior: Flickable.StopAtBounds
			model: root.model
			delegate: root.delegateComponent ? root.delegateComponent : defaultDelegateComponent

			onModelChanged: {
				// Restored synchronously, in the same tick as the model swap below -
				// a Timer (even interval: 0) defers to the next event-loop turn, which
				// is one turn too late: Qt Quick has already composited a frame with
				// contentY reset to 0 by then, so the fix itself became a visible
				// jump-to-top-then-to-middle instead of a silent one. Setting it here
				// happens before this tick's frame is ever painted, so only the final,
				// correct position reaches the screen. No clamping needed either - the
				// old position was valid for the smaller pre-append content, so it is
				// still valid now that content only grew.
				if (root.__restorePagingScroll) {
					root.__restorePagingScroll = false
					itemListView.contentY = root.__pagingContentY
				}
			}

			onContentYChanged: {
				if (itemListView.count === 0 || root.loadingMore)
					return

				if (itemListView.contentHeight <= itemListView.height)
					return
				if (itemListView.atYEnd)
					root.loadMoreRequested()
			}
		}

		Item {
			id: loadingMoreRow
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			visible: root.loadingMore
			height: visible ? Style.controlHeightM : 0

			BaseText {
				anchors.centerIn: parent
				text: qsTr("Loading more...")
				font.pixelSize: Style.fontSizeS
				font.italic: true
				color: Style.inactiveTextColor
			}
		}

		Component {
			id: defaultDelegateComponent

			SimpleCollectionItemDelegateBase {
				id: defaultDelegate
				selectionManager: root.selectionManager
				actionHandler: root.actionHandler
				showCheckBox: root.selectionEnabled

				onItemClicked: root.itemClicked(defaultDelegate.itemId)

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
			anchors.top: columnHeader.bottom
			anchors.bottom: parent.bottom
			targetItem: itemListView
			secondSize: 8
		}
	}
}
