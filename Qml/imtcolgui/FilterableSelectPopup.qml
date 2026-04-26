import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype FilterableSelectPopup
	\inqmlmodule imtcolgui
	\brief Reusable filterable select popup for large remote datasets.

	Pure UI component — all data fetching, pagination and normalization are handled
	by the injected \c dataProvider (FilterableSelectDataProvider).
	The popup works only with normalized JS objects \c { id, title, ... }.

	Supports:
	- Debounced server-side search (300ms default)
	- Infinite scroll via dataProvider.fetchMore()
	- Single and multi selection (id-based, independent from visible dataset)
	- Persistent selectedItems map: selected items remain accessible even when
	  not in the current visible dataset (pagination/filtering)
	- Preselected IDs resolved via dataProvider.fetchByIds() for items not yet loaded
	- Injectable delegate component with explicit roles (\c idRole, \c textRole)
	- Keyboard navigation (↑↓ Enter Escape)
	- Error display with click-to-retry
	- Split loading states (initial vs page loading)

	\sa FilterableSelectDataProvider, FilterableSelectGqlDataProvider
*/
Item {
	id: root

	visible: false
	z: 1000

	width: itemWidth
	height: filterField.height + itemBody.height + (footerItem.visible ? footerItem.height : 0)

	// --- Data Controller (dependency injection) ---
	property QtObject dataProvider: null

	// --- Configuration ---
	property int debounceInterval: 300

	// --- Delegate roles ---
	property string idRole: "id"
	property string textRole: "title"

	// --- Visual ---
	property int itemWidth: Style.sizeHintXXS
	property int itemHeight: Style.controlHeightM
	property int maxVisibleItems: 8
	property int textSize: Style.fontSizeM
	property string fontColor: Style.textColor

	// --- Selection ---
	property bool multiSelect: false
	property var preselectedIds: []

	// --- Injectable delegate ---
	property Component delegate: Component {
		PopupMenuDelegate {
			width: root.itemWidth
			height: root.itemHeight
			textSize: root.textSize
			fontColor: root.fontColor

			text: root.getItemText(model.index)

			selected: __internal.focusedIndex === model.index
			highlighted: root.isItemSelected(root.getItemId(model.index))

			onClicked: {
				root.toggleSelection(root.getItemId(model.index), model.index)
			}

			onEntered: {
				__internal.focusedIndex = model.index
			}
		}
	}

	// --- Signals ---
	signal itemSelected(string itemId, int index)
	signal selectionChanged(var selectedIds)
	signal opened()
	signal closed()

	// --- Delegate helpers (public, use in custom delegates) ---

	/*!
		Returns the id of the item at the given index using the configured \c idRole.
		\param index The index in the dataProvider.items array.
	*/
	function getItemId(index){
		if (!root.dataProvider || index < 0 || index >= root.dataProvider.items.length){
			return ""
		}
		return String(root.dataProvider.items[index][root.idRole] || "")
	}

	/*!
		Returns the display text of the item at the given index using the configured \c textRole.
		\param index The index in the dataProvider.items array.
	*/
	function getItemText(index){
		if (!root.dataProvider || index < 0 || index >= root.dataProvider.items.length){
			return ""
		}
		return String(root.dataProvider.items[index][root.textRole] || "")
	}

	/*!
		Returns the full item object at the given index (for custom delegates needing extra fields).
		\param index The index in the dataProvider.items array.
	*/
	function getItem(index){
		if (!root.dataProvider || index < 0 || index >= root.dataProvider.items.length){
			return null
		}
		return root.dataProvider.items[index]
	}

	// --- Selected items API ---

	/*!
		Returns the display text for a selected item by its ID.
		Resolves from dataProvider.selectedItems which persists across pagination/filtering.
		\param id The item ID.
	*/
	function getSelectedItemText(id){
		if (!root.dataProvider){
			return ""
		}
		return root.dataProvider.getSelectedItemText(id)
	}

	/*!
		Returns an array of all selected item objects from the persistent selectedItems map.
	*/
	function getSelectedItems(){
		if (!root.dataProvider){
			return []
		}
		return root.dataProvider.getSelectedItems()
	}

	// --- Internal state ---
	property QtObject __internal: QtObject {
		property var selectedIds: ({})
		property int focusedIndex: -1
		property bool hoverBlocked: true
	}

	// --- Public API ---
	function open(){
		if (root.visible){
			return
		}

		__internal.selectedIds = ({})
		__internal.focusedIndex = -1
		__internal.hoverBlocked = true

		filterField.text = ""

		applyPreselectedIds()

		root.visible = true
		root.opened()

		if (root.dataProvider){
			root.dataProvider.fetch("")
		}
	}

	function close(){
		if (!root.visible){
			return
		}

		root.visible = false
		root.closed()
	}

	function isItemSelected(itemId){
		return itemId !== undefined && itemId !== null && itemId !== "" && __internal.selectedIds[String(itemId)] === true
	}

	function toggleSelection(itemId, index){
		if (itemId === undefined || itemId === null || itemId === ""){
			return
		}

		var id = String(itemId)
		var selected = ({})
		for (var key in __internal.selectedIds){
			selected[key] = __internal.selectedIds[key]
		}

		if (root.multiSelect){
			if (selected[id]){
				delete selected[id]
				if (root.dataProvider){
					root.dataProvider.removeSelectedItem(id)
				}
			}
			else {
				selected[id] = true
				if (root.dataProvider){
					var item = root.getItem(index)
					if (item){
						root.dataProvider.addSelectedItem(id, item)
					}
				}
			}
		}
		else {
			selected = ({})
			selected[id] = true
			if (root.dataProvider){
				root.dataProvider.clearSelectedItems()
				var singleItem = root.getItem(index)
				if (singleItem){
					root.dataProvider.addSelectedItem(id, singleItem)
				}
			}
		}

		__internal.selectedIds = selected
		root.itemSelected(id, index)
		root.selectionChanged(getSelectedIds())

		if (!root.multiSelect){
			root.close()
		}
	}

	function getSelectedIds(){
		var result = []
		for (var id in __internal.selectedIds){
			if (__internal.selectedIds[id]){
				result.push(id)
			}
		}
		return result
	}

	// --- Internal helpers ---
	function applyPreselectedIds(){
		var selected = ({})
		var ids = root.preselectedIds || []
		for (var i = 0; i < ids.length; i++){
			var id = String(ids[i])
			if (id !== ""){
				selected[id] = true
			}
		}
		__internal.selectedIds = selected

		// Resolve missing items via fetchByIds
		if (root.dataProvider && ids.length > 0){
			var missingIds = []
			for (var j = 0; j < ids.length; j++){
				var sid = String(ids[j])
				if (sid !== "" && !root.dataProvider.getSelectedItemText(sid)){
					missingIds.push(sid)
				}
			}
			if (missingIds.length > 0){
				root.dataProvider.fetchByIds(missingIds)
			}
		}
	}

	// --- Background overlay to close on outside click ---
	MouseArea {
		id: backgroundOverlay

		parent: root.parent ? root.parent : root
		anchors.fill: parent
		z: root.z - 1
		visible: root.visible
		enabled: root.visible

		onClicked: {
			root.close()
		}
	}

	// --- Data provider connection ---
	Connections {
		target: root.dataProvider
		enabled: root.dataProvider !== null

		onDataChanged: {
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			if (__internal.focusedIndex >= itemCount){
				__internal.focusedIndex = itemCount > 0 ? itemCount - 1 : -1
			}
		}

		onSelectedItemsResolved: {
			// Preselected items have been resolved — emit signal for consumers
			root.selectionChanged(root.getSelectedIds())
		}
	}

	// --- Debounce Timer ---
	PauseAnimation {
		id: debounce

		duration: root.debounceInterval
		onFinished: {
			if (root.dataProvider){
				__internal.focusedIndex = -1
				root.dataProvider.fetch(filterField.text)
			}
		}
	}

	// --- Search Field ---
	CustomTextField {
		id: filterField

		z: 100
		width: root.itemWidth
		height: Style.controlHeightM
		anchors.top: parent.top
		anchors.left: parent.left
		margin: Style.marginM
		textSize: root.textSize
		fontColor: root.fontColor
		placeHolderText: qsTr("Filter...")

		onTextChanged: {
			__internal.focusedIndex = -1
			debounce.stop()
			debounce.start()
		}

		onAccepted: {
			debounce.stop()
			if (root.dataProvider){
				__internal.focusedIndex = -1
				root.dataProvider.fetch(filterField.text)
			}
		}

		Button {
			id: clearButton

			anchors.verticalCenter: filterField.verticalCenter
			anchors.right: filterField.right
			anchors.rightMargin: Style.marginXS

			width: height
			height: 12
			z: filterField.z + 1

			decorator: Component { IconButtonDecorator {} }
			iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)

			visible: filterField.text !== ""
			enabled: visible

			onClicked: {
				filterField.text = ""
			}
		}
	}

	// --- List Body ---
	Rectangle {
		id: itemBody

		anchors.top: filterField.bottom
		anchors.left: parent.left

		width: root.itemWidth
		height: errorItem.visible ? errorItem.height
				: noDataItem.visible ? noDataItem.height
				: popupListView.height
		radius: Style.buttonRadius

		color: Style.baseColor
		border.width: 1
		border.color: Style.alternateBaseColor

		Rectangle {
			id: noDataItem

			width: parent.width
			height: 50
			radius: parent.radius
			color: parent.color
			visible: root.dataProvider
					&& !root.dataProvider.isInitialLoading
					&& !root.dataProvider.error
					&& root.dataProvider.items.length === 0

			Text {
				anchors.centerIn: parent
				font.pixelSize: root.textSize
				color: root.fontColor
				text: qsTr("No items found")
			}
		}

		Rectangle {
			id: errorItem

			width: parent.width
			height: 50
			radius: parent.radius
			color: parent.color
			visible: root.dataProvider
					&& root.dataProvider.error !== null
					&& !root.dataProvider.isInitialLoading
					&& !root.dataProvider.isPageLoading

			Text {
				anchors.centerIn: parent
				font.pixelSize: root.textSize
				color: Style.errorColor
				text: root.dataProvider && root.dataProvider.error
					? (root.dataProvider.error.message || qsTr("Error loading items"))
					: qsTr("Error loading items")
			}

			MouseArea {
				anchors.fill: parent
				onClicked: {
					if (root.dataProvider){
						root.dataProvider.retry()
					}
				}
			}
		}

		Rectangle {
			id: loadingOverlay

			anchors.fill: parent
			opacity: 0.5
			color: "transparent"
			visible: root.dataProvider && root.dataProvider.isInitialLoading

			Text {
				anchors.centerIn: parent
				color: Style.textColor
				font.pixelSize: Style.fontSizeM
				text: qsTr("Loading...")
			}
		}

		ListView {
			id: popupListView

			property int itemCount: root.dataProvider ? root.dataProvider.items.length : 0

			width: root.itemWidth
			height: itemCount === 0 ? 0
					: (root.maxVisibleItems === -1 ? itemCount : Math.min(root.maxVisibleItems, itemCount)) * root.itemHeight

			boundsBehavior: Flickable.StopAtBounds
			clip: true
			model: popupListView.itemCount

			delegate: root.delegate

			onContentYChanged: {
				if (contentHeight > 0 && contentHeight - contentY - popupListView.height <= root.itemHeight){
					if (root.dataProvider){
						root.dataProvider.fetchMore()
					}
				}
			}
		}

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			visible: __internal.hoverBlocked

			onPositionChanged: {
				__internal.hoverBlocked = false
			}
		}
	}

	// --- Footer (loading more) ---
	Item {
		id: footerItem

		anchors.top: itemBody.bottom
		anchors.left: parent.left

		width: root.itemWidth
		height: visible ? 30 : 0
		visible: root.dataProvider
				&& root.dataProvider.isPageLoading
				&& root.dataProvider.items.length > 0

		Text {
			anchors.centerIn: parent
			color: Style.placeHolderTextColor
			font.pixelSize: Style.fontSizeS
			text: qsTr("Loading more...")
		}
	}

	// --- Shadow ---
	DropShadow {
		id: dropShadow

		anchors.fill: itemBody
		z: itemBody.z - 1

		horizontalOffset: 2
		verticalOffset: 2
		radius: 4
		color: Style.shadowColor
		source: itemBody
	}

	// --- Keyboard navigation ---
	Shortcut {
		sequence: "Escape"
		enabled: root.visible
		onActivated: {
			root.close()
		}
	}

	Shortcut {
		sequence: "Return"
		enabled: root.visible && !filterField.textInputFocus
		onActivated: {
			if (__internal.focusedIndex >= 0){
				var id = root.getItemId(__internal.focusedIndex)
				root.toggleSelection(id, __internal.focusedIndex)
			}
		}
	}

	Shortcut {
		sequence: "Up"
		enabled: root.visible
		onActivated: {
			if (filterField.textInputFocus){
				filterField.setFocus(false)
			}
			__internal.hoverBlocked = true
			if (__internal.focusedIndex > 0){
				__internal.focusedIndex--
				contentYCorrection(false)
			}
		}
	}

	Shortcut {
		sequence: "Down"
		enabled: root.visible
		onActivated: {
			if (filterField.textInputFocus){
				filterField.setFocus(false)
			}
			__internal.hoverBlocked = true
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			if (__internal.focusedIndex < itemCount - 1){
				__internal.focusedIndex++
				contentYCorrection(true)
			}
			else if (__internal.focusedIndex === itemCount - 1 && root.dataProvider){
				root.dataProvider.fetchMore()
			}
		}
	}

	function contentYCorrection(down_){
		if (__internal.focusedIndex >= 0){
			var contentY = popupListView.contentY
			var itemH = root.itemHeight
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			var visibleCount = root.maxVisibleItems === -1 ? itemCount : Math.min(root.maxVisibleItems, itemCount)
			if (visibleCount <= 0) return
			var index = __internal.focusedIndex

			if (down_){
				if ((index + 1) * itemH > contentY + visibleCount * itemH){
					popupListView.contentY = (index + 1) * itemH - visibleCount * itemH
				}
			}
			else {
				if (index * itemH < contentY){
					popupListView.contentY = index * itemH
				}
			}
		}
	}
}
