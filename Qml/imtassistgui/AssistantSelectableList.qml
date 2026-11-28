// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
	AssistantSelectableList

	Standard representation "imtassist.selectableList", version 1: a filterable list
	with selectable entries. The user works with the list independently of the chat
	and hands the selection over to the assistant as context.
*/
Rectangle {
	id: selectableList
	objectName: "AssistantSelectableList"

	color: Style.baseColor
	border.color: Style.borderColor
	radius: Style.radiusS
	height: listColumn.height + 2 * Style.marginS

	property var items: []
	property string title: ""
	property bool multiSelection: false
	property string selectionKind: "selection"
	property var selectedIds: []
	property string filterText: ""
	property var filteredItems: []

	signal selectionHandedOver(string contextKind, string contextLabel, var contextData)

	function isValidItem(item){
		return item !== null && typeof item === "object" && typeof item.id === "string" && item.id !== "" && typeof item.label === "string"
	}

	function isValidSelectionKind(kind){
		if (typeof kind !== "string" || kind.length === 0 || kind.length > 64){
			return false
		}

		for (let i = 0; i < kind.length; i++){
			let code = kind.charCodeAt(i)
			if (!((code >= 48 && code <= 57) || (code >= 65 && code <= 90) || (code >= 97 && code <= 122) ||
					code === 46 || code === 95 || code === 45)){
				return false
			}
		}

		return true
	}

	function setRepresentationData(data){
		if (data === null || typeof data !== "object" || !Array.isArray(data.items)){
			return false
		}

		if (data.selectionKind !== undefined && data.selectionKind !== null && !selectableList.isValidSelectionKind(data.selectionKind)){
			return false
		}

		for (let i = 0; i < data.items.length; i++){
			if (!selectableList.isValidItem(data.items[i])){
				return false
			}
		}

		selectableList.items = data.items
		selectableList.title = typeof data.title === "string" ? data.title : ""
		selectableList.multiSelection = data.selectionMode === "multiple"
		selectableList.selectionKind = typeof data.selectionKind === "string" && data.selectionKind !== "" ? data.selectionKind : "selection"
		selectableList.selectedIds = []
		selectableList.updateFilter()

		return true
	}

	function updateFilter(){
		let text = selectableList.filterText.toLowerCase()
		let result = []
		for (let i = 0; i < selectableList.items.length; i++){
			let item = selectableList.items[i]
			if (text === "" || item.label.toLowerCase().indexOf(text) >= 0){
				result.push(item)
			}
		}

		selectableList.filteredItems = result
	}

	function isSelected(id){
		return selectableList.selectedIds.indexOf(id) >= 0
	}

	function toggleSelection(id){
		let ids = selectableList.selectedIds.slice()
		let index = ids.indexOf(id)
		if (index >= 0){
			ids.splice(index, 1)
		}
		else if (selectableList.multiSelection){
			ids.push(id)
		}
		else{
			ids = [id]
		}

		selectableList.selectedIds = ids
	}

	function findLabel(id){
		for (let i = 0; i < selectableList.items.length; i++){
			if (selectableList.items[i].id === id){
				return selectableList.items[i].label
			}
		}

		return id
	}

	function handOverSelection(){
		if (selectableList.selectedIds.length === 0){
			return
		}

		if (selectableList.selectedIds.length === 1){
			let id = selectableList.selectedIds[0]
			selectableList.selectionHandedOver(selectableList.selectionKind, selectableList.findLabel(id), {"id": id})
		}
		else{
			selectableList.selectionHandedOver(
						selectableList.selectionKind,
						qsTr("%1 entries").arg(selectableList.selectedIds.length),
						{"ids": selectableList.selectedIds})
		}
	}

	onFilterTextChanged: {
		selectableList.updateFilter()
	}

	Column {
		id: listColumn
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Style.marginS
		spacing: Style.spacingS

		BaseText {
			width: parent.width
			visible: selectableList.title !== ""
			text: selectableList.title
			textFormat: Text.PlainText
			font.bold: true
			wrapMode: Text.WordWrap
		}

		CustomTextField {
			id: filterField
			objectName: "AssistantListFilter"
			width: parent.width
			height: Style.controlHeightM
			placeHolderText: qsTr("Filter")
			visible: selectableList.items.length > 5

			onTextChanged: {
				selectableList.filterText = filterField.text
			}
		}

		Repeater {
			id: itemsRepeater
			model: selectableList.filteredItems

			Rectangle {
				id: itemRow
				objectName: "AssistantListItem"
				width: listColumn.width
				height: Math.max(Style.controlHeightM, itemTexts.height + Style.marginXS * 2)
				radius: Style.radiusS
				color: selectableList.isSelected(modelData.id) ? Style.selectedColor : "transparent"
				activeFocusOnTab: true
				border.color: itemRow.activeFocus ? Style.imagingToolsAccentColor : "transparent"
				border.width: itemRow.activeFocus ? 1 : 0

				CheckBox {
					id: itemCheckBox
					anchors.left: parent.left
					anchors.leftMargin: Style.marginXS
					anchors.verticalCenter: parent.verticalCenter
					isActive: false
					checkState: selectableList.isSelected(modelData.id) ? Qt.Checked : Qt.Unchecked
				}

				Column {
					id: itemTexts
					anchors.left: itemCheckBox.right
					anchors.leftMargin: Style.marginS
					anchors.right: parent.right
					anchors.rightMargin: Style.marginXS
					anchors.verticalCenter: parent.verticalCenter

					BaseText {
						width: parent.width
						text: modelData.label
						textFormat: Text.PlainText
						elide: Text.ElideRight
					}

					BaseText {
						width: parent.width
						visible: typeof modelData.description === "string" && modelData.description !== ""
						text: typeof modelData.description === "string" ? modelData.description : ""
						textFormat: Text.PlainText
						color: Style.inactiveTextColor
						font.pixelSize: Style.fontSizeS
						elide: Text.ElideRight
					}
				}

				MouseArea {
					anchors.fill: parent
					cursorShape: Qt.PointingHandCursor

					onClicked: {
						itemRow.forceActiveFocus()
						selectableList.toggleSelection(modelData.id)
					}
				}

				Keys.onSpacePressed: {
					selectableList.toggleSelection(modelData.id)
					event.accepted = true
				}
			}
		}

		BaseText {
			width: parent.width
			visible: selectableList.filteredItems.length === 0
			text: qsTr("No entries")
			color: Style.inactiveTextColor
		}

		Button {
			id: handOverButton
			objectName: "AssistantHandOverSelection"
			text: qsTr("Hand over selection")
			enabled: selectableList.selectedIds.length > 0

			onClicked: {
				selectableList.handOverSelection()
			}
		}
	}
}
