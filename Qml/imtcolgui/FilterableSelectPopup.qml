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

	// Fields to which text filter will be applied
	property var textFilteringInfoIds: ["name"]

	// --- Visual ---
	property int itemWidth: Style.sizeHintXXS
	property int itemHeight: Style.controlHeightM
	property int maxVisibleItems: 8
	property int textSize: Style.fontSizeM
	property string fontColor: Style.textColor
	property string idKey: "id"
	property string nameKey: "name"

	// --- Selection ---
	property bool multiSelect: false
	property var preselectedIds: []

	// --- State ---
	property var model: null
	property int selectedIndex: -1
	property bool hoverBlocked: true
	property bool endListStatus: false
	property string filterText: ""
	property bool isLoading: false

	// --- Injectable delegate ---
	property Component delegate: Component {
		PopupMenuDelegate {
			width: root.itemWidth
			height: root.itemHeight
			textSize: root.textSize
			fontColor: root.fontColor

			text: model[root.nameKey] || ""

			selected: root.selectedIndex === model.index
			highlighted: root.isItemSelected(model[root.idKey])

			onClicked: {
				let resultId = model[root.idKey] || model.id || ""
				root.toggleSelection(resultId, model.index)
			}

			onEntered: {
				root.selectedIndex = model.index
			}
		}
	}

	// --- Signals ---
	signal itemSelected(string itemId, int index)
	signal selectionChanged(var selectedIds)
	signal opened()
	signal closed()

	// --- Internal ---
	property var _selectedIds: ({})
	property int _currentOffset: 0

	property CollectionFilter _filter: CollectionFilter {}

	function open(){
		if (root.visible){
			return
		}

		if (textFilteringInfoIds && typeof textFilteringInfoIds === 'object' && textFilteringInfoIds.length > 0){
			_filter.setFilteringInfoIds(textFilteringInfoIds)
		}

		applyPreselectedIds()

		root.model = null
		root.endListStatus = false
		root.selectedIndex = -1
		root.filterText = ""
		filterField.text = ""

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

	// --- Background overlay to close on outside click ---
	MouseArea {
		id: backgroundOverlay

		parent: root.parent
		anchors.fill: parent ? parent : undefined
		z: root.z - 1
		visible: root.visible
		enabled: root.visible

		onClicked: {
			root.close()
		}
	}

	// --- Data fetch ---
	function requestItems(offset){
		if (!root.dataProvider){
			return
		}

		root._currentOffset = offset
		root.isLoading = true
		root.dataProvider.getSelectableItems(root.pageCount, offset, root._filter)
	}

	// --- Selection Management ---
	function applyPreselectedIds(){
		let selected = ({})
		let ids = root.preselectedIds || []
		for (let i = 0; i < ids.length; ++i){
			let id = String(ids[i])
			if (id !== ""){
				selected[id] = true
			}
		}
		root._selectedIds = selected
	}

	function isItemSelected(itemId){
		return itemId !== undefined && itemId !== null && root._selectedIds[String(itemId)] === true
	}

	function toggleSelection(itemId, index){
		if (itemId === undefined || itemId === null || itemId === ""){
			return
		}

		let id = String(itemId)
		let selected = ({})
		for (let key in root._selectedIds){
			selected[key] = root._selectedIds[key]
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

		root._selectedIds = selected
		root.itemSelected(id, index)
		root.selectionChanged(getSelectedIds())

		if (!root.multiSelect){
			root.close()
		}
	}

	function getSelectedIds(){
		let result = []
		for (let id in root._selectedIds){
			if (root._selectedIds[id]){
				result.push(id)
			}
		}
		return result
	}

	// --- Data provider connection ---
	Connections {
		target: root.dataProvider

		onListObjectsReceived: {
			root.isLoading = false

			if (root._currentOffset === 0){
				root.model = listObjects
				root.endListStatus = false
				root.selectedIndex = -1
			}
			else {
				if (!listObjects || listObjects.getItemsCount() <= 0){
					root.endListStatus = true
				}
				else {
					if (root.model){
						for (let i = 0; i < listObjects.getItemsCount(); i++){
							let index_ = root.model.insertNewItem()
							listObjects.copyItemDataToModel(i, root.model, index_)
						}
					}
				}
			}

			if (root.model){
				root.model.refresh()
			}
		}

		onListObjectsReceiveFailed: {
			root.isLoading = false
		}
	}

	// --- Debounce Timer ---
	PauseAnimation {
		id: debounce

		duration: root.debounceInterval
		onFinished: {
			root._filter.setTextFilter(filterField.text)
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
			root.filterText = text
			root.selectedIndex = -1
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
		height: !noDataItem.visible * popupListView.height + noDataItem.height * noDataItem.visible
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
			visible: root.model ? root.model.getItemsCount() === 0 && !root.isLoading : !root.dataProvider

			Text {
				anchors.centerIn: parent
				font.pixelSize: root.textSize
				color: root.fontColor
				text: qsTr("No items found")
			}
		}

		Rectangle {
			id: loadingOverlay

			anchors.fill: parent
			opacity: 0.5
			color: "transparent"
			visible: root.isLoading

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
			model: root.model

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
			visible: root.hoverBlocked

			onPositionChanged: {
				root.hoverBlocked = false
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
		visible: !root.endListStatus && root.model && root.model.getItemsCount() > 0 && root.isLoading

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

	// --- Infinite scroll ---
	function requestNextBatch(){
		if (!root.dataProvider || root.endListStatus || root.isLoading){
			return
		}

		let currentCount = root.model ? root.model.getItemsCount() : 0
		if (currentCount > 0){
			root.requestItems(currentCount)
		}
	}

	// --- Keyboard navigation ---
	function setTextFocus(focus){
		filterField.setFocus(focus)
	}

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
			if (root.selectedIndex >= 0 && root.model){
				let id = root.model.getData(root.idKey, root.selectedIndex)
				root.toggleSelection(id, root.selectedIndex)
			}
		}
	}

	Shortcut {
		sequence: "Up"
		enabled: root.visible
		onActivated: {
			if (filterField.textInputFocus){
				root.setTextFocus(false)
			}
			root.hoverBlocked = true
			if (root.selectedIndex > 0){
				root.selectedIndex--
				root.contentYCorrection(false)
			}
		}
	}

	Shortcut {
		sequence: "Down"
		enabled: root.visible
		onActivated: {
			if (filterField.textInputFocus){
				root.setTextFocus(false)
			}
			root.hoverBlocked = true
			if (root.model && root.selectedIndex < root.model.getItemsCount() - 1){
				root.selectedIndex++
				root.contentYCorrection(true)
			}
			else if (root.model && root.selectedIndex === root.model.getItemsCount() - 1){
				root.requestNextBatch()
			}
		}
	}

	function contentYCorrection(down_){
		if (root.selectedIndex >= 0){
			let contentY = popupListView.contentY
			let itemH = root.itemHeight
			let visibleCount = root.maxVisibleItems
			let index = root.selectedIndex

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
