import QtQuick 2.12
import Qt5Compat.GraphicalEffects
import QtGraphicalEffects 1.0
import Acf 1.0
import imtcontrols 1.0

// Explorer-style editor for normalized TreeModelBuilder nodes. Navigation and
// selection stay inside the view; model mutations are requested through signals.
//
// Left pane, top to bottom:
//   location bar - back button and the full breadcrumb path of the level shown
//   command row  - the level's commands on the left, a compact search on the right
//   column head  - titles, drawn outside the table so the table needs no rules
//   table        - borderless rows on Style.baseColor, banded instead of ruled
// A draggable split separates it from the optional side panel on the right.
//
// Rows are read-only by default. Editing is an explicit mode entered per row -
// through the row's pencil, the Edit command or F2 - so an ordinary click can
// never be swallowed by an input field.
FocusScope {
	id: root

	property var model: []
	property var navigationStack: []
	property var currentEntries: []
	// Derived, never assigned: one ticked row is "the selection", anything else
	// has no single subject for the editor or the side panel to describe.
	property var selectedNode: checkedNodes.length === 1 ? checkedNodes[0] : null
	property var editingNode: null
	// Which cell a row opens on, and a counter the open row watches: bumping it
	// is how "apply your drafts" is asked for. Reaching into the delegate through
	// ListView.itemAtIndex only worked when the delegate happened to exist and
	// the method happened to be there - which is why Done used to lose the edit.
	property int editColumn: 0
	property int commitRequest: 0
	property var movingNode: null
	property var renamingNode: null
	property string filterText: ""
	property string rootTitle: qsTr("Root")
	property string nameColumnTitle: qsTr("Name")
	property string secondaryColumnTitle: ""
	property string createButtonText: qsTr("New item")
	property string searchPlaceholder: qsTr("Search in %1").arg(root.currentLevelTitle())
	property string emptyText: qsTr("This folder is empty")
	property string filteredEmptyText: qsTr("No matching items")
	property string emptyDescription: qsTr("Create an item to start this level")
	// Short warning shown next to the path, e.g. "2 incomplete", with the list
	// behind it: hovering the chip spells out which rows are at fault.
	property string levelStatusText: ""
	property var levelStatusDetails: []
	// One-line coaching under the commands, per state of the table.
	property string idleHintText: qsTr("Select a row to work with it")
	property string selectedHintText: qsTr("Press Edit or F2 to change this row")
	property bool editable: true
	// Turns drill-down off entirely: no back button, no breadcrumb.
	// Used by flat lists that still want the table chrome (see ProductView).
	property bool navigationEnabled: true
	property bool createVisible: true
	// Off when the level cannot take new children - e.g. its owner is still blank.
	property bool createEnabled: true
	property bool removeVisible: true
	property bool renameVisible: true
	property bool moveVisible: true
	// Row editing: the Edit command and the per-row pencil. Off for tables whose
	// rows are references to data owned somewhere else.
	property bool editVisible: true
	// Ticking rows builds a set the level's commands act on; the highlighted
	// row stays a separate, single thing, because the panel on the right and
	// the row editor both need exactly one subject.
	property bool multiSelectEnabled: true
	property var checkedNodes: []
	property bool rowIconVisible: true
	property alias listView: entriesView
	property var textProvider: null
	property var descriptionProvider: null
	property var secondaryTextProvider: null
	property var iconProvider: null
	property var containerProvider: null
	property var searchableTextProvider: null
	property string defaultIconSource: "Icons/FeaturePackage"
	property bool componentReady: false
	// When set, replaces the header's default Name/Secondary titles and each
	// row's default Name+Description column with fully custom content (e.g. an
	// inline-editable multi-column table row). The row component's root item
	// receives "node" (the row's tree node, via the Loader) and may expose an
	// optional function focusColumn(index), used to open a specific cell when
	// the row enters edit mode.
	property Component headerContentComponent: null
	property Component rowContentComponent: null
	// Detail pane docked to the right of the table behind a draggable split.
	// It stays on screen with nothing selected - a panel that comes and goes
	// makes the whole page jump - and shows its own placeholder instead.
	property Component sidePanelComponent: null
	property bool sidePanelVisible: true
	property real sidePanelWidth: Style.sizeHintS
	property real sidePanelMinWidth: Style.sizeHintXXS
	property real mainPaneMinWidth: Style.sizeHintXS
	// The panel folds away rather than squeezing the table into nothing.
	property bool sidePanelShown: root.sidePanelComponent !== null && root.sidePanelVisible
		&& root.width >= root.sidePanelMinWidth + root.mainPaneMinWidth + Style.marginXL
	property real sidePanelActualWidth: Math.max(root.sidePanelMinWidth,
		Math.min(root.sidePanelWidth, root.width - root.mainPaneMinWidth))

	property real rowHeight: Style.controlHeightL + Style.marginS
	property bool rowEditingEnabled: root.editable && root.editVisible && root.rowContentComponent !== null
	// Horizontal insets shared by the column header and every row, so custom
	// header/row content stays column-aligned.
	property real rowLeftInset: (root.multiSelectEnabled ? Style.itemSizeS + Style.marginM : 0)
		+ (root.rowIconVisible ? Style.iconSizeS + Style.marginM : 0) + Style.marginL
	// Two slots when rows can be edited - the tick and the cross - reserved
	// at all times so the columns never shift as a row opens.
	property real rowRightInset: root.rowEditingEnabled ? 2 * Style.iconSizeM + 3 * Style.marginM : Style.marginL

	signal createRequested(var parentNode)
	signal removeNodesRequested(var nodes, var parentNode)
	signal renameRequested(var node, string newName)
	signal moveRequested(var node, var oldParentNode, var newParentNode)
	signal nodeActivated(var node)

	onModelChanged: {
		if (componentReady)
			synchronizeModel()
	}

	Component.onCompleted: {
		componentReady = true
		synchronizeModel()
	}

	function nodeText(node) {
		if (textProvider)
			return textProvider(node)
		return node && node.text ? node.text : qsTr("Untitled item")
	}

	function nodeDescription(node) {
		if (descriptionProvider)
			return descriptionProvider(node)
		return node && node.description ? node.description : ""
	}

	function nodeSecondaryText(node) {
		if (secondaryTextProvider)
			return secondaryTextProvider(node)
		return node && node.secondaryText ? node.secondaryText : ""
	}

	function nodeIcon(node) {
		if (iconProvider)
			return iconProvider(node)
		return defaultIconSource
	}

	function isContainer(node) {
		if (containerProvider)
			return containerProvider(node)
		return node && node.children !== undefined
	}

	function nodeSearchableText(node) {
		if (searchableTextProvider)
			return searchableTextProvider(node)
		return nodeText(node) + " " + nodeDescription(node) + " " + nodeSecondaryText(node)
	}

	// --- ticked rows ------------------------------------------------------ //

	function isChecked(node) {
		return checkedNodes.indexOf(node) >= 0
	}

	function toggleChecked(node) {
		if (!multiSelectEnabled || !editable || !node)
			return
		let nodes = checkedNodes.slice()
		let index = nodes.indexOf(node)
		if (index >= 0)
			nodes.splice(index, 1)
		else
			nodes.push(node)
		checkedNodes = nodes
	}

	function checkAll(value) {
		if (!multiSelectEnabled || !editable)
			return
		checkedNodes = value ? currentEntries.slice() : []
	}

	function allChecked() {
		return currentEntries.length > 0 && checkedNodes.length === currentEntries.length
	}

	// What a command should act on: the ticked rows if there are any, and the
	// highlighted row otherwise.
	function commandTargets() {
		if (checkedNodes.length > 0)
			return checkedNodes
		if (selectedNode)
			return [selectedNode]
		return []
	}

	// Commands that only make sense on one row at a time.
	function singleTargetOnly() {
		return checkedNodes.length < 2
	}

	function currentParentNode() {
		if (navigationStack.length === 0)
			return null
		return navigationStack[navigationStack.length - 1]
	}

	function currentSourceEntries() {
		let parentNode = currentParentNode()
		if (!parentNode)
			return model || []
		return parentNode.children || []
	}

	// Name of the level being listed: the root title, or the node navigated into.
	function currentLevelTitle() {
		let parentNode = currentParentNode()
		return parentNode ? nodeText(parentNode) : rootTitle
	}

	// Breadcrumb segment 0 is the root title, segment i the i-th navigated node.
	function pathSegmentText(index) {
		if (index <= 0)
			return rootTitle
		let node = navigationStack[index - 1]
		return node ? nodeText(node) : ""
	}

	function rebuildCurrentEntries() {
		let source = currentSourceEntries()
		let entries = []
		let filter = filterText.toLowerCase()
		for (let i = 0; i < source.length; ++i) {
			let node = source[i]
			if (filter === "" || nodeSearchableText(node).toLowerCase().indexOf(filter) >= 0)
				entries.push(node)
		}
		currentEntries = entries
		// Ticks only ever apply to rows that are on screen, so anything the
		// filter or a model change took away stops being a command target.
		// Filtering the set is all that is needed - the selection is derived
		// from it, so dropping a row drops its selection with it.
		let survivors = []
		for (let j = 0; j < checkedNodes.length; ++j) {
			if (currentEntries.indexOf(checkedNodes[j]) >= 0)
				survivors.push(checkedNodes[j])
		}
		if (survivors.length !== checkedNodes.length)
			checkedNodes = survivors
	}

	// Edit mode is never closed behind the user's back: only Done, Cancel,
	// Enter, Escape, picking another row or leaving the level end it. Reacting
	// to focus alone was closing rows on things as innocent as a tooltip.

	function synchronizeModel() {
		let pathKeys = []
		for (let i = 0; i < navigationStack.length; ++i)
			pathKeys.push(navigationStack[i].key)
		let selectedKey = selectedNode ? selectedNode.key : ""
		let source = model || []
		let nextStack = []
		for (let pathIndex = 0; pathIndex < pathKeys.length; ++pathIndex) {
			let match = findNodeByKey(source, pathKeys[pathIndex])
			if (!match)
				break
			nextStack.push(match)
			source = match.children || []
		}
		navigationStack = nextStack
		rebuildCurrentEntries()
		selectNode(selectedKey !== "" ? findNodeByKey(currentEntries, selectedKey) : null)
		if (editingNode && currentEntries.indexOf(editingNode) < 0)
			editingNode = null
	}

	function findNodeByKey(nodes, key) {
		if (!nodes)
			return null
		for (let i = 0; i < nodes.length; ++i) {
			if (nodes[i].key === key)
				return nodes[i]
		}
		return null
	}

	function findParent(node, nodes, parentNode) {
		if (!node || !nodes)
			return null
		for (let i = 0; i < nodes.length; ++i) {
			if (nodes[i] === node || nodes[i].key === node.key)
				return parentNode
			let found = findParent(node, nodes[i].children || [], nodes[i])
			if (found)
				return found
		}
		return null
	}

	function containsNode(parentNode, candidateNode) {
		if (!parentNode || !candidateNode)
			return false
		if (parentNode === candidateNode || parentNode.key === candidateNode.key)
			return true
		let children = parentNode.children || []
		for (let i = 0; i < children.length; ++i) {
			if (containsNode(children[i], candidateNode))
				return true
		}
		return false
	}

	// Selecting is ticking exactly one row. Rows are picked through their
	// checkbox only - a click on the row itself never changes what is selected,
	// which is what keeps "the commands are lit but nothing is highlighted"
	// impossible: both read the same set.
	function selectNode(node) {
		// Moving off a row that is open reads as "I am done with it", so the
		// pending edit is applied rather than quietly dropped.
		if (editingNode && editingNode !== node)
			commitEditRow()
		// A node that is not on this level cannot be shown as selected, so it
		// is not selected at all.
		if (node && currentEntries.indexOf(node) < 0)
			node = null
		checkedNodes = node ? [node] : []
		entriesView.currentIndex = node ? currentEntries.indexOf(node) : -1
	}

	function clearSelection() {
		commitEditRow()
		checkedNodes = []
		entriesView.currentIndex = -1
	}

	function clearFilter() {
		filterText = ""
		searchField.text = ""
		rebuildCurrentEntries()
	}

	function navigateInto(node) {
		if (!navigationEnabled || !node || !isContainer(node))
			return
		let stack = navigationStack.slice()
		stack.push(node)
		navigationStack = stack
		filterText = ""
		searchField.text = ""
		editingNode = null
		checkedNodes = []
		selectNode(null)
		rebuildCurrentEntries()
		nodeActivated(node)
	}

	function navigateUp() {
		if (navigationStack.length === 0)
			return
		let stack = navigationStack.slice()
		stack.pop()
		navigationStack = stack
		filterText = ""
		searchField.text = ""
		editingNode = null
		checkedNodes = []
		rebuildCurrentEntries()
		// Nothing is selected on the level we land on: leaving the folder we
		// came out of selected lit up Edit/Move/Remove with no visible row.
		selectNode(null)
	}

	// Breadcrumb jump: depth 0 is the root level, depth i the level below the
	// i-th navigated node. Deeper-or-equal targets are the current level already.
	function navigateToDepth(depth) {
		if (depth >= navigationStack.length)
			return
		navigationStack = navigationStack.slice(0, depth)
		filterText = ""
		searchField.text = ""
		editingNode = null
		checkedNodes = []
		selectNode(null)
		rebuildCurrentEntries()
	}

	// --- row editing ------------------------------------------------------ //

	function beginEditRow(node, column) {
		if (!rowEditingEnabled || !node)
			return
		// selectNode() first: it commits whatever else was open, and would take
		// this row down with it if editingNode were already pointing here.
		selectNode(node)
		editColumn = column
		editingNode = node
		let idx = currentEntries.indexOf(node)
		if (idx >= 0)
			entriesView.positionViewAtIndex(idx, ListView.Contain)
	}

	// Guards against re-entering through selectNode(), which commits on its way
	// out and would otherwise be reachable from inside a commit.
	property bool committingRow: false

	// The counter is bumped while the row is still open, because a cell only
	// hands back its draft as long as it is in edit mode.
	function commitEditRow() {
		if (committingRow || !editingNode)
			return
		committingRow = true
		commitRequest = commitRequest + 1
		editingNode = null
		committingRow = false
	}

	function cancelEditRow() {
		editingNode = null
	}

	function endEditRow() {
		commitEditRow()
	}

	function toggleEditRow(node) {
		if (editingNode === node)
			commitEditRow()
		else
			beginEditRow(node, 0)
	}

	// Carries edit mode to the neighbouring row when Tab or Shift+Tab runs past
	// the first or last cell, so a whole column can be filled in without a mouse.
	function moveEditRow(node, direction, column) {
		let idx = currentEntries.indexOf(node)
		if (idx < 0)
			return
		let nextIdx = idx + direction
		if (nextIdx < 0 || nextIdx >= currentEntries.length) {
			endEditRow()
			return
		}
		let nextNode = currentEntries[nextIdx]
		selectNode(nextNode)
		editColumn = column
		editingNode = nextNode
		entriesView.positionViewAtIndex(nextIdx, ListView.Contain)
	}


	function beginRename() {
		if (!editable || !selectedNode)
			return
		if (rowContentComponent !== null) {
			beginEditRow(selectedNode, 0)
			return
		}
		renamingNode = selectedNode
	}

	function finishRename(node, text) {
		if (!node || renamingNode !== node)
			return
		renamingNode = null
		if (text !== "" && text !== nodeText(node))
			renameRequested(node, text)
	}

	function beginMove() {
		if (!editable || !selectedNode)
			return
		movingNode = selectedNode
	}

	function canMoveHere() {
		let target = currentParentNode()
		if (!movingNode)
			return false
		if (target === movingNode)
			return false
		return !containsNode(movingNode, target)
	}

	function finishMove() {
		if (!canMoveHere())
			return
		let node = movingNode
		let oldParent = findParent(node, model, null)
		let newParent = currentParentNode()
		movingNode = null
		moveRequested(node, oldParent, newParent)
	}

	function cancelMove() {
		movingNode = null
	}

	// Moves the current row selection (used for the arrow, Home/End and page
	// keys). "absolute" jumps to a row number instead of stepping.
	function selectRelative(direction) {
		if (editingNode)
			return
		let idx = selectedNode ? currentEntries.indexOf(selectedNode) : -1
		let nextIdx = idx + direction
		if (idx < 0)
			nextIdx = direction > 0 ? 0 : currentEntries.length - 1
		// Clamped rather than ignored, so a page jump near either end lands on
		// the first or last row instead of doing nothing.
		if (nextIdx < 0)
			nextIdx = 0
		if (nextIdx >= currentEntries.length)
			nextIdx = currentEntries.length - 1
		selectAt(nextIdx)
	}

	function selectAt(index) {
		if (editingNode || index < 0 || index >= currentEntries.length)
			return
		selectNode(currentEntries[index])
		entriesView.positionViewAtIndex(index, ListView.Contain)
	}

	// How many rows fit, so Page Up and Page Down move by a screenful.
	function pageStep() {
		return Math.max(1, Math.floor(entriesView.height / root.rowHeight) - 1)
	}

	// Opens the selected row - the keyboard and toolbar equivalent of the
	// double click, which is not something a user can be expected to guess.
	function openSelected() {
		if (selectedNode)
			navigateInto(selectedNode)
	}

	function canOpenSelected() {
		return navigationEnabled && selectedNode !== null && isContainer(selectedNode)
	}

	function removeSelection() {
		let targets = commandTargets()
		if (!editable || targets.length === 0)
			return
		removeNodesRequested(targets, currentParentNode())
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_Escape) {
			if (editingNode)
				cancelEditRow()
			else
				clearSelection()
			event.accepted = true
		}
		else if (editingNode !== null) {
			// Every other shortcut here would move or destroy the row being
			// edited, so while it is open they are simply not shortcuts.
			return
		}
		else if (event.key === Qt.Key_Backspace || event.key === Qt.Key_Left) {
			navigateUp()
			event.accepted = true
		}
		else if (event.key === Qt.Key_F2) {
			beginRename()
			event.accepted = true
		}
		else if (event.key === Qt.Key_Delete) {
			removeSelection()
			event.accepted = true
		}
		else if (event.key === Qt.Key_Space && selectedNode) {
			toggleChecked(selectedNode)
			event.accepted = true
		}
		else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter
			|| event.key === Qt.Key_Right) {
			openSelected()
			event.accepted = true
		}
		else if (event.key === Qt.Key_Up) {
			selectRelative(-1)
			event.accepted = true
		}
		else if (event.key === Qt.Key_Down) {
			selectRelative(1)
			event.accepted = true
		}
		else if (event.key === Qt.Key_Home) {
			selectAt(0)
			event.accepted = true
		}
		else if (event.key === Qt.Key_End) {
			selectAt(currentEntries.length - 1)
			event.accepted = true
		}
		else if (event.key === Qt.Key_PageUp) {
			selectRelative(-pageStep())
			event.accepted = true
		}
		else if (event.key === Qt.Key_PageDown) {
			selectRelative(pageStep())
			event.accepted = true
		}
		else if (event.key === Qt.Key_A && (event.modifiers & Qt.ControlModifier)) {
			checkAll(!allChecked())
			event.accepted = true
		}
	}

	Item {
		id: mainPane
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: splitHandle.left
		anchors.bottom: parent.bottom

		Item {
			id: locationBar
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: Style.controlHeightL

			ToolButton {
				id: upButton
				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				width: Style.controlHeightM
				height: Style.controlHeightM
				visible: root.navigationEnabled && root.navigationStack.length > 0
				tooltipText: qsTr("Back to the parent level")
				iconSource: "qrc:/" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
				onClicked: root.navigateUp()
			}

			// The whole path is drawn at the same weight as the level you are on:
			// ancestors are the clickable, quieter part of one continuous title.
			Item {
				id: breadcrumbArea
				anchors.left: upButton.visible ? upButton.right : parent.left
				anchors.leftMargin: upButton.visible ? Style.marginS : 0
				anchors.right: statusChip.visible ? statusChip.left : countText.visible ? countText.left : parent.right
				anchors.rightMargin: statusChip.visible || countText.visible ? Style.marginM : 0
				anchors.verticalCenter: parent.verticalCenter
				height: parent.height
				clip: true

				Row {
					id: breadcrumbRow
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.spacingS

					Repeater {
						model: root.navigationEnabled ? root.navigationStack.length + 1 : 1

						delegate: Row {
							id: breadcrumbSegment
							spacing: Style.spacingS

							property bool isLast: index === root.navigationStack.length

							BaseText {
								anchors.verticalCenter: parent.verticalCenter
								visible: index > 0
								text: "›"
								font.pixelSize: Style.fontSizeL
								color: Style.inactiveTextColor
							}

							BaseText {
								id: segmentText
								anchors.verticalCenter: parent.verticalCenter
								text: root.pathSegmentText(index)
								font.pixelSize: Style.fontSizeL
								font.family: breadcrumbSegment.isLast ? Style.fontFamilyBold : Style.fontFamily
								color: breadcrumbSegment.isLast ? Style.titleColor
									: segmentMouse.containsMouse ? Style.titleColor : Style.subtitleColor

								MouseArea {
									id: segmentMouse
									anchors.fill: parent
									hoverEnabled: true
									enabled: !breadcrumbSegment.isLast
									cursorShape: Qt.PointingHandCursor
									onClicked: root.navigateToDepth(index)
								}
							}
						}
					}
				}
			}

			// Warning chip: how much of this level is still unfinished. It sits
			// with the path so the state of a level is visible before you read it.
			Rectangle {
				id: statusChip
				anchors.right: countText.visible ? countText.left : parent.right
				anchors.rightMargin: countText.visible ? Style.marginM : 0
				anchors.verticalCenter: parent.verticalCenter
				width: visible ? statusText.width + Style.iconSizeM + 3 * Style.marginS : 0
				height: Style.controlHeightM - Style.marginXS
				radius: height / 2
				visible: root.levelStatusText !== ""
				color: Style.baseColor
				border.color: Style.errorTextColor
				border.width: 1

				Row {
					anchors.centerIn: parent
					spacing: Style.spacingXS

					Image {
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeM
						height: width
						source: "qrc:/" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal)
						sourceSize.width: width
						sourceSize.height: height
					}
					BaseText {
						id: statusText
						anchors.verticalCenter: parent.verticalCenter
						text: root.levelStatusText
						font.pixelSize: Style.fontSizeS
						color: Style.errorTextColor
					}
				}

				MouseArea {
					id: statusArea
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: root.forceActiveFocus()
				}
			}

			BaseText {
				id: countText
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				// First thing to go when the pane gets narrow - it is a nicety,
				// the path and the commands are not.
				visible: mainPane.width > Style.sizeHintM
				text: root.filterText === "" ? qsTr("%1 items").arg(root.currentEntries.length)
					: qsTr("%1 of %2 items").arg(root.currentEntries.length).arg(root.currentSourceEntries().length)
				font.pixelSize: Style.fontSizeS
				color: Style.subtitleColor
			}
		}

		// Commands on the left, filter on the right. Two breakpoints keep them
		// from ever meeting: below "compact" everything but the primary command
		// folds into an overflow menu, below "stacked" the filter drops onto its
		// own line. Both are fixed widths rather than measurements of the row,
		// because a layout that measures what it is about to change oscillates.
		Item {
			id: commandRow
			anchors.top: locationBar.bottom
			anchors.topMargin: Style.marginM
			anchors.left: parent.left
			anchors.right: parent.right
			height: commandRow.stacked ? 2 * Style.controlHeightM + Style.marginS : Style.controlHeightM
			// Nothing may spill outside the row even if a translation makes the
			// commands wider than the breakpoints below expect.
			clip: true

			// The full command set is wide, so it only comes out when there is
			// room for it and a filter next to it.
			property bool compact: mainPane.width < Style.sizeHintXXL
			property bool stacked: mainPane.width < Style.sizeHintM

			// The menu belongs to the compact layout and goes away with it,
			// otherwise widening the pane left it floating over the table next
			// to the very commands it stands in for.
			onCompactChanged: {
				if (!commandRow.compact)
					overflowMenu.visible = false
			}

			Row {
				id: actions
				anchors.left: parent.left
				anchors.top: parent.top
				height: Style.controlHeightM
				spacing: Style.spacingXS

				ToolbarButton {
					primary: true
					plusGlyph: true
					visible: root.createVisible && root.movingNode === null && root.editingNode === null
					active: root.editable && root.createEnabled
					tooltipText: root.createEnabled ? "" : qsTr("Fill in this feature before giving it children")
					text: root.createButtonText
					onClicked: {
						// A filter would hide the new row and with it the selection.
						root.clearFilter()
						root.createRequested(root.currentParentNode())
					}
				}
				ToolbarButton {
					primary: true
					visible: root.editingNode !== null
					iconSource: "qrc:/" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Selected)
					text: qsTr("Done")
					onClicked: root.commitEditRow()
				}
				ToolbarButton {
					visible: root.editingNode !== null
					iconSource: "qrc:/" + Style.getIconPath("Icons/Cancel", Icon.State.On, Icon.Mode.Normal)
					text: qsTr("Cancel")
					onClicked: root.cancelEditRow()
				}
				ToolbarButton {
					id: openCommand
					visible: root.navigationEnabled && root.movingNode === null
						&& root.editingNode === null && !commandRow.compact
					active: root.canOpenSelected() && root.singleTargetOnly()
					iconSource: "qrc:/" + Style.getIconPath("Icons/Right", Icon.State.On,
						openCommand.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
					text: qsTr("Open")
					tooltipText: qsTr("Show what is inside the selected row")
					onClicked: root.openSelected()
				}
				ToolbarButton {
					id: editCommand
					visible: root.rowEditingEnabled && root.movingNode === null
						&& root.editingNode === null && !commandRow.compact
					active: root.editable && root.selectedNode !== null && root.singleTargetOnly()
					iconSource: "qrc:/" + Style.getIconPath("Icons/Edit", Icon.State.On,
						editCommand.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
					text: qsTr("Edit")
					onClicked: root.beginEditRow(root.selectedNode, 0)
				}
				ToolbarButton {
					id: renameCommand
					visible: root.renameVisible && root.movingNode === null
						&& root.editingNode === null && !commandRow.compact
					active: root.editable && root.selectedNode !== null && root.singleTargetOnly()
					iconSource: "qrc:/" + Style.getIconPath("Icons/Edit", Icon.State.On,
						renameCommand.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
					text: qsTr("Rename")
					onClicked: root.beginRename()
				}
				ToolbarButton {
					id: moveCommand
					visible: root.moveVisible && root.movingNode === null
						&& root.editingNode === null && !commandRow.compact
					active: root.editable && root.selectedNode !== null && root.singleTargetOnly()
					iconSource: "qrc:/" + Style.getIconPath("Icons/Export", Icon.State.On,
						moveCommand.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
					text: qsTr("Move")
					onClicked: root.beginMove()
				}
				ToolbarButton {
					id: removeCommand
					visible: root.removeVisible && root.movingNode === null
						&& root.editingNode === null && !commandRow.compact
					active: root.editable && root.commandTargets().length > 0
					iconSource: "qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.On,
						removeCommand.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
					text: root.checkedNodes.length > 1
						? qsTr("Remove %1").arg(root.checkedNodes.length) : qsTr("Remove")
					onClicked: root.removeSelection()
				}
				ToolbarButton {
					id: overflowButton
					visible: commandRow.compact && root.movingNode === null && root.editingNode === null
						&& (root.rowEditingEnabled || root.renameVisible || root.moveVisible
							|| root.removeVisible || root.navigationEnabled)
					text: "···"
					tooltipText: qsTr("More commands")
					onClicked: overflowMenu.visible = !overflowMenu.visible
				}
				ToolbarButton {
					visible: root.movingNode !== null
					primary: true
					active: root.canMoveHere()
					text: qsTr("Move here")
					onClicked: root.finishMove()
				}
				ToolbarButton {
					visible: root.movingNode !== null
					iconSource: "qrc:/" + Style.getIconPath("Icons/Cancel", Icon.State.On, Icon.Mode.Normal)
					text: qsTr("Cancel")
					onClicked: root.cancelMove()
				}
			}

			// Tells the user what the table expects of them right now. It is the
			// same strip in all three states, so the eye always looks in one place.
			BaseText {
				id: hintText
				anchors.left: actions.right
				anchors.leftMargin: Style.marginL
				anchors.right: searchField.visible ? searchField.left : parent.right
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: actions.verticalCenter
				// Last in, first out: the commands and the filter both come
				// before a sentence of advice.
				visible: !commandRow.stacked && text !== "" && width > Style.sizeHintBXS
				text: root.editingNode !== null
						? qsTr("Editing - Tab moves on, Enter applies, Esc discards")
					: root.movingNode !== null
						? qsTr("Open the level you want it in, then press Move here")
					: root.checkedNodes.length > 1
						? qsTr("%1 rows ticked - Remove works on all of them").arg(root.checkedNodes.length)
					: root.selectedNode !== null
						? root.selectedHintText
					: root.idleHintText
				font.pixelSize: Style.fontSizeS
				color: Style.subtitleColor
				elide: Text.ElideRight
			}

			SearchTextInput {
				id: searchField
				anchors.right: parent.right
				anchors.top: commandRow.stacked ? actions.bottom : parent.top
				anchors.topMargin: commandRow.stacked ? Style.marginS : 0
				// Purely what is left over. The old floor of sizeHintBXS was the
				// overlap: once the commands took more than the row had, the
				// filter still claimed those hundred pixels and sat on top of
				// them. Below a usable width it steps aside altogether.
				width: commandRow.stacked ? parent.width
					: Math.max(0, Math.min(Style.sizeHintXS, parent.width - actions.width - Style.marginL))
				visible: commandRow.stacked || width >= Style.sizeHintBXS
				height: Style.controlHeightM
				radius: Style.radiusM
				placeHolderText: root.searchPlaceholder
				onSearchChanged: {
					root.filterText = text
					root.rebuildCurrentEntries()
				}
			}
		}

		Item {
			id: columnHeader
			anchors.top: commandRow.bottom
			anchors.topMargin: Style.marginL
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.rightMargin: Style.marginM
			height: Style.controlHeightS

			// Ticks or clears the whole level. Tri-state, so a partial selection
			// is visible from the header alone.
			CheckBox {
				id: headerCheck
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				visible: root.multiSelectEnabled && root.currentEntries.length > 0
				isActive: root.editable
				tristate: true
				checkState: root.checkedNodes.length === 0 ? Qt.Unchecked
					: root.allChecked() ? Qt.Checked : Qt.PartiallyChecked
				mouseArea.enabled: false
			}

			MouseArea {
				anchors.left: parent.left
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				width: root.rowLeftInset
				visible: headerCheck.visible
				cursorShape: Qt.PointingHandCursor
				onClicked: root.checkAll(!root.allChecked())
			}

			BaseText {
				anchors.left: parent.left
				anchors.leftMargin: root.rowLeftInset
				anchors.verticalCenter: parent.verticalCenter
				text: root.nameColumnTitle
				font.family: Style.fontFamilyBold
				font.pixelSize: Style.fontSizeS
				color: Style.subtitleColor
				visible: root.headerContentComponent === null
			}
			BaseText {
				anchors.right: parent.right
				anchors.rightMargin: root.rowRightInset
				anchors.verticalCenter: parent.verticalCenter
				text: root.secondaryColumnTitle
				font.family: Style.fontFamilyBold
				font.pixelSize: Style.fontSizeS
				color: Style.subtitleColor
				visible: root.headerContentComponent === null && text !== ""
			}
			Loader {
				anchors.fill: parent
				anchors.leftMargin: root.rowLeftInset
				anchors.rightMargin: root.rowRightInset
				active: root.headerContentComponent !== null
				sourceComponent: root.headerContentComponent
			}
		}

		Rectangle {
			id: entriesFrame
			anchors.top: columnHeader.bottom
			anchors.topMargin: Style.marginXS
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			radius: Style.radiusM
			color: Style.baseColor
			border.color: Style.borderColor
			border.width: 1
			clip: true

			ListView {
				id: entriesView
				anchors.fill: parent
				anchors.margins: 1
				anchors.rightMargin: 1 + Style.marginM
				model: root.currentEntries
				clip: true
				boundsBehavior: Flickable.StopAtBounds
				currentIndex: -1

				delegate: Rectangle {
					id: entryRow
					width: entriesView.width
					height: root.rowHeight
					// currentEntries[index], not modelData: a JS object handed to a
					// delegate goes through QVariant and comes back as a different
					// object, so identity checks against the set of ticked nodes
					// silently failed and the boxes never drew as checked.
					property var treeNode: root.currentEntries[index]
					property bool selected: root.selectedNode === treeNode
					property bool rowEditing: root.editingNode === treeNode
					// Includes the row's own buttons: while the pointer is on one of
					// them the row's mouse area has lost it, and a hover that stops
					// at their edge made the row flicker.
					property bool hovered: rowHoverArea.containsMouse
						|| editButton.hovered || cancelButton.hovered
					property bool hasCheck: root.multiSelectEnabled
					property bool rowChecked: root.isChecked(entryRow.treeNode)
					// Banding instead of rules: rows stay apart without adding a
					// line for every one of them. The row being edited turns into a
					// card - own background, accent outline, accent bar - so it reads
					// as lifted out of the table rather than merely highlighted.
					color: entryRow.rowEditing ? Style.baseColor
						: entryRow.selected ? Style.selectedColor
						: entryRow.hovered ? Style.hover
						: index % 2 === 1 ? Style.backgroundColor2 : "transparent"

					// Declared first so it sits under the row content: display cells
					// are plain text and let the click through, the text fields of a
					// row in edit mode keep it for themselves.
					MouseArea {
						id: rowHoverArea
						anchors.fill: parent
						hoverEnabled: true
						enabled: root.renamingNode !== entryRow.treeNode
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							root.forceActiveFocus()
							// Not while the row is open: unticking it would take the
							// editor's subject away from under it.
							if (!entryRow.rowEditing)
								root.toggleChecked(entryRow.treeNode)
						}
						onDoubleClicked: root.navigateInto(entryRow.treeNode)
					}

					// Marks the row that is open for editing.
					Rectangle {
						anchors.fill: parent
						visible: entryRow.rowEditing
						radius: Style.radiusM
						color: "transparent"
						border.color: Style.titleColor
						border.width: 1
					}

					Rectangle {
						anchors.left: parent.left
						anchors.top: parent.top
						anchors.topMargin: Style.marginXS
						anchors.bottom: parent.bottom
						anchors.bottomMargin: Style.marginXS
						width: entryRow.rowEditing ? Style.spacingXS : 0
						radius: width / 2
						color: Style.titleColor
					}

					// Ticking a row is the only way to select it. The hit area is a
					// sibling rather than a child of the box: a ControlBase builds
					// its visuals from a decorator, and parenting anything into it
					// left the tick no longer following checkState.
					CheckBox {
						id: rowCheck
						anchors.left: parent.left
						anchors.leftMargin: Style.marginL
						anchors.verticalCenter: parent.verticalCenter
						visible: entryRow.hasCheck
						isActive: root.editable
						checkState: entryRow.rowChecked ? Qt.Checked : Qt.Unchecked
						mouseArea.enabled: false
					}


					Image {
						id: entryIcon
						anchors.left: entryRow.hasCheck ? rowCheck.right : parent.left
						anchors.leftMargin: entryRow.hasCheck ? Style.marginM : Style.marginL
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeS
						height: width
						visible: root.rowIconVisible
						source: "qrc:/" + Style.getIconPath(root.nodeIcon(entryRow.treeNode), Icon.State.On, Icon.Mode.Normal)
						sourceSize.width: width
						sourceSize.height: height
					}

					Item {
						id: contentArea
						anchors.left: parent.left
						anchors.leftMargin: root.rowLeftInset
						anchors.right: parent.right
						anchors.rightMargin: root.rowRightInset
						anchors.verticalCenter: parent.verticalCenter
						height: parent.height

						Column {
							id: defaultContent
							anchors.left: parent.left
							anchors.right: secondaryText.left
							anchors.rightMargin: Style.marginL
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.spacingXXS
							visible: root.rowContentComponent === null

							BaseText {
								width: parent.width
								text: root.nodeText(entryRow.treeNode)
								font.family: Style.fontFamilyBold
								color: Style.textColor
								elide: Text.ElideRight
								visible: root.renamingNode !== entryRow.treeNode
							}
							CustomTextField {
								id: renameInput
								width: parent.width
								height: Style.controlHeightM
								visible: root.renamingNode === entryRow.treeNode
								text: visible ? root.nodeText(entryRow.treeNode) : ""
								autoEditingFinished: false
								onVisibleChanged: {
									if (visible)
										forceActiveFocus()
								}
								onAccepted: root.finishRename(entryRow.treeNode, text)
								onEditingFinished: root.finishRename(entryRow.treeNode, text)
								onCancelled: root.renamingNode = null
							}
							BaseText {
								width: parent.width
								text: root.nodeDescription(entryRow.treeNode)
								font.pixelSize: Style.fontSizeS
								color: Style.subtitleColor
								elide: Text.ElideRight
								visible: text !== "" && root.renamingNode !== entryRow.treeNode
							}
						}

						BaseText {
							id: secondaryText
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							width: text === "" ? 0 : Math.min(Style.sizeHintXXS, entryRow.width * 0.28)
							visible: root.rowContentComponent === null
							text: root.nodeSecondaryText(entryRow.treeNode)
							color: Style.subtitleColor
							horizontalAlignment: Text.AlignRight
							elide: Text.ElideRight
						}

						Loader {
							id: rowContentLoader
							anchors.fill: parent
							active: root.rowContentComponent !== null
							sourceComponent: root.rowContentComponent
							property var node: entryRow.treeNode
							property bool editing: entryRow.rowEditing
							// The row reads these to know which cell to open on and
							// when to write its drafts back.
							property int editColumn: root.editColumn
							property int commitRequest: root.commitRequest
						}
					}

					// Per-row edit affordance: a pencil that fades in with the pointer
					// or the selection, and turns into a tick while the row is open.
					ToolButton {
						id: cancelButton
						anchors.right: parent.right
						anchors.rightMargin: Style.marginM
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeM
						height: Style.iconSizeM
						visible: entryRow.rowEditing
						iconSource: "qrc:/" + Style.getIconPath("Icons/Cancel", Icon.State.On, Icon.Mode.Normal)
						onClicked: root.cancelEditRow()
					}

					// Tick and cross of the row itself, so a row can be finished or
					// abandoned without going back up to the toolbar.
					ToolButton {
						id: editButton
						anchors.right: cancelButton.visible ? cancelButton.left : parent.right
						anchors.rightMargin: Style.marginM
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeM
						height: Style.iconSizeM
						visible: root.rowEditingEnabled
						opacity: entryRow.selected || entryRow.rowEditing || editButton.hovered
							? 1 : Style.opacityLow
						iconSource: "qrc:/" + Style.getIconPath(entryRow.rowEditing ? "Icons/Ok" : "Icons/Edit",
							Icon.State.On, Icon.Mode.Normal)
						onClicked: root.toggleEditRow(entryRow.treeNode)
					}
				}
			}

			CustomScrollbar {
				z: entriesView.z + 1
				anchors.right: parent.right
				anchors.rightMargin: Style.marginXS
				anchors.top: entriesView.top
				anchors.topMargin: Style.marginXS
				anchors.bottom: entriesView.bottom
				anchors.bottomMargin: Style.marginXS
				secondSize: Style.marginS
				targetItem: entriesView
				radius: Style.radiusS
			}

			Column {
				anchors.centerIn: parent
				width: Math.min(parent.width - 2 * Style.marginXL, Style.sizeHintM)
				spacing: Style.spacingL
				visible: root.currentEntries.length === 0

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					width: Style.iconSizeL
					height: width
					source: "qrc:/" + Style.getIconPath(root.defaultIconSource, Icon.State.On, Icon.Mode.Disabled)
					sourceSize.width: width
					sourceSize.height: height
				}
				BaseText {
					width: parent.width
					horizontalAlignment: Text.AlignHCenter
					text: root.filterText === "" ? root.emptyText : root.filteredEmptyText
					font.pixelSize: Style.fontSizeL
					color: Style.textColor
					wrapMode: Text.WordWrap
				}
				BaseText {
					width: parent.width
					horizontalAlignment: Text.AlignHCenter
					text: root.filterText === "" ? root.emptyDescription : qsTr("Try a different search")
					color: Style.subtitleColor
					wrapMode: Text.WordWrap
				}
			}
		}

		// Hover panel behind the "N incomplete" chip: which rows are at fault
		// and what to do about them. Drawn over the table rather than in it, so
		// nothing in the layout has to make room for it.
		Rectangle {
			id: statusPopup
			z: 100
			anchors.top: locationBar.bottom
			anchors.topMargin: Style.marginXS
			anchors.right: parent.right
			width: Math.min(Style.sizeHintS, mainPane.width - 2 * Style.marginL)
			height: statusColumn.height + 2 * Style.marginM
			visible: statusArea.containsMouse && root.levelStatusDetails.length > 0
			radius: Style.radiusM
			color: Style.baseColor
			border.color: Style.errorTextColor
			border.width: 1

			Column {
				id: statusColumn
				anchors.top: parent.top
				anchors.topMargin: Style.marginM
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.spacingS

				Row {
					spacing: Style.spacingS

					Image {
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeS
						height: width
						source: "qrc:/" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal)
						sourceSize.width: width
						sourceSize.height: height
					}
					BaseText {
						anchors.verticalCenter: parent.verticalCenter
						text: root.levelStatusText
						font.family: Style.fontFamilyBold
						color: Style.errorTextColor
					}
				}

				Repeater {
					model: root.levelStatusDetails

					delegate: BaseText {
						width: statusColumn.width
						text: modelData
						font.pixelSize: Style.fontSizeS
						color: Style.textColor
						wrapMode: Text.WordWrap
					}
				}

				BaseText {
					width: statusColumn.width
					text: qsTr("Open a row with Edit and fill in the missing values.")
					font.pixelSize: Style.fontSizeS
					color: Style.subtitleColor
					wrapMode: Text.WordWrap
				}
			}
		}

		// Overflow menu for the commands that do not fit. Click-away closes it,
		// which is what the full-size area underneath the panel is for.
		Item {
			id: overflowMenu
			anchors.fill: parent
			z: 100
			visible: false

			MouseArea {
				anchors.fill: parent
				onClicked: overflowMenu.visible = false
			}

			// Same treatment PopupDecorator gives its popups.
			DropShadow {
				anchors.fill: overflowPanel
				z: overflowPanel.z - 1
				horizontalOffset: 3
				verticalOffset: 3
				radius: Style.radiusL
				samples: 17
				spread: 0
				color: Style.shadowColor
				source: overflowPanel
			}

			Rectangle {
				id: overflowPanel
				anchors.top: parent.top
				anchors.topMargin: commandRow.y + Style.controlHeightM + Style.marginXS
				anchors.left: parent.left
				// Opens under the three dots, pulled back inside the pane when
				// the button sits too close to its right edge.
				anchors.leftMargin: Math.max(0, Math.min(overflowButton.x,
					mainPane.width - overflowPanel.width))
				width: Style.sizeHintXXS
				height: overflowColumn.height + 2 * Style.marginS
				radius: Style.radiusM
				color: Style.baseColor
				border.color: Style.borderColor
				border.width: 1

				Column {
					id: overflowColumn
					anchors.top: parent.top
					anchors.topMargin: Style.marginS
					anchors.left: parent.left
					anchors.leftMargin: Style.marginS
					anchors.right: parent.right
					anchors.rightMargin: Style.marginS
					spacing: Style.spacingXXS

					ToolbarButton {
						id: overflowOpen
						width: overflowColumn.width
						leftAligned: true
						visible: root.navigationEnabled
						active: root.canOpenSelected() && root.singleTargetOnly()
						iconSource: "qrc:/" + Style.getIconPath("Icons/Right", Icon.State.On,
							overflowOpen.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
						text: qsTr("Open")
						onClicked: {
							overflowMenu.visible = false
							root.openSelected()
						}
					}
					ToolbarButton {
						id: overflowEdit
						width: overflowColumn.width
						leftAligned: true
						visible: root.rowEditingEnabled
						active: root.editable && root.selectedNode !== null && root.singleTargetOnly()
						iconSource: "qrc:/" + Style.getIconPath("Icons/Edit", Icon.State.On,
							overflowEdit.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
						text: qsTr("Edit")
						onClicked: {
							overflowMenu.visible = false
							root.beginEditRow(root.selectedNode, 0)
						}
					}
					ToolbarButton {
						id: overflowRename
						width: overflowColumn.width
						leftAligned: true
						visible: root.renameVisible
						active: root.editable && root.selectedNode !== null && root.singleTargetOnly()
						iconSource: "qrc:/" + Style.getIconPath("Icons/Edit", Icon.State.On,
							overflowRename.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
						text: qsTr("Rename")
						onClicked: {
							overflowMenu.visible = false
							root.beginRename()
						}
					}
					ToolbarButton {
						id: overflowMove
						width: overflowColumn.width
						leftAligned: true
						visible: root.moveVisible
						active: root.editable && root.selectedNode !== null && root.singleTargetOnly()
						iconSource: "qrc:/" + Style.getIconPath("Icons/Export", Icon.State.On,
							overflowMove.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
						text: qsTr("Move")
						onClicked: {
							overflowMenu.visible = false
							root.beginMove()
						}
					}
					ToolbarButton {
						id: overflowRemove
						width: overflowColumn.width
						leftAligned: true
						visible: root.removeVisible
						active: root.editable && root.commandTargets().length > 0
						iconSource: "qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.On,
							overflowRemove.active ? Icon.Mode.Normal : Icon.Mode.Disabled)
						text: root.checkedNodes.length > 1
							? qsTr("Remove %1").arg(root.checkedNodes.length) : qsTr("Remove")
						onClicked: {
							overflowMenu.visible = false
							root.removeSelection()
						}
					}
				}
			}
		}
	}

	// Drag handle between the table and the side panel. Only the grip is drawn,
	// so resizing the panel does not cost another full-height line.
	Item {
		id: splitHandle
		anchors.right: sidePanel.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		width: root.sidePanelShown ? Style.marginXL : 0
		visible: root.sidePanelShown

		Rectangle {
			anchors.centerIn: parent
			width: Style.spacingXXS
			height: Style.iconSizeXL
			radius: width
			color: splitArea.containsMouse || splitArea.pressed ? Style.titleColor : Style.borderColor
		}

		MouseArea {
			id: splitArea
			anchors.fill: parent
			hoverEnabled: true
			preventStealing: true
			cursorShape: Qt.SplitHCursor

			property real pressRootX: 0
			property real pressWidth: 0

			onPressed: {
				splitArea.pressRootX = splitArea.mapToItem(root, splitArea.mouseX, 0).x
				splitArea.pressWidth = root.sidePanelActualWidth
			}
			onPositionChanged: {
				if (!splitArea.pressed)
					return
				let delta = splitArea.mapToItem(root, splitArea.mouseX, 0).x - splitArea.pressRootX
				root.sidePanelWidth = splitArea.pressWidth - delta
			}
		}
	}

	Item {
		id: sidePanel
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		width: root.sidePanelShown ? root.sidePanelActualWidth : 0
		visible: root.sidePanelShown

		Loader {
			anchors.fill: parent
			active: root.sidePanelShown
			sourceComponent: root.sidePanelComponent
		}
	}
}
