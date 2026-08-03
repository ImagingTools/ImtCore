import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// Searchable, tree-indented list of checkable entries, laid out to sit next to
// a TreeExplorerView as its side panel: header, search and column titles line
// up row for row with the explorer's, so both tables start at the same height.
//
// The panel never disappears. With nothing to show it keeps its frame and puts
// a placeholder inside, which is steadier than a pane that comes and goes.
//
// Entries are plain objects; every column is read through a provider function
// so the owner keeps full control over what a row means.
FocusScope {
	id: root

	// Keyboard-reachable like the table it sits next to: arrows move the cursor,
	// Space ticks, Home/End and the page keys jump.
	property int currentRow: -1

	property var model: []
	property string title: qsTr("Details")
	// Rendered next to the title, e.g. the name of the row the panel belongs to.
	property string subtitle: ""
	property string searchPlaceholder: qsTr("Search")
	property string nameColumnTitle: qsTr("Name")
	property string emptyText: qsTr("Nothing to show")
	property string emptyDescription: ""
	property string filteredEmptyText: qsTr("No matching items")
	property string iconSource: "Icons/FeaturePackage"
	// False when the owner has nothing selected: the list is replaced by
	// placeholderText and the search box is switched off.
	property bool contentActive: true
	property string placeholderText: qsTr("Nothing selected")
	property string placeholderDescription: ""
	// Optional command shown at the right of the header row.
	property string actionText: ""
	property bool actionEnabled: true
	// A standing note under the list - something true of the whole panel rather
	// than of any one row, e.g. where a choice shown here is actually made.
	property string footerText: ""

	property var entryTitleProvider: null
	property var entrySubtitleProvider: null
	// Short note rendered as a chip next to the entry's name - the reason it is
	// unavailable, or what kind of entry it is. It replaces a whole column.
	property var entryBadgeProvider: null
	property var entrySearchableTextProvider: null
	property var entryCheckStateProvider: null
	property var entryCheckEnabledProvider: null

	property string filterText: ""
	property var visibleEntries: []
	property real rowHeight: Style.controlHeightL + Style.marginXS
	// Change this whenever the panel starts describing something else; the list
	// then scrolls to the first ticked entry so the current choices are in view.
	property string revealKey: ""

	signal toggleRequested(var entry)
	signal actionRequested()

	function moveCurrent(step) {
		if (root.visibleEntries.length === 0)
			return
		let next = root.currentRow < 0 ? 0 : root.currentRow + step
		if (next < 0)
			next = 0
		if (next >= root.visibleEntries.length)
			next = root.visibleEntries.length - 1
		root.currentRow = next
		entriesView.positionViewAtIndex(next, ListView.Contain)
	}

	function toggleCurrent() {
		if (root.currentRow < 0 || root.currentRow >= root.visibleEntries.length)
			return
		let entry = root.visibleEntries[root.currentRow]
		if (root.entryCheckEnabled(entry))
			root.toggleRequested(entry)
	}

	function pageStep() {
		return Math.max(1, Math.floor(entriesView.height / root.rowHeight) - 1)
	}

	Keys.onPressed: {
		if (!root.contentActive)
			return
		if (event.key === Qt.Key_Up) {
			root.moveCurrent(-1)
			event.accepted = true
		}
		else if (event.key === Qt.Key_Down) {
			root.moveCurrent(1)
			event.accepted = true
		}
		else if (event.key === Qt.Key_Home) {
			root.currentRow = -1
			root.moveCurrent(1)
			event.accepted = true
		}
		else if (event.key === Qt.Key_End) {
			root.currentRow = root.visibleEntries.length
			root.moveCurrent(-1)
			event.accepted = true
		}
		else if (event.key === Qt.Key_PageUp) {
			root.moveCurrent(-root.pageStep())
			event.accepted = true
		}
		else if (event.key === Qt.Key_PageDown) {
			root.moveCurrent(root.pageStep())
			event.accepted = true
		}
		else if (event.key === Qt.Key_Space || event.key === Qt.Key_Return
			|| event.key === Qt.Key_Enter) {
			root.toggleCurrent()
			event.accepted = true
		}
	}

	onModelChanged: root.rebuildVisibleEntries()

	onRevealKeyChanged: revealTimer.restart()

	Component.onCompleted: root.rebuildVisibleEntries()

	// The delegates of a just-swapped model only exist on the next turn, and
	// positionViewAtIndex needs them to know how far down to go.
	Timer {
		id: revealTimer
		interval: 0
		onTriggered: root.revealFirstChecked()
	}

	function revealFirstChecked() {
		if (!root.contentActive)
			return
		for (let i = 0; i < root.visibleEntries.length; ++i) {
			if (root.entryCheckState(root.visibleEntries[i]) === Qt.Checked) {
				entriesView.positionViewAtIndex(i, ListView.Center)
				return
			}
		}
		entriesView.positionViewAtBeginning()
	}

	function entryTitle(entry) {
		if (entryTitleProvider)
			return entryTitleProvider(entry)
		return entry && entry.title ? entry.title : ""
	}

	function entrySubtitle(entry) {
		if (entrySubtitleProvider)
			return entrySubtitleProvider(entry)
		return entry && entry.subtitle ? entry.subtitle : ""
	}

	function entryBadge(entry) {
		if (entryBadgeProvider)
			return entryBadgeProvider(entry)
		return ""
	}

	function entrySearchableText(entry) {
		if (entrySearchableTextProvider)
			return entrySearchableTextProvider(entry)
		return entryTitle(entry) + " " + entrySubtitle(entry)
	}

	function entryCheckState(entry) {
		if (entryCheckStateProvider)
			return entryCheckStateProvider(entry)
		return Qt.Unchecked
	}

	function entryCheckEnabled(entry) {
		if (entryCheckEnabledProvider)
			return entryCheckEnabledProvider(entry)
		return true
	}

	function rebuildVisibleEntries() {
		let source = root.model || []
		let entries = []
		let filter = root.filterText.toLowerCase()
		for (let i = 0; i < source.length; ++i) {
			if (filter === "" || entrySearchableText(source[i]).toLowerCase().indexOf(filter) >= 0)
				entries.push(source[i])
		}
		root.visibleEntries = entries
		// The row under the cursor is gone or is now something else, so the
		// cursor goes with it rather than pointing at a stranger.
		root.currentRow = -1
	}

	Item {
		id: panelHeader
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		// Matches the explorer's location bar, so both tables start at the
		// same height however the two panes are sized.
		height: Style.controlHeightL

		BaseText {
			id: titleText
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter
			text: root.title
			font.family: Style.fontFamilyBold
			font.pixelSize: Style.fontSizeL
			color: Style.titleColor
		}

		BaseText {
			anchors.left: titleText.right
			anchors.leftMargin: Style.marginS
			anchors.right: actionButton.visible ? actionButton.left : parent.right
			anchors.rightMargin: Style.marginS
			anchors.verticalCenter: parent.verticalCenter
			visible: root.subtitle !== ""
			text: root.subtitle
			font.pixelSize: Style.fontSizeS
			color: Style.subtitleColor
			elide: Text.ElideRight
		}

		ToolbarButton {
			id: actionButton
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			visible: root.actionText !== ""
			active: root.actionEnabled && root.contentActive
			text: root.actionText
			onClicked: root.actionRequested()
		}
	}

	SearchTextInput {
		id: searchField
		anchors.top: panelHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.right: parent.right
		height: Style.controlHeightM
		radius: Style.radiusM
		enabled: root.contentActive
		placeHolderText: root.searchPlaceholder
		onSearchChanged: {
			root.filterText = text
			root.rebuildVisibleEntries()
		}
	}

	Item {
		id: columnHeader
		anchors.top: searchField.bottom
		anchors.topMargin: Style.marginL
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.rightMargin: Style.marginM
		height: Style.controlHeightS

		BaseText {
			anchors.left: parent.left
			anchors.leftMargin: Style.marginL
			anchors.verticalCenter: parent.verticalCenter
			text: root.nameColumnTitle
			font.family: Style.fontFamilyBold
			font.pixelSize: Style.fontSizeS
			color: Style.subtitleColor
		}
	}

	Rectangle {
		id: panelFrame
		anchors.top: columnHeader.bottom
		anchors.topMargin: Style.marginXS
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: footerLabel.visible ? footerLabel.top : parent.bottom
		anchors.bottomMargin: footerLabel.visible ? Style.marginS : 0
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
			model: root.contentActive ? root.visibleEntries : []
			clip: true
			boundsBehavior: Flickable.StopAtBounds

			delegate: Rectangle {
				id: entryRow
				width: entriesView.width
				height: root.rowHeight
				color: root.currentRow === index ? Style.selectedColor
					: entryMouse.containsMouse && entryRow.entryEnabled ? Style.hover
					: index % 2 === 1 ? Style.backgroundColor2 : "transparent"

				property var entry: modelData
				property bool entryEnabled: root.entryCheckEnabled(entryRow.entry)
				property bool entryChecked: root.entryCheckState(entryRow.entry) === Qt.Checked

				CheckBox {
					id: entryCheck
					anchors.left: parent.left
					anchors.leftMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					isActive: entryRow.entryEnabled
					checkState: entryRow.entryChecked ? Qt.Checked : Qt.Unchecked
					mouseArea.enabled: false
				}

				Column {
					anchors.left: entryCheck.right
					anchors.leftMargin: Style.marginM
					anchors.right: parent.right
					anchors.rightMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.spacingXXS

					Row {
						width: parent.width
						spacing: Style.spacingS

						BaseText {
							anchors.verticalCenter: parent.verticalCenter
							width: Math.max(0, parent.width - badgeChip.width - parent.spacing)
							text: root.entryTitle(entryRow.entry)
							color: entryRow.entryEnabled || entryRow.entryChecked ? Style.textColor : Style.inactiveTextColor
							elide: Text.ElideRight
						}

						// Reason chip: why this entry cannot be picked, or what kind
						// of entry it is. Sits with the name instead of in a column.
						Rectangle {
							id: badgeChip
							anchors.verticalCenter: parent.verticalCenter
							width: badgeText.text === "" ? 0 : badgeText.width + 2 * Style.marginS
							height: Style.controlHeightS
							radius: height / 2
							visible: badgeText.text !== ""
							color: Style.backgroundColor2
							border.color: Style.borderColor
							border.width: 1

							BaseText {
								id: badgeText
								anchors.centerIn: parent
								text: root.entryBadge(entryRow.entry)
								font.pixelSize: Style.fontSizeS
								color: Style.subtitleColor
							}
						}
					}

					BaseText {
						width: parent.width
						visible: text !== ""
						text: root.entrySubtitle(entryRow.entry)
						font.pixelSize: Style.fontSizeS
						color: Style.subtitleColor
						elide: Text.ElideRight
					}
				}

				MouseArea {
					id: entryMouse
					anchors.fill: parent
					hoverEnabled: true
					enabled: entryRow.entryEnabled
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						root.forceActiveFocus()
						root.currentRow = index
						root.toggleRequested(entryRow.entry)
					}
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
			visible: !root.contentActive || root.visibleEntries.length === 0

			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				width: Style.iconSizeL
				height: width
				source: "qrc:/" + Style.getIconPath(root.iconSource, Icon.State.On, Icon.Mode.Disabled)
				sourceSize.width: width
				sourceSize.height: height
			}
			BaseText {
				width: parent.width
				horizontalAlignment: Text.AlignHCenter
				text: !root.contentActive ? root.placeholderText
					: root.filterText === "" ? root.emptyText : root.filteredEmptyText
				font.pixelSize: Style.fontSizeL
				color: Style.textColor
				wrapMode: Text.WordWrap
			}
			BaseText {
				width: parent.width
				horizontalAlignment: Text.AlignHCenter
				visible: text !== ""
				text: !root.contentActive ? root.placeholderDescription
					: root.filterText === "" ? root.emptyDescription : qsTr("Try a different search")
				color: Style.subtitleColor
				wrapMode: Text.WordWrap
			}
		}
	}

	BaseText {
		id: footerLabel
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		visible: root.footerText !== ""
		text: root.footerText
		font.pixelSize: Style.fontSizeS
		color: Style.subtitleColor
		wrapMode: Text.WordWrap
	}
}
