// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtdeskImtDeskSdl 1.0
import imtdeskTicketCollectionDocumentServiceSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: container

	property string permissionPath: ""
	collectionId: "Tickets"
	gqlGetListCommandId: ImtdeskImtDeskSdlCommandIds.s_ticketsList
	documentCollectionFilter: null

	Component.onCompleted: {
		table.setSortingInfo(TicketItemDataTypeMetaInfo.s_createdAt, "DESC")
		table.nonSortableColumns = [TicketItemDataTypeMetaInfo.s_assignee]
		registerFieldFilterDelegate("statusFilter", statusDelegateFilterComp)
		registerFieldFilterDelegate("priorityFilter", priorityDelegateFilterComp)
		registerFieldFilterDelegate("assignedToMeFilter", assignedToMeDelegateFilterComp)
		registerFieldFilterDelegate("withoutAssigneesFilter", withoutAssigneesDelegateFilterComp)
		registerFieldFilterDelegate("reportedByMeFilter", reportedByMeDelegateFilterComp)
	}

	onHeadersChanged: {
		table.setColumnContentById(TicketItemDataTypeMetaInfo.s_createdAt, createdAtCellDelegateComp)
		table.setColumnContentById(TicketItemDataTypeMetaInfo.s_status, statusCellDelegateComp)
		table.setColumnContentById(TicketItemDataTypeMetaInfo.s_priority, priorityCellDelegateComp)

		if (table.headers.getItemsCount() > 0){
			container.applyColumnWidths()
		}
	}

	// The number is short and the title is what people read: fixed widths for the rest, title takes what is left.
	function applyColumnWidths(){
		ticketTableDecoratorModel.clear()
		let cellWidthModel = ticketTableDecoratorModel.addTreeModel("CellWidth")
		for (let i = 0; i < table.headers.getItemsCount(); i++){
			let headerId = String(table.getHeaderId(i))
			let width = -1
			if (headerId === String(TicketItemDataTypeMetaInfo.s_number)){
				width = Style.buttonWidthXL
			}
			else if (headerId === String(TicketItemDataTypeMetaInfo.s_status) || headerId === String(TicketItemDataTypeMetaInfo.s_priority)){
				width = Style.sizeHintXXXS
			}
			else if (headerId === String(TicketItemDataTypeMetaInfo.s_assignee) || headerId === String(TicketItemDataTypeMetaInfo.s_createdAt)){
				width = Style.sizeHintXXS
			}
			let index = cellWidthModel.insertNewItem()
			cellWidthModel.setData("Width", width, index)
		}
		table.tableDecorator = ticketTableDecoratorModel
	}

	TreeItemModel {
		id: ticketTableDecoratorModel
	}


	Component {
		id: createdAtCellDelegateComp
		TableCellDateDelegate {}
	}

	Component {
		id: priorityCellDelegateComp
		TableCellDelegateBase {
			id: priorityDelegate

			readonly property var _priorityColors: ({
				"Low": Style.greenColor,
				"Medium": Style.middleAccentColor,
				"High": Style.errorTextColor,
				"Critical": Style.negativeAccentColor
			})

			onReused: {
				var val = priorityDelegate.getValue()
				priorityLabel.text = val ? qsTr(String(val)) : ""
				priorityCircle.color = _priorityColors[val] || Style.buttonInactiveTextColor
			}

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				spacing: Style.spacingS

				Rectangle {
					id: priorityCircle
					anchors.verticalCenter: parent.verticalCenter
					width: 10
					height: 10
					radius: width / 2
					color: Style.buttonInactiveTextColor
				}

				Text {
					id: priorityLabel
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
				}
			}
		}
	}

	Component {
		id: statusCellDelegateComp
		TableCellDelegateBase {
			id: statusDelegate

			onReused: {
				var val = String(statusDelegate.getValue())
				statusBadge.value = val === String("Open") ? 0 : 1
			}

			TicketBadge {
				id: statusBadge
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				height: 25
				badgeType: "status"
			}
		}
	}

	Component {
		id: statusDelegateFilterComp
		FieldFilterDelegate {
			name: qsTr("Status")
			defaultFieldFilter.m_fieldId: "Status"
			defaultFieldFilter.m_filterValueType: "Integer"

			Component.onCompleted: {
				createAndAddOption("0", qsTr("Open"), "", true)
				createAndAddOption("1", qsTr("Closed"), "", true)
			}
		}
	}

	Component {
		id: priorityDelegateFilterComp
		FieldFilterDelegate {
			name: qsTr("Priority")
			defaultFieldFilter.m_fieldId: "Priority"
			defaultFieldFilter.m_filterValueType: "Integer"

			Component.onCompleted: {
				createAndAddOption("0", qsTr("Low"), "", true)
				createAndAddOption("1", qsTr("Medium"), "", true)
				createAndAddOption("2", qsTr("High"), "", true)
				createAndAddOption("3", qsTr("Critical"), "", true)
			}
		}
	}

	Component {
		id: assignedToMeDelegateFilterComp
		EnableableFilterDelegate {
			name: qsTr("Assigned to me")
			defaultFieldFilter.m_fieldId: "AssigneeIds"
			defaultFieldFilter.m_filterValueType: "String"
			defaultFieldFilter.m_filterOperations: ["Contains"]
			filterValue: AuthorizationController.getUserId()
		}
	}

	Component {
		id: withoutAssigneesDelegateFilterComp
		EnableableFilterDelegate {
			name: qsTr("Without assignees")
			defaultFieldFilter.m_fieldId: "AssigneeIds"
			defaultFieldFilter.m_filterValueType: "String"
			defaultFieldFilter.m_filterOperations: ["Equal"]
			filterValue: ""
			allowEmptyFilterValue: true
		}
	}

	Component {
		id: reportedByMeDelegateFilterComp
		EnableableFilterDelegate {
			name: qsTr("Reported by me")
			defaultFieldFilter.m_fieldId: "ReporterId"
			defaultFieldFilter.m_filterValueType: "String"
			defaultFieldFilter.m_filterOperations: ["Equal"]
			filterValue: AuthorizationController.getUserId()
		}
	}

	commandsDelegateComp: Component {
		DocCollectionViewDelegate {
			collectionView: container

			Component.onCompleted: {
				registerDocumentType("Ticket", qsTr("Ticket"))
				addDocumentView("Ticket", "TicketEditor", ticketEditorComp, ticketDataControllerFactory)
			}

			Component {
				id: ticketEditorComp

				TicketEditor {
					id: ticketEditor
					commandsControllerComp: Component {
						id: ticketGqlCommandsComp
						GqlBasedCommandsController {
							typeId: "Ticket"
						}
					}
					commandsPanelVisible: isNewIssue

					onDocumentSaved: {
						if (isNewIssue){
							if (ticketEditor.representationController){
								ticketEditor.representationController.updateRepresentationFromDocument()
							}
						}
					}
				}
			}

			Component {
				id: ticketDataControllerFactory

				DocumentRepresentationController {
					id: root

					// A refused change stays in the editor
					readBackOnRefusal: false

					// The context types a new ticket can link to only come with the server representation.
					requestRepresentationOnCreate: true

					representationModel: TicketData {
						m_status: "Open"
						m_ticketType: "SupportRequest"
						m_priority: "Medium"
						m_stateReason: "None"
						m_accessLevel: "FullAccess"
					}

					function updateRepresentationFromDocument(){
						startUpdateRepresentation(documentId, representationModel)

						documentIdInput.m_id = documentId
						documentIdInput.m_collectionId = container.collectionId

						getTicketRequest.send(documentIdInput)
					}

					function updateDocumentFromRepresentation(){
						startUpdateDocument(documentId)

						updateTicketInput.m_documentId = documentId
						updateTicketInput.m_ticket = representationModel

						updateTicketRequest.send(updateTicketInput)
						root.markSentComments()
					}

					// A new comment is sent once: once its request is written, it gets a local ID, and the server skips comments with one.
					// The ID is given here, not when the comment is added: a queued update is written later (see DocumentDecorator).
					function markSentComments(){
						let comments = representationModel ? representationModel.m_comments : null
						if (!comments){
							return
						}

						for (let i = 0; i < comments.count; ++i){
							let comment = comments.get(i).item
							if (comment && !comment.m_id){
								comment.enableNotifications = false
								comment.m_id = "local-" + Date.now().toString(36) + Math.random().toString(36).slice(2, 8)
								comment.enableNotifications = true
							}
						}
					}

					property DocumentId documentIdInput: DocumentId {}
					property DocumentRepresentationRequestSender getTicketRequest: DocumentRepresentationRequestSender {
						representationController: root
						permissionPath: container.permissionPath
						gqlCommandId: ImtdeskTicketCollectionDocumentServiceSdlCommandIds.s_getTicketRepresentation
						sdlObjectComp: Component {
							TicketData {}
						}
					}

					property UpdateTicketInput updateTicketInput: UpdateTicketInput {}
					property DocumentUpdateRequestSender updateTicketRequest: DocumentUpdateRequestSender {
						representationController: root
						permissionPath: container.permissionPath
						gqlCommandId: ImtdeskTicketCollectionDocumentServiceSdlCommandIds.s_updateTicketFromRepresentation
						refusalMessage: ""
					}
				}
			}
		}
	}
}
