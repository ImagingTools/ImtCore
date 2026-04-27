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
	- Optional header component above filter (\c headerComponent) for custom controls like type selectors

	\sa FilterableSelectDataProvider, FilterableSelectGqlDataProvider
*/
PopupView {
	id: root

	width: root.embedded ? (parent ? parent.width : itemWidth) : (itemWidth + 2 * Style.marginL)
	height: root.embedded ? contentColumn.height : Math.min(contentColumn.height + 2 * Style.marginL, root.__maxPopupHeight)

	// --- Data Controller (dependency injection) ---
	property QtObject dataProvider: null

	// --- Configuration ---
	property int debounceInterval: 500

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

	// --- Optional header component (rendered above filter field) ---
	property Component headerComponent: null

	// --- Selected items group at top ---
	property bool showSelectedGroup: false

	// --- Max height for selected group (in items) before scrolling ---
	property int maxSelectedGroupItems: 4

	// --- Private: max popup height (screen height minus margins) ---
	property int __maxPopupHeight: 600

	// --- Private: suppress selectionChanged during initialization ---
	property bool __initializing: false

	// --- Embedded mode (no own background/shadow, parent provides chrome) ---
	property bool embedded: false

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
	signal requestClose()

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
		property int selectedFocusedIndex: -1
		property bool clearAllFocused: false
		property bool hoverBlocked: true
		property var selectedItemsList: []
		property int selectedItemsCount: 0
	}

	// --- Lifecycle (called by ModalDialogManager via DialogManagerView) ---
	function started(){
		root.__initializing = true

		root.__internal.focusedIndex = -1
		root.__internal.selectedFocusedIndex = -1
		root.__internal.hoverBlocked = true
		root.__internal.clearAllFocused = false
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

		root.__initializing = false

		// Delayed focus to prevent other elements from stealing it during dialog setup
		__focusTimer.start()
	}

	function closePopup(){
		if (root.embedded){
			root.requestClose()
			return
		}
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
			root.__internal.selectedFocusedIndex = -1
			root.__internal.clearAllFocused = false
			return
		}
		var items = root.dataProvider.getSelectedItems()
		root.__internal.selectedItemsList = items
		root.__internal.selectedItemsCount = items.length
		// Adjust focused index if it's out of bounds after rebuild
		if (root.__internal.selectedFocusedIndex >= items.length){
			root.__internal.selectedFocusedIndex = items.length > 0 ? items.length - 1 : -1
		}
	}

	function __handleRemoveSelectedItem(itemId){
		if (!root.dataProvider || !itemId){
			return
		}
		// When removing, toggleItem only needs the ID (item is already selected,
		// so __removeFromSelection uses only the ID, not the item object)
		root.dataProvider.toggleItem(String(itemId), null)
	}

	function __focusFilter(){
		root.__internal.selectedFocusedIndex = -1
		root.__internal.clearAllFocused = false
		filterField.setFocus(true)
		filterField.forceActiveFocus()
	}

	// --- Focus delay timer (prevents focus theft during dialog setup) ---
	PauseAnimation {
		id: __focusTimer

		duration: 50
		onFinished: {
			root.__focusFilter()
		}
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
			// Suppress during initialization to avoid unwanted side-effects (e.g. doUpdateModel in TicketEditor)
			if (root.__initializing){
				return
			}
			root.selectionChanged(root.dataProvider.getSelectedIds())
			root.__rebuildSelectedGroup()

			// Re-fetch to apply server-side excludeIds filtering after selection change
			if (root.showSelectedGroup && root.dataProvider){
				root.dataProvider.fetch(filterField.text)
			}
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
		visible: !root.embedded
	}

	MouseArea {
		anchors.fill: background
		visible: !root.embedded
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
		anchors.margins: root.embedded ? 0 : Style.marginM

		// --- Content column ---
		Column {
			id: contentColumn

			width: parent.width
			spacing: Style.marginM

			// --- Optional header (e.g. entity type selector) ---
			Loader {
				id: headerLoader
				width: parent.width
				sourceComponent: root.headerComponent
				visible: root.headerComponent !== null
			}

			// --- Search Field ---
			CustomTextField {
				id: filterField

				width: parent.width
				height: Style.controlHeightM
				margin: Style.marginM
				textFieldRightMargin: clearButton.width + Style.marginM + Style.marginXS
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
				spacing: Style.marginS

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
						id: clearAllBtn

						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						font.pixelSize: Style.fontSizeS
						color: root.__internal.clearAllFocused ? Style.textSelectedColor : Style.linkColor
						text: qsTr("Clear all")
						font.underline: root.__internal.clearAllFocused

						MouseArea {
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								if (root.dataProvider){
									root.dataProvider.clearSelection()
								}
								root.__internal.clearAllFocused = false
								root.__focusFilter()
							}
						}
					}
				}

				// Selected items list (scrollable)
				Item {
					id: selectedListContainer

					width: parent.width
					height: {
						var totalHeight = root.__internal.selectedItemsCount * root.itemHeight
						var maxHeight = root.maxSelectedGroupItems * root.itemHeight
						return Math.min(totalHeight, maxHeight)
					}

					ListView {
						id: selectedListView

						anchors.fill: parent
						clip: true
						boundsBehavior: Flickable.StopAtBounds
						model: root.__internal.selectedItemsCount

						delegate: Item {
							width: selectedListView.width
							height: root.itemHeight

							property var __selItem: model.index >= 0 && model.index < root.__internal.selectedItemsList.length
								? root.__internal.selectedItemsList[model.index] : null

							// Highlight when keyboard-focused
							Rectangle {
								anchors.fill: parent
								color: root.__internal.selectedFocusedIndex === model.index ? Style.hoverColor : "transparent"
								opacity: 0.5
							}

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
										root.__focusFilter()
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
								anchors.rightMargin: Style.marginM + (selectedScrollbar.visible ? selectedScrollbar.secondSize + Style.marginXS : 0)

								width: Style.iconSizeXS
								height: Style.iconSizeXS

								decorator: Component { IconButtonDecorator {} }
								iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)

								onClicked: {
									var si = parent.__selItem
									if (si){
										root.__handleRemoveSelectedItem(si[root.idRole])
									}
									root.__focusFilter()
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

					CustomScrollbar {
						id: selectedScrollbar

						z: 100

						anchors.right: parent.right
						anchors.bottom: parent.bottom

						secondSize: 8
						targetItem: selectedListView
						visible: root.__internal.selectedItemsCount > root.maxSelectedGroupItems
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
					visible: popupListView.itemCount > 0 && popupListView.contentHeight > popupListView.height
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
		visible: !root.embedded

		horizontalOffset: 3
		verticalOffset: 3
		radius: Style.radiusL
		color: Style.shadowColor
		source: background
	}

	// --- Helper: clear all focus indicators ---
	function __clearAllFocusIndicators(){
		root.__internal.focusedIndex = -1
		root.__internal.selectedFocusedIndex = -1
		root.__internal.clearAllFocused = false
	}

	// --- Helper: handle "Clear all" action from keyboard ---
	function __handleClearAllAction(){
		if (root.dataProvider){
			root.dataProvider.clearSelection()
		}
		root.__internal.clearAllFocused = false
		root.__focusFilter()
	}

	// --- Helper: remove focused selected item and adjust focus ---
	function __handleRemoveSelectedItemAndAdjustFocus(){
		var idx = root.__internal.selectedFocusedIndex
		var si = idx >= 0 && idx < root.__internal.selectedItemsList.length
			? root.__internal.selectedItemsList[idx] : null
		if (si){
			root.__handleRemoveSelectedItem(si[root.idRole])
		}
		// Adjust index after removal
		if (root.__internal.selectedFocusedIndex >= root.__internal.selectedItemsCount){
			root.__internal.selectedFocusedIndex = root.__internal.selectedItemsCount - 1
		}
		if (root.__internal.selectedItemsCount === 0){
			root.__internal.selectedFocusedIndex = -1
			root.__focusFilter()
		}
	}

	// --- Helper: handle keyboard activation (Return/Enter/Space) ---
	function __handleKeyboardActivation(){
		// "Clear all" button action
		if (root.__internal.clearAllFocused){
			root.__handleClearAllAction()
			return
		}
		// Selected group item — remove
		if (root.__internal.selectedFocusedIndex >= 0){
			root.__handleRemoveSelectedItemAndAdjustFocus()
			return
		}
		// Main list item — toggle
		if (root.__internal.focusedIndex >= 0){
			var id = root.getItemId(root.__internal.focusedIndex)
			root.handleItemClick(id, root.__internal.focusedIndex)
		}
	}

	// --- Keyboard navigation ---
	Shortcut {
		sequence: "Escape"
		enabled: !root.embedded
		onActivated: {
			root.closePopup()
		}
	}

	Shortcut {
		sequence: "Return"
		enabled: !filterField.textInputFocus
		onActivated: root.__handleKeyboardActivation()
	}

	// Enter key (numpad) — same behavior as Return
	Shortcut {
		sequence: "Enter"
		enabled: !filterField.textInputFocus
		onActivated: root.__handleKeyboardActivation()
	}

	Shortcut {
		sequence: "Space"
		enabled: !filterField.textInputFocus
		onActivated: root.__handleKeyboardActivation()
	}

	Shortcut {
		sequence: "Tab"
		onActivated: {
			if (filterField.textInputFocus){
				filterField.setFocus(false)
				root.__internal.hoverBlocked = true
				// If selected group visible, tab to "Clear all" first
				if (root.showSelectedGroup && root.__internal.selectedItemsCount > 0){
					root.__internal.clearAllFocused = true
					root.__internal.selectedFocusedIndex = -1
					root.__internal.focusedIndex = -1
				} else {
					var count = root.dataProvider ? root.dataProvider.items.length : 0
					if (root.__internal.focusedIndex < 0 && count > 0){
						root.__internal.focusedIndex = 0
					}
				}
			} else if (root.__internal.clearAllFocused){
				// Tab from "Clear all" → first selected item
				root.__internal.clearAllFocused = false
				if (root.__internal.selectedItemsCount > 0){
					root.__internal.selectedFocusedIndex = 0
				} else {
					var cnt = root.dataProvider ? root.dataProvider.items.length : 0
					root.__internal.focusedIndex = cnt > 0 ? 0 : -1
				}
			} else if (root.__internal.selectedFocusedIndex >= 0){
				// Tab from selected group → main list
				root.__internal.selectedFocusedIndex = -1
				var mainCount = root.dataProvider ? root.dataProvider.items.length : 0
				root.__internal.focusedIndex = mainCount > 0 ? 0 : -1
			} else {
				// Tab from main list → back to filter
				root.__focusFilter()
				root.__clearAllFocusIndicators()
			}
		}
	}

	Shortcut {
		sequence: "Shift+Tab"
		onActivated: {
			if (root.__internal.focusedIndex >= 0){
				// Shift+Tab from main list → selected group (last item) or "Clear all" or filter
				root.__internal.focusedIndex = -1
				if (root.showSelectedGroup && root.__internal.selectedItemsCount > 0){
					root.__internal.selectedFocusedIndex = root.__internal.selectedItemsCount - 1
				} else {
					root.__focusFilter()
				}
			} else if (root.__internal.selectedFocusedIndex >= 0){
				// Shift+Tab from selected group → "Clear all"
				root.__internal.selectedFocusedIndex = -1
				root.__internal.clearAllFocused = true
			} else if (root.__internal.clearAllFocused){
				// Shift+Tab from "Clear all" → filter
				root.__internal.clearAllFocused = false
				root.__focusFilter()
			} else if (!filterField.textInputFocus){
				root.__focusFilter()
				root.__clearAllFocusIndicators()
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

			// Navigate within selected group
			if (root.__internal.selectedFocusedIndex > 0){
				root.__internal.selectedFocusedIndex--
				root.__selectedContentYCorrection(false)
				return
			}
			if (root.__internal.selectedFocusedIndex === 0){
				// Up from first selected item → "Clear all"
				root.__internal.selectedFocusedIndex = -1
				root.__internal.clearAllFocused = true
				return
			}
			if (root.__internal.clearAllFocused){
				// Up from "Clear all" → go to filter
				root.__internal.clearAllFocused = false
				root.__focusFilter()
				return
			}

			// Navigate within main list
			if (root.__internal.focusedIndex > 0){
				root.__internal.focusedIndex--
				root.contentYCorrection(false)
			} else if (root.__internal.focusedIndex === 0){
				// Up from first main item → last selected item (if exists) or filter
				root.__internal.focusedIndex = -1
				if (root.showSelectedGroup && root.__internal.selectedItemsCount > 0){
					root.__internal.selectedFocusedIndex = root.__internal.selectedItemsCount - 1
					root.__selectedContentYCorrection(false)
				} else {
					root.__focusFilter()
				}
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

			// From filter or "Clear all" → navigate into selected group or main list
			if (root.__internal.clearAllFocused){
				root.__internal.clearAllFocused = false
				if (root.__internal.selectedItemsCount > 0){
					root.__internal.selectedFocusedIndex = 0
					root.__selectedContentYCorrection(true)
				} else {
					var mc = root.dataProvider ? root.dataProvider.items.length : 0
					root.__internal.focusedIndex = mc > 0 ? 0 : -1
				}
				return
			}

			// Navigate within selected group
			if (root.__internal.selectedFocusedIndex >= 0){
				if (root.__internal.selectedFocusedIndex < root.__internal.selectedItemsCount - 1){
					root.__internal.selectedFocusedIndex++
					root.__selectedContentYCorrection(true)
				} else {
					// Down from last selected item → first main list item
					root.__internal.selectedFocusedIndex = -1
					var mainCount = root.dataProvider ? root.dataProvider.items.length : 0
					root.__internal.focusedIndex = mainCount > 0 ? 0 : -1
				}
				return
			}

			// Navigate within main list
			var itemCount = root.dataProvider ? root.dataProvider.items.length : 0
			if (root.__internal.focusedIndex < 0){
				// Down from filter → "Clear all" if selected group visible, else first main item
				if (root.showSelectedGroup && root.__internal.selectedItemsCount > 0){
					root.__internal.clearAllFocused = true
				} else if (itemCount > 0){
					root.__internal.focusedIndex = 0
				}
				return
			}
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

	function __selectedContentYCorrection(down_){
		if (root.__internal.selectedFocusedIndex >= 0){
			var contentY = selectedListView.contentY
			var itemH = root.itemHeight
			var visibleCount = root.maxSelectedGroupItems
			if (visibleCount <= 0) return
			var index = root.__internal.selectedFocusedIndex

			if (down_){
				if ((index + 1) * itemH > contentY + visibleCount * itemH){
					selectedListView.contentY = (index + 1) * itemH - visibleCount * itemH
				}
			}
			else {
				if (index * itemH < contentY){
					selectedListView.contentY = index * itemH
				}
			}
		}
	}
}
