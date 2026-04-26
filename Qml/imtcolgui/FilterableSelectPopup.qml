import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

/*!
	\qmltype FilterableSelectPopup
	\inqmlmodule imtcolgui
	\brief Reusable filterable select popup for large remote datasets.

	Pure UI component — all data fetching, pagination, normalization and selection
	are handled by the injected \c dataProvider (FilterableSelectDataProvider).
	The popup works only with normalized JS objects \c { id, title, ... }.
	Selection state lives entirely in the DataProvider.

	Supports:
	- Debounced server-side search (300ms default)
	- Infinite scroll via dataProvider.fetchMore()
	- Single and multi selection (delegated to dataProvider)
	- Preselected IDs resolved via dataProvider.setPreselectedIds()
	- Injectable delegate component with explicit roles (\c idRole, \c textRole)
	- Keyboard navigation (↑↓ Enter Escape)
	- Error display with click-to-retry
	- Split loading states (initial vs page loading)

	\sa FilterableSelectDataProvider, FilterableSelectGqlDataProvider
*/
PopupView {
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
	property var preselectedIds: []

	// --- Injectable delegate ---
	property Component delegate: Component {
		PopupMenuDelegate {
			width: root.itemWidth
			height: root.itemHeight
			textSize: root.textSize
			fontColor: root.fontColor

			text: root.getItemText(model.index)

			selected: root.__internal.focusedIndex === model.index
			highlighted: root.dataProvider ? root.dataProvider.isItemSelected(root.getItemId(model.index)) : false

			onClicked: {
				root.handleItemClick(root.getItemId(model.index), model.index)
			}

			onEntered: {
				root.__internal.focusedIndex = model.index
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

	// --- Internal state (UI-only) ---
	property QtObject __internal: QtObject {
		property int focusedIndex: -1
		property bool hoverBlocked: true
	}

	// --- Public API ---
	function open(){
		if (root.visible){
			return
		}

		root.__internal.focusedIndex = -1
		root.__internal.hoverBlocked = true

		filterField.text = ""

		if (root.dataProvider){
			root.dataProvider.setPreselectedIds(root.preselectedIds)
		}

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

	function handleItemClick(itemId, index){
		if (!root.dataProvider || itemId === ""){
			return
		}
		var item = root.getItem(index)
		root.dataProvider.toggleItem(itemId, item)
		root.itemSelected(itemId, index)

		if (!root.dataProvider.multiSelect){
			root.close()
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

		function onDataChanged() {
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			if (root.__internal.focusedIndex >= itemCount){
				root.__internal.focusedIndex = itemCount > 0 ? itemCount - 1 : -1
			}
		}

		function onSelectionChanged(){
			root.selectionChanged(root.dataProvider.getSelectedIds())
		}
	}

	// --- Debounce Timer ---
	PauseAnimation {
		id: debounce

		duration: root.debounceInterval
		onFinished: {
			if (root.dataProvider){
				root.__internal.focusedIndex = -1
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
			root.__internal.focusedIndex = -1
			debounce.stop()
			debounce.start()
		}

		onAccepted: {
			debounce.stop()
			if (root.dataProvider){
				root.__internal.focusedIndex = -1
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
			visible: root.__internal.hoverBlocked

			onPositionChanged: {
				root.__internal.hoverBlocked = false
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
			if (root.__internal.focusedIndex >= 0){
				var id = root.getItemId(root.__internal.focusedIndex)
				root.handleItemClick(id, root.__internal.focusedIndex)
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
			root.__internal.hoverBlocked = true
			if (root.__internal.focusedIndex > 0){
				root.__internal.focusedIndex--
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
			root.__internal.hoverBlocked = true
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			if (root.__internal.focusedIndex < itemCount - 1){
				root.__internal.focusedIndex++
				contentYCorrection(true)
			}
			else if (root.__internal.focusedIndex === itemCount - 1 && root.dataProvider){
				root.dataProvider.fetchMore()
			}
		}
	}

	function contentYCorrection(down_){
		if (root.__internal.focusedIndex >= 0){
			var contentY = popupListView.contentY
			var itemH = root.itemHeight
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			var visibleCount = root.maxVisibleItems === -1 ? itemCount : Math.min(root.maxVisibleItems, itemCount)
			if (visibleCount <= 0) return
			var index = root.__internal.focusedIndex

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
