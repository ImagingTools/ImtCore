// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0

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
	// CollectionId for the data provider
	property string collectionId: ""
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

	name: itemSelectElementView.showCount && itemSelectElementView.items.length > 0
		? itemSelectElementView.label + " (" + itemSelectElementView.items.length + ")"
		: itemSelectElementView.label

	// --- Name resolution via FilterableSelectPopup's data provider pattern ---
	property bool __resolvingNames: false

	FilterableSelectGqlDataProvider {
		id: nameResolver
		collectionId: itemSelectElementView.collectionId
		multiSelect: true
		pageSize: 100

		onDataChanged: {
			itemSelectElementView.__resolveItemNames()
		}
	}

	onItemsChanged: {
		if (!itemSelectElementView.__resolvingNames)
			itemSelectElementView.__triggerResolveIfNeeded()
	}

	function __triggerResolveIfNeeded() {
		if (!itemSelectElementView.items || itemSelectElementView.items.length === 0) return
		var hasUnresolved = false
		for (var i = 0; i < itemSelectElementView.items.length; i++) {
			var item = itemSelectElementView.items[i]
			if (!item.name || item.name === item.id) {
				hasUnresolved = true
				break
			}
		}
		if (hasUnresolved) {
			nameResolver.fetch("")
		}
	}

	function __resolveItemNames() {
		var resolverItems = nameResolver.items
		if (!resolverItems || resolverItems.length === 0) return

		var nameMap = ({})
		for (var i = 0; i < resolverItems.length; i++) {
			var ri = resolverItems[i]
			if (ri.id && ri.title && ri.title !== "")
				nameMap[ri.id] = ri.title
		}

		var updated = false
		var newItems = []
		for (var j = 0; j < itemSelectElementView.items.length; j++) {
			var cur = itemSelectElementView.items[j]
			var resolved = nameMap[cur.id]
			if (resolved && cur.name !== resolved) {
				newItems.push({ id: cur.id, name: resolved })
				updated = true
			} else {
				newItems.push(cur)
			}
		}
		if (updated) {
			itemSelectElementView.__resolvingNames = true
			itemSelectElementView.items = newItems
			itemSelectElementView.__resolvingNames = false
		}
	}

	controlComp: Component {
		Text {
			id: addBtn
			visible: itemSelectElementView.editable
			text: "+ " + itemSelectElementView.addButtonText
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: itemSelectElementView.accentColor

			MouseArea {
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
						width: Math.min(chipText.contentWidth + chipRemove.width + Style.paddingS * 3, 200)
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
							visible: itemSelectElementView.editable && itemSelectElementView.nonRemovableIds.indexOf(modelData.id) < 0
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
							decorator: Component {
								ToolButtonDecorator {
									color: "transparent"
									icon.width: Style.iconSizeXS
								}
							}
							onClicked: {
								var removedIndex = index
								var removedData = modelData
								var arr = itemSelectElementView.items.slice()
								arr.splice(removedIndex, 1)
								itemSelectElementView.__resolvingNames = true
								itemSelectElementView.items = arr
								itemSelectElementView.__resolvingNames = false
								itemSelectElementView.itemRemoved(removedIndex, removedData)
								itemSelectElementView.selectionChanged(arr)
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
			dataProvider: FilterableSelectGqlDataProvider {
				collectionId: itemSelectElementView ? itemSelectElementView.collectionId : ""
				multiSelect: true
			}

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
				itemSelectElementView.__resolvingNames = true
				itemSelectElementView.items = arr
				itemSelectElementView.__resolvingNames = false
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
