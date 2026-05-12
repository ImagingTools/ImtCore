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
import imtauthTenantCollectionDocumentManagerSdl 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtauthTenantMembershipsSdl 1.0

RemoteCollectionView {
	id: container

	collectionId: "Tenants"
	gqlGetListCommandId: ImtauthTenantsSdlCommandIds.s_getTenantList
	documentCollectionFilter: null
	additionalFieldIds: ["id", "name", TenantItemDataTypeMetaInfo.s_tenantRelationScope, TenantItemDataTypeMetaInfo.s_invitationId, TenantItemDataTypeMetaInfo.s_invitedByName]

	Component.onCompleted: {
		table.setSortingInfo(TenantItemDataTypeMetaInfo.s_createdAt, "DESC")
		table.nonSortableColumns = [TenantItemDataTypeMetaInfo.s_ownerId, TenantItemDataTypeMetaInfo.s_isActive, TenantItemDataTypeMetaInfo.s_membersCount, TenantItemDataTypeMetaInfo.s_tenantRelationScope, TenantItemDataTypeMetaInfo.s_invitationId, TenantItemDataTypeMetaInfo.s_invitedByName]
		registerFieldFilterDelegate("isActiveFilter", isActiveDelegateFilterComp)
		registerFieldFilterDelegate("tenantRelationFilter", tenantRelationDelegateFilterComp)
	}

	onHeadersChanged: {
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_membersCount, membersCountCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_ownerId, ownerIdCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_createdAt, createdAtCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_updatedAt, updatedAtCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_isActive, isActiveCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_tenantRelationScope, tenantRelationScopeCellDelegateComp)
	}

	function acceptInvitation(invitationId) {
		acceptInvitationInput.m_invitationId = invitationId
		acceptInvitationSender.send(acceptInvitationInput)
	}

	function rejectInvitation(invitationId) {
		rejectInvitationInput.m_invitationId = invitationId
		rejectInvitationSender.send(rejectInvitationInput)
	}

	property AcceptTenantInvitationInput acceptInvitationInput: AcceptTenantInvitationInput {}
	property GqlSdlRequestSender acceptInvitationSender: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_acceptTenantInvitation
		sdlObjectComp: Component {
			AcceptTenantInvitationPayload {
				onFinished: {
					if (m_success) {
						container.doUpdateModel()
					} else if (m_errorMessage && m_errorMessage !== "") {
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
					if (m_success) {
						container.doUpdateModel()
					} else if (m_errorMessage && m_errorMessage !== "") {
						ModalDialogManager.showInfoDialog(m_errorMessage)
					}
				}
			}
		}

		function onError(message, type) {
			ModalDialogManager.showInfoDialog(message)
		}
	}

	Component {
		id: tenantRelationScopeCellDelegateComp
		TableCellDelegateBase {
			id: tenantRelationScopeDelegate

			property string scopeValue: ""
			property string invitationIdValue: ""
			property string invitedByNameValue: ""

			onReused: {
				var val = tenantRelationScopeDelegate.getValue()
				scopeValue = val !== undefined && val !== null ? val.toString() : ""

				if (tenantRelationScopeDelegate.rowDelegate && tenantRelationScopeDelegate.rowDelegate.dataModel) {
					var item = tenantRelationScopeDelegate.rowDelegate.dataModel.item
					if (item) {
						invitationIdValue = (item.m_invitationId || "").toString()
						invitedByNameValue = (item.m_invitedByName || "").toString()
					}
				}
			}

			Text {
				id: scopeLabel
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				font.pixelSize: Style.fontSizeM
				color: tenantRelationScopeDelegate.scopeValue === "Invited" ? Style.accentColor : Style.textColor
				text: tenantRelationScopeDelegate.scopeValue
				font.underline: tenantRelationScopeDelegate.scopeValue === "Invited"

				MouseArea {
					id: scopeMouseArea
					anchors.fill: parent
					hoverEnabled: tenantRelationScopeDelegate.scopeValue === "Invited"
					cursorShape: tenantRelationScopeDelegate.scopeValue === "Invited" ? Qt.PointingHandCursor : Qt.ArrowCursor
					onEntered: {
						if (tenantRelationScopeDelegate.scopeValue === "Invited") {
							invitationPopup.open()
						}
					}
				}
			}

			Popup {
				id: invitationPopup
				x: scopeLabel.x
				y: scopeLabel.y + scopeLabel.height + Style.spacingS
				width: invitationPopupContent.width + 2 * Style.marginL
				height: invitationPopupContent.height + 2 * Style.marginL
				modal: false
				closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

				background: Rectangle {
					color: Style.panelColor
					border.color: Style.borderColor
					border.width: 1
					radius: Style.radiusM
				}

				Column {
					id: invitationPopupContent
					anchors.centerIn: parent
					spacing: Style.spacingM

					Text {
						text: qsTr("Invited by: %1").arg(tenantRelationScopeDelegate.invitedByNameValue || qsTr("Unknown"))
						font.pixelSize: Style.fontSizeM
						color: Style.textColor
					}

					Row {
						spacing: Style.spacingM
						anchors.horizontalCenter: parent.horizontalCenter

						Rectangle {
							width: acceptBtnLabel.contentWidth + 2 * Style.marginL
							height: Style.controlHeightS
							radius: Style.radiusM
							color: "#3FB950"

							Text {
								id: acceptBtnLabel
								anchors.centerIn: parent
								text: qsTr("Accept")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (tenantRelationScopeDelegate.invitationIdValue !== "") {
										container.acceptInvitation(tenantRelationScopeDelegate.invitationIdValue)
										invitationPopup.close()
									}
								}
							}
						}

						Rectangle {
							width: rejectBtnLabel.contentWidth + 2 * Style.marginL
							height: Style.controlHeightS
							radius: Style.radiusM
							color: "#DA3633"

							Text {
								id: rejectBtnLabel
								anchors.centerIn: parent
								text: qsTr("Reject")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (tenantRelationScopeDelegate.invitationIdValue !== "") {
										container.rejectInvitation(tenantRelationScopeDelegate.invitationIdValue)
										invitationPopup.close()
									}
								}
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: hiddenCellDelegateComp
		TableCellDelegateBase {
			Item {}
		}
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
				createAndAddOption("invited", qsTr("I am invited"), "", true)
				setFieldFilterForOption("owner", ownerFieldFilterComp.createObject(this))
				setFieldFilterForOption("member", memberFieldFilterComp.createObject(this))
				setFieldFilterForOption("invited", invitedFieldFilterComp.createObject(this))
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

			Component {
				id: invitedFieldFilterComp
				FieldFilter {
					m_fieldId: "TenantRelationScope"
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
					m_filterValue: "Invited"
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
