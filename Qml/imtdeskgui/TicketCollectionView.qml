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
import imtdeskTicketCollectionDocumentManagerSdl 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: container

	collectionId: "Tickets"
	gqlGetListCommandId: ImtdeskImtDeskSdlCommandIds.s_ticketsList
	documentCollectionFilter: null

	Component.onCompleted: {
		table.setSortingInfo(TicketItemDataTypeMetaInfo.s_createdAt, "DESC")
		table.nonSortableColumns = [TicketItemDataTypeMetaInfo.s_assignee]
		registerFieldFilterDelegate("statusFilter", statusDelegateFilterComp)
		registerFieldFilterDelegate("priorityFilter", priorityDelegateFilterComp)
		registerFieldFilterDelegate("assignedToMeFilter", assignedToMeDelegateFilterComp)
		registerFieldFilterDelegate("reportedByMeFilter", reportedByMeDelegateFilterComp)
	}

	onHeadersChanged: {
		table.setColumnContentById(TicketItemDataTypeMetaInfo.s_createdAt, createdAtCellDelegateComp)
		table.setColumnContentById(TicketItemDataTypeMetaInfo.s_status, statusCellDelegateComp)
	}

	// Subscribes to server-side ticket message notifications and surfaces
	// them via PopupManager. The server-side filter (CTicketMessageNotifierComp)
	// already restricts delivery to users related to the ticket
	// (reporter / assignees / admin), excluding the sender.
	SubscriptionClient {
		id: ticketMessageSubscription
		gqlCommandId: "OnTicketMessageReceived"
		onMessageReceived: {
			if (!data){
				return
			}
			var ticketNumber = data.containsKey("ticketNumber") ? data.getData("ticketNumber") : ""
			var ticketTitle = data.containsKey("ticketTitle") ? data.getData("ticketTitle") : ""
			var senderName = data.containsKey("senderUserName") ? data.getData("senderUserName") : ""
			var content = data.containsKey("content") ? data.getData("content") : ""
			var messageId = data.containsKey("messageId") ? data.getData("messageId") : ""

			var preview = content ? String(content) : ""
			if (preview.length > 80){
				preview = preview.substring(0, 80) + "…"
			}

			var ticketLabel = ticketNumber ? ("#" + ticketNumber) : ""
			if (ticketTitle){
				ticketLabel = ticketLabel ? (ticketLabel + " " + ticketTitle) : String(ticketTitle)
			}
			var who = senderName ? String(senderName) : qsTr("Someone")
			var header = ticketLabel
				? qsTr("New message in %1 from %2").arg(ticketLabel).arg(who)
				: qsTr("New ticket message from %1").arg(who)
			var text = preview ? (header + ":\n" + preview) : header

			PopupManager.addSuccessMessage(text, true, "TicketMessage_" + messageId)
		}
	}

	Component {
		id: createdAtCellDelegateComp
		TableCellDateDelegate {}
	}

	Component {
		id: statusCellDelegateComp
		TableCellDelegateBase {
			id: statusDelegate

			readonly property var _statusIndex: ({
				"Open": 0,
				"Closed": 1
			})

			onReused: {
				var val = statusDelegate.getValue()
				statusBadge.value = _statusIndex[val] !== undefined ? _statusIndex[val] : 0
			}

			TicketBadge {
				id: statusBadge
				badgeType: "status"
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: statusDelegate && statusDelegate.rowDelegate ? statusDelegate.rowDelegate.textLeftMargin : 0
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

					SubscriptionClient {
						gqlCommandId: container.subscriptionCommandId
						onMessageReceived: {
							if (!data){
								return
							}

							if (!ticketEditor.ticketData){
								return
							}

							let itemId = data.getData("itemId")
							let typeOperation = data.getData("typeOperation")
							if (typeOperation === "updated" && itemId === ticketEditor.ticketData.m_id){
								if (ticketEditor.representationController){
									ticketEditor.representationController.updateRepresentationFromDocument()
								}
							}
						}
					}
				}
			}

			Component {
				id: ticketDataControllerFactory

				DocumentRepresentationController {
					id: root

					representationModel: TicketData {
						m_status: "Open"
						m_ticketType: "SupportRequest"
						m_priority: "Medium"
						m_stateReason: "None"
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
					}

					property DocumentId documentIdInput: DocumentId {}
					property GqlSdlRequestSender getTicketRequest: GqlSdlRequestSender {
						gqlCommandId: ImtdeskTicketCollectionDocumentManagerSdlCommandIds.s_getTicketRepresentation
						sdlObjectComp: Component {
							TicketData {
								onFinished: {
									root.representationModel.copyFrom(this)
									root.representationUpdated(root.documentId, root.representationModel)
								}
							}
						}
					}

					property UpdateTicketInput updateTicketInput: UpdateTicketInput {}
					property GqlSdlRequestSender updateTicketRequest: GqlSdlRequestSender {
						gqlCommandId: ImtdeskTicketCollectionDocumentManagerSdlCommandIds.s_updateTicketFromRepresentation
						requestType: 1
						sdlObjectComp: Component {
							DocumentOperationStatus {
								onFinished: {
									if (m_status === "Success"){
										root.documentUpdated(root.documentId)
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
