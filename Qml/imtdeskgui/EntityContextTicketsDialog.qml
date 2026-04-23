// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtdeskImtDeskSdl 1.0

Dialog {
	id: root

	title: qsTr("Entity Tickets")
	canMove: false
	width: Math.min(ModalDialogManager.activeView.width - 80, 980)
	height: Math.min(ModalDialogManager.activeView.height - 80, 760)

	property string entityType: ""
	property string entityId: ""
	property string entityDisplayName: ""
	readonly property bool hasValidEntityContext: entityType !== "" && entityId !== ""
	readonly property string resolvedEntityDisplayName: entityDisplayName !== "" ? entityDisplayName : entityId

	property bool loading: false
	property int ticketsPageSize: 250
	property int listHeightPadding: 24
	readonly property int defaultOffset: 0
	readonly property int listContentHeight: root.height - (createCol.height + Style.marginM * 4 + root.listHeightPadding)

	Component.onCompleted: {
		addButton(Enums.cancel, qsTr("Close"), true)
	}

	onStarted: {
		reloadTickets()
	}

	contentComp: Component {
		Item {
			width: root.width
			height: root.height - 100

			Column {
				anchors.fill: parent
				anchors.margins: Style.marginM
				spacing: Style.marginM

				Text {
					width: parent.width
					text: qsTr("Context: ") + root.entityType + "/" + root.resolvedEntityDisplayName
					font.pixelSize: Style.fontSizeM
					color: Style.textSecondaryColor
					elide: Text.ElideRight
				}

				Rectangle {
					width: parent.width
					height: createCol.height + Style.marginM * 2
					radius: Style.radiusM
					color: Style.backgroundColor2
					border.color: Style.borderColor

					Column {
						id: createCol
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginM
						spacing: Style.marginS

						Text {
							text: qsTr("Create ticket for this entity")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						TicketTitleDescriptionFields {
							id: ticketFields
							width: parent.width
							titleLabelText: qsTr("Title")
							descriptionLabelText: qsTr("Description")
							titlePlaceholderText: qsTr("Brief summary of the issue")
							descriptionPlaceholderText: qsTr("Describe the issue...")
							minDescriptionHeight: Style.controlHeightM * 3
							maxDescriptionHeight: Style.controlHeightM * 5
						}

						Row {
							anchors.right: parent.right
							spacing: Style.marginS

							Button {
								text: qsTr("Create")
								enabled: ticketFields.titleText.length > 0
								onClicked: {
									root.createTicket(ticketFields.titleText, ticketFields.descriptionText)
								}
							}
						}
					}
				}

				Rectangle {
					width: parent.width
					height: root.listContentHeight
					radius: Style.radiusM
					color: "transparent"
					border.color: Style.borderColor
					border.width: 1

					Item {
						anchors.fill: parent
						anchors.margins: 1

						ListView {
							id: ticketsListView
							anchors.fill: parent
							clip: true
							model: ticketsModel

							delegate: Rectangle {
								width: ticketsListView.width
								height: 48
								color: mouseArea.containsMouse ? Style.backgroundColor2 : "transparent"

								Text {
									anchors.left: parent.left
									anchors.leftMargin: Style.marginM
									anchors.verticalCenter: parent.verticalCenter
									width: parent.width - Style.marginM * 2
									text: "#" + (model.number || "") + " " + (model.title || "")
									font.pixelSize: Style.fontSizeM
									color: Style.textColor
									elide: Text.ElideRight
								}

								MouseArea {
									id: mouseArea
									anchors.fill: parent
									hoverEnabled: true
									onClicked: {
										root.openTicket(model.id)
									}
								}

								Rectangle {
									anchors.bottom: parent.bottom
									width: parent.width
									height: 1
									color: Style.borderColor
									opacity: 0.5
								}
							}
						}

						Text {
							anchors.centerIn: parent
							visible: !root.loading && ticketsModel.count === 0
							text: qsTr("No tickets linked to this entity")
							color: Style.inactiveTextColor
							font.pixelSize: Style.fontSizeM
						}

						Text {
							anchors.centerIn: parent
							visible: root.loading
							text: qsTr("Loading tickets...")
							color: Style.inactiveTextColor
							font.pixelSize: Style.fontSizeM
						}
					}
				}
			}
		}
	}

	ListModel {
		id: ticketsModel
	}

	function reloadTickets() {
		if (!hasValidEntityContext) {
			ticketsModel.clear()
			return
		}

		loading = true
		ticketsModel.clear()

		entityContextTicketsInput.m_entityId = root.entityId
		entityContextTicketsInput.m_entityType = root.entityType

		ticketsListRequest.send(entityContextTicketsInput)
	}

	function openTicket(ticketId) {
		if (!ticketId) {
			return
		}
		MainDocumentManager.openDocument("Tickets", ticketId, "Ticket")
	}

	function createTicket(title, description) {
		if (!title || !hasValidEntityContext) {
			return
		}

		createEntityContextTicketInput.m_entityId = root.entityId
		createEntityContextTicketInput.m_entityType = root.entityType
		createEntityContextTicketInput.m_entityDisplayName = root.resolvedEntityDisplayName
		createEntityContextTicketInput.m_title = title
		createEntityContextTicketInput.m_description = description

		createTicketRequest.send(createEntityContextTicketInput)
	}

	function clearInputFields() {
		ticketFields.clearFields();
	}

	function appendTicketItems(itemsModel) {
		if (!itemsModel) {
			return
		}

		console.log("appendTicketItems", itemsModel.toJson())
		let count = 0
		if (itemsModel.count !== undefined) {
			count = itemsModel.count
		}
		else if (itemsModel.getItemsCount !== undefined) {
			count = itemsModel.getItemsCount()
		}

		for (let i = 0; i < count; ++i) {
			let row = itemsModel.get !== undefined ? itemsModel.get(i) : null
			let item = row && row.item ? row.item : row
			if (!item || !item.m_id) {
				continue
			}

			ticketsModel.append({
								  id: String(item.m_id),
								  number: item.m_number !== undefined ? String(item.m_number) : "",
								  title: item.m_title !== undefined ? String(item.m_title) : ""
							  })
		}
	}

	EntityContextTicketsInput {
		id: entityContextTicketsInput
	}

	GqlSdlRequestSender {
		id: ticketsListRequest
		gqlCommandId: ImtdeskImtDeskSdlCommandIds.s_entityContextTickets

		sdlObjectComp: Component {
			EntityContextTicketsPayload {
				onFinished: {
					ticketsModel.clear()
					root.appendTicketItems(m_items)
					root.loading = false
				}
			}
		}

		onFinished: {
			if (status < 0) {
				root.loading = false
				ticketsModel.clear()
			}
		}
	}

	CreateEntityContextTicketInput {
		id: createEntityContextTicketInput
	}

	GqlSdlRequestSender {
		id: createTicketRequest
		gqlCommandId: ImtdeskImtDeskSdlCommandIds.s_createEntityContextTicket
		requestType: 1

		sdlObjectComp: Component {
			CreateEntityContextTicketPayload {
				onFinished: {
					let createdId = m_id ? String(m_id) : ""
					root.clearInputFields()
					root.reloadTickets()
					if (createdId !== "") {
						root.openTicket(createdId)
					}
				}
			}
		}

	}
}
