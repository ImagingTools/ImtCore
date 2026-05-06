// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0

ElementView {
	id: root

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

	// Chip colors (matching TicketEditor accent palette)
	readonly property string accentColor: "#5b8fd6"
	readonly property string accentBgLight: "#DFECF9"
	readonly property string accentBorderLight: "#B4D3F2"

	signal itemRemoved(int index, var itemData)
	signal selectionChanged(var selectedItems)
	signal popupClosed()

	name: root.showCount && root.items.length > 0
		? root.label + " (" + root.items.length + ")"
		: root.label

	// --- Name resolution via FilterableSelectPopup's data provider pattern ---
	FilterableSelectGqlDataProvider {
		id: nameResolver
		collectionId: root.collectionId
		multiSelect: true
		pageSize: 100

		onDataChanged: {
			root.__resolveItemNames()
		}
	}

	onItemsChanged: {
		root.__triggerResolveIfNeeded()
	}

	function __triggerResolveIfNeeded() {
		if (!root.items || root.items.length === 0) return
		var hasUnresolved = false
		for (var i = 0; i < root.items.length; i++) {
			var item = root.items[i]
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
		for (var j = 0; j < root.items.length; j++) {
			var cur = root.items[j]
			var resolved = nameMap[cur.id]
			if (resolved && cur.name !== resolved) {
				newItems.push({ id: cur.id, name: resolved })
				updated = true
			} else {
				newItems.push(cur)
			}
		}
		if (updated) {
			root.items = newItems
		}
	}

	controlComp: Component {
		Text {
			id: addBtn
			visible: root.editable
			text: "+ " + root.addButtonText
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: root.accentColor

			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					var known = []
					for (var j = 0; j < root.items.length; j++) {
						var item = root.items[j]
						known.push({ id: item.id, title: item.name || item.id })
					}
					var ids = []
					for (var i = 0; i < root.items.length; i++)
						ids.push(root.items[i].id)
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
				visible: root.items.length > 0

				Repeater {
					model: root.items
					delegate: Rectangle {
						width: Math.min(chipText.contentWidth + chipRemove.width + Style.paddingS * 3, 200)
						height: 28
						radius: 14
						color: root.accentBgLight
						border.color: root.accentBorderLight
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
							color: root.accentColor
							elide: Text.ElideRight
							maximumLineCount: 1
						}

						ToolButton {
							id: chipRemove
							visible: root.editable
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
								root.itemRemoved(index, modelData)
							}
						}
					}
				}
			}

			Text {
				visible: root.items.length === 0
				width: parent.width
				text: root.emptyText
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
				collectionId: root.collectionId
				multiSelect: true
			}

			itemWidth: 280
			showCheckBox: true
			showSelectedGroup: true
			filterPlaceholder: root.filterPlaceholder

			onSelectionChanged: {
				var arr = []
				for (var i = 0; i < selectedIds.length; i++) {
					var selId = selectedIds[i]
					var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
					if (!selName)
						selName = selId
					arr.push({id: selId, name: selName})
				}
				root.selectionChanged(arr)
			}

			Component.onDestruction: {
				root.popupClosed()
			}
		}
	}
}
