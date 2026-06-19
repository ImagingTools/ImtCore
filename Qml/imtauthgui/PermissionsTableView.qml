// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
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
	property bool readOnly: false
	property bool showControlPanel: true
	property int treeToScrollbarSpacing: 0
	property int controlPanelTopMargin: Style.marginXL
	property int treeTopMargin: Style.marginM
	property int treeBottomMargin: Style.marginXL
	property int __selectionUpdateDepth: 0

	readonly property real preferredHeight:
		(showControlPanel ? controlPanelTopMargin + headerArea.implicitHeight + treeTopMargin : 0)
		+ permissionsTreeView.contentHeight
		+ treeBottomMargin

	signal selectionChanged()

	// --- Public API ---

	function getCheckedIds() {
		var result = []
		var checkedNodes = permissionsTreeView.getCheckedNodes()
		for (var i = 0; i < checkedNodes.length; i++) {
			var node = checkedNodes[i]
			var children = node.children || []
			if (children.length === 0) {
				var nodeData = node.data || {}
				if (nodeData.id)
					result.push(nodeData.id)
			}
		}
		result.sort()
		return result
	}

	function checkAll() {
		__beginSelectionUpdate()
		permissionsTreeView.checkAll()
		__endSelectionUpdate()
	}

	function uncheckAll() {
		__beginSelectionUpdate()
		permissionsTreeView.uncheckAll()
		__endSelectionUpdate()
	}

	function expandAll() {
		permissionsTreeView.expandAll()
	}

	function collapseAll() {
		permissionsTreeView.collapseAll()
	}

	function applySelection(ids) {
		__beginSelectionUpdate()
		permissionsTreeView.uncheckAll()
		var allNodesList = permissionsTreeView.allNodes()
		for (var i = 0; i < allNodesList.length; i++) {
			var nodeObj = allNodesList[i]
			var nodeChildren = nodeObj.children || []
			if (nodeChildren.length === 0) {
				var nodeData = nodeObj.data || {}
				if (ids.includes(nodeData.id))
					permissionsTreeView.checkItem(nodeObj.key)
			}
		}
		__endSelectionUpdate()
	}

	function rebuild(treeData) {
		__beginSelectionUpdate()
		root.permissionsTree = treeData
		__buildFlatModel()
		__endSelectionUpdate()
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
		permissionsTreeView.model = flatGrouped
		__endSelectionUpdate()
	}

	/**
	 * Rebuild from a flat array of permission objects from the server.
	 * Rebuild from an array of permission groups from the server.
	 * Each group: { groupId, groupName, entries: [{ permissionId, displayName, description }] }
	 * Each group becomes a non-checkable parent node.
	 * Each entry becomes a checkable leaf with permissionId as the key.
	 */
	function rebuildFromFlatArray(groups) {
		__beginSelectionUpdate()
		if (!groups || groups.length === 0) {
			permissionsTreeView.model = []
			__endSelectionUpdate()
			return
		}

		var result = []
		for (var gi = 0; gi < groups.length; gi++) {
			var group = groups[gi]
			var groupChildren = []
			var entries = group.entries || []

			for (var ei = 0; ei < entries.length; ei++) {
				var entry = entries[ei]
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
				key: group.groupId || ("group_" + gi),
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

		permissionsTreeView.model = result
		__endSelectionUpdate()
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
				key: topNode.id || ("group_" + gi),
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

		permissionsTreeView.model = flatGrouped
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
				key: topNode.key || ("group_" + gi),
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
		height: root.showControlPanel ? implicitHeight : 0
		anchors.top: parent.top
		anchors.topMargin: root.controlPanelTopMargin
		anchors.left: parent.left
		anchors.right: parent.right
		spacing: Style.marginM

		Row {
			width: parent.width
			spacing: Style.marginM

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
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Check All")
				font.pixelSize: Style.fontSizeM
				color: root.readOnly ? Style.inactiveTextColor : Style.linkColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
					enabled: !root.readOnly
					onClicked: root.checkAll()
				}
			}

			Text {
				id: uncheckAllBtn
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Uncheck All")
				font.pixelSize: Style.fontSizeM
				color: root.readOnly ? Style.inactiveTextColor : Style.linkColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
					enabled: !root.readOnly
					onClicked: root.uncheckAll()
				}
			}

			Text {
				id: expandBtn
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Expand All")
				font.pixelSize: Style.fontSizeM
				color: Style.linkColor

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: root.expandAll()
				}
			}

			Text {
				id: collapseBtn
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Collapse All")
				font.pixelSize: Style.fontSizeM
				color: Style.linkColor

				MouseArea {
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
			onTextChanged: permissionsTreeView.filterText = text
		}
	}

	// --- Scrollbar ---
	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: permissionsTreeView.top
		anchors.topMargin: permissionsTreeView.showHeader ? permissionsTreeView.headerHeight + 1 : 0
		anchors.bottom: permissionsTreeView.bottom
		secondSize: Style.marginM
		targetItem: permissionsTreeView.contentListView
	}

	// --- Tree (flat grouped) ---
	BasicTreeView {
		id: permissionsTreeView
		anchors.top: root.showControlPanel ? headerArea.bottom : parent.top
		anchors.topMargin: root.showControlPanel ? root.treeTopMargin : 0
		anchors.bottom: parent.bottom
		anchors.bottomMargin: root.treeBottomMargin
		anchors.left: parent.left
		anchors.right: scrollbar.left
		anchors.rightMargin: root.treeToScrollbarSpacing
		showHeader: true
		tristate: true
		columns: [
			{ id: "name", name: qsTr("Permission"), tree: true },
			{ id: "description", name: qsTr("Description"), tree: false }
		]
		filterRole: ["name", "description"]

		onCheckedItemsChanged: {
			if (!root.readOnly && root.__selectionUpdateDepth === 0)
				root.selectionChanged()
		}
	}
}
