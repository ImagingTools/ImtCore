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
 *   Sensor Management           (group header, not checkable)
 *   ├── View Sensor History
 *   ├── Edit Sensor
 *   ├── Edit Sensor / Add Sensor
 *   ├── Edit Sensor / Change Sensor
 *   └── Edit Sensor / Change Sensor / Change Production Status
 *
 * Top-level nodes become group headers (depth 0, one level of tree nesting).
 * All descendants are flattened into children of that group with path-style
 * display names joined by " / ".
 *
 * Properties:
 *   permissionsTree   — source tree data (array/ListModel of permission nodes
 *                       with m_id, m_name, m_description, m_children)
 *   selectedIds       — array of currently selected permission IDs
 *   readOnly          — disables checkbox interaction
 *
 * Functions:
 *   getCheckedIds()   — returns sorted array of checked leaf IDs
 *   checkAll()        — checks all items
 *   uncheckAll()      — unchecks all items
 *
 * Signals:
 *   selectionChanged() — emitted when checked items change
 */
Item {
	id: root

	property var permissionsTree: null
	property var selectedIds: []
	property var lockedPermissions: ({})
	// Checked and locked because a checked permission requires them {permissionId: [names of the requiring ones]}.
	property var requiredPermissions: ({})
	// Permissions each permission requires directly {permissionId: [permissionIds]}.
	property var __requirements: ({})
	// Ancestors of the required permissions, their check state is computed from all children {key: true}.
	property var __requiredParents: ({})
	property var __groupKeys: []
	// Row picked in the "Selected only" table.
	property string __selectedTableKey: ""
	property string __filterText: ""
	// Panel under the tree with the full name, path, description, source and requirements of the selected row.
	property bool showDetails: true
	// Shows the checked permissions, own and inherited, as a read-only table instead of the tree and its commands.
	property bool selectedOnly: false
	// Shown instead of the tree when there is nothing to show.
	property string emptyText: qsTr("No permissions available.")
	readonly property int checkedCount: root.__checkedCount
	property int __checkedCount: 0
	property var __selectedRows: []
	// Ancestors disabled because a descendant is locked {key: true}.
	property var __lockedParents: ({})
	property bool readOnly: false
	property bool showControlPanel: true
	property int treeToScrollbarSpacing: 0
	property int controlPanelTopMargin: Style.marginXL
	property int treeTopMargin: Style.marginM
	property int treeBottomMargin: Style.marginXL
	property int __selectionUpdateDepth: 0

	readonly property real preferredHeight: root.selectedOnly
		? (showControlPanel ? controlPanelTopMargin + headerArea.implicitHeight + treeTopMargin : 0)
			+ selectedTable.headerHeight + selectedTable.contentHeight + (details.visible ? details.height + Style.marginM : 0) + treeBottomMargin
		: (showControlPanel ? controlPanelTopMargin + headerArea.implicitHeight + treeTopMargin : 0)
			+ permissionsTreeView.contentHeight + (details.visible ? details.height + Style.marginM : 0) + treeBottomMargin

	signal selectionChanged()

	onSelectedOnlyChanged: root.updateSelectedRows()

	// --- Public API ---

	// Leaf node keys are the raw permission ids (see rebuildFromFlatArray /
	// __buildFlatModel below); only synthetic group wrappers use the
	// "__group__:" key prefix. Reading the raw checked keys avoids allocating
	// a createIndex() wrapper (with its half-dozen closures) for every node
	// in the tree, which is what used to make this O(nodeCount) with a heavy
	// constant factor for large permission sets.
	function getCheckedIds(excludeLocked) {
		var keys = permissionsTreeView.getCheckedKeys()
		var result = []
		for (var i = 0; i < keys.length; i++) {
			if (keys[i].indexOf("__group__:") === 0)
				continue
			if (excludeLocked && root.lockedPermissions && root.lockedPermissions[keys[i]])
				continue
			if (excludeLocked && root.requiredPermissions[keys[i]])
				continue
			result.push(keys[i])
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
		__beginSelectionUpdate()
		for (var oldId in root.lockedPermissions) {
			var oldNode = permissionsTreeView.nodeForKey(oldId)
			if (oldNode && oldNode.data) {
				oldNode.data.source = ""
				oldNode.data.badge = ""
			}
			permissionsTreeView.setNodeEnabled(oldId, true)
			permissionsTreeView.refreshNode(oldId)
		}
		for (var oldParentKey in root.__lockedParents)
			permissionsTreeView.setNodeEnabled(oldParentKey, true)

		// A parent with a locked child is locked too, or its check box would toggle the locked child.
		var lockedParents = ({})
		for (var id in lockedMap) {
			var node = permissionsTreeView.nodeForKey(id)
			if (!node)
				continue
			if (node.data) {
				node.data.source = qsTr("Inherited from:") + " " + lockedMap[id]
				node.data.badge = qsTr("Inherited")
			}
			permissionsTreeView.checkItem(id)
			permissionsTreeView.setNodeEnabled(id, false)
			// Cells re-read node data only when its tick changes.
			permissionsTreeView.refreshNode(id)
			var parentKey = node.parentKey
			while (parentKey && parentKey !== "") {
				lockedParents[parentKey] = true
				var parentNode = permissionsTreeView.nodeForKey(parentKey)
				parentKey = parentNode ? parentNode.parentKey : ""
			}
		}
		for (var parentId in lockedParents)
			permissionsTreeView.setNodeEnabled(parentId, false)

		root.lockedPermissions = lockedMap
		root.__lockedParents = lockedParents
		root.__applyRequirements()
		__endSelectionUpdate()
		root.__updateLockedParentStates()
		root.updateSelectedRows()
	}

	/**
	 * Checks and locks what the checked permissions require, transitively, and unchecks the required
	 * ones nothing requires any more. Requirements cycles end, as every permission is visited once.
	 */
	function __applyRequirements() {
		__beginSelectionUpdate()
		var previous = root.requiredPermissions
		var own = ({})
		var queue = []
		var visited = ({})
		var keys = permissionsTreeView.getCheckedKeys()
		for (var i = 0; i < keys.length; i++) {
			if (keys[i].indexOf("__group__:") === 0)
				continue
			// Required ones count only through what requires them, unless they are inherited now.
			if (previous[keys[i]] && !root.lockedPermissions[keys[i]])
				continue
			if (!root.lockedPermissions[keys[i]])
				own[keys[i]] = true
			queue.push(keys[i])
			visited[keys[i]] = true
		}

		var required = ({})
		while (queue.length > 0) {
			var key = queue.shift()
			var targets = root.__requirements[key] || []
			var sourceName = key
			for (var t = 0; t < targets.length; t++) {
				var targetId = targets[t]
				// Requirements outside this tree cannot be granted here.
				if (!permissionsTreeView.nodeForKey(targetId))
					continue
				if (!own[targetId] && !root.lockedPermissions[targetId]) {
					if (!required[targetId])
						required[targetId] = []
					if (required[targetId].indexOf(sourceName) < 0)
						required[targetId].push(sourceName)
				}
				if (!visited[targetId]) {
					visited[targetId] = true
					queue.push(targetId)
				}
			}
		}

		for (var oldId in previous) {
			if (required[oldId] || root.lockedPermissions[oldId])
				continue
			var oldNode = permissionsTreeView.nodeForKey(oldId)
			if (oldNode && oldNode.data) {
				oldNode.data.source = ""
				oldNode.data.badge = ""
			}
			permissionsTreeView.setNodeEnabled(oldId, true)
			permissionsTreeView.uncheckItem(oldId)
			permissionsTreeView.refreshNode(oldId)
		}

		var requiredParents = ({})
		for (var id in required) {
			var node = permissionsTreeView.nodeForKey(id)
			if (node.data) {
				node.data.source = qsTr("Required by:") + " " + required[id].join(", ")
				node.data.badge = qsTr("Required")
			}
			permissionsTreeView.setNodeEnabled(id, true)
			permissionsTreeView.checkItem(id)
			permissionsTreeView.setNodeEnabled(id, false)
			permissionsTreeView.refreshNode(id)
			var parentKey = node.parentKey
			while (parentKey && parentKey !== "") {
				requiredParents[parentKey] = true
				var parentNode = permissionsTreeView.nodeForKey(parentKey)
				parentKey = parentNode ? parentNode.parentKey : ""
			}
		}

		root.requiredPermissions = required
		root.__requiredParents = requiredParents
		__endSelectionUpdate()
		root.__updateLockedParentStates()
	}

	// Forgets the required permissions, e.g. before a selection is applied anew.
	function __clearRequirements() {
		for (var id in root.requiredPermissions) {
			if (root.lockedPermissions[id])
				continue
			var node = permissionsTreeView.nodeForKey(id)
			if (node && node.data) {
				node.data.source = ""
				node.data.badge = ""
			}
			permissionsTreeView.setNodeEnabled(id, true)
			permissionsTreeView.refreshNode(id)
		}
		root.requiredPermissions = ({})
		root.__requiredParents = ({})
	}

	// Locked parents are skipped by the tree's own roll-up, so their state is computed here from all children.
	function __updateLockedParentStates() {
		var parents = ({})
		for (var lockedKey in root.__lockedParents)
			parents[lockedKey] = true
		for (var requiredKey in root.__requiredParents)
			parents[requiredKey] = true

		for (var key in parents) {
			var node = permissionsTreeView.nodeForKey(key)
			if (!node)
				continue
			var allChecked = true
			var allUnchecked = true
			for (var i = 0; i < node.childrenKeys.length; i++) {
				var child = permissionsTreeView.nodeForKey(node.childrenKeys[i])
				if (!child)
					continue
				if (child.checked !== Qt.Checked)
					allChecked = false
				if (child.checked !== Qt.Unchecked)
					allUnchecked = false
			}
			permissionsTreeView.setCheckStateSilent(key, allChecked ? Qt.Checked : allUnchecked ? Qt.Unchecked : Qt.PartiallyChecked)
		}
	}

	// Rows of the "Selected only" table: every checked permission, inherited ones with their source.
	function __setTreeModel(nodes) {
		var groupKeys = []
		for (var i = 0; i < nodes.length; i++)
			groupKeys.push(nodes[i].key)
		root.__groupKeys = groupKeys
		permissionsTreeView.model = nodes
	}

	// "checked/all" next to each group name.
	function __updateGroupCounters() {
		for (var i = 0; i < root.__groupKeys.length; i++) {
			var group = permissionsTreeView.nodeForKey(root.__groupKeys[i])
			if (!group || !group.data)
				continue
			var checked = 0
			for (var k = 0; k < group.childrenKeys.length; k++) {
				var child = permissionsTreeView.nodeForKey(group.childrenKeys[k])
				if (child && child.checked === Qt.Checked)
					checked++
			}
			var counter = checked + "/" + group.childrenKeys.length
			if (group.data.badge !== counter) {
				group.data.badge = counter
				permissionsTreeView.refreshNode(root.__groupKeys[i])
			}
		}
	}

	// Fills the details panel from the selected row of the tree, or of the table in the "Selected only" mode.
	function __updateDetails() {
		var keys = root.selectedOnly ? [root.__selectedTableKey] : permissionsTreeView.selectedKeys()
		var node = keys.length > 0 && keys[0] !== "" ? permissionsTreeView.nodeForKey(keys[0]) : null
		if (!node) {
			details.path = ""
			return
		}

		var key = keys[0]
		var data = node.data || ({})
		var isGroup = node.childrenKeys.length > 0
		// Leaf keys are full paths already; a group is keyed by its feature id.
		details.path = isGroup ? "/" + (data.id || "") : (data.id || key)
		details.badge = data.badge || ""
		details.description = data.description || ""
		if (isGroup) {
			details.source = qsTr("%1 of the permissions are selected").arg(data.badge || "")
			details.requires = ""
			return
		}

		details.source = data.source ? data.source
			: node.checked === Qt.Checked ? qsTr("Selected here")
			: qsTr("Not selected")
		var requiredNames = []
		var requirements = root.__requirements[key] || []
		for (var i = 0; i < requirements.length; i++) {
			if (permissionsTreeView.nodeForKey(requirements[i]))
				requiredNames.push(requirements[i])
		}
		details.requires = requiredNames.join(", ")
	}

	function updateSelectedRows() {
		var ids = root.getCheckedIds(false)
		root.__checkedCount = ids.length
		root.__updateGroupCounters()
		// The table is refilled below, so its selection does not survive.
		if (root.selectedOnly)
			root.__selectedTableKey = ""
		root.__updateDetails()
		if (!root.selectedOnly)
			return

		var rows = []
		var filter = root.__filterText.toLowerCase()
		for (var i = 0; i < ids.length; i++) {
			if (filter !== "" && ids[i].toLowerCase().indexOf(filter) < 0)
				continue
			var node = permissionsTreeView.nodeForKey(ids[i])
			rows.push({
				id: ids[i],
				fullPath: ids[i],
				badge: node && node.data && node.data.badge ? node.data.badge : ""
			})
		}
		root.__selectedRows = rows
		selectedTable.resetSelection(true)
		selectedTable.elements = rows
	}

	function checkAll() {
		__beginSelectionUpdate()
		permissionsTreeView.checkAll()
		root.__applyRequirements()
		__endSelectionUpdate()
		root.updateSelectedRows()
		if (!root.readOnly)
			root.selectionChanged()
	}

	function uncheckAll() {
		__beginSelectionUpdate()
		permissionsTreeView.uncheckAll()
		// The tree unchecks disabled nodes as well; locked permissions stay checked.
		if (root.lockedPermissions) {
			for (var id in root.lockedPermissions)
				permissionsTreeView.setCheckStateSilent(id, Qt.Checked)
		}
		root.__clearRequirements()
		root.__applyRequirements()
		__endSelectionUpdate()
		root.updateSelectedRows()
		if (!root.readOnly)
			root.selectionChanged()
	}

	function expandAll() {
		permissionsTreeView.expandAll()
	}

	function collapseAll() {
		permissionsTreeView.collapseAll()
	}

	function applySelection(ids) {
		__beginSelectionUpdate()
		var prevFilter = permissionsTreeView.filterText
		permissionsTreeView.filterText = ""
		root.__clearRequirements()
		permissionsTreeView.uncheckAll()
		// Leaf keys are the raw permission ids, so each granted id can be
		// checked directly in O(1) instead of enumerating every node in the
		// tree (allNodes()) and testing membership with ids.includes() for
		// each one. That used to be O(totalPermissionCount * ids.length) -
		// with a large permission set and a few hundred granted ids this
		// reached millions of comparisons and froze the GUI on every load.
		for (var i = 0; i < ids.length; i++)
			permissionsTreeView.checkItem(ids[i])
		permissionsTreeView.filterText = prevFilter
		root.__applyRequirements()
		__endSelectionUpdate()
		root.updateSelectedRows()
	}

	function rebuild(treeData) {
		__beginSelectionUpdate()
		root.permissionsTree = treeData
		__buildFlatModel()
		__endSelectionUpdate()
		root.updateSelectedRows()
	}

	/**
	 * Rebuild from a C++ TreeItemModel (used in RoleView legacy path).
	 * First converts to intermediate tree nodes via TreeModelBuilder, then flattens.
	 */
	function rebuildFromTreeItemModel(treeItemModel) {
		if (!treeItemModel)
			return

		__beginSelectionUpdate()

		var treeNodes = TreeModelBuilder.fromTreeItemModel(
			treeItemModel,
			function(wrapper, index) {
				return {
					key: wrapper.data("FeatureId", ""),
					text: wrapper.data("FeatureName", ""),
					checkable: true,
					expanded: true,
					children: [],
					data: {
						id: wrapper.data("FeatureId", ""),
						name: wrapper.data("FeatureName", ""),
						description: wrapper.data("FeatureDescription", "")
					}
				}
			},
			function(wrapper, index) {
				return wrapper.childModel("ChildModel")
			}
		)

		var flatGrouped = __flattenTreeNodes(treeNodes)
		root.__setTreeModel(flatGrouped)
		__endSelectionUpdate()
		root.updateSelectedRows()
	}

	/**
	 * Rebuild from a flat array of permission objects from the server.
	 * Rebuild from an array of permission groups from the server.
	 * Each group: { groupId, groupName, entries: [{ permissionId, displayName, description }] }
	 * Each group becomes a non-checkable parent node.
	 * Each entry becomes a checkable leaf with permissionId as the key.
	 */
	function rebuildFromFlatArray(groups) {
		loading.start()
		__beginSelectionUpdate()
		root.requiredPermissions = ({})
		root.__requiredParents = ({})
		root.__requirements = ({})
		if (!groups || groups.length === 0) {
			root.__setTreeModel([])
			__endSelectionUpdate()
			root.updateSelectedRows()
			__buildSettleTimer.restart()
			return
		}

		var result = []
		var requirements = ({})
		for (var gi = 0; gi < groups.length; gi++) {
			var group = groups[gi]
			var groupChildren = []
			var entries = group.entries || []

			for (var ei = 0; ei < entries.length; ei++) {
				var entry = entries[ei]
				if (entry.permissionId && entry.requirements && entry.requirements.length > 0)
					requirements[entry.permissionId] = entry.requirements
				groupChildren.push(TreeModelBuilder.node({
					key: entry.permissionId || ("perm_" + gi + "_" + ei),
					text: entry.displayName || "",
					checkable: true,
					expanded: true,
					children: [],
					data: {
						id: entry.permissionId || "",
						name: entry.displayName || "",
						description: entry.description || ""
					}
				}))
			}

			result.push(TreeModelBuilder.node({
				// Prefixed so this synthetic group wrapper can never collide with a
				// leaf entry's key. A top-level feature without children is sent by
				// the server as a group whose single entry reuses the same
				// permissionId as the groupId; BasicTreeView keys its whole tree in
				// one flat map, so an identical key on parent and child turns the
				// node into its own child and __appendBranchIterative loops forever.
				key: "__group__:" + (group.groupId || ("group_" + gi)),
				text: group.groupName || "",
				checkable: true,
				expanded: true,
				children: groupChildren,
				data: {
					id: group.groupId || "",
					name: group.groupName || "",
					description: ""
				}
			}))
		}

		root.__requirements = requirements
		root.__setTreeModel(result)
		__endSelectionUpdate()
		root.updateSelectedRows()
		__buildSettleTimer.restart()
	}

	function __beginSelectionUpdate() {
		root.__selectionUpdateDepth = root.__selectionUpdateDepth + 1
	}

	function __endSelectionUpdate() {
		if (root.__selectionUpdateDepth > 0)
			root.__selectionUpdateDepth = root.__selectionUpdateDepth - 1
	}

	// --- Internals ---

	/**
	 * Flattens a recursive permissions tree into a single-level grouped model
	 * suitable for BasicTreeView.
	 *
	 * Each top-level permission becomes a group node (not checkable).
	 * All its descendants become flat children with path-style display names.
	 */
	function __buildFlatModel() {
		if (!root.permissionsTree)
			return

		var topNodes = __extractTopLevelNodes(root.permissionsTree)
		var flatGrouped = []

		for (var gi = 0; gi < topNodes.length; gi++) {
			var topNode = topNodes[gi]
			var groupChildren = []
			var directChildren = __getChildren(topNode)

			// Flatten all descendants with path prefix
			__collectFlattened(directChildren, "", groupChildren)

			flatGrouped.push(TreeModelBuilder.node({
				// See the identical comment in rebuildFromFlatArray(): the group
				// wrapper's key must never collide with a descendant's key, or
				// BasicTreeView's flat node map turns it into its own child and
				// hangs. Prefixed defensively even though this path only hits it
				// if the source tree itself reuses topNode.id on a descendant.
				key: "__group__:" + (topNode.id || ("group_" + gi)),
				text: topNode.name || "",
				checkable: true,
				expanded: true,
				children: groupChildren,
				data: {
					id: topNode.id || "",
					name: topNode.name || "",
					description: topNode.description || ""
				}
			}))
		}

		root.__setTreeModel(flatGrouped)
	}

	/**
	 * Recursively collect all nodes from a children list into flat array.
	 * Each node gets a path-style name: "Parent / Child / Grandchild".
	 */
	function __collectFlattened(childrenList, parentPath, result) {
		if (!childrenList)
			return

		var count = childrenList.count !== undefined ? childrenList.count : childrenList.length
		if (!count)
			return

		for (var i = 0; i < count; i++) {
			var rawItem = childrenList.count !== undefined ? childrenList.get(i) : childrenList[i]
			var item = rawItem && rawItem.item ? rawItem.item : rawItem
			if (!item)
				continue

			var itemName = item.m_name || item.name || ""
			var itemId = item.m_id || item.id || ""
			var itemDesc = item.m_description || item.description || ""
			var displayPath = parentPath ? (parentPath + " / " + itemName) : itemName

			var subChildren = __getChildren(item)
			var hasChildren = subChildren && (subChildren.count > 0 || subChildren.length > 0)

			// Always add the node itself to the flat list
			result.push(TreeModelBuilder.node({
				key: itemId || ("perm_" + result.length),
				text: displayPath,
				checkable: true,
				expanded: true,
				children: [],
				data: {
					id: itemId,
					name: displayPath,
					description: itemDesc
				}
			}))

			// Recurse into children
			if (hasChildren)
				__collectFlattened(subChildren, displayPath, result)
		}
	}

	/**
	 * Extract top-level permission nodes from the source data.
	 * Handles both ListModel and plain JS arrays.
	 */
	function __extractTopLevelNodes(source) {
		var nodes = []
		var count = source.count !== undefined ? source.count : source.length
		if (!count)
			return nodes

		for (var i = 0; i < count; i++) {
			var rawItem = source.count !== undefined ? source.get(i) : source[i]
			var item = rawItem && rawItem.item ? rawItem.item : rawItem
			if (!item)
				continue
			nodes.push({
				id: item.m_id || item.id || "",
				name: item.m_name || item.name || "",
				description: item.m_description || item.description || "",
				_source: item
			})
		}
		return nodes
	}

	/**
	 * Get children from a permission node (handles m_children / children).
	 */
	function __getChildren(item) {
		if (!item)
			return null
		var src = item._source || item
		var children = src.m_children || src.children || null
		if (!children)
			return null
		var cnt = children.count !== undefined ? children.count : children.length
		return (cnt && cnt > 0) ? children : null
	}

	/**
	 * Flatten already-converted tree nodes (from TreeModelBuilder) into
	 * single-level grouped model.
	 */
	function __flattenTreeNodes(treeNodes) {
		var flatGrouped = []
		for (var gi = 0; gi < treeNodes.length; gi++) {
			var topNode = treeNodes[gi]
			var groupChildren = []
			var directChildren = topNode.children || []

			__collectFlattenedFromNodes(directChildren, "", groupChildren)

			flatGrouped.push(TreeModelBuilder.node({
				// See the identical comment in rebuildFromFlatArray(): the group
				// wrapper's key must never collide with a descendant's key, or
				// BasicTreeView's flat node map turns it into its own child and
				// hangs.
				key: "__group__:" + (topNode.key || ("group_" + gi)),
				text: topNode.text || "",
				checkable: true,
				expanded: true,
				children: groupChildren,
				data: topNode.data || {}
			}))
		}
		return flatGrouped
	}

	/**
	 * Recursively collect nodes from an already-converted children array
	 * into a flat list with path-style names.
	 */
	function __collectFlattenedFromNodes(children, parentPath, result) {
		if (!children)
			return
		for (var i = 0; i < children.length; i++) {
			var node = children[i]
			if (!node)
				continue
			var itemName = node.text || ""
			var displayPath = parentPath ? (parentPath + " / " + itemName) : itemName
			var nodeData = node.data || {}

			result.push(TreeModelBuilder.node({
				key: node.key || ("perm_" + result.length),
				text: displayPath,
				checkable: true,
				expanded: true,
				children: [],
				data: {
					id: nodeData.id || "",
					name: displayPath,
					description: nodeData.description || ""
				}
			}))

			var subChildren = node.children || []
			if (subChildren.length > 0)
				__collectFlattenedFromNodes(subChildren, displayPath, result)
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
		Row {
			width: parent.width
			spacing: Style.marginM
			visible: !root.selectedOnly

			Item {
				width: parent.width
					- checkAllBtn.width
					- uncheckAllBtn.width
					- expandBtn.width
					- collapseBtn.width
					- parent.spacing * 4
				height: 1
			}

			Text {
				id: checkAllBtn
				// Test instrumentation: this reusable control's Text+MouseArea "buttons" had no
				// objectName at all - inner MouseArea named "MouseArea" so click()'s standard nested
				// lookup finds it (same pattern already used for ItemSelectElementView.qml's add
				// button). Inert.
				objectName: "CheckAllButton"
				anchors.verticalCenter: parent.verticalCenter
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
				anchors.verticalCenter: parent.verticalCenter
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

			Text {
				id: expandBtn
				objectName: "ExpandAllButton"
				anchors.verticalCenter: parent.verticalCenter
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
				anchors.verticalCenter: parent.verticalCenter
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

		SearchTextInput {
			id: filterInput
			width: parent.width
			placeHolderText: qsTr("Filter permissions...")
			onTextChanged: {
				permissionsTreeView.filterText = text
				root.__filterText = text
				if (root.selectedOnly)
					root.updateSelectedRows()
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
		anchors.topMargin: selectedTable.headerHeight + Style.marginXL
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Style.marginL
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
	// One column: the name with a short mark on the right ("Inherited", "Required", or "checked/all" on groups);
	// the rest of a row is in the details panel under the tree.
	Component {
		id: permissionCellComp

		Item {
			id: permissionCell

			property var cellData: parent ? parent.nodeData : null
			property bool cellSelected: parent ? parent.selected : false
			property bool isGroup: parent ? parent.hasChildren : false
			property string badge: permissionCell.cellData ? permissionCell.cellData.badge || "" : ""

			Text {
				objectName: "PermissionName"
				anchors.left: parent.left
				anchors.right: permissionCell.isGroup ? counterText.left : badgeItem.visible ? badgeItem.left : parent.right
				anchors.rightMargin: permissionCell.badge !== "" ? Style.marginM : 0
				anchors.verticalCenter: parent.verticalCenter
				text: permissionCell.cellData ? permissionCell.cellData.name || "" : ""
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				font.bold: permissionCell.isGroup
				color: permissionCell.cellSelected ? Style.textSelectedColor : Style.textColor
				elide: Text.ElideMiddle
			}

			Text {
				id: counterText
				objectName: "PermissionCounter"
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				visible: permissionCell.isGroup
				text: permissionCell.isGroup ? permissionCell.badge : ""
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			StatusBadge {
				id: badgeItem
				objectName: "PermissionBadge"
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				visible: !permissionCell.isGroup && permissionCell.badge !== ""
				text: permissionCell.isGroup ? "" : permissionCell.badge
				textColor: Style.inactiveTextColor
			}
		}
	}

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

		CustomScrollbar {
			id: scrollbar
			z: parent.z + 1
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.topMargin: permissionsTreeView.showHeader ? permissionsTreeView.headerHeight + 1 : 0
			anchors.bottom: permissionsTreeView.bottom
			secondSize: Style.marginM
			targetItem: permissionsTreeView.contentListView
		}

		BasicTreeView {
			id: permissionsTreeView
			cacheBuffer: rowHeight * 4
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: scrollbar.left
			anchors.rightMargin: root.treeToScrollbarSpacing
			showHeader: true
			tristate: true
			// Locked rows stay selectable to show their details; their check box stays disabled.
			allowDisabledSelection: true
			rowHeight: Style.tableRowHeight
			headerHeight: Style.tableHeaderHeight
			hintRole: "source"
			hintIcon: "Icons/Lock"
			columns: [
				{ id: "name", name: qsTr("Permission"), tree: true, delegate: permissionCellComp }
			]
			filterRole: ["name", "description"]

			onSelectionChanged: root.__updateDetails()

			onCheckedItemsChanged: {
				if (root.__selectionUpdateDepth === 0)
					root.__applyRequirements()
				else
					root.__updateLockedParentStates()
				if (!root.readOnly && root.__selectionUpdateDepth === 0)
					root.selectionChanged()
				root.updateSelectedRows()
			}
		}

		Text {
			anchors.top: parent.top
			anchors.topMargin: permissionsTreeView.showHeader ? permissionsTreeView.headerHeight + Style.marginXL : Style.marginXL
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: Style.marginL
			visible: permissionsTreeView.visibleCount === 0 && !loading.visible
			text: permissionsTreeView.filterText !== "" ? qsTr("No permissions match the filter.") : root.emptyText
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
		visible: root.showDetails && (root.selectedOnly ? root.__selectedRows.length > 0 : permissionsTreeView.visibleCount > 0)
		placeholderText: qsTr("Select a permission to see its path, source and requirements")
	}

	// Rebuilding + settling the tree's layout (see GroupElementView's own
	// deferred relayout) spans a few frames; cover it so the user sees a
	// spinner instead of the tree reflowing mid-render.
	Timer {
		id: __buildSettleTimer
		interval: 80
		repeat: false
		onTriggered: loading.stop()
	}

	Loading {
		id: loading
		anchors.fill: parent
		visible: false
	}
}
