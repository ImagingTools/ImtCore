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
				"Low": "#3FB950",       // green
				"Medium": "#D29922",    // amber
				"High": "#DB6D28",      // orange
				"Critical": "#F85149"   // red
			})

			onReused: {
				var val = priorityDelegate.getValue()
				priorityLabel.text = val ? qsTr(String(val)) : ""
				priorityCircle.color = _priorityColors[val] || "#8C95A6"
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
					color: "#8C95A6"
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
					}

					property DocumentId documentIdInput: DocumentId {}
					property GqlSdlRequestSender getTicketRequest: GqlSdlRequestSender {
						gqlCommandId: ImtdeskTicketCollectionDocumentServiceSdlCommandIds.s_getTicketRepresentation
						sdlObjectComp: Component {
							TicketData {
								onFinished: {
									root.representationModel.copyFrom(this)
									root.representationUpdated(root.documentId, root.representationModel)
								}
							}
						}

						function onError(message, type){
							root.updateRepresentationFailed(root.documentId, message)
						}
					}

					property UpdateTicketInput updateTicketInput: UpdateTicketInput {}
					property GqlSdlRequestSender updateTicketRequest: GqlSdlRequestSender {
						gqlCommandId: ImtdeskTicketCollectionDocumentServiceSdlCommandIds.s_updateTicketFromRepresentation
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

						function onError(message, type){
							root.updateDocumentFailed(root.documentId, message)
						}
					}
				}
			}
		}
	}
}
