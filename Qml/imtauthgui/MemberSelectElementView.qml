// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0

ElementView {
	id: root

	// Data: array of {id, name, membershipId?} objects
	property var members: []
	// Whether the member list can be edited (add/remove)
	property bool editable: true
	// Label for the element
	property string label: qsTr("Members")
	// Label for the add button
	property string addButtonText: qsTr("Add member")
	// Placeholder for the filter popup
	property string filterPlaceholder: qsTr("Type or choose a user")
	// CollectionId for the data provider
	property string collectionId: "Users"
	// Text shown when no members
	property string emptyText: qsTr("No members")

	signal memberRemoved(int index, var memberData)
	signal selectionChanged(var selectedMembers)
	signal popupClosed()

	name: root.label

	controlComp: Component {
		Text {
			id: addBtn
			visible: root.editable
			text: "+ " + root.addButtonText
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: Style.accentColor

			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					var known = []
					for (var j = 0; j < root.members.length; j++) {
						var member = root.members[j]
						known.push({ id: member.id, title: member.name || member.id })
					}
					var ids = []
					for (var i = 0; i < root.members.length; i++)
						ids.push(root.members[i].id)
					var point = addBtn.mapToItem(null, 0, addBtn.height)
					ModalDialogManager.openDialog(selectComp, {
						"x": point.x,
						"y": point.y,
						"knownItems": known,
						"preselectedIds": ids
					})
				}
			}
		}
	}

	bottomComp: Component {
		Column {
			width: parent.width
			spacing: Style.spacingXS

			Flow {
				width: parent.width
				clip: true
				spacing: Style.spacingXS
				visible: root.members.length > 0

				Repeater {
					model: root.members
					delegate: Rectangle {
						width: Math.min(chipText.contentWidth + chipRemove.width + Style.paddingS * 3, 200)
						height: 28
						radius: 14
						color: Qt.rgba(Style.accentColor.r, Style.accentColor.g, Style.accentColor.b, 0.1)
						border.color: Qt.rgba(Style.accentColor.r, Style.accentColor.g, Style.accentColor.b, 0.3)
						border.width: 1

						Text {
							id: chipText
							anchors.left: parent.left
							anchors.leftMargin: Style.paddingS + 2
							anchors.right: chipRemove.left
							anchors.rightMargin: 2
							anchors.verticalCenter: parent.verticalCenter
							text: modelData.name || modelData.id
							font.pixelSize: Style.fontSizeM
							color: Style.accentColor
							elide: Text.ElideRight
							maximumLineCount: 1
						}

						ToolButton {
							id: chipRemove
							visible: root.editable
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
							decorator: Component {
								ToolButtonDecorator {
									color: "transparent"
									icon.width: Style.iconSizeXS
								}
							}
							onClicked: {
								root.memberRemoved(index, modelData)
							}
						}
					}
				}
			}

			Text {
				visible: root.members.length === 0
				width: parent.width
				text: root.emptyText
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				wrapMode: Text.WordWrap
			}
		}
	}

	// Picker Component — opened via ModalDialogManager
	Component {
		id: selectComp

		FilterableSelectPopup {
			dataProvider: FilterableSelectGqlDataProvider {
				collectionId: root.collectionId
				multiSelect: true
			}

			itemWidth: 280
			showCheckBox: true
			showSelectedGroup: true
			filterPlaceholder: root.filterPlaceholder

			onSelectionChanged: {
				var arr = []
				for (var i = 0; i < selectedIds.length; i++) {
					var selId = selectedIds[i]
					var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
					if (!selName)
						selName = selId
					arr.push({id: selId, name: selName})
				}
				root.selectionChanged(arr)
			}

			Component.onDestruction: {
				root.popupClosed()
			}
		}
	}
}
