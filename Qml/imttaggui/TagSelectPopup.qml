// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0

/*!
	\qmltype TagSelectPopup
	\inqmlmodule imttaggui
	\brief Searchable multi-select of tags; every row shows the tag color, name and description.

	\code
	TagSelectPopup {
		preselectedIds: currentTagIds
		dataProvider: TagSelectDataProvider {}
		onSelectionChanged: { ... }
	}
	\endcode
*/
FilterableSelectPopup {
	id: tagSelectPopup

	itemWidth: 320
	showCheckBox: true
	filterPlaceholder: qsTr("Filter tags...")

	delegate: Component {
		Item {
			id: tagRow
			objectName: "TagSelectItem_" + model.index
			width: tagSelectPopup.itemWidth
			height: tagSelectPopup.itemHeight

			readonly property var tag: tagSelectPopup.getItem(model.index)
			readonly property string tagId: tagSelectPopup.getItemId(model.index)
			readonly property bool isSelected: tagSelectPopup.dataProvider ? tagSelectPopup.dataProvider.isItemSelected(tagRow.tagId) : false

			Rectangle {
				anchors.fill: parent
				color: tagSelectPopup.rowBackgroundColor(model.index, tagRowMouseArea.containsMouse || tagSelectPopup.focusedIndex === model.index, tagRow.isSelected)
			}

			MouseArea {
				id: tagRowMouseArea
				anchors.fill: parent
				z: 20
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor

				onEntered: {
					tagSelectPopup.focusItem(model.index)
				}

				onClicked: {
					tagSelectPopup.handleItemClick(tagRow.tagId, model.index)
				}
			}

			Row {
				z: 10
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.marginS

				CheckBox {
					anchors.verticalCenter: parent.verticalCenter
					width: Style.itemSizeS
					height: Style.itemSizeS
					mainMargin: Style.marginM
					borderColor: Style.grayColor
					visible: tagSelectPopup.showCheckBox
					checkState: tagRow.isSelected ? Qt.Checked : Qt.Unchecked

					function nextCheckState(){
						tagSelectPopup.handleItemClick(tagRow.tagId, model.index)
					}
				}

				Rectangle {
					anchors.verticalCenter: parent.verticalCenter
					width: Style.iconSizeXS
					height: Style.iconSizeXS
					radius: width / 2
					color: "#" + (tagRow.tag && tagRow.tag.color ? tagRow.tag.color : "ededed")
					border.width: 1
					border.color: Style.borderColor
				}

				Text {
					id: tagNameText
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: tagSelectPopup.textSize
					color: tagSelectPopup.fontColor
					text: tagSelectPopup.getItemText(model.index)
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: Math.max(0, tagRow.width - tagNameText.x - tagNameText.width - 2 * Style.marginM - Style.marginS)
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
					text: tagRow.tag ? String(tagRow.tag.description) : ""
					elide: Text.ElideRight
				}
			}
		}
	}
}
