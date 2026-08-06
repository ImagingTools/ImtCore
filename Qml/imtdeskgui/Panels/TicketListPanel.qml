// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdeskgui 1.0

Rectangle {
	id: ticketListPanelRoot
	objectName: "TicketListPanel"
	color: "transparent"

	property var ticketsModel: null
	property string currentTicketId: ""
	property int statusFilter: -1  // -1 = all, 0 = Open, 1 = Closed

	signal ticketSelected(string ticketId)
	signal newTicketRequested()

	Column {
		anchors.fill: parent
		spacing: 0

		// Panel header
		Rectangle {
			width: parent.width
			height: Style.controlHeightS
			color: Style.alternateBaseColor

			Row {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: Style.paddingM
				anchors.rightMargin: Style.paddingS

				Text {
					text: qsTr("Tickets")
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.imaginToolsAccentColor
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
						color: Style.imaginToolsAccentColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketListPanelRoot.newTicketRequested()
						}
					}
				}
			}
		}

		// Status filter tabs — GitHub Issues style: Open / Closed
		Row {
			width: parent.width
			height: Style.buttonHeightM

			Repeater {
				model: [
					{ label: qsTr("All"),    value: -1 },
					{ label: qsTr("Open"),   value: 0  },
					{ label: qsTr("Closed"), value: 1  }
				]

				Rectangle {
					width: parent.width / 3
					height: Style.buttonHeightM
					color: "transparent"

					Rectangle {
						anchors.bottom: parent.bottom
						width: parent.width
						height: 2
						color: Style.imaginToolsAccentColor
						visible: ticketListPanelRoot.statusFilter === modelData.value
					}

					Text {
						anchors.centerIn: parent
						text: modelData.label
						font.pixelSize: Style.fontSizeXS
						color: ticketListPanelRoot.statusFilter === modelData.value
							? Style.imaginToolsAccentColor
							: Style.buttonInactiveTextColor
						font.bold: ticketListPanelRoot.statusFilter === modelData.value
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketListPanelRoot.statusFilter = modelData.value;
							if (ticketListPanelRoot.ticketsModel) {
								ticketListPanelRoot.ticketsModel.statusFilter = modelData.value;
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
			clip: true
			model: ticketListPanelRoot.ticketsModel
			spacing: 1

			delegate: Rectangle {
				width: ticketListView.width
				height: ticketItemContent.height + Style.paddingS * 2

				Column {
					id: ticketItemContent
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.leftMargin: Style.paddingM
					anchors.rightMargin: Style.paddingS
					spacing: Style.paddingS

					Row {
						width: parent.width
						spacing: Style.paddingS

						// Status icon (colored dot)
						Rectangle {
							width: 10
							height: 10
							radius: 5
							color: (model.status || 0) === 0 ? "#1a7f37" : "#8957e5"
							anchors.verticalCenter: parent.verticalCenter
						}

						// Number
						Text {
							text: "#" + (model.number || "")
							font.pixelSize: Style.fontSizeXS
							color: Style.buttonInactiveTextColor
							anchors.verticalCenter: parent.verticalCenter
							visible: (model.number || 0) > 0
						}

						Text {
							text: model.title || ""
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: Style.imaginToolsAccentColor
							elide: Text.ElideRight
							width: parent.width - priorityBadge.width - Style.paddingS * 4 - 30
							anchors.verticalCenter: parent.verticalCenter
						}

						TicketBadge {
							id: priorityBadge
							badgeType: "priority"
							value: model.priority || 1
							anchors.verticalCenter: parent.verticalCenter
						}
					}

					Text {
						text: model.createdAt || ""
						font.pixelSize: Style.fontSizeXS
						color: Style.buttonInactiveTextColor
					}
				}

				Rectangle {
					anchors.bottom: parent.bottom
					width: parent.width
					height: 1
					color: Style.borderColor2
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
		}
	}
}
