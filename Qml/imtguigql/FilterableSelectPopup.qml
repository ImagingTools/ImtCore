import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype FilterableSelectPopup
	\inqmlmodule imtguigql
	\brief A GitHub-style filterable select popup with server-side search and pagination.

	Provides an overlay popup with text filtering (debounced), offset-based pagination,
	single/multi select by id, injectable delegate, keyboard navigation, and loading/empty states.

	The popup uses an abstract data provider that must be set via the \l dataProvider property.
	Use FilterableSelectGqlDataProvider for GQL-backed data, or implement a custom
	provider with the same interface (collectionModel, state, updateModel, setCustomInputParams).

	The popup is opened via ModalDialogManager and uses the existing decorator/popup
	infrastructure of the framework.

	Usage:
	\code
	FilterableSelectPopup {
		id: selectPopup

		dataProvider: FilterableSelectGqlDataProvider {
			commandId: "GetSelectableItems"
			fields: ["id", "name", "description"]
			textFilteringInfoIds: ["name"]
		}

		multiSelect: true
		preselectedIds: ["id-1", "id-2"]

		delegate: Component {
			PopupMenuDelegate {
				text: model.name
			}
		}

		onItemSelected: function(itemId, index) {
			console.log("Selected:", itemId)
		}
	}
	\endcode
*/
Item {
	id: root

	width: itemWidth
	height: filterField.height + itemBody.height + (footerItem.visible ? footerItem.height : 0)

	// --- Abstract Data Provider ---
	/*!
		\qmlproperty QtObject FilterableSelectPopup::dataProvider
		\brief The data provider for loading selectable items.

		Must expose: collectionModel (TreeItemModel), state (string "Ready"/"Loading"/etc.),
		updateModel(offset), applyTextFilter(text), resetAndFetch(), fetchNextPage(),
		endListReached (bool).
	*/
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

	// --- Dialog manager refs ---
	property string uuid: ""
	property Item root_: null

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
	signal finished(string commandId, int index)

	// --- Internal ---
	property var _selectedIds: ({})

	Component.onCompleted: {
		Events.subscribeEvent("AppSizeChanged", onAppSizeChanged)
		applyPreselectedIds()
		if (root.dataProvider){
			root.dataProvider.updateModel(0)
		}
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged)
	}

	function onAppSizeChanged(){
		onBackgroundClicked()
	}

	function onBackgroundClicked(){
		root.finished('', -1)
		if (root_){
			root_.closeDialog()
		}
	}

	function close(){
		if (root_){
			root_.closeDialog()
		}
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
			root.finished(id, index)
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

		onModelUpdated: {
			if (!root.dataProvider){
				return
			}

			if (root.dataProvider.offset === 0){
				root.model = root.dataProvider.collectionModel
				root.endListStatus = false
				root.selectedIndex = -1
			}
			else {
				let newItems = root.dataProvider.collectionModel
				if (!newItems || newItems.getItemsCount() <= 0){
					root.endListStatus = true
				}
				else {
					for (let i = 0; i < newItems.getItemsCount(); i++){
						let index_ = root.model.insertNewItem()
						newItems.copyItemDataToModel(i, root.model, index_)
					}
				}
			}

			if (root.dataProvider.collectionModel){
				root.dataProvider.collectionModel.refresh()
			}
		}

		onFailed: {
			ModalDialogManager.showErrorDialog(message)
		}
	}

	// --- Debounce Timer ---
	PauseAnimation {
		id: debounce

		duration: root.debounceInterval
		onFinished: {
			if (root.dataProvider){
				root.dataProvider.filter.setTextFilter(filterField.text)
				root.dataProvider.offset = 0
				root.dataProvider.updateModel(0)
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
			visible: root.model ? root.model.getItemsCount() === 0 && root.dataProvider && root.dataProvider.state.toLowerCase() === "ready" : !root.dataProvider

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
			visible: root.dataProvider ? root.dataProvider.state.toLowerCase() !== "ready" : false

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
		visible: !root.endListStatus && root.model && root.model.getItemsCount() > 0 && root.dataProvider && root.dataProvider.state.toLowerCase() !== "ready"

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
		if (!root.dataProvider || root.endListStatus){
			return
		}
		if (root.dataProvider.state.toLowerCase() !== "ready"){
			return
		}

		let currentCount = root.dataProvider.collectionModel ? root.dataProvider.collectionModel.getItemsCount() : 0
		if (currentCount > 0 && currentCount > root.dataProvider.offset){
			root.dataProvider.updateModel(root.dataProvider.offset + root.dataProvider.count)
		}
	}

	// --- Keyboard navigation ---
	function setTextFocus(focus){
		filterField.setFocus(focus)
	}

	Shortcut {
		sequence: "Escape"
		enabled: true
		onActivated: {
			root.onBackgroundClicked()
		}
	}

	Shortcut {
		sequence: "Return"
		enabled: !filterField.textInputFocus
		onActivated: {
			if (root.selectedIndex >= 0 && root.model){
				let id = root.model.getData(root.idKey, root.selectedIndex)
				root.toggleSelection(id, root.selectedIndex)
			}
		}
	}

	Shortcut {
		sequence: "Up"
		enabled: true
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
		enabled: true
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
