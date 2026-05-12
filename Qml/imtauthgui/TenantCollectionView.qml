// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtauthTenantsSdl 1.0
import imtauthTenantMembershipsSdl 1.0
import imtauthTenantCollectionDocumentManagerSdl 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: container

	collectionId: "Tenants"
	gqlGetListCommandId: ImtauthTenantsSdlCommandIds.s_getTenantList
	documentCollectionFilter: null

	// --- Pending invitations for the current user ---
	property var myPendingInvitations: []

	function loadMyPendingInvitations() {
		getMyInvitationsInput.m_statuses = ["Pending"]
		getMyInvitationsSender.send(getMyInvitationsInput)
	}

	function __removeMyInvitation(invitationId) {
		var updated = []
		for (var i = 0; i < container.myPendingInvitations.length; i++) {
			if (container.myPendingInvitations[i].id !== invitationId)
				updated.push(container.myPendingInvitations[i])
		}
		container.myPendingInvitations = updated
	}

	property GetMyTenantInvitationsInput getMyInvitationsInput: GetMyTenantInvitationsInput {}
	property GqlSdlRequestSender getMyInvitationsSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_getMyTenantInvitations
		sdlObjectComp: Component {
			GetMyTenantInvitationsPayload {
				onFinished: {
					var invitations = []
					if (m_invitations) {
						var count = m_invitations.count || 0
						for (var i = 0; i < count; i++) {
							var inv = m_invitations.get(i).item
							if (inv) {
								invitations.push({
									id: inv.m_id || "",
									tenantId: inv.m_tenantId || "",
									tenantName: inv.m_tenantName || inv.m_tenantId || "",
									role: inv.m_role || "Member",
									createdAt: inv.m_createdAt || ""
								})
							}
						}
					}
					container.myPendingInvitations = invitations
				}
			}
		}
	}

	property AcceptTenantInvitationInput acceptInvitationInput: AcceptTenantInvitationInput {}
	property GqlSdlRequestSender acceptInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_acceptTenantInvitation
		sdlObjectComp: Component {
			AcceptTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						container.doUpdateGui()
					}
					else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	property RejectTenantInvitationInput rejectInvitationInput: RejectTenantInvitationInput {}
	property GqlSdlRequestSender rejectInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_rejectTenantInvitation
		sdlObjectComp: Component {
			RejectTenantInvitationPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	Component.onCompleted: {
		table.setSortingInfo(TenantItemDataTypeMetaInfo.s_createdAt, "DESC")
		table.nonSortableColumns = [TenantItemDataTypeMetaInfo.s_ownerId, TenantItemDataTypeMetaInfo.s_isActive, TenantItemDataTypeMetaInfo.s_membersCount]
		registerFieldFilterDelegate("isActiveFilter", isActiveDelegateFilterComp)
		registerFieldFilterDelegate("tenantRelationFilter", tenantRelationDelegateFilterComp)
		loadMyPendingInvitations()
	}

	// --- Pending Invitations Panel (above the table) ---
	Item {
		id: pendingInvitationsPanel
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		height: visible ? pendingInvitationsColumn.height + 2 * Style.marginM : 0
		visible: container.myPendingInvitations.length > 0
		z: 10

		Rectangle {
			anchors.fill: parent
			color: Style.baseColor
			border.color: Style.borderColor
			border.width: 1
		}

		Column {
			id: pendingInvitationsColumn
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.margins: Style.marginM
			spacing: Style.marginS

			BaseText {
				font.pixelSize: Style.fontSizeL
				font.bold: true
				text: qsTr("Pending Invitations")
			}

			Repeater {
				model: container.myPendingInvitations

				delegate: Rectangle {
					width: pendingInvitationsColumn.width
					height: Style.controlHeightL
					color: invitationMouseArea.containsMouse ? Style.hoveredColor : "transparent"
					radius: Style.radiusS

					MouseArea {
						id: invitationMouseArea
						anchors.fill: parent
						hoverEnabled: true
					}

					Row {
						anchors.fill: parent
						anchors.leftMargin: Style.marginM
						anchors.rightMargin: Style.marginM
						spacing: Style.marginL

						BaseText {
							width: (parent.width - acceptBtn.width - rejectBtn.width - parent.spacing * 3) * 0.5
							anchors.verticalCenter: parent.verticalCenter
							text: modelData.tenantName
							elide: Text.ElideRight
							font.pixelSize: Style.fontSizeM
						}

						BaseText {
							width: (parent.width - acceptBtn.width - rejectBtn.width - parent.spacing * 3) * 0.5
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Role: %1").arg(modelData.role)
							elide: Text.ElideRight
							font.pixelSize: Style.fontSizeM
							color: Style.secondaryTextColor
						}

						Button {
							id: acceptBtn
							anchors.verticalCenter: parent.verticalCenter
							width: Style.buttonWidthL
							height: Style.controlHeightM
							text: qsTr("Accept")

							onClicked: {
								container.acceptInvitationInput.m_invitationId = modelData.id
								container.acceptInvitationSender.send(container.acceptInvitationInput)
								container.__removeMyInvitation(modelData.id)
							}
						}

						Button {
							id: rejectBtn
							anchors.verticalCenter: parent.verticalCenter
							width: Style.buttonWidthL
							height: Style.controlHeightM
							text: qsTr("Reject")

							onClicked: {
								container.rejectInvitationInput.m_invitationId = modelData.id
								container.rejectInvitationSender.send(container.rejectInvitationInput)
								container.__removeMyInvitation(modelData.id)
							}
						}
					}
				}
			}
		}
	}

	onHeadersChanged: {
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_membersCount, membersCountCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_ownerId, ownerIdCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_createdAt, createdAtCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_updatedAt, updatedAtCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_isActive, isActiveCellDelegateComp)
	}

	Component {
		id: membersCountCellDelegateComp
		TableCellDelegateBase {
			id: membersCountDelegate

			onReused: {
				var val = membersCountDelegate.getValue()
				membersCountLabel.text = val !== undefined && val !== null ? val.toString() : "0"
			}

			Text {
				id: membersCountLabel
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
			}
		}
	}

	Component {
		id: ownerIdCellDelegateComp
		TableCellDelegateBase {
			id: ownerIdDelegate

			onReused: {
				var val = ownerIdDelegate.getValue()
				ownerIdLabel.text = val !== undefined && val !== null ? val.toString() : ""
			}

			Text {
				id: ownerIdLabel
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				width: parent.width - 2 * Style.marginM
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
				elide: Text.ElideRight
			}
		}
	}

	Component {
		id: createdAtCellDelegateComp
		TableCellDateDelegate {}
	}

	Component {
		id: updatedAtCellDelegateComp
		TableCellDateDelegate {}
	}

	Component {
		id: isActiveCellDelegateComp
		TableCellDelegateBase {
			id: isActiveDelegate

			onReused: {
				var val = isActiveDelegate.getValue()
				isActiveLabel.text = val ? qsTr("Active") : qsTr("Inactive")
				isActiveIndicator.color = val ? "#3FB950" : "#8C95A6"
			}

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				spacing: Style.spacingS

				Rectangle {
					id: isActiveIndicator
					anchors.verticalCenter: parent.verticalCenter
					width: 10
					height: 10
					radius: width / 2
					color: "#8C95A6"
				}

				Text {
					id: isActiveLabel
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
				}
			}
		}
	}

	Component {
		id: isActiveDelegateFilterComp
		FieldFilterDelegate {
			name: qsTr("Status")
			defaultFieldFilter.m_fieldId: "IsActive"
			defaultFieldFilter.m_filterValueType: "Integer"

			Component.onCompleted: {
				createAndAddOption("1", qsTr("Active"), "", true)
				createAndAddOption("0", qsTr("Inactive"), "", true)
			}
		}
	}

	Component {
		id: tenantRelationDelegateFilterComp
		FieldFilterDelegate {
			name: qsTr("My tenants")

			Component.onCompleted: {
				createAndAddOption("owner", qsTr("I am owner"), "", true)
				createAndAddOption("member", qsTr("I am member"), "", true)
				setFieldFilterForOption("owner", ownerFieldFilterComp.createObject(this))
				setFieldFilterForOption("member", memberFieldFilterComp.createObject(this))
			}

			Component {
				id: ownerFieldFilterComp
				FieldFilter {
					m_fieldId: "TenantRelationScope"
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
					m_filterValue: "Owner"
				}
			}

			Component {
				id: memberFieldFilterComp
				FieldFilter {
					m_fieldId: "TenantRelationScope"
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
					m_filterValue: "Member"
				}
			}
		}
	}

	commandsDelegateComp: Component {
		DocCollectionViewDelegate {
			collectionView: container

			Component.onCompleted: {
				registerDocumentType("Tenant", qsTr("Tenant"))
				addDocumentView("Tenant", "TenantEditor", tenantEditorComp, tenantDataControllerFactory)
			}

			Component {
				id: tenantEditorComp

				TenantEditor {
					id: tenantEditor
					commandsControllerComp: Component {
						GqlBasedCommandsController {
							typeId: "Tenant"
						}
					}
					commandsPanelVisible: isNewTenant
					onDocumentSaved: {
						if (isNewTenant){
							if (tenantEditor.representationController){
								tenantEditor.representationController.updateRepresentationFromDocument()
							}
						}
					}
				}
			}

			Component {
				id: tenantDataControllerFactory

				DocumentRepresentationController {
					id: root

					representationModel: TenantData {
						m_isActive: true
					}

					function updateRepresentationFromDocument(){
						startUpdateRepresentation(documentId, representationModel)

						documentIdInput.m_id = documentId
						documentIdInput.m_collectionId = container.collectionId

						getTenantRequest.send(documentIdInput)
					}

					function updateDocumentFromRepresentation(){
						startUpdateDocument(documentId)

						updateTenantInput.m_documentId = documentId
						updateTenantInput.m_tenant = representationModel

						updateTenantRequest.send(updateTenantInput)
					}

					property DocumentId documentIdInput: DocumentId {}
					property GqlSdlRequestSender getTenantRequest: GqlSdlRequestSender {
						gqlCommandId: ImtauthTenantCollectionDocumentManagerSdlCommandIds.s_getTenantRepresentation
						sdlObjectComp: Component {
							TenantData {
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

					property UpdateTenantFromRepresentationInput updateTenantInput: UpdateTenantFromRepresentationInput {}
					property GqlSdlRequestSender updateTenantRequest: GqlSdlRequestSender {
						gqlCommandId: ImtauthTenantCollectionDocumentManagerSdlCommandIds.s_updateTenantFromRepresentation
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
