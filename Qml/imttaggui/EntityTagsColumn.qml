// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype EntityTagsColumn
	\inqmlmodule imttaggui
	\brief Shows the tags of every row of a collection view as colored chips, like GitHub
	shows labels right after an issue title.

	The chips follow the text of one column (the first one by default). The tags of the
	loaded page come with one EntityTagsGet request each time the rows are reloaded.

	\code
	EntityTagsColumn {
		view: collectionView
		entityType: "Devices"
	}
	\endcode
*/
Item {
	id: entityTagsColumnRoot

	property string context: ""
	property string entityType: ""

	//! Collection view whose table shows the chips.
	property Item view: null

	//! Header ID of the column the chips follow; empty for the first column.
	property string headerId: ""

	//! False leaves the column as it is, e.g. without the ViewTags permission.
	property bool active: true

	//! More tags than that collapse into "+N".
	property int maxChips: 3

	function reload(){
		if (!entityTagsColumnRoot.active || !entityTagsColumnRoot.view || !entityTagsColumnRoot.view.table || !entityTagsColumnRoot.view.table.elements){
			return
		}

		var elements = entityTagsColumnRoot.view.table.elements
		var entityIds = []
		for (var i = 0; i < elements.getItemsCount(); i++){
			var entityId = elements.getData("id", i)
			if (entityId){
				entityIds.push(String(entityId))
			}
		}

		entityTagsProvider.load(entityIds)
	}

	function install(){
		if (!entityTagsColumnRoot.active || !entityTagsColumnRoot.view || !entityTagsColumnRoot.view.table){
			return
		}

		var table = entityTagsColumnRoot.view.table
		var targetHeaderId = entityTagsColumnRoot.headerId !== "" ? entityTagsColumnRoot.headerId : table.getHeaderId(0)
		if (targetHeaderId !== ""){
			table.setColumnContentById(targetHeaderId, tagsCellComp)
		}
	}

	Component.onCompleted: {
		entityTagsColumnRoot.install()
		entityTagsColumnRoot.reload()
	}

	Connections {
		target: entityTagsColumnRoot.view

		function onHeadersChanged(){
			entityTagsColumnRoot.install()
		}

		function onElementsChanged(){
			entityTagsColumnRoot.reload()
		}
	}

	EntityTagsProvider {
		id: entityTagsProvider
		context: entityTagsColumnRoot.context
		entityType: entityTagsColumnRoot.entityType
	}

	Component {
		id: tagsCellComp

		TableCellDelegateBase {
			id: tagsCell
			clip: true

			property var cellTags: []
			property var visibleTags: []

			readonly property int textLeftMargin: tagsCell.rowDelegate ? tagsCell.rowDelegate.textLeftMargin : 0
			readonly property int textRightMargin: tagsCell.rowDelegate ? tagsCell.rowDelegate.textRightMargin : 0

			function updateTags(){
				var entityId = ""
				if (tagsCell.table && tagsCell.table.elements && tagsCell.rowIndex >= 0){
					entityId = String(tagsCell.table.elements.getData("id", tagsCell.rowIndex) || "")
				}

				var tags = entityTagsProvider.getTags(entityId)
				var visible = []
				for (var i = 0; i < tags.length && i < entityTagsColumnRoot.maxChips; i++){
					visible.push(tags[i])
				}

				tagsCell.cellTags = tags
				tagsCell.visibleTags = visible
			}

			onReused: {
				tagsCellText.text = tagsCell.getValue()
				tagsCell.updateTags()
			}

			Connections {
				target: entityTagsProvider

				function onLoaded(){
					tagsCell.updateTags()
				}
			}

			TableCellText {
				id: tagsCellText
				anchors.left: parent.left
				anchors.leftMargin: tagsCell.textLeftMargin
				anchors.verticalCenter: parent.verticalCenter
				width: Math.max(0, Math.min(tagsCellText.implicitWidth, tagsCell.width - tagsCell.textLeftMargin - tagsCell.textRightMargin - (tagChipsRow.visible ? tagChipsRow.width + Style.spacingS : 0)))
				rowDelegate: tagsCell.rowDelegate
				columnIndex: tagsCell.columnIndex
				elide: Text.ElideRight
			}

			Row {
				id: tagChipsRow
				anchors.left: tagsCellText.right
				anchors.leftMargin: Style.spacingS
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.spacingXS
				visible: tagsCell.cellTags.length > 0

				Repeater {
					model: tagsCell.visibleTags

					delegate: TagChip {
						anchors.verticalCenter: parent.verticalCenter
						tagName: String(modelData.name)
						tagColor: String(modelData.color)
						isSystem: modelData.isSystem === true
					}
				}

				BaseText {
					anchors.verticalCenter: parent.verticalCenter
					visible: tagsCell.cellTags.length > tagsCell.visibleTags.length
					text: "+" + (tagsCell.cellTags.length - tagsCell.visibleTags.length)
					font.pixelSize: Style.fontSizeXS
					color: Style.inactiveTextColor
				}
			}
		}
	}
}
