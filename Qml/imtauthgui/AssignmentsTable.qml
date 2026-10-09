// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * AssignmentsTable
 *
 * Roles, groups, users or permissions in one list: the ones assigned directly and the
 * inherited ones with the paths they come through. Rendered like the document history
 * table. Direct items can be checked and removed together when editable, inherited ones are
 * locked. Clicking the name navigates to the item when navigationPath is set.
 *
 * The rows are loaded from an SDL Assignment list and written back with syncAssignments().
 * Give this item a bounded height, or bind it to contentHeight.
 */
Rectangle {
	id: container

	color: Style.baseColor
	clip: true

	// SDL list of AccessWarning, optional.
	property var warnings: null
	// Ready rows [{id, title, direct, paths: [string]}] for a read-only table.
	property var plainRows: null
	// Optional {id: name} map, e.g. permission display names.
	property var nameMap: null
	property bool editable: false
	// Prefix the item id is appended to for NavigationController, e.g. "Administration/Roles/Role/".
	property string navigationPath: ""
	property string nameTitle: qsTr("Name")
	property string emptyText: qsTr("Nothing assigned")
	property string filterPlaceholder: qsTr("Filter...")
	// Explains a locked row; the sources of the row are appended.
	property string lockedHint: qsTr("Inherited, it cannot be removed here. Remove it from its source:")

	readonly property int itemsCount: container.__visibleRows.length
	readonly property int selectedCount: itemsTable.selectionManager.selectedIds.length
	property string removeTitle: qsTr("Remove the selected items?")
	property string removeMessage: qsTr("The selected items will be removed from the list.")
	readonly property real contentHeight: warningsText.height + itemsTable.contentHeight
		+ Style.controlHeightM + 2 * Style.marginM + Style.marginL

	// All rows [{id, title, direct, paths}], the state edited through the functions below.
	property var rows: []
	property var __visibleRows: []
	readonly property int __checkBoxSize: Style.itemSizeS + Style.marginXS
	property string __filterText: ""

	// The checked direct items were removed after the user confirmed it.
	signal removed()

	onRowsChanged: {
		container.keepSelectedDirectRows()
		container.updateRows()
	}
	onPlainRowsChanged: {
		if (container.plainRows){
			container.rows = container.plainRows
		}
	}
	onNameMapChanged: container.updateRows()
	onWarningsChanged: container.updateWarnings()

	Component.onCompleted: {
		if (container.plainRows){
			container.rows = container.plainRows
		}
		container.updateRows()
		container.updateWarnings()
	}

	function kindText(kind){
		let kindId = String(kind)
		if (kindId === "Group"){
			return qsTr("Group")
		}
		if (kindId === "ParentGroup"){
			return qsTr("Parent group")
		}
		if (kindId === "Role"){
			return qsTr("Role")
		}
		if (kindId === "ParentRole"){
			return qsTr("Parent role")
		}
		if (kindId === "DelegatedRole"){
			return qsTr("Delegated role")
		}

		return kindId
	}

	function pathText(steps){
		let parts = []
		for (let i = 0; i < steps.count; i++){
			let step = steps.get(i).item
			let stepName = step.m_name ? String(step.m_name) : String(step.m_id)
			parts.push(container.kindText(step.m_kind) + " " + stepName)
		}

		return parts.join(" " + String.fromCharCode(0x2192) + " ")
	}

	// Rows from an SDL list of Assignment.
	function loadAssignments(assignments){
		let rows = []
		for (let i = 0; assignments && i < assignments.count; i++){
			let item = assignments.get(i).item
			let itemId = String(item.m_id)
			let row = {id: itemId, title: item.m_name ? String(item.m_name) : itemId, direct: item.m_direct === true, paths: []}
			let sources = item.m_sources
			for (let k = 0; sources && k < sources.count; k++){
				let steps = sources.get(k).item.m_steps
				if (steps && steps.count > 0){
					row.paths.push(container.pathText(steps))
				}
			}
			rows.push(row)
		}
		container.rows = rows
	}

	// Direct rows as [{id, name}], e.g. the preselection of a picker.
	function directItems(){
		let items = []
		for (let i = 0; i < container.rows.length; i++){
			if (container.rows[i].direct){
				items.push({id: container.rows[i].id, name: container.rows[i].title})
			}
		}
		return items
	}

	// Makes exactly the given items [{id, name}] direct; rows left with no source disappear.
	function setDirectItems(items){
		let rows = []
		let ids = []
		for (let i = 0; i < items.length; i++){
			ids.push(String(items[i].id))
		}
		for (let k = 0; k < container.rows.length; k++){
			let row = container.rows[k]
			let direct = ids.indexOf(row.id) >= 0
			if (direct || row.paths.length > 0){
				rows.push({id: row.id, title: row.title, direct: direct, paths: row.paths})
			}
		}
		for (let j = 0; j < items.length; j++){
			let found = false
			for (let n = 0; n < rows.length; n++){
				if (rows[n].id === ids[j]){
					found = true
				}
			}
			if (!found){
				rows.push({id: ids[j], title: items[j].name ? String(items[j].name) : ids[j], direct: true, paths: []})
			}
		}
		container.rows = rows
	}

	function removeDirect(itemIds){
		let items = container.directItems()
		let kept = []
		for (let i = 0; i < items.length; i++){
			if (itemIds.indexOf(items[i].id) < 0){
				kept.push(items[i])
			}
		}
		container.setDirectItems(kept)
	}

	// Unchecks the rows that are gone or no longer direct, e.g. after the assignments were re-read.
	function keepSelectedDirectRows(){
		let directIds = []
		for (let i = 0; i < container.rows.length; i++){
			if (container.rows[i].direct){
				directIds.push(container.rows[i].id)
			}
		}

		let selectedIds = itemsTable.selectionManager.selectedIds
		let staleIds = []
		for (let k = 0; k < selectedIds.length; k++){
			if (directIds.indexOf(selectedIds[k]) < 0){
				staleIds.push(selectedIds[k])
			}
		}
		itemsTable.selectionManager.deselect(staleIds)
	}

	// Asks for confirmation, then removes the checked items.
	function removeSelected(){
		if (container.selectedCount === 0){
			return
		}

		ModalDialogManager.openDialog(confirmRemoveDialogComp, {})
	}

	Component {
		id: confirmRemoveDialogComp

		MessageDialog {
			title: container.removeTitle
			message: container.removeMessage

			onFinished: {
				if (buttonId == Enums.yes){
					container.removeDirect(itemsTable.selectionManager.selectedIds.slice())
					container.removed()
				}
			}
		}
	}

	/**
	 * Brings an SDL list of Assignment in line with the rows: updates the direct flag, drops items that
	 * are no longer there and returns the direct rows the caller has to add as new elements.
	 */
	function syncAssignments(assignments){
		let present = []
		for (let i = assignments.count - 1; i >= 0; i--){
			let item = assignments.get(i).item
			let itemId = String(item.m_id)
			let row = null
			for (let k = 0; k < container.rows.length; k++){
				if (container.rows[k].id === itemId){
					row = container.rows[k]
				}
			}
			if (!row){
				assignments.removeElement(i)
				continue
			}
			item.m_direct = row.direct
			present.push(itemId)
		}

		let added = []
		for (let n = 0; n < container.rows.length; n++){
			if (container.rows[n].direct && present.indexOf(container.rows[n].id) < 0){
				added.push(container.rows[n])
			}
		}
		return added
	}

	function updateRows(){
		let rows = []
		let filter = container.__filterText.toLowerCase()
		let source = container.rows ? container.rows : []
		for (let i = 0; i < source.length; i++){
			let row = {id: source[i].id, title: source[i].title, direct: source[i].direct, paths: source[i].paths}
			if (container.nameMap && container.nameMap[row.id]){
				row.title = container.nameMap[row.id]
			}
			if (filter !== "" && row.title.toLowerCase().indexOf(filter) < 0){
				continue
			}
			rows.push(row)
		}

		rows.sort(function(a, b){
			if (a.direct !== b.direct){
				return a.direct ? -1 : 1
			}
			return a.title.localeCompare(b.title)
		})

		container.__visibleRows = rows
		itemsTable.model = rows
	}

	function updateWarnings(){
		let lines = []
		let warnings = container.warnings
		for (let i = 0; warnings && i < warnings.count; i++){
			let warning = warnings.get(i).item
			let ids = warning.m_ids ? warning.m_ids : []
			lines.push(String(warning.m_message) + (ids.length > 0 ? ": " + ids.join(" " + String.fromCharCode(0x2192) + " ") : ""))
		}
		warningsText.text = lines.join("\n")
	}

	Text {
		id: warningsText
		objectName: "AccessWarnings"
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		visible: text !== ""
		height: visible ? implicitHeight + Style.marginM : 0
		color: Style.errorTextColor
		font.family: Style.fontFamily
		font.pixelSize: Style.fontSizeM
		wrapMode: Text.WordWrap
	}

	Component {
		id: columnHeaderComp

		Item {
			height: Style.controlHeightL + Style.marginM

			Rectangle {
				anchors.fill: parent
				color: Style.backgroundColor2
			}

			Row {
				id: headerRow
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.leftMargin: Style.marginL
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginL

				readonly property real columnsWidth: width - (container.editable ? container.__checkBoxSize + 2 * spacing : spacing)

				Item {
					visible: container.editable
					width: container.__checkBoxSize
					height: 1
				}

				BaseText {
					width: headerRow.columnsWidth * 0.35
					text: container.nameTitle
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				BaseText {
					width: headerRow.columnsWidth * 0.65
					text: qsTr("Inherited from")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				height: 1
				color: Style.borderColor
			}
		}
	}

	Component {
		id: rowDelegateComp

		SimpleCollectionItemDelegateBase {
			id: rowDelegate
			showCheckBox: false
			showDefaultActionsMenu: false
			enableDefaultDoubleClickEdit: false

			readonly property var row: rowDelegate.modelItem
			readonly property bool isDirect: rowDelegate.row ? rowDelegate.row.direct : false
			readonly property string sourcesText: rowDelegate.row ? rowDelegate.row.paths.join("; ") : ""

			Row {
				id: dataRow
				width: parent.width
				spacing: Style.marginL

				readonly property real columnsWidth: width - (container.editable ? container.__checkBoxSize + 2 * spacing : spacing)

				Item {
					anchors.verticalCenter: parent.verticalCenter
					visible: container.editable
					width: container.__checkBoxSize
					height: container.__checkBoxSize

					CheckBox {
						objectName: "SelectCheckBox"
						anchors.fill: parent
						visible: rowDelegate.isDirect
						checkState: itemsTable.selectionManager.selectedIds.indexOf(rowDelegate.itemId) >= 0 ? Qt.Checked : Qt.Unchecked

						MouseArea {
							objectName: "MouseArea"
							anchors.fill: parent
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								itemsTable.selectionManager.toggleSelect(rowDelegate.itemId)
							}
						}
					}

					Image {
						id: lockImage
						objectName: "LockedIcon"
						anchors.centerIn: parent
						visible: !rowDelegate.isDirect
						width: Style.iconSizeS
						height: width
						sourceSize.width: width
						sourceSize.height: height
						source: "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)

						MouseArea {
							anchors.fill: parent
							hoverEnabled: true

							onEntered: {
								lockTooltip.show(mouseX, mouseY)
							}

							onExited: {
								lockTooltip.hide()
							}
						}

						CustomTooltip {
							id: lockTooltip
							text: container.lockedHint + " " + rowDelegate.sourcesText
							componentWidth: Style.sizeHintXS
						}
					}
				}

				Item {
					anchors.verticalCenter: parent.verticalCenter
					width: dataRow.columnsWidth * 0.35
					height: nameText.height

					Text {
						id: nameText
						objectName: "ItemName"
						width: Math.min(implicitWidth, parent.width)
						text: rowDelegate.row ? rowDelegate.row.title : ""
						font.pixelSize: Style.fontSizeM
						font.underline: nameMouseArea.containsMouse
						color: container.navigationPath !== "" ? Style.linkColor : Style.textColor
						elide: Text.ElideRight

						MouseArea {
							id: nameMouseArea
							objectName: "MouseArea"
							anchors.fill: parent
							enabled: container.navigationPath !== ""
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								NavigationController.navigate(container.navigationPath + rowDelegate.row.id)
							}
						}
					}
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: dataRow.columnsWidth * 0.65
					text: rowDelegate.sourcesText
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					elide: Text.ElideRight
				}
			}
		}
	}

	SimpleCollectionTable {
		id: itemsTable
		anchors.top: warningsText.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectionEnabled: false
		maximumWidth: container.width
		horizontalMargin: 0
		emptyText: container.emptyText
		filterPlaceholder: container.filterPlaceholder
		columnHeaderComponent: columnHeaderComp
		delegateComponent: rowDelegateComp

		onFilterRequested: {
			container.__filterText = text
			container.updateRows()
		}
	}
}
