// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * ItemSelectElementView
 *
 * A reusable ElementView that displays a list of selectable items as chips (tags).
 * Provides add/remove functionality via a FilterableSelectPopup.
 *
 * This base version does NOT perform GQL-based name resolution.
 * For GQL-based name resolution, use GqlBasedItemSelectElementView from imtguigql.
 *
 * Usage:
 *   ItemSelectElementView {
 *       label: "Roles"
 *       items: [{id: "1", name: "Admin"}, {id: "2", name: "Editor"}]
 *       dataProvider: myFilterableSelectDataProvider
 *   }
 */
ElementView {
	id: itemSelectElementView

	// Data: array of {id, name} objects
	property var items: []
	// Whether the list can be edited (add/remove)
	property bool editable: true
	// Label for the element
	property string label: qsTr("Items")
	// Label for the add button
	property string addButtonText: qsTr("Add item")
	// Placeholder for the filter popup
	property string filterPlaceholder: qsTr("Type or choose an item")

	// Data provider for the select popup (FilterableSelectDataProvider or subclass)
	property var dataProvider: null

	// Text shown when no items selected
	property string emptyText: qsTr("No items")
	// Whether to show selected count next to the label
	property bool showCount: false
	// List of item IDs that cannot be removed
	property var nonRemovableIds: []

	// Chip colors (matching TicketEditor accent palette)
	readonly property string accentColor: "#5b8fd6"
	readonly property string accentBgLight: "#DFECF9"
	readonly property string accentBorderLight: "#B4D3F2"

	signal itemRemoved(int index, var itemData)
	signal selectionChanged(var selectedItems)
	signal popupClosed()

	// --- Name resolution ------------------------------------------------------------
	// Items are usually assigned as bare ids, so a derived view feeds the rows of a
	// second provider through here to turn them into readable names. Guarded against
	// re-entering itemsChanged while the resolved list is written back.
	property bool __resolvingNames: false

	/*!
		Returns whether any item still shows its id instead of a name - a derived view
		uses it to decide if a resolving request is worth sending.
	*/
	function hasUnresolvedItems(){
		if (!itemSelectElementView.items || itemSelectElementView.items.length === 0){
			return false
		}
		for (var i = 0; i < itemSelectElementView.items.length; i++){
			var item = itemSelectElementView.items[i]
			if (!item.name || item.name === item.id){
				return true
			}
		}
		return false
	}

	/*!
		Fills in the names of the current items from normalized provider rows.
		\param providerItems Array of { id, title } as delivered by a data provider.
	*/
	function resolveItemNames(providerItems){
		if (!providerItems || providerItems.length === 0){
			return
		}

		var nameMap = ({})
		for (var i = 0; i < providerItems.length; i++){
			var providerItem = providerItems[i]
			if (providerItem.id && providerItem.title && providerItem.title !== ""){
				nameMap[providerItem.id] = providerItem.title
			}
		}

		var updated = false
		var newItems = []
		for (var j = 0; j < itemSelectElementView.items.length; j++){
			var current = itemSelectElementView.items[j]
			var resolved = nameMap[current.id]
			if (resolved && current.name !== resolved){
				newItems.push({ id: current.id, name: resolved })
				updated = true
			}
			else {
				newItems.push(current)
			}
		}

		if (updated){
			itemSelectElementView.__resolvingNames = true
			itemSelectElementView.items = newItems
			itemSelectElementView.__resolvingNames = false
		}
	}

	function __applyRemoval(newItems, removedIndex, removedData) {
		itemSelectElementView.items = newItems
		itemSelectElementView.itemRemoved(removedIndex, removedData)
		itemSelectElementView.selectionChanged(newItems)
	}

	name: itemSelectElementView.showCount && itemSelectElementView.items.length > 0
		? itemSelectElementView.label + " (" + itemSelectElementView.items.length + ")"
		: itemSelectElementView.label

	controlComp: Component {
		Text {
			id: addBtn

			// Test instrumentation: this reusable control has no objectName of its own (it's a plain
			// Text with a MouseArea, not a Button-derived control that would auto-derive one from
			// `text`), and it's used more than once per document (e.g. UserView.qml's "Roles" AND
			// "Groups" sections) - derive a distinct name from `label` (e.g. "AddRoles"/"AddGroups") so
			// each usage is addressable. Inert - no runtime/visual effect.
			objectName: "Add" + itemSelectElementView.label.replace(/[^A-Za-z0-9]/g, "")

			visible: itemSelectElementView.editable
			text: "+ " + itemSelectElementView.addButtonText
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: itemSelectElementView.accentColor

			MouseArea {
				// Test instrumentation: click()'s mouseAreaOf() helper specifically looks for
				// [objectName="MouseArea"] inside the target - every other clickable control in this
				// codebase names its inner MouseArea this way (see TabDelegate.qml etc.); this one had
				// none. Inert - no runtime/visual effect.
				objectName: "MouseArea"

				anchors.fill: parent
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					var known = []
					for (var j = 0; j < itemSelectElementView.items.length; j++) {
						var item = itemSelectElementView.items[j]
						known.push({ id: item.id, title: item.name || item.id })
					}
					var ids = []
					for (var i = 0; i < itemSelectElementView.items.length; i++)
						ids.push(itemSelectElementView.items[i].id)
					var point = addBtn.mapToItem(null, 0, addBtn.height)
					ModalDialogManager.openDialog(selectComp, {
						"x": point.x,
						"y": point.y,
						"knownItems": known,
						"preselectedIds": ids
					})
				}
			}
		}
	}

	bottomComp: Component {
		Column {
			width: parent.width
			spacing: Style.spacingXS

			Flow {
				width: parent.width
				clip: true
				spacing: Style.spacingXS
				visible: itemSelectElementView.items.length > 0

				Repeater {
					model: itemSelectElementView.items
					delegate: Rectangle {
						objectName: "AssignedItem_" + index
						width: Math.min(chipText.implicitWidth + chipRemove.width + Style.paddingS * 3, 200)
						height: 28
						radius: 14
						color: itemSelectElementView.accentBgLight
						border.color: itemSelectElementView.accentBorderLight
						border.width: 1

						Text {
							id: chipText
							anchors.left: parent.left
							anchors.leftMargin: Style.paddingS + 2
							anchors.right: chipRemove.left
							anchors.rightMargin: 2
							anchors.verticalCenter: parent.verticalCenter
							text: modelData.name || modelData.id
							font.pixelSize: Style.fontSizeM
							color: itemSelectElementView.accentColor
							elide: Text.ElideRight
							maximumLineCount: 1
						}

						ToolButton {
							id: chipRemove
							objectName: "RemoveButton"
							visible: itemSelectElementView.editable && itemSelectElementView.nonRemovableIds.indexOf(modelData.id) < 0
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
							decorator: Component {
								ToolButtonDecorator {
									color: "transparent"
									icon.width: Style.iconSizeXS
								}
							}
							onClicked: {
								var removedIndex = index
								var removedData = modelData
								var removedId = modelData ? modelData.id : ""
								var arr = []
								if (removedId) {
									for (var k = 0; k < itemSelectElementView.items.length; k++) {
										var it = itemSelectElementView.items[k]
										if (it && it.id !== removedId)
											arr.push(it)
									}
								} else {
									arr = itemSelectElementView.items.slice()
									if (removedIndex >= 0 && removedIndex < arr.length)
										arr.splice(removedIndex, 1)
								}
								itemSelectElementView.__applyRemoval(arr, removedIndex, removedData)
							}
						}
					}
				}
			}

			Text {
				visible: itemSelectElementView.items.length === 0
				width: parent.width
				text: itemSelectElementView.emptyText
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				wrapMode: Text.WordWrap
			}
		}
	}

	// Picker Component — opened via ModalDialogManager
	Component {
		id: selectComp

		FilterableSelectPopup {
			dataProvider: itemSelectElementView ? itemSelectElementView.dataProvider : null

			itemWidth: 280
			showCheckBox: true
			showSelectedGroup: true
			filterPlaceholder: itemSelectElementView ? itemSelectElementView.filterPlaceholder : ""

			onSelectionChanged: {
				var arr = []
				for (var i = 0; i < selectedIds.length; i++) {
					var selId = selectedIds[i]
					var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
					if (!selName)
						selName = selId
					arr.push({id: selId, name: selName})
				}
				itemSelectElementView.items = arr
				itemSelectElementView.selectionChanged(arr)
			}

			Component.onDestruction: {
				if (itemSelectElementView){
					itemSelectElementView.popupClosed()
				}
			}
		}
	}
}
