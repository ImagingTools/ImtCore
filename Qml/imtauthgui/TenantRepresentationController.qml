// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthTenantsSdl 1.0
import imtauthTenantCollectionDocumentServiceSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0

DocumentRepresentationController {
	id: root

	property string collectionId
	property string permissionPath

	representationModel: TenantData {
		m_isActive: true
	}

	function updateRepresentationFromDocument(){
		startUpdateRepresentation(documentId, representationModel)

		documentIdInput.m_id = documentId
		documentIdInput.m_collectionId = root.collectionId

		getTenantRequest.send(documentIdInput)
	}

	function updateDocumentFromRepresentation(){
		startUpdateDocument(documentId)

		updateTenantInput.m_documentId = documentId
		updateTenantInput.m_tenant = representationModel

		updateTenantRequest.send(updateTenantInput)
	}

	// The refused change was not applied: the reason is shown and the editor returns to what the document holds
	function rejectUpdate(message){
		root.updateDocumentFailed(root.documentId, message)
		root.updateRepresentationFromDocument()
	}

	property DocumentId documentIdInput: DocumentId {}
	property GqlSdlRequestSender getTenantRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantCollectionDocumentServiceSdlCommandIds.s_getTenantRepresentation
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
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantCollectionDocumentServiceSdlCommandIds.s_updateTenantFromRepresentation
		requestType: 1
		sdlObjectComp: Component {
			DocumentOperationStatus {
				onFinished: {
					if (m_status === "Success"){
						root.documentUpdated(root.documentId)
					}
					else{
						root.rejectUpdate(m_message !== "" ? m_message : qsTr("The organization could not be saved"))
					}
				}
			}
		}

		function onError(message, type){
			root.rejectUpdate(message)
		}
	}
}
