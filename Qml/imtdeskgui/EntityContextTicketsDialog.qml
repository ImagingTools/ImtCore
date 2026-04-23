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
	property int descriptionInputHeight: Style.controlHeightM * 3
	property int listHeightPadding: 24
	readonly property int defaultOffset: 0
	readonly property string defaultTicketType: "SupportRequest"
	readonly property string defaultTicketStatus: "Open"
	readonly property string defaultTicketStateReason: "None"
	readonly property string defaultTicketPriority: "Medium"
	readonly property string defaultEntityLinkScheme: "applink"
	readonly property string entityLinkPath: root.entityType + "/" + root.entityId
	readonly property int listContentHeight: root.height - (createCol.height + Style.marginM * 4 + root.listHeightPadding)
	property int _scanIndex: 0
	property int _currentOffset: 0
	property var _ticketQueue: []
	property var _ticketPreviewById: ({})
	property string _currentDetailTicketId: ""

	Component.onCompleted: {
		addButton(Enums.cancel, qsTr("Close"), true)
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

						CustomTextField {
							id: titleInput
							width: parent.width
							height: Style.controlHeightM
							placeHolderText: qsTr("Title")
						}

						Rectangle {
							width: parent.width
							height: root.descriptionInputHeight
							radius: Style.radiusS
							color: Style.baseColor
							border.color: Style.borderColor

							TextEdit {
								id: descriptionInput
								anchors.fill: parent
								anchors.margins: Style.marginS
								wrapMode: TextEdit.Wrap
								font.pixelSize: Style.fontSizeM
								color: Style.textColor
							}
						}

						Row {
							anchors.right: parent.right
							spacing: Style.marginS

							Button {
								width: Style.buttonWidthL
								height: Style.controlHeightM
								text: qsTr("Create")
								enabled: titleInput.text.trim().length > 0
								onClicked: {
									root.createTicket(titleInput.text.trim(), descriptionInput.text.trim())
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

		_resetTicketState()
		loading = true
		_currentOffset = defaultOffset
		ticketsModel.clear()
		ticketsListRequest.send({offset: _currentOffset})
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
		let ticketId = UuidGenerator.generateUUID()
		createTicketRequest.send({
								   id: ticketId,
								   title: title,
								   description: description
							   })
	}

	function clearInputFields() {
		titleInput.text = ""
		descriptionInput.text = ""
	}

	function _resetTicketState() {
		_scanIndex = 0
		_ticketQueue = []
		_ticketPreviewById = {}
		_currentDetailTicketId = ""
	}

	function _matchCurrentEntity(entityRefsModel) {
		if (!entityRefsModel) {
			return false
		}

		for (let i = 0; i < entityRefsModel.getItemsCount(); ++i) {
			let refType = String(entityRefsModel.getData("entityType", i) || "")
			let refId = String(entityRefsModel.getData("entityId", i) || "")
			if (refType === root.entityType && refId === root.entityId) {
				return true
			}
		}

		return false
	}

	function _scanNextTicketDetails() {
		if (_scanIndex >= _ticketQueue.length) {
			loading = false
			return
		}

		_currentDetailTicketId = _ticketQueue[_scanIndex]
		ticketItemRequest.send({id: _currentDetailTicketId})
	}

	GqlRequestSender {
		id: ticketsListRequest
		gqlCommandId: "TicketsList"

		function createQueryParams(query, params) {
			let input = Gql.GqlObject("input")
			let offset = params && params.offset !== undefined ? params.offset : root.defaultOffset
			input.fromObject({
							 viewParams: {
								 count: root.ticketsPageSize,
								 offset: offset
							 }
						 })
			query.AddParam(input)

			let items = Gql.GqlObject("items")
			items.InsertField("id")
			items.InsertField("number")
			items.InsertField("title")
			query.AddField(items)
		}

		function onResult(data) {
			let loadedCount = 0
			if (data && data.containsKey("items")) {
				let itemsModel = data.getData("items")
				loadedCount = itemsModel.getItemsCount()
				for (let i = 0; i < loadedCount; ++i) {
					let tid = String(itemsModel.getData("id", i) || "")
					if (tid === "") {
						continue
					}

					root._ticketQueue.push(tid)
					root._ticketPreviewById[tid] = {
						id: tid,
						number: String(itemsModel.getData("number", i) || ""),
						title: String(itemsModel.getData("title", i) || "")
					}
				}
			}

			if (loadedCount >= root.ticketsPageSize) {
				root._currentOffset += root.ticketsPageSize
				ticketsListRequest.send({offset: root._currentOffset})
				return
			}

			root._scanIndex = 0
			root._scanNextTicketDetails()
		}

		function onError(message, type) {
			root._resetTicketState()
			root.loading = false
			ticketsModel.clear()
			ModalDialogManager.showErrorDialog(message || qsTr("Failed to load tickets"))
		}
	}

	GqlRequestSender {
		id: ticketItemRequest
		gqlCommandId: "TicketItem"

		function createQueryParams(query, params) {
			let input = Gql.GqlObject("input")
			input.fromObject({id: params.id})
			query.AddParam(input)

			let refs = Gql.GqlObject("entityReferences")
			refs.InsertField("entityType")
			refs.InsertField("entityId")
			query.AddField(refs)
		}

		function onResult(data) {
			if (data && data.containsKey("entityReferences")) {
				let refsModel = data.getData("entityReferences")
				if (root._matchCurrentEntity(refsModel)) {
					let item = root._ticketPreviewById[root._currentDetailTicketId]
					if (item) {
						ticketsModel.append(item)
					}
				}
			}

			root._scanIndex += 1
			root._scanNextTicketDetails()
		}

		function onError(message, type) {
			root._scanIndex += 1
			root._scanNextTicketDetails()
		}
	}

	GqlRequestSender {
		id: createTicketRequest
		gqlCommandId: "TicketCreate"
		requestType: 1

		function createQueryParams(query, params) {
			let ticketId = String(params.id)
			let input = Gql.GqlObject("input")
			input.fromObject({
							 id: ticketId,
							 item: {
								 id: ticketId,
								 title: String(params.title || ""),
								 description: String(params.description || ""),
								 ticketType: root.defaultTicketType,
								 status: root.defaultTicketStatus,
								 stateReason: root.defaultTicketStateReason,
								 priority: root.defaultTicketPriority,
								 entityReferences: [{
									 entityType: root.entityType,
									 entityId: root.entityId,
									 displayName: root.resolvedEntityDisplayName,
									 entityLink: {
										 id: root.entityId,
										 typeId: root.entityType,
										 name: root.resolvedEntityDisplayName,
										 url: {
											 scheme: root.defaultEntityLinkScheme,
											 path: root.entityLinkPath
										 }
									 }
								 }]
							 }
						 })
			query.AddParam(input)

			let idField = Gql.GqlObject("id")
			query.AddField(idField)
		}

		function onResult(data) {
			root.clearInputFields()
			root.reloadTickets()
			if (data && data.containsKey("id")) {
				let createdId = String(data.getData("id") || "")
				if (createdId !== "") {
					root.openTicket(createdId)
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showErrorDialog(message || qsTr("Failed to create ticket"))
		}
	}
}
