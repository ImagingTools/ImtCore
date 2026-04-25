import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: root

	visible: false
	z: 1000

	width: itemWidth
	height: filterField.height + itemBody.height + (footerItem.visible ? footerItem.height : 0)

	// --- Data Controller (dependency injection) ---
	property QtObject dataProvider: null

	// --- Configuration ---
	property int pageCount: 20
	property int debounceInterval: 300

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

			text: model.name || ""

			selected: __internal.selectedIndex === model.index
			highlighted: root.isItemSelected(model.id)

			onClicked: {
				root.toggleSelection(model.id || "", model.index)
			}

			onEntered: {
				__internal.selectedIndex = model.index
			}
		}
	}

	// --- Signals ---
	signal itemSelected(string itemId, int index)
	signal selectionChanged(var selectedIds)
	signal opened()
	signal closed()

	// --- Internal state ---
	property QtObject __internal: QtObject {
		property var selectedIds: ({})
		property int currentOffset: 0
		property var model: null
		property int selectedIndex: -1
		property bool hoverBlocked: true
		property bool endListStatus: false
		property bool isLoading: false
		property bool hasError: false
		property string errorMessage: ""
	}

	property CollectionFilter __filter: CollectionFilter {}

	// --- Public API ---
	function open(){
		if (root.visible){
			return
		}

		__internal.selectedIds = ({})
		__internal.currentOffset = 0
		__internal.model = null
		__internal.selectedIndex = -1
		__internal.hoverBlocked = true
		__internal.endListStatus = false
		__internal.isLoading = false
		__internal.hasError = false
		__internal.errorMessage = ""

		__filter.clearAllFilters(true)
		filterField.text = ""

		applyPreselectedIds()

		root.visible = true
		root.opened()
		requestItems(0)
	}

	function close(){
		if (!root.visible){
			return
		}

		root.visible = false
		root.closed()
	}

	function isItemSelected(itemId){
		return itemId !== undefined && itemId !== null && __internal.selectedIds[String(itemId)] === true
	}

	function toggleSelection(itemId, index){
		if (itemId === undefined || itemId === null || itemId === ""){
			return
		}

		let id = String(itemId)
		let selected = ({})
		for (let key in __internal.selectedIds){
			selected[key] = __internal.selectedIds[key]
		}

		if (root.multiSelect){
			if (selected[id]){
				delete selected[id]
			}
			else {
				selected[id] = true
			}
		}
		else {
			selected = ({})
			selected[id] = true
		}

		__internal.selectedIds = selected
		root.itemSelected(id, index)
		root.selectionChanged(getSelectedIds())

		if (!root.multiSelect){
			root.close()
		}
	}

	function getSelectedIds(){
		let result = []
		for (let id in __internal.selectedIds){
			if (__internal.selectedIds[id]){
				result.push(id)
			}
		}
		return result
	}

	// --- Internal helpers ---
	function applyPreselectedIds(){
		let selected = ({})
		let ids = root.preselectedIds || []
		for (let i = 0; i < ids.length; ++i){
			let id = String(ids[i])
			if (id !== ""){
				selected[id] = true
			}
		}
		__internal.selectedIds = selected
	}

	function requestItems(offset){
		if (!root.dataProvider){
			return
		}

		__internal.currentOffset = offset
		__internal.isLoading = true
		__internal.hasError = false
		__internal.errorMessage = ""
		root.dataProvider.getSelectableItems(root.pageCount, offset, root.__filter)
	}

	function requestNextBatch(){
		if (!root.dataProvider || __internal.endListStatus || __internal.isLoading){
			return
		}

		let currentCount = __internal.model ? __internal.model.getItemsCount() : 0
		if (currentCount > 0){
			requestItems(currentCount)
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

		onListObjectsReceived: {
			__internal.isLoading = false
			__internal.hasError = false

			if (__internal.currentOffset === 0){
				__internal.model = listObjects
				__internal.endListStatus = false
				__internal.selectedIndex = -1
			}
			else {
				if (!listObjects || listObjects.getItemsCount() <= 0){
					__internal.endListStatus = true
				}
				else {
					if (__internal.model){
						for (let i = 0; i < listObjects.getItemsCount(); i++){
							let index_ = __internal.model.insertNewItem()
							listObjects.copyItemDataToModel(i, __internal.model, index_)
						}
					}
				}
			}

			if (__internal.model){
				__internal.model.refresh()
			}
		}

		onListObjectsReceiveFailed: {
			__internal.isLoading = false
			__internal.hasError = true
			__internal.errorMessage = message || qsTr("Failed to load items")
		}
	}

	// --- Debounce Timer ---
	PauseAnimation {
		id: debounce

		duration: root.debounceInterval
		onFinished: {
			root.__filter.setTextFilter(filterField.text)
			root.requestItems(0)
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
			__internal.selectedIndex = -1
			debounce.stop()
			debounce.start()
		}

		onAccepted: {
			root.close()
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
			iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)

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
		height: !noDataItem.visible * !errorItem.visible * popupListView.height
				+ noDataItem.height * noDataItem.visible
				+ errorItem.height * errorItem.visible
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
			visible: !__internal.hasError
					&& (__internal.model ? __internal.model.getItemsCount() === 0 && !__internal.isLoading : !root.dataProvider)

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
			visible: __internal.hasError && !__internal.isLoading

			Text {
				anchors.centerIn: parent
				font.pixelSize: root.textSize
				color: Style.errorColor
				text: __internal.errorMessage || qsTr("Error loading items")
			}

			MouseArea {
				anchors.fill: parent
				onClicked: {
					root.requestItems(__internal.currentOffset)
				}
			}
		}

		Rectangle {
			id: loadingOverlay

			anchors.fill: parent
			opacity: 0.5
			color: "transparent"
			visible: __internal.isLoading

			Text {
				anchors.centerIn: parent
				color: Style.textColor
				font.pixelSize: Style.fontSizeM
				text: qsTr("Loading...")
			}
		}

		ListView {
			id: popupListView

			width: root.itemWidth
			height: !count ? 0 :
					(root.maxVisibleItems == -1 || root.maxVisibleItems > popupListView.count) ?
						popupListView.count * root.itemHeight :
						root.maxVisibleItems * root.itemHeight

			boundsBehavior: Flickable.StopAtBounds
			clip: true
			model: __internal.model

			delegate: root.delegate

			onContentYChanged: {
				if (contentHeight - contentY - popupListView.height <= root.itemHeight){
					root.requestNextBatch()
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
		visible: !__internal.endListStatus && __internal.model && __internal.model.getItemsCount() > 0 && __internal.isLoading

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
			if (__internal.selectedIndex >= 0 && __internal.model){
				let id = __internal.model.getData("id", __internal.selectedIndex)
				root.toggleSelection(id, __internal.selectedIndex)
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
			if (__internal.selectedIndex > 0){
				__internal.selectedIndex--
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
			if (__internal.model && __internal.selectedIndex < __internal.model.getItemsCount() - 1){
				__internal.selectedIndex++
				contentYCorrection(true)
			}
			else if (__internal.model && __internal.selectedIndex === __internal.model.getItemsCount() - 1){
				root.requestNextBatch()
			}
		}
	}

	function contentYCorrection(down_){
		if (__internal.selectedIndex >= 0){
			let contentY = popupListView.contentY
			let itemH = root.itemHeight
			let visibleCount = root.maxVisibleItems
			let index = __internal.selectedIndex

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
