import QtQuick 2.12
import Acf 1.0

/**
	Newline-separated list of names or paths ("/Group/Feature/SubFeature").
	Paths are grouped by their first segment; each item takes the full width and
	wraps, so nothing is cut in a narrow panel. Long lists are collapsed.
*/
MetaInfoViewDelegateBase {
	id: root

	height: column.height

	property int collapsedItemCount: 8
	property bool expanded: false
	property int totalCount: 0
	property var groups: []
	property string message: ""

	onViewDataChanged: {
		root.expanded = false
		root.rebuild()
	}

	onExpandedChanged: {
		root.rebuild()
	}

	function rebuild(){
		let lines = root.viewData.split("\n")
		let entries = []
		for (let i = 0; i < lines.length; i++){
			let line = lines[i].trim()
			if (line !== ""){
				entries.push(line)
			}
		}

		// A single plain line is a message such as "No items", not an item.
		if (entries.length === 1 && !entries[0].startsWith("/")){
			root.message = entries[0]
			root.totalCount = 0
			root.groups = []
			return
		}

		root.message = ""
		root.totalCount = entries.length

		let limit = root.expanded ? entries.length : root.collapsedItemCount
		let result = []
		let groupIndexes = {}
		for (let i = 0; i < entries.length; i++){
			// "/Group/Feature/SubFeature" -> ["Group", "Feature", "SubFeature"]; names stay ungrouped
			let segments = entries[i].startsWith("/") ? entries[i].slice(1).split("/") : [entries[i]]
			let groupName = segments.length > 1 ? segments[0] : ""

			if (!(groupName in groupIndexes)){
				groupIndexes[groupName] = result.length
				result.push({"name": groupName, "count": 0, "items": []})
			}

			let group = result[groupIndexes[groupName]]
			group.count++

			if (i < limit){
				let title = segments[segments.length - 1]
				let location = segments.length > 2 ? segments.slice(1, segments.length - 1).join(" › ") : ""
				group.items.push({"title": title, "location": location})
			}
		}

		let visibleGroups = []
		for (let i = 0; i < result.length; i++){
			if (result[i].items.length > 0){
				visibleGroups.push(result[i])
			}
		}

		root.groups = visibleGroups
	}

	Column {
		id: column

		width: parent.width
		spacing: Style.spacingS

		Text {
			width: column.width
			visible: root.message !== ""
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeS
			wrapMode: Text.Wrap
			color: Style.subtitleColor
			text: root.message
		}

		Repeater {
			model: root.groups

			delegate: Column {
				width: column.width
				spacing: Style.spacingXS

				Text {
					width: parent.width
					visible: modelData.name !== ""
					font.family: Style.fontFamily
					font.pixelSize: Style.fontSizeXS
					wrapMode: Text.Wrap
					color: Style.subtitleColor
					text: modelData.name + " · " + modelData.count
				}

				Repeater {
					model: modelData.items

					delegate: Item {
						width: column.width
						height: itemColumn.height

						Column {
							id: itemColumn

							width: parent.width

							Text {
								width: parent.width
								font.family: Style.fontFamily
								font.pixelSize: Style.fontSizeS
								wrapMode: Text.Wrap
								color: Style.textColor
								text: modelData.title
							}

							Text {
								width: parent.width
								visible: modelData.location !== ""
								font.family: Style.fontFamily
								font.pixelSize: Style.fontSizeXS
								wrapMode: Text.Wrap
								color: Style.subtitleColor
								text: modelData.location
							}
						}
					}
				}
			}
		}

		Text {
			visible: root.totalCount > root.collapsedItemCount
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeS
			color: Style.linkColor
			text: root.expanded ? qsTr("Show less") : qsTr("Show all (%1)").arg(root.totalCount)

			MouseArea {
				anchors.fill: parent
				cursorShape: Qt.PointingHandCursor

				onClicked: {
					root.expanded = !root.expanded
				}
			}
		}
	}
}
