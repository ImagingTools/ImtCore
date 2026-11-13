// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
 * PermissionsTableView
 *
 * Reusable component that displays permissions as a flat table with path-like
 * names instead of a nested tree.
 *
 * A tree such as:
 *   Sensor Management
 *   ├── View Sensor History
 *   ├── Edit Sensor
 *   │   ├── Add Sensor
 *   │   └── Change Sensor
 *   │       └── Change Production Status
 *
 * Is displayed as:
 *   Sensor Management           (group header)
 *   ├── View Sensor History
 *   ├── Edit Sensor
 *   ├── Edit Sensor / Add Sensor
 *   ├── Edit Sensor / Change Sensor
 *   └── Edit Sensor / Change Sensor / Change Production Status
 *
 * The rows live in plain arrays indexed by row number, the list model holds only
 * the numbers of the visible rows. A change edits the arrays and increments
 * __revision once, so only the rows on screen re-read their state.
 *
 * A permission is checked when it is selected here (own), inherited (locked) or
 * required by another checked one (required); the last two cannot be unchecked.
 *
 * Properties:
 *   selectedOnly      — shows the checked permissions as a read-only table
 *   readOnly          — disables checkbox interaction
 *
 * Functions:
 *   rebuildFromFlatArray(groups), rebuild(tree), rebuildFromTreeItemModel(model)
 *   applySelection(ids), setLockedPermissions(map), getCheckedIds(excludeLocked)
 *   checkAll(), uncheckAll(), expandAll(), collapseAll()
 *
 * Signals:
 *   selectionChanged() — emitted when the user changes the checked items
 */
