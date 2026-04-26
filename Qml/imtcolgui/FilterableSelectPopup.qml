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
	- Keyboard navigation (↑↓ Enter Space Escape Tab)
	- Error display with click-to-retry
	- Split loading states (initial vs page loading)
	- Optional CheckBox selection mode (\c showCheckBox)
	- Optional separator lines between delegates (\c showSeparator)
	- Customizable filter placeholder text (\c filterPlaceholder)
	- Optional selected items group at top with "Clear all" button (\c showSelectedGroup)

	\sa FilterableSelectDataProvider, FilterableSelectGqlDataProvider
*/
PopupView {
	id: root

	width: itemWidth + 2 * Style.marginL
	height: contentColumn.height + 2 * Style.marginL

	// --- Data Controller (dependency injection) ---
	property QtObject dataProvider: null

	// --- Configuration ---
	property int debounceInterval: 300

	// --- Delegate roles ---
	property string idRole: "id"
	property string textRole: "title"

	// --- Visual ---
	property int itemWidth: Style.sizeHintXXS
	property int itemHeight: Style.controlHeightL
	property int maxVisibleItems: 8
	property int textSize: Style.fontSizeM
	property string fontColor: Style.textColor

	// --- Selection ---
	property var preselectedIds: []
	property var knownItems: []

	// --- CheckBox mode ---
	property bool showCheckBox: false

	// --- Separator between delegates ---
	property bool showSeparator: false

	// --- Filter field placeholder ---
	property string filterPlaceholder: qsTr("Filter...")

	// --- Selected items group at top ---
	property bool showSelectedGroup: false

	// --- Injectable delegate ---
	property Component delegate: Component {
		PopupMenuDelegate {
			width: root.itemWidth
			height: root.itemHeight

			isSeparator: false

			text: root.showCheckBox ? "" : root.getItemText(model.index)

			selected: root.__internal.focusedIndex === model.index
			highlighted: !root.showCheckBox && root.dataProvider ? root.dataProvider.isItemSelected(root.getItemId(model.index)) : false

			onClicked: {
				root.handleItemClick(root.getItemId(model.index), model.index)
			}

			onEntered: {
				root.__internal.focusedIndex = model.index
			}

			Row {
				z: 10
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				spacing: Style.marginS
				visible: root.showCheckBox

				CheckBox {
					id: delegateCheckBox

					width: Style.itemSizeS
					height: Style.itemSizeS
					mainMargin: Style.marginM
					borderColor: Style.grayColor
					anchors.verticalCenter: parent.verticalCenter

					checkState: root.dataProvider && root.dataProvider.isItemSelected(root.getItemId(model.index)) ? Qt.Checked : Qt.Unchecked

					function nextCheckState() {
						root.handleItemClick(root.getItemId(model.index), model.index)
					}
				}

				Text {
					z: 10
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: root.textSize
					color: root.fontColor
					text: root.getItemText(model.index)
				}
			}

			Rectangle {
				anchors.bottom: parent.bottom
				width: parent.width
				height: 1
				color: Style.borderColor
				visible: root.showSeparator
				opacity: 0.4
			}
		}
	}

	// --- Signals ---
	signal itemSelected(string itemId, int index)
	signal selectionChanged(var selectedIds)

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
		property var selectedItemsList: []
		property int selectedItemsCount: 0
	}

	// --- Lifecycle (called by ModalDialogManager via DialogManagerView) ---
	function started(){
		root.__internal.focusedIndex = -1
		root.__internal.hoverBlocked = true
		popupListView.itemCount = 0

		filterField.text = ""

		if (root.dataProvider){
			for (var i = 0; i < root.knownItems.length; i++){
				var item = root.knownItems[i]
				root.dataProvider.addKnownItem(item.id, item)
			}
			root.dataProvider.setPreselectedIds(root.preselectedIds)
			root.dataProvider.fetch("")
		}

		root.__rebuildSelectedGroup()

		filterField.setFocus(true)
		filterField.forceActiveFocus()
	}

	function closePopup(){
		ModalDialogManager.closeDialog()
	}

	function handleItemClick(itemId, index){
		if (!root.dataProvider || itemId === ""){
			return
		}
		var item = root.getItem(index)
		root.dataProvider.toggleItem(itemId, item)
		root.itemSelected(itemId, index)

		if (!root.dataProvider.multiSelect){
			root.closePopup()
		}
	}

	function __rebuildSelectedGroup(){
		if (!root.showSelectedGroup || !root.dataProvider){
			root.__internal.selectedItemsList = []
			root.__internal.selectedItemsCount = 0
			return
		}
		var items = root.dataProvider.getSelectedItems()
		root.__internal.selectedItemsList = items
		root.__internal.selectedItemsCount = items.length
	}

	function __handleRemoveSelectedItem(itemId){
		if (!root.dataProvider || !itemId){
			return
		}
		// When removing, toggleItem only needs the ID (item is already selected,
		// so __removeFromSelection uses only the ID, not the item object)
		root.dataProvider.toggleItem(String(itemId), null)
	}

	// --- Data provider connection ---
	Connections {
		target: root.dataProvider
		enabled: root.dataProvider !== null

		function onDataChanged() {
			var count = root.dataProvider ? root.dataProvider.items.length : 0
			popupListView.itemCount = count
			if (root.__internal.focusedIndex >= count){
				root.__internal.focusedIndex = count > 0 ? count - 1 : -1
			}
		}

		function onSelectionChanged(){
			root.selectionChanged(root.dataProvider.getSelectedIds())
			root.__rebuildSelectedGroup()
		}

		function onIsInitialLoadingChanged(){
			if (root.dataProvider && root.dataProvider.isInitialLoading){
				loadingOverlay.start()
			} else {
				loadingOverlay.stop()
			}
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

	// --- Background (single visual container for shadow) ---
	Rectangle {
		id: background

		anchors.fill: parent
		color: Style.baseColor
		radius: Style.radiusL
		border.width: 1
		border.color: Style.alternateBaseColor
	}

	MouseArea {
		anchors.fill: background
		onWheel: {
			wheel.accepted = true
		}
		onClicked: {
			mouse.accepted = true
		}
		onReleased: {}
		onPressAndHold: {}
		onPressed: {}
		onPositionChanged: {}
	}

	// --- Body with margins (following TimeFilterParamView pattern) ---
	Item {
		id: body

		anchors.fill: parent
		anchors.margins: Style.marginM

		// --- Content column ---
		Column {
			id: contentColumn

			width: parent.width
			spacing: Style.marginM

			// --- Search Field ---
			CustomTextField {
				id: filterField

				width: parent.width
				height: Style.controlHeightM
				margin: Style.marginM
				textSize: root.textSize
				fontColor: root.fontColor
				placeHolderText: root.filterPlaceholder
				radius: Style.radiusL

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

			// --- Selected items group ---
			Column {
				id: selectedGroup

				width: parent.width
				visible: root.showSelectedGroup && root.__internal.selectedItemsCount > 0
				spacing: 0

				// Header row: "Selected (N)" + "Clear all"
				Item {
					width: parent.width
					height: Style.controlHeightS

					Text {
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter
						font.pixelSize: Style.fontSizeS
						color: Style.placeHolderTextColor
						text: qsTr("Selected") + " (" + root.__internal.selectedItemsCount + ")"
					}

					Text {
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						font.pixelSize: Style.fontSizeS
						color: Style.linkColor
						text: qsTr("Clear all")

						MouseArea {
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								if (root.dataProvider){
									root.dataProvider.clearSelection()
								}
							}
						}
					}
				}

				// Selected items list
				Column {
					width: parent.width
					spacing: 0

					Repeater {
						id: selectedItemsRepeater

						model: root.__internal.selectedItemsCount

						delegate: Item {
							width: selectedGroup.width
							height: root.itemHeight

							property var __selItem: model.index >= 0 && model.index < root.__internal.selectedItemsList.length
								? root.__internal.selectedItemsList[model.index] : null

							Row {
								anchors.verticalCenter: parent.verticalCenter
								anchors.left: parent.left
								anchors.leftMargin: Style.marginM
								anchors.right: removeBtn.left
								anchors.rightMargin: Style.marginS
								spacing: Style.marginS

								CheckBox {
									width: Style.itemSizeS
									height: Style.itemSizeS
									mainMargin: Style.marginM
									borderColor: Style.grayColor
									anchors.verticalCenter: parent.verticalCenter
									visible: root.showCheckBox
									checkState: Qt.Checked
									function nextCheckState() {
										var si = parent.parent.__selItem
										if (si){
											root.__handleRemoveSelectedItem(si[root.idRole])
										}
									}
								}

								Text {
									anchors.verticalCenter: parent.verticalCenter
									font.pixelSize: root.textSize
									color: root.fontColor
									text: {
										var si = parent.parent.__selItem
										return si ? String(si[root.textRole] || "") : ""
									}
									elide: Text.ElideRight
									width: parent.width - parent.spacing - (root.showCheckBox ? Style.itemSizeS + parent.spacing : 0)
								}
							}

							Button {
								id: removeBtn

								anchors.verticalCenter: parent.verticalCenter
								anchors.right: parent.right
								anchors.rightMargin: Style.marginM

								width: Style.iconSizeXS
								height: Style.iconSizeXS

								decorator: Component { IconButtonDecorator {} }
								iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)

								onClicked: {
									var si = parent.__selItem
									if (si){
										root.__handleRemoveSelectedItem(si[root.idRole])
									}
								}
							}

							Rectangle {
								anchors.bottom: parent.bottom
								width: parent.width
								height: 1
								color: Style.borderColor
								visible: root.showSeparator
								opacity: 0.4
							}
						}
					}
				}

				// Group separator line
				Rectangle {
					width: parent.width
					height: 1
					color: Style.borderColor
					opacity: 0.6
				}
			}

			// --- List Body ---
			Item {
				id: itemBody

				width: parent.width
				height: loadingOverlay.visible ? loadingOverlay.height
						: errorItem.visible ? errorItem.height
						: noDataItem.visible ? noDataItem.height
						: popupListView.height

				Item {
					id: noDataItem

					width: parent.width
					height: 50
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

				Item {
					id: errorItem

					width: parent.width
					height: 50
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

				Loading {
					id: loadingOverlay

					width: parent.width
					height: root.maxVisibleItems > 0 ? root.maxVisibleItems * root.itemHeight : 100
					visible: false
					indicatorSize: 30
					background.color: "transparent"
				}

				CustomScrollbar {
					id: scrollbar

					z: 100

					anchors.right: parent.right
					anchors.bottom: parent.bottom

					secondSize: 8
					targetItem: popupListView
				}

				ListView {
					id: popupListView

					// Managed imperatively via onDataChanged signal handler
					property int itemCount: 0

					width: parent.width
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

				width: parent.width
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
		}
	}

	// --- Shadow (covers entire background including filter field) ---
	DropShadow {
		id: dropShadow

		anchors.fill: background
		z: background.z - 1

		horizontalOffset: 3
		verticalOffset: 3
		radius: Style.radiusL
		color: Style.shadowColor
		source: background
	}

	// --- Keyboard navigation ---
	Shortcut {
		sequence: "Escape"
		onActivated: {
			root.closePopup()
		}
	}

	Shortcut {
		sequence: "Return"
		enabled: !filterField.textInputFocus
		onActivated: {
			if (root.__internal.focusedIndex >= 0){
				var id = root.getItemId(root.__internal.focusedIndex)
				root.handleItemClick(id, root.__internal.focusedIndex)
			}
		}
	}

	Shortcut {
		sequence: "Space"
		enabled: !filterField.textInputFocus
		onActivated: {
			if (root.__internal.focusedIndex >= 0){
				var id = root.getItemId(root.__internal.focusedIndex)
				root.handleItemClick(id, root.__internal.focusedIndex)
			}
		}
	}

	Shortcut {
		sequence: "Tab"
		onActivated: {
			if (filterField.textInputFocus){
				filterField.setFocus(false)
				root.__internal.hoverBlocked = true
				var count = root.dataProvider ? root.dataProvider.items.length : 0
				if (root.__internal.focusedIndex < 0 && count > 0){
					root.__internal.focusedIndex = 0
				}
			} else {
				filterField.setFocus(true)
				filterField.forceActiveFocus()
				root.__internal.focusedIndex = -1
			}
		}
	}

	Shortcut {
		sequence: "Shift+Tab"
		onActivated: {
			if (!filterField.textInputFocus){
				filterField.setFocus(true)
				filterField.forceActiveFocus()
				root.__internal.focusedIndex = -1
			}
		}
	}

	Shortcut {
		sequence: "Up"
		onActivated: {
			if (filterField.textInputFocus){
				filterField.setFocus(false)
			}
			root.__internal.hoverBlocked = true
			if (root.__internal.focusedIndex > 0){
				root.__internal.focusedIndex--
				root.contentYCorrection(false)
			}
		}
	}

	Shortcut {
		sequence: "Down"
		onActivated: {
			if (filterField.textInputFocus){
				filterField.setFocus(false)
			}
			root.__internal.hoverBlocked = true
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			if (root.__internal.focusedIndex < itemCount - 1){
				root.__internal.focusedIndex++
				root.contentYCorrection(true)
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
