// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: ticketListPanelRoot
	objectName: "TicketListPanel"
	color: "transparent"

	property var ticketsModel: null
	property string currentTicketId: ""
	property int statusFilter: -1  // -1 = all

	signal ticketSelected(string ticketId)
	signal newTicketRequested()

	Column {
		anchors.fill: parent
		spacing: 0

		// Panel header
		Rectangle {
			width: parent.width
			height: Style.sizeS
			color: Style.panelHeaderColor

			Row {
				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
					leftMargin: Style.paddingM
					rightMargin: Style.paddingS
				}

				Text {
					text: qsTr("Tickets")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.textPrimaryColor
					width: parent.width - newTicketBtn.width
					anchors.verticalCenter: parent.verticalCenter
				}

				Rectangle {
					id: newTicketBtn
					width: Style.iconSizeS
					height: Style.iconSizeS
					color: "transparent"
					anchors.verticalCenter: parent.verticalCenter

					Text {
						anchors.centerIn: parent
						text: "+"
						font.pixelSize: Style.fontSizeL
						color: Style.accentColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: ticketListPanelRoot.newTicketRequested()
					}
				}
			}
		}

		// Status filter tabs
		Row {
			width: parent.width
			height: Style.tabBarHeight

			Repeater {
				model: [
					{ label: qsTr("All"),    value: -1 },
					{ label: qsTr("Open"),   value: 0  },
					{ label: qsTr("Active"), value: 1  },
					{ label: qsTr("Closed"), value: 3  }
				]

				Rectangle {
					width: parent.width / 4
					height: Style.tabBarHeight
					color: "transparent"

					Rectangle {
						anchors.bottom: parent.bottom
						width: parent.width
						height: 2
						color: Style.accentColor
						visible: ticketListPanelRoot.statusFilter === modelData.value
					}

					Text {
						anchors.centerIn: parent
						text: modelData.label
						font.pixelSize: Style.fontSizeXS
						color: ticketListPanelRoot.statusFilter === modelData.value
							? Style.accentColor
							: Style.textSecondaryColor
						font.bold: ticketListPanelRoot.statusFilter === modelData.value
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketListPanelRoot.statusFilter = modelData.value;
							if (ticketsModel) {
								ticketsModel.statusFilter = modelData.value;
							}
						}
					}
				}
			}
		}

		// Ticket list
		ListView {
			id: ticketListView
			width: parent.width
			height: parent.height - Style.sizeS - Style.tabBarHeight
			clip: true
			model: ticketListPanelRoot.ticketsModel
			spacing: 1

			delegate: Rectangle {
				width: ticketListView.width
				height: ticketItemContent.implicitHeight + Style.paddingS * 2
				color: model.id === ticketListPanelRoot.currentTicketId
					? Style.selectedItemColor
					: "transparent"

				Column {
					id: ticketItemContent
					anchors {
						left: parent.left
						right: parent.right
						verticalCenter: parent.verticalCenter
						leftMargin: Style.paddingM
						rightMargin: Style.paddingS
					}
					spacing: Style.paddingXS

					Row {
						width: parent.width
						spacing: Style.paddingXS

						Text {
							text: model.title || ""
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: Style.textPrimaryColor
							elide: Text.ElideRight
							width: parent.width - priorityBadge.width - statusBadge.width - Style.paddingXS * 2
							anchors.verticalCenter: parent.verticalCenter
						}

						TicketBadge {
							id: priorityBadge
							badgeType: "priority"
							value: model.priority || 1
							anchors.verticalCenter: parent.verticalCenter
						}

						TicketBadge {
							id: statusBadge
							badgeType: "status"
							value: model.status || 0
							anchors.verticalCenter: parent.verticalCenter
						}
					}

					Text {
						text: model.createdAt || ""
						font.pixelSize: Style.fontSizeXS
						color: Style.textSecondaryColor
					}
				}

				Rectangle {
					anchors.bottom: parent.bottom
					width: parent.width
					height: 1
					color: Style.separatorColor
					opacity: 0.5
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						ticketListPanelRoot.currentTicketId = model.id;
						ticketListPanelRoot.ticketSelected(model.id);
					}
				}
			}

			ScrollBar.vertical: ScrollBar {}
		}
	}
}
