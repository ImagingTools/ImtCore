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

RemoteCollectionView {
	id: container

	property string productId;
	property var documentManager: null;

	collectionId: "Tenants"
	gqlGetListCommandId: ImtauthTenantsSdlCommandIds.s_getTenantList
	documentCollectionFilter: null

	Component.onCompleted: {
		table.setSortingInfo(TenantItemDataTypeMetaInfo.s_createdAt, "DESC")
		registerFieldFilterDelegate("isActiveFilter", isActiveDelegateFilterComp)
	}

	onHeadersChanged: {
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_createdAt, createdAtCellDelegateComp)
		table.setColumnContentById(TenantItemDataTypeMetaInfo.s_isActive, isActiveCellDelegateComp)
	}


	Component {
		id: createdAtCellDelegateComp
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
										root.updateRepresentationFromDocument()
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
