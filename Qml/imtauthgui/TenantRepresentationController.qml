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

	// Created inside an organization a new one is its sub-organization: the server tells the parent and its permission scope
	requestRepresentationOnCreate: true

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

	property DocumentId documentIdInput: DocumentId {}
	property DocumentRepresentationRequestSender getTenantRequest: DocumentRepresentationRequestSender {
		representationController: root
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantCollectionDocumentServiceSdlCommandIds.s_getTenantRepresentation
		sdlObjectComp: Component {
			TenantData {}
		}
	}

	property UpdateTenantFromRepresentationInput updateTenantInput: UpdateTenantFromRepresentationInput {}
	property DocumentUpdateRequestSender updateTenantRequest: DocumentUpdateRequestSender {
		representationController: root
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantCollectionDocumentServiceSdlCommandIds.s_updateTenantFromRepresentation
		refusalMessage: qsTr("The organization could not be saved")
	}
}
