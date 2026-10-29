import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype TableCellTagsDelegate
	\inqmlmodule imtcolgui
	\brief Text cell that shows the tags of its row as chips after the text, like GitHub
	shows labels right after an issue title.

	Reads the \c tags list ({ id, name, color, isSystem }) that a collection controller with a
	\c TagInfoProvider adds to every list item. A row without tags looks exactly like
	TableCellTextDelegate. CollectionView puts it into its first column.
*/
TableCellDelegateBase {
	id: tagsCell

	clip: true

	//! More tags than that collapse into "+N".
	property int maxChips: 3

	property var cellTags: []
	property var visibleTags: []

	readonly property bool hasTags: tagsCell.cellTags.length > 0
	readonly property int textLeftMargin: tagsCell.rowDelegate ? tagsCell.rowDelegate.textLeftMargin : 0
	readonly property int textRightMargin: tagsCell.rowDelegate ? tagsCell.rowDelegate.textRightMargin : 0
	readonly property real textSpace: Math.max(0, tagsCell.width - tagsCell.textLeftMargin - tagsCell.textRightMargin)

	function readTags(){
		var tags = []
		if (tagsCell.table && tagsCell.table.elements && tagsCell.rowIndex >= 0){
			var tagsModel = tagsCell.table.elements.getData("tags", tagsCell.rowIndex)
			var count = tagsModel && tagsModel.getItemsCount ? tagsModel.getItemsCount() : 0
			for (var i = 0; i < count; i++){
				tags.push({
					name: String(tagsModel.getData("name", i) || ""),
					color: String(tagsModel.getData("color", i) || "ededed"),
					isSystem: tagsModel.getData("isSystem", i) === true
				})
			}
		}

		var visible = []
		for (var j = 0; j < tags.length && j < tagsCell.maxChips; j++){
			visible.push(tags[j])
		}

		tagsCell.cellTags = tags
		tagsCell.visibleTags = visible
	}

	onReused: {
		tagsCellText.text = tagsCell.getValue()
		tagsCell.readTags()
	}

	TableCellText {
		id: tagsCellText

		anchors.left: parent.left
		anchors.leftMargin: tagsCell.textLeftMargin
		anchors.verticalCenter: parent.verticalCenter
		width: tagsCell.hasTags ? Math.max(0, Math.min(tagsCellText.implicitWidth, tagsCell.textSpace - tagChipsRow.width - Style.spacingS)) : tagsCell.textSpace
		rowDelegate: tagsCell.rowDelegate
		columnIndex: tagsCell.columnIndex
	}

	Row {
		id: tagChipsRow

		anchors.left: tagsCellText.right
		anchors.leftMargin: Style.spacingS
		anchors.verticalCenter: parent.verticalCenter
		spacing: Style.spacingXS
		visible: tagsCell.hasTags

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
