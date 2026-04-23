// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtdeskImtDeskSdl 1.0

Dialog {
	id: root

	title: qsTr("Entity Tickets") + " — " + entityType + " / " + resolvedEntityDisplayName
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

	// TicketEditor-matching design constants
	readonly property real cardPadding: 16
	readonly property real cardRadius: Style.radiusL
	readonly property string cardColor: Style.baseColor
	readonly property string cardBorderColor: Style.borderColor
	readonly property string pageBgColor: Style.backgroundColor2
	readonly property string accentColor: Style.imaginToolsAccentColor
	readonly property string sectionLabelColor: "#8C95A6"

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

			// Page background matching TicketEditor
			Rectangle {
				anchors.fill: parent
				color: root.pageBgColor
			}

			Item {
				anchors.fill: parent
				anchors.margins: Style.marginL

				// Create ticket card (TicketEditor detailsCard style)
				Item {
					id: createCardWrapper
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					height: createCard.height

					Rectangle {
						id: createCard
						width: parent.width
						height: createCardCol.height + root.cardPadding * 2
						radius: root.cardRadius
						color: root.cardColor
						border.color: root.cardBorderColor
						border.width: 1

						Column {
							id: createCardCol
							x: root.cardPadding
							y: root.cardPadding
							width: parent.width - root.cardPadding * 2
							spacing: Style.spacingM

							Text {
								text: qsTr("Create ticket for this entity")
								font.pixelSize: Style.fontSizeL
								font.bold: true
								color: Style.textColor
							}

							// Title
							Column {
								width: parent.width
								spacing: 4

								Text {
									text: qsTr("Title")
									font.pixelSize: Style.fontSizeM
									font.bold: true
									color: root.sectionLabelColor
								}

								CustomTextField {
									id: editTitleInput
									width: parent.width
									height: Style.controlHeightM
									placeHolderText: qsTr("Enter ticket title...")
								}
							}

							// Description
							Column {
								width: parent.width
								spacing: 4

								Text {
									text: qsTr("Description")
									font.pixelSize: Style.fontSizeM
									font.bold: true
									color: root.sectionLabelColor
								}

								Rectangle {
									width: parent.width
									height: Math.min(160, Math.max(60, editDescriptionInput.contentHeight)) + Style.paddingM * 2
									radius: Style.radiusM
									border.color: editDescriptionInput.activeFocus ? root.accentColor : root.cardBorderColor
									border.width: editDescriptionInput.activeFocus ? 2 : 1
									color: editDescriptionInput.activeFocus ? root.cardColor : "#FAFBFC"

									Flickable {
										id: descriptionFlick
										anchors.fill: parent
										anchors.margins: Style.paddingM
										anchors.rightMargin: Style.paddingM + Style.marginM
										contentWidth: width
										contentHeight: editDescriptionInput.height
										clip: true
										boundsBehavior: Flickable.StopAtBounds

										TextEdit {
											id: editDescriptionInput
											width: descriptionFlick.width
											height: Math.max(60, contentHeight)
											font.pixelSize: Style.fontSizeM
											color: Style.textColor
											wrapMode: TextEdit.Wrap
											textFormat: TextEdit.PlainText

											onCursorRectangleChanged: {
												var cy = cursorRectangle.y
												var ch = cursorRectangle.height
												if (cy < descriptionFlick.contentY) {
													descriptionFlick.contentY = cy
												} else if (cy + ch > descriptionFlick.contentY + descriptionFlick.height) {
													descriptionFlick.contentY = cy + ch - descriptionFlick.height
												}
											}

											Text {
												anchors.fill: parent
												text: qsTr("Describe the issue...")
												color: Style.inactiveTextColor
												font.pixelSize: Style.fontSizeM
												visible: editDescriptionInput.text.length === 0
											}
										}
									}

									CustomScrollbar {
										z: parent.z + 1
										anchors.right: parent.right
										anchors.rightMargin: 2
										anchors.top: parent.top
										anchors.bottom: parent.bottom
										anchors.topMargin: 2
										anchors.bottomMargin: 2
										secondSize: Style.marginM
										targetItem: descriptionFlick
										visible: descriptionFlick.contentHeight > descriptionFlick.height
									}
								}
							}
							
							Button {
								id: createButton
								text: qsTr("Create Ticket")
								enabled: editTitleInput.text.trim().length > 0
								decorator: Component {
									ButtonDecorator {
										color: createButton.enabled ? Style.imaginToolsAccentColor : Style.baseColor
										textColor: createButton.enabled ? "white" : Style.inactiveTextColor
										opacity: createButton.hovered ? 0.85 : 1
									}
								}

								onClicked: {
									root.createTicket(editTitleInput.text, editDescriptionInput.text)
								}
							}
						}
					}

					DropShadow {
						anchors.fill: createCard
						z: createCard.z - 1
						horizontalOffset: 3
						verticalOffset: 3
						radius: Style.radiusL
						spread: 0
						color: Style.shadowColor
						source: createCard
					}
				}

				// Tickets list card (same card style)
				Item {
					id: listCardWrapper
					anchors.top: createCardWrapper.bottom
					anchors.topMargin: Style.spacingL
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom

					Rectangle {
						id: listCard
						anchors.fill: parent
						radius: root.cardRadius
						color: root.cardColor
						border.color: root.cardBorderColor
						border.width: 1

						Column {
							id: listCardCol
							anchors.fill: parent
							anchors.margins: root.cardPadding
							spacing: Style.spacingS

							// Header row
							Row {
								id: listHeaderRow
								width: parent.width
								spacing: Style.spacingS

								Text {
									text: qsTr("Linked Tickets")
									font.pixelSize: Style.fontSizeL
									font.bold: true
									color: Style.textColor
									anchors.verticalCenter: parent.verticalCenter
								}

								Rectangle {
									visible: ticketsModel.count > 0
									width: ticketCountText.contentWidth + Style.paddingS * 2
									height: 22
									radius: 11
									color: "#E5F0FB"
									anchors.verticalCenter: parent.verticalCenter

									Text {
										id: ticketCountText
										anchors.centerIn: parent
										text: ticketsModel.count
										font.pixelSize: Style.fontSizeM - 1
										font.bold: true
										color: root.accentColor
									}
								}
							}

							// Divider
							Rectangle {
								id: listDivider
								width: parent.width
								height: 1
								color: root.cardBorderColor
								opacity: 0.6
							}

							// ListView area
							Item {
								width: parent.width
								height: parent.height - listHeaderRow.height - listDivider.height - Style.spacingS * 2

								ListView {
									id: ticketsListView
									anchors.fill: parent
									clip: true
									model: ticketsModel

									delegate: Rectangle {
										width: ticketsListView.width
										height: ticketDelegateCol.height + Style.paddingS * 2
										color: ticketDelegateMa.containsMouse ? "#F6F8FC" : "transparent"
										radius: Style.radiusS

										Column {
											id: ticketDelegateCol
											anchors.left: parent.left
											anchors.right: parent.right
											anchors.verticalCenter: parent.verticalCenter
											anchors.leftMargin: Style.paddingM
											anchors.rightMargin: Style.paddingM
											spacing: 2

											Row {
												width: parent.width
												spacing: Style.spacingS

												Text {
													text: "#" + (model.number || "")
													font.pixelSize: Style.fontSizeM
													font.bold: true
													color: root.accentColor
												}

												Text {
													text: model.title || ""
													font.pixelSize: Style.fontSizeM
													color: Style.textColor
													elide: Text.ElideRight
													width: parent.width - 80
												}
											}

											Row {
												width: parent.width
												spacing: Style.spacingM
												visible: (model.status || "") !== "" || (model.priority || "") !== ""

												Text {
													visible: (model.status || "") !== ""
													text: model.status || ""
													font.pixelSize: Style.fontSizeS
													color: root.sectionLabelColor
												}

												Text {
													visible: (model.priority || "") !== ""
													text: model.priority || ""
													font.pixelSize: Style.fontSizeS
													color: root.sectionLabelColor
												}

												Text {
													visible: (model.assignee || "") !== ""
													text: model.assignee || ""
													font.pixelSize: Style.fontSizeS
													color: root.sectionLabelColor
													elide: Text.ElideRight
													width: parent.width * 0.4
												}
											}
										}

										MouseArea {
											id: ticketDelegateMa
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: {
												root.openTicket(model.id)
											}
										}

										Rectangle {
											anchors.bottom: parent.bottom
											anchors.left: parent.left
											anchors.right: parent.right
											anchors.leftMargin: Style.paddingM
											anchors.rightMargin: Style.paddingM
											height: 1
											color: root.cardBorderColor
											opacity: 0.3
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

					DropShadow {
						anchors.fill: listCard
						z: listCard.z - 1
						horizontalOffset: 3
						verticalOffset: 3
						radius: Style.radiusL
						spread: 0
						color: Style.shadowColor
						source: listCard
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

		NavigationController.navigate("Tickets/Ticket/" + ticketId)
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
		try {
			editTitleInput.text = ""
			editDescriptionInput.text = ""
		} catch(e) {
			console.warn("EntityContextTicketsDialog: clearInputFields failed:", e)
		}
	}

	function appendTicketItems(itemsModel) {
		if (!itemsModel) {
			return
		}

		let count = 0
		if (itemsModel.count !== undefined) {
			count = itemsModel.count
		} else if (itemsModel.getItemsCount !== undefined) {
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
				title: item.m_title !== undefined ? String(item.m_title) : "",
				status: item.m_status !== undefined ? String(item.m_status) : "",
				priority: item.m_priority !== undefined ? String(item.m_priority) : "",
				assignee: item.m_assignee !== undefined ? String(item.m_assignee) : ""
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
					root.clearInputFields()
					root.reloadTickets()

					root.openTicket(m_id)
				}
			}
		}
	}
}