Item {
	id: root

	property var permissionsTree: null
	property var selectedIds: []
	// Inherited permissions {permissionId: source text}, checked and locked.
	property var lockedPermissions: ({})
	// Checked and locked because a checked permission requires them {permissionId: [ids of the requiring ones]}.
	property var requiredPermissions: ({})
	// Panel under the tree with the full name, path, description, source and requirements of the selected row.
	property bool showDetails: true
	// Shows the checked permissions, own and inherited, as a read-only table instead of the tree and its commands.
	property bool selectedOnly: false
	// Shown instead of the tree when there is nothing to show.
	property string emptyText: qsTr("No permissions available.")
	readonly property int checkedCount: root.__checkedCount
	property bool readOnly: false
	property bool showControlPanel: true
	property int treeToScrollbarSpacing: 0
	property int controlPanelTopMargin: Style.marginXL
	property int treeTopMargin: Style.marginM
	property int treeBottomMargin: Style.marginXL
	property int rowHeight: Style.tableRowHeight
	property int indentation: 18

	// The details panel is shown only while the list above it keeps its header and three rows.
	readonly property bool __detailsFit: root.height >= (root.showControlPanel ? root.controlPanelTopMargin + headerArea.height + root.treeTopMargin : 0)
		+ Style.tableHeaderHeight + 1 + 3 * root.rowHeight + Style.marginM + details.height + root.treeBottomMargin

	readonly property real preferredHeight: root.selectedOnly
		? (showControlPanel ? controlPanelTopMargin + headerArea.implicitHeight + treeTopMargin : 0)
			+ selectedTable.headerHeight + selectedTable.contentHeight + (details.visible ? details.height + Style.marginM : 0) + treeBottomMargin
		: (showControlPanel ? controlPanelTopMargin + headerArea.implicitHeight + treeTopMargin : 0)
			+ treeHeader.height + 1 + visibleModel.count * root.rowHeight + (details.visible ? details.height + Style.marginM : 0) + treeBottomMargin

	signal selectionChanged()

	// Rows, filled by __build(); a group row is followed by its leaf rows.
	property var __keys: []
	property var __ids: []
	property var __names: []
	property var __descriptions: []
	property var __isGroup: []
	// Leaf row -> its group row, group row -> -1.
	property var __groupRow: []
	property var __childCount: []
	property var __search: []
	property var __expanded: []
	// Rows of every key, a permission can be listed in several groups {key: [rows]}.
	property var __rowsByKey: ({})
	// Permissions each permission requires directly {permissionId: [permissionIds]}.
	property var __requirements: ({})
	// The first row of the key of each row; the state of a permission is kept on that row.
	property var __canon: []
	// Rows each row requires directly, null without requirements.
	property var __requiredRows: []
	// Selected here, by row.
	property var __own: []
	// Source text of the inherited permissions, by row.
	property var __lockSource: []

	// Row state, recomputed by __refresh().
	property var __rowState: []
	property var __rowLocked: []
	property var __rowBadge: []
	property var __rowHint: []
	// Rows passing the filter, recomputed by __applyFilter().
	property var __passes: []
	// Mirrors visibleModel.
	property var __visibleRows: []
	property int __revision: 0
	property int __checkedCount: 0
	property int __currentRow: -1

	// Row picked in the "Selected only" table.
	property string __selectedTableKey: ""
	property string __filterText: ""
	property var __selectedRows: []

	property string __okIcon: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal)
	property string __okInactiveIcon: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.Off, Icon.Mode.Disabled)
	property string __downIcon: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
	property string __rightIcon: "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
	property string __lockIcon: "../../../" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
	property bool __alternating: Style.enableTableAlternating !== undefined ? Style.enableTableAlternating : false
	property string __alternateColor: Style.tableAlternateRowColor !== undefined ? Style.tableAlternateRowColor : Style.alternateBaseColor

	onSelectedOnlyChanged: root.updateSelectedRows()

	// --- Public API ---

	function getCheckedIds(excludeLocked) {
		var keys = root.__keys
		var isGroup = root.__isGroup
		var canon = root.__canon
		var states = root.__rowState
		var lockedRows = root.__rowLocked
		var result = []
		for (var row = 0; row < keys.length; row++) {
			if (isGroup[row] || canon[row] !== row || states[row] !== Qt.Checked)
				continue
			if (excludeLocked && lockedRows[row])
				continue
			result.push(keys[row])
		}
		result.sort()
		return result
	}

	/**
	 * Inherited permissions {permissionId: source text} are checked and locked: they cannot
	 * be unchecked here and show where they come from. Pass {} to unlock everything; unlocking
	 * keeps the check state, so apply the own selection afterwards.
	 */
	function setLockedPermissions(lockedMap) {
		lockedMap = lockedMap || ({})
		var keys = root.__keys
		var own = root.__own
		var oldSources = root.__lockSource
		var sources = root.__filledArray(keys.length, null)
		for (var row = 0; row < keys.length; row++) {
			if (oldSources[row] !== null && lockedMap[keys[row]] === undefined)
				own[row] = true
		}
		for (var id in lockedMap) {
			var lockedRow = root.__permissionRow(id)
			if (lockedRow >= 0)
				sources[lockedRow] = String(lockedMap[id])
		}
		root.__lockSource = sources
		root.lockedPermissions = lockedMap
		root.__refresh()
	}

	function applySelection(ids) {
		var own = root.__filledArray(root.__keys.length, false)
		for (var i = 0; i < ids.length; i++) {
			var row = root.__permissionRow(ids[i])
			if (row >= 0)
				own[row] = true
		}
		root.__own = own
		root.__refresh()
	}

	function checkAll() {
		root.__setOwn(-1, true)
	}

	function uncheckAll() {
		root.__setOwn(-1, false)
	}

	function expandAll() {
		root.__setAllExpanded(true)
	}

	function collapseAll() {
		root.__setAllExpanded(false)
	}

	// Rebuild from an array of permission groups from the server:
	// { groupId, groupName, entries: [{ permissionId, displayName, description, requirements }] }
	function rebuildFromFlatArray(groups) {
		root.__build(groups || [])
	}

	// Rebuild from a recursive permissions tree (array/ListModel of nodes with m_id, m_name, m_description, m_children).
	function rebuild(treeData) {
		root.permissionsTree = treeData
		root.__build(root.__groupsFromTree(treeData))
	}

	// Rebuild from a C++ TreeItemModel (used in RoleView legacy path).
	function rebuildFromTreeItemModel(treeItemModel) {
		if (!treeItemModel)
			return

		var treeNodes = TreeModelBuilder.fromTreeItemModel(
			treeItemModel,
			function(wrapper, index) {
				return {
					key: wrapper.data("FeatureId", ""),
					text: wrapper.data("FeatureName", ""),
					children: [],
					data: {
						description: wrapper.data("FeatureDescription", "")
					}
				}
			},
			function(wrapper, index) {
				return wrapper.childModel("ChildModel")
			}
		)
		root.__build(root.__groupsFromTree(treeNodes))
	}

	// Refills the "Selected only" table and the details panel.
	function updateSelectedRows() {
		if (root.selectedOnly) {
			var ids = root.getCheckedIds(false)
			var rows = []
			var filter = root.__filterText.toLowerCase()
			for (var i = 0; i < ids.length; i++) {
				if (filter !== "" && ids[i].toLowerCase().indexOf(filter) < 0)
					continue
				rows.push({
					id: ids[i],
					fullPath: ids[i],
					badge: root.__rowBadge[root.__rowsByKey[ids[i]][0]]
				})
			}
			root.__selectedRows = rows
			// The table is refilled, so its selection does not survive.
			root.__selectedTableKey = ""
			selectedTable.resetSelection(true)
			selectedTable.elements = rows
		}
		root.__updateDetails()
	}

	// --- Internals ---

	function __build(groups) {
		var keys = []
		var ids = []
		var names = []
		var descriptions = []
		var isGroup = []
		var groupRow = []
		var childCount = []
		var search = []
		var expanded = []
		var rowsByKey = ({})
		var requirements = ({})

		for (var gi = 0; gi < groups.length; gi++) {
			var group = groups[gi]
			var entries = group.entries || []
			var g = keys.length
			// Prefixed so a group can never share a key with a permission of the same id.
			var groupKey = "__group__:" + (group.groupId || ("group_" + gi))
			rowsByKey[groupKey] = [g]
			keys.push(groupKey)
			ids.push(group.groupId || "")
			names.push(group.groupName || "")
			descriptions.push(group.description || "")
			isGroup.push(true)
			groupRow.push(-1)
			childCount.push(entries.length)
			search.push(((group.groupName || "") + "\n" + (group.description || "")).toLowerCase())
			expanded.push(true)

			for (var ei = 0; ei < entries.length; ei++) {
				var entry = entries[ei]
				var key = entry.permissionId || ("perm_" + gi + "_" + ei)
				if (entry.permissionId && entry.requirements && entry.requirements.length > 0)
					requirements[entry.permissionId] = entry.requirements
				if (!rowsByKey[key])
					rowsByKey[key] = []
				rowsByKey[key].push(keys.length)
				keys.push(key)
				ids.push(entry.permissionId || "")
				names.push(entry.displayName || "")
				descriptions.push(entry.description || "")
				isGroup.push(false)
				groupRow.push(g)
				childCount.push(0)
				search.push(((entry.displayName || "") + "\n" + (entry.description || "")).toLowerCase())
				expanded.push(false)
			}
		}

		var canon = []
		var requiredRows = []
		for (var row = 0; row < keys.length; row++) {
			canon.push(rowsByKey[keys[row]][0])
			var targets = isGroup[row] ? null : requirements[keys[row]]
			if (!targets) {
				requiredRows.push(null)
				continue
			}
			// Requirements outside this tree cannot be granted here.
			var targetRows = []
			for (var t = 0; t < targets.length; t++) {
				var targetKeyRows = rowsByKey[targets[t]]
				if (targetKeyRows && !isGroup[targetKeyRows[0]])
					targetRows.push(targetKeyRows[0])
			}
			requiredRows.push(targetRows.length > 0 ? targetRows : null)
		}

		root.__keys = keys
		root.__ids = ids
		root.__names = names
		root.__descriptions = descriptions
		root.__isGroup = isGroup
		root.__groupRow = groupRow
		root.__childCount = childCount
		root.__search = search
		root.__expanded = expanded
		root.__rowsByKey = rowsByKey
		root.__requirements = requirements
		root.__canon = canon
		root.__requiredRows = requiredRows
		root.__own = root.__filledArray(keys.length, false)
		root.__lockSource = root.__filledArray(keys.length, null)
		root.lockedPermissions = ({})
		root.__currentRow = -1
		root.__applyFilter(false)
		root.__refresh()
	}

	/**
	 * Recomputes the required permissions and the state of every row, then repaints once.
	 * Requirements are followed transitively from the own and inherited permissions;
	 * cycles end, as every permission is visited once.
	 */
	function __refresh() {
		// Property reads are slow in loops, so the arrays are read once.
		var keys = root.__keys
		var isGroup = root.__isGroup
		var groupRows = root.__groupRow
		var childCount = root.__childCount
		var canon = root.__canon
		var requiredRows = root.__requiredRows
		var own = root.__own
		var lockSource = root.__lockSource
		var count = keys.length

		// Arrays are filled with push: writing by index into new Array(n) and reading its holes is slow in V4.
		var queue = []
		var visited = []
		var requiredBy = []
		for (var sourceRow = 0; sourceRow < count; sourceRow++) {
			var isSource = own[sourceRow] || lockSource[sourceRow] !== null
			visited.push(isSource)
			requiredBy.push(null)
			if (isSource)
				queue.push(sourceRow)
		}

		// Keys of the permissions requiring each row.
		for (var head = 0; head < queue.length; head++) {
			var source = queue[head]
			var targets = requiredRows[source]
			if (!targets)
				continue
			for (var t = 0; t < targets.length; t++) {
				var target = targets[t]
				if (!own[target] && lockSource[target] === null) {
					if (!requiredBy[target])
						requiredBy[target] = []
					if (requiredBy[target].indexOf(keys[source]) < 0)
						requiredBy[target].push(keys[source])
				}
				if (!visited[target]) {
					visited[target] = true
					queue.push(target)
				}
			}
		}

		var required = ({})
		var states = []
		var lockedRows = []
		var badges = []
		var hints = []
		var groupChecked = []
		var checkedCount = 0
		var inheritedBadge = qsTr("Inherited")
		var requiredBadge = qsTr("Required")
		var inheritedPrefix = qsTr("Inherited from:") + " "
		var requiredPrefix = qsTr("Required by:") + " "
		for (var row = 0; row < count; row++) {
			groupChecked.push(0)
			if (isGroup[row]) {
				// Filled in below, once its permissions are counted.
				states.push(Qt.Unchecked)
				lockedRows.push(false)
				badges.push("")
				hints.push("")
				continue
			}
			var c = canon[row]
			var isLocked = lockSource[c] !== null
			var isRequired = requiredBy[c] !== null
			var checked = own[c] || isLocked || isRequired
			states.push(checked ? Qt.Checked : Qt.Unchecked)
			lockedRows.push(isLocked || isRequired)
			badges.push(isLocked ? inheritedBadge : isRequired ? requiredBadge : "")
			hints.push(isLocked ? inheritedPrefix + lockSource[c]
				: isRequired ? requiredPrefix + requiredBy[c].join(", ")
				: "")
			if (checked && c === row)
				checkedCount++
			if (isRequired && c === row)
				required[keys[row]] = requiredBy[row]
			var g = groupRows[row]
			if (checked)
				groupChecked[g] += 1
			// A group with an inherited permission is locked, or its check box would toggle the inherited one.
			if (isLocked)
				lockedRows[g] = true
		}
		for (var groupRow = 0; groupRow < count; groupRow++) {
			if (!isGroup[groupRow])
				continue
			var total = childCount[groupRow]
			var done = groupChecked[groupRow]
			states[groupRow] = done === 0 ? Qt.Unchecked : done === total ? Qt.Checked : Qt.PartiallyChecked
			badges[groupRow] = done + "/" + total
		}

		root.requiredPermissions = required
		root.__rowState = states
		root.__rowLocked = lockedRows
		root.__rowBadge = badges
		root.__rowHint = hints
		root.__checkedCount = checkedCount
		root.__revision++
		root.updateSelectedRows()
	}

	// Selects or unselects the permissions of a row, or all shown by the filter for row -1.
	function __setOwn(row, value) {
		if (root.readOnly)
			return
		var isGroup = root.__isGroup
		var canon = root.__canon
		var lockedRows = root.__rowLocked
		var passes = root.__passes
		var own = root.__own
		var first = row < 0 ? 0 : row
		var last = row < 0 ? isGroup.length - 1 : isGroup[row] ? row + root.__childCount[row] : row
		for (var r = first; r <= last; r++) {
			if (!isGroup[r] && !lockedRows[r] && passes[r])
				own[canon[r]] = value
		}
		root.__refresh()
		root.selectionChanged()
	}

	function __filledArray(count, value) {
		var result = []
		for (var i = 0; i < count; i++)
			result.push(value)
		return result
	}

	// Row of a permission id, -1 if it is not in the tree.
	function __permissionRow(id) {
		var rows = root.__rowsByKey[id]
		return rows && !root.__isGroup[rows[0]] ? rows[0] : -1
	}

	function __toggleRow(row) {
		if (root.__rowLocked[row])
			return
		root.__setOwn(row, root.__rowState[row] !== Qt.Checked)
	}

	function __applyFilter(expandMatches) {
		var text = root.__filterText.trim().toLowerCase()
		var isGroup = root.__isGroup
		var search = root.__search
		var groupRows = root.__groupRow
		var expanded = root.__expanded
		var count = isGroup.length
		var passes = []
		var groupMatches = false
		for (var row = 0; row < count; row++) {
			if (isGroup[row]) {
				groupMatches = text === "" || search[row].indexOf(text) >= 0
				passes.push(groupMatches)
				continue
			}
			var matches = groupMatches || search[row].indexOf(text) >= 0
			passes.push(matches)
			if (matches)
				passes[groupRows[row]] = true
		}
		if (expandMatches && text !== "") {
			for (var g = 0; g < count; g++) {
				if (isGroup[g] && passes[g])
					expanded[g] = true
			}
		}
		root.__passes = passes
		root.__rebuildVisible()
	}

	function __rebuildVisible() {
		var isGroup = root.__isGroup
		var passes = root.__passes
		var expanded = root.__expanded
		var groupRows = root.__groupRow
		var rows = []
		var items = []
		for (var row = 0; row < isGroup.length; row++) {
			if (!passes[row])
				continue
			if (!isGroup[row] && !expanded[groupRows[row]])
				continue
			rows.push(row)
			items.push({ row: row })
		}
		root.__visibleRows = rows
		// A destroyed row does not report the pointer leaving, so its tooltip is closed here.
		rowTooltip.hide()
		visibleModel.clear()
		if (items.length > 0)
			visibleModel.append(items)
		root.__revision++
	}

	function __setExpanded(groupRow, value) {
		if (!root.__isGroup[groupRow] || root.__expanded[groupRow] === value)
			return
		root.__expanded[groupRow] = value
		var at = root.__visibleRows.indexOf(groupRow)
		if (at >= 0) {
			if (value) {
				var rows = []
				var items = []
				for (var row = groupRow + 1; row <= groupRow + root.__childCount[groupRow]; row++) {
					if (root.__passes[row]) {
						rows.push(row)
						items.push({ row: row })
					}
				}
				if (items.length > 0) {
					root.__visibleRows.splice.apply(root.__visibleRows, [at + 1, 0].concat(rows))
					visibleModel.insert(at + 1, items)
				}
			}
			else {
				var removeCount = 0
				while (at + 1 + removeCount < root.__visibleRows.length && root.__groupRow[root.__visibleRows[at + 1 + removeCount]] === groupRow)
					removeCount++
				if (removeCount > 0) {
					root.__visibleRows.splice(at + 1, removeCount)
					visibleModel.remove(at + 1, removeCount)
				}
			}
		}
		root.__revision++
	}

	function __setAllExpanded(value) {
		for (var row = 0; row < root.__keys.length; row++) {
			if (root.__isGroup[row])
				root.__expanded[row] = value
		}
		root.__rebuildVisible()
	}

	function __selectRow(row) {
		root.__currentRow = row
		root.__updateDetails()
		var at = root.__visibleRows.indexOf(row)
		if (at >= 0)
			listView.positionViewAtIndex(at, ListView.Contain)
	}

	function __handleKey(event) {
		var at = root.__visibleRows.indexOf(root.__currentRow)
		if (event.key === Qt.Key_Down) {
			if (at + 1 < root.__visibleRows.length)
				root.__selectRow(root.__visibleRows[at + 1])
			return true
		}
		if (event.key === Qt.Key_Up) {
			if (at > 0)
				root.__selectRow(root.__visibleRows[at - 1])
			return true
		}
		if (at < 0)
			return false
		if (event.key === Qt.Key_Space) {
			root.__toggleRow(root.__currentRow)
			return true
		}
		if (event.key === Qt.Key_Right || event.key === Qt.Key_Left) {
			root.__setExpanded(root.__currentRow, event.key === Qt.Key_Right)
			return true
		}
		return false
	}

	// Fills the details panel from the selected row of the tree, or of the table in the "Selected only" mode.
	function __updateDetails() {
		var row = root.__currentRow
		if (root.selectedOnly)
			row = root.__selectedTableKey !== "" && root.__rowsByKey[root.__selectedTableKey] ? root.__rowsByKey[root.__selectedTableKey][0] : -1
		if (row < 0 || row >= root.__keys.length) {
			details.path = ""
			return
		}

		details.badge = root.__rowBadge[row]
		details.description = root.__descriptions[row]
		if (root.__isGroup[row]) {
			details.path = "/" + root.__ids[row]
			details.source = qsTr("%1 of the permissions are selected").arg(root.__rowBadge[row])
			details.requires = ""
			return
		}

		var key = root.__keys[row]
		details.path = root.__ids[row] || key
		details.source = root.__rowHint[row] !== "" ? root.__rowHint[row]
			: root.__rowState[row] === Qt.Checked ? qsTr("Selected here")
			: qsTr("Not selected")
		var requiredNames = []
		var requirements = root.__requirements[key] || []
		for (var i = 0; i < requirements.length; i++) {
			if (root.__rowsByKey[requirements[i]])
				requiredNames.push(requirements[i])
		}
		details.requires = requiredNames.join(", ")
	}

	// Tooltip of the hovered row: the full name if the row cuts it, and where a locked permission comes from.
	function __showRowTooltip(nameItem, hint, mouseItem, x, y) {
		var lines = []
		if (nameItem.implicitWidth > nameItem.width + 1)
			lines.push(nameItem.text)
		if (hint !== "")
			lines.push(hint)
		if (lines.length === 0) {
			rowTooltip.hide()
			return
		}

		var text = lines.join("\n")
		if (rowTooltip.text !== text || rowTooltip.targetItem !== mouseItem)
			rowTooltip.hide()
		// The decorator places the tooltip next to the tooltip item, so put the item over the name.
		var point = nameItem.mapToItem(root, 0, 0)
		rowTooltip.x = point.x
		rowTooltip.y = point.y
		rowTooltip.width = nameItem.width
		rowTooltip.height = nameItem.height
		rowTooltip.targetItem = mouseItem
		rowTooltip.text = text
		rowTooltip.show(x, y)
	}

	// Converts a recursive tree into groups: top nodes become groups, their descendants
	// become entries named by their path ("Parent / Child / Grandchild").
	function __groupsFromTree(source) {
		var groups = []
		var count = source ? (source.count !== undefined ? source.count : source.length) : 0
		for (var i = 0; i < count; i++) {
			var item = root.__treeItem(source, i)
			if (!item)
				continue
			var entries = []
			root.__collectEntries(root.__treeChildren(item), "", entries)
			groups.push({
				groupId: root.__treeId(item),
				groupName: root.__treeName(item),
				description: root.__treeDescription(item),
				entries: entries
			})
		}
		return groups
	}

	function __collectEntries(children, parentPath, result) {
		var count = children ? (children.count !== undefined ? children.count : children.length) : 0
		for (var i = 0; i < count; i++) {
			var item = root.__treeItem(children, i)
			if (!item)
				continue
			var name = root.__treeName(item)
			var path = parentPath ? parentPath + " / " + name : name
			result.push({
				permissionId: root.__treeId(item),
				displayName: path,
				description: root.__treeDescription(item)
			})
			root.__collectEntries(root.__treeChildren(item), path, result)
		}
	}

	function __treeItem(list, index) {
		var item = list.count !== undefined ? list.get(index) : list[index]
		return item && item.item ? item.item : item
	}

	function __treeId(item) {
		return item.m_id || item.id || item.key || ""
	}

	function __treeName(item) {
		return item.m_name || item.name || item.text || ""
	}

	function __treeDescription(item) {
		return item.m_description || item.description || (item.data ? item.data.description : "") || ""
	}

	function __treeChildren(item) {
		return item.m_children || item.children || null
	}

	ListModel {
		id: visibleModel
	}

	Timer {
		id: filterTimer
		interval: 150
		repeat: false
		onTriggered: {
			root.__applyFilter(true)
			if (root.selectedOnly)
				root.updateSelectedRows()
		}
	}

	// --- Header ---
	Column {
		id: headerArea
		visible: root.showControlPanel
		height: visible ? implicitHeight : 0
		anchors.top: parent.top
		anchors.topMargin: root.controlPanelTopMargin
		anchors.left: parent.left
		anchors.right: parent.right
		spacing: Style.marginM

		// The commands change the tree, so the "Selected only" table keeps only the filter.
		// Right-aligned; when narrow the expand group goes to the next line.
		Item {
			width: parent.width
			height: commandsFlow.height
			visible: !root.selectedOnly

			Flow {
				id: commandsFlow
				anchors.right: parent.right
				width: Math.min(checkGroup.width + commandsFlow.spacing + expandGroup.width, parent.width)
				spacing: Style.marginM

				Row {
					id: checkGroup
					spacing: Style.marginM

					Text {
						id: checkAllBtn
						objectName: "CheckAllButton"
						text: qsTr("Check All")
						font.pixelSize: Style.fontSizeM
						color: root.readOnly ? Style.inactiveTextColor : Style.linkColor

						MouseArea {
							objectName: "MouseArea"
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
							enabled: !root.readOnly
							onClicked: root.checkAll()
						}
					}

					Text {
						id: uncheckAllBtn
						objectName: "UncheckAllButton"
						text: qsTr("Uncheck All")
						font.pixelSize: Style.fontSizeM
						color: root.readOnly ? Style.inactiveTextColor : Style.linkColor

						MouseArea {
							objectName: "MouseArea"
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
							enabled: !root.readOnly
							onClicked: root.uncheckAll()
						}
					}
				}

				Row {
					id: expandGroup
					spacing: Style.marginM

					Text {
						id: expandBtn
						objectName: "ExpandAllButton"
						text: qsTr("Expand All")
						font.pixelSize: Style.fontSizeM
						color: Style.linkColor

						MouseArea {
							objectName: "MouseArea"
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: root.expandAll()
						}
					}

					Text {
						id: collapseBtn
						objectName: "CollapseAllButton"
						text: qsTr("Collapse All")
						font.pixelSize: Style.fontSizeM
						color: Style.linkColor

						MouseArea {
							objectName: "MouseArea"
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: root.collapseAll()
						}
					}
				}
			}
		}

		SearchTextInput {
			id: filterInput
			width: parent.width
			placeHolderText: qsTr("Filter permissions...")
			onTextChanged: {
				root.__filterText = text
				filterTimer.restart()
			}
		}
	}

	// --- Selected only (read-only) ---
	// Every checked permission, own and locked, in a standard table; the details panel tells where each comes from.
	Table {
		id: selectedTable
		objectName: "SelectedPermissionsTable"
		anchors.top: root.showControlPanel ? headerArea.bottom : parent.top
		anchors.topMargin: root.showControlPanel ? root.treeTopMargin : 0
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: details.visible ? details.top : parent.bottom
		anchors.bottomMargin: details.visible ? Style.marginM : root.treeBottomMargin
		visible: root.selectedOnly
		canMoveColumns: false
		isMultiSelect: false

		onSelectionChanged: {
			root.__selectedTableKey = selection.length > 0 && selection[0] < root.__selectedRows.length ? root.__selectedRows[selection[0]].id : ""
			root.__updateDetails()
		}
	}

	Text {
		anchors.top: selectedTable.top
		anchors.topMargin: Math.max(selectedTable.headerHeight, Style.tableHeaderHeight) + Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginL
		visible: root.selectedOnly && root.__selectedRows.length === 0
		text: root.__filterText !== "" ? qsTr("No permissions match the filter.") : qsTr("No permissions are selected.")
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
		font.pixelSize: Style.fontSizeM
		color: Style.inactiveTextColor
	}

	TableHeaders {
		id: selectedHeaders

		Component.onCompleted: {
			addHeader("fullPath", qsTr("Full path"), false, false)
			selectedTable.headers = selectedHeaders
			selectedTable.setColumnContentById("fullPath", selectedPathCellComp)
		}
	}

	// The path with its mark ("Inherited", "Required") on the right.
	Component {
		id: selectedPathCellComp

		TableCellDelegateBase {
			id: selectedPathCell

			readonly property var row: selectedPathCell.rowIndex >= 0 && selectedPathCell.rowIndex < root.__selectedRows.length
				? root.__selectedRows[selectedPathCell.rowIndex] : null

			Item {
				anchors.fill: parent

				BaseText {
					objectName: "PermissionPath"
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.right: selectedBadge.visible ? selectedBadge.left : parent.right
					anchors.rightMargin: Style.marginM
					text: selectedPathCell.row ? selectedPathCell.row.fullPath : ""
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
					elide: Text.ElideLeft
				}

				StatusBadge {
					id: selectedBadge
					objectName: "PermissionBadge"
					anchors.verticalCenter: parent.verticalCenter
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					visible: selectedPathCell.row !== null && selectedPathCell.row.badge !== ""
					text: selectedPathCell.row ? selectedPathCell.row.badge : ""
					textColor: Style.inactiveTextColor
				}
			}
		}
	}

	// --- Tree (flat grouped) ---
	// The scrollbar sets its own visibility from the content height, so the tree hides it through this parent.
	Item {
		id: treeArea
		anchors.top: root.showControlPanel ? headerArea.bottom : parent.top
		anchors.topMargin: root.showControlPanel ? root.treeTopMargin : 0
		anchors.bottom: details.visible ? details.top : parent.bottom
		anchors.bottomMargin: details.visible ? Style.marginM : root.treeBottomMargin
		anchors.left: parent.left
		anchors.right: parent.right
		visible: !root.selectedOnly

		Rectangle {
			id: treeHeader
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: listView.right
			height: Style.tableHeaderHeight
			color: Style.baseColor

			Text {
				anchors.fill: parent
				anchors.leftMargin: Style.marginM
				anchors.rightMargin: Style.marginM
				text: qsTr("Permission")
				color: Style.titleColor
				font.family: Style.fontFamilyBold
				font.pixelSize: Style.fontSizeS
				font.bold: true
				verticalAlignment: Text.AlignVCenter
				elide: Text.ElideRight
			}
		}

		Rectangle {
			id: treeHeaderDivider
			anchors.top: treeHeader.bottom
			anchors.left: treeHeader.left
			anchors.right: treeHeader.right
			height: 1
			color: Style.borderColor
		}

		CustomScrollbar {
			id: scrollbar
			z: parent.z + 1
			anchors.right: parent.right
			anchors.top: treeHeaderDivider.bottom
			anchors.bottom: parent.bottom
			secondSize: Style.marginM
			targetItem: listView
		}

		ListView {
			id: listView
			anchors.top: treeHeaderDivider.bottom
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: scrollbar.left
			anchors.rightMargin: root.treeToScrollbarSpacing
			clip: true
			cacheBuffer: root.rowHeight * 4
			boundsBehavior: Flickable.StopAtBounds
			focus: true
			model: visibleModel

			Keys.onPressed: {
				if (root.__handleKey(event))
					event.accepted = true
			}

			delegate: Item {
				id: rowItem
				objectName: "TreeRow_" + index

				property int row: model.row
				property bool isGroup: root.__isGroup[rowItem.row] === true
				property int checkState: root.__revision >= 0 ? root.__rowState[rowItem.row] : Qt.Unchecked
				property bool checkActive: root.__revision >= 0 ? !root.__rowLocked[rowItem.row] && !root.readOnly : false
				property string badge: root.__revision >= 0 ? root.__rowBadge[rowItem.row] : ""
				property string hint: root.__revision >= 0 ? root.__rowHint[rowItem.row] : ""
				property bool expanded: root.__revision >= 0 ? root.__expanded[rowItem.row] === true : false
				property bool selected: root.__currentRow === rowItem.row
				property bool hovered: false

				width: listView.width
				height: root.rowHeight

				Rectangle {
					anchors.fill: parent
					color: rowItem.selected ? Style.selectedColor
						: rowItem.hovered ? Style.hover
						: root.__alternating && index % 2 === 1 ? root.__alternateColor
						: "transparent"
				}

				MouseArea {
					id: rowMouseArea
					objectName: "MouseArea"
					anchors.fill: parent
					hoverEnabled: true
					acceptedButtons: Qt.LeftButton

					onContainsMouseChanged: {
						rowItem.hovered = containsMouse
						if (!containsMouse)
							rowTooltip.hide()
					}
					onPositionChanged: root.__showRowTooltip(nameText, rowItem.hint, rowMouseArea, mouse.x, mouse.y)
					onClicked: {
						listView.forceActiveFocus()
						root.__currentRow = rowItem.row
						root.__updateDetails()
					}
					onDoubleClicked: root.__setExpanded(rowItem.row, !rowItem.expanded)
				}

				Item {
					id: arrowItem
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.leftMargin: (rowItem.isGroup ? 0 : root.indentation) + Style.marginS
					width: 16
					height: 16

					// Hover as on ButtonDecorator.
					Rectangle {
						anchors.fill: parent
						anchors.margins: -2
						radius: Style.radiusS
						color: arrowMouse.containsMouse ? Style.buttonHoverColor : "transparent"
						visible: rowItem.isGroup
					}

					Image {
						anchors.fill: parent
						sourceSize.width: width
						sourceSize.height: height
						visible: rowItem.isGroup
						source: rowItem.isGroup ? (rowItem.expanded ? root.__downIcon : root.__rightIcon) : ""
					}

					MouseArea {
						id: arrowMouse
						anchors.fill: parent
						enabled: rowItem.isGroup
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: root.__setExpanded(rowItem.row, !rowItem.expanded)
					}
				}

				// Drawn like CheckBox with its decorator, without a control per row.
				Rectangle {
					id: checkRect
					objectName: "RowCheckBox"
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: arrowItem.right
					anchors.leftMargin: Style.spacingS
					width: Style.itemSizeS
					height: Style.itemSizeS
					radius: Style.radiusS
					color: checkMouse.containsMouse ? Style.buttonHoverColor : Style.inputBackgroundColor
					border.width: 1
					border.color: !rowItem.checkActive ? Style.inactiveTextColor
						: checkMouse.containsMouse ? Style.imaginToolsAccentColor
						: Style.inputBorderColor

					Image {
						anchors.centerIn: parent
						width: parent.height - 1
						height: width
						sourceSize.width: width
						sourceSize.height: height
						visible: rowItem.checkState === Qt.Checked
						source: rowItem.checkState !== Qt.Checked ? "" : rowItem.checkActive ? root.__okIcon : root.__okInactiveIcon
					}

					Rectangle {
						anchors.centerIn: parent
						width: parent.height - 4
						height: width
						radius: parent.radius
						color: Style.textColor
						visible: rowItem.checkState === Qt.PartiallyChecked
					}

					MouseArea {
						id: checkMouse
						objectName: "MouseArea"
						anchors.fill: parent
						enabled: rowItem.checkActive
						visible: rowItem.checkActive
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: root.__toggleRow(rowItem.row)
					}
				}

				Image {
					id: hintImage
					objectName: "HintIcon"
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: checkRect.right
					anchors.leftMargin: Style.spacingS
					width: visible ? Style.iconSizeS : 0
					height: Style.iconSizeS
					sourceSize.width: Style.iconSizeS
					sourceSize.height: Style.iconSizeS
					visible: rowItem.hint !== ""
					source: rowItem.hint !== "" ? root.__lockIcon : ""
				}

				Text {
					id: nameText
					objectName: "PermissionName"
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: hintImage.visible ? hintImage.right : checkRect.right
					anchors.leftMargin: Style.spacingS
					anchors.right: rowItem.isGroup ? counterText.left : badgeItem.visible ? badgeItem.left : parent.right
					anchors.rightMargin: Style.marginM
					text: root.__names[rowItem.row]
					font.family: Style.fontFamily
					font.pixelSize: Style.fontSizeM
					font.bold: rowItem.isGroup
					color: rowItem.selected ? Style.textSelectedColor : Style.textColor
					elide: Text.ElideMiddle
				}

				Text {
					id: counterText
					objectName: "PermissionCounter"
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					visible: rowItem.isGroup
					text: rowItem.isGroup ? rowItem.badge : ""
					font.family: Style.fontFamily
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				StatusBadge {
					id: badgeItem
					objectName: "PermissionBadge"
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					visible: !rowItem.isGroup && rowItem.badge !== ""
					text: rowItem.isGroup ? "" : rowItem.badge
					textColor: Style.inactiveTextColor
				}
			}
		}

		Text {
			anchors.top: treeHeaderDivider.bottom
			anchors.topMargin: Style.marginXL
			anchors.left: parent.left
			anchors.leftMargin: Style.marginL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginL
			visible: visibleModel.count === 0
			text: root.__filterText !== "" ? qsTr("No permissions match the filter.") : root.emptyText
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			font.pixelSize: Style.fontSizeM
			color: Style.inactiveTextColor
		}
	}

	PermissionDetailsPanel {
		id: details
		objectName: "PermissionDetails"
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.bottomMargin: root.treeBottomMargin
		visible: root.showDetails && root.__detailsFit && (root.selectedOnly ? root.__selectedRows.length > 0 : visibleModel.count > 0)
		placeholderText: qsTr("Select a permission to see its path, source and requirements")
	}

	CustomTooltip {
		id: rowTooltip
		z: 10
		componentWidth: Style.sizeHintS
	}
}
