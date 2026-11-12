// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtauthConnectedProductsSdl 1.0
import imtauthConnectedProductCollectionDocumentServiceSdl 1.0
import imtguigql 1.0

/**
 * Moves a catalog entry between its document on the server and the editor over GraphQL.
 */
DocumentRepresentationController {
	id: productController

	representationModel: ConnectedProductData {
		m_id: UuidGenerator.generateUUID()
	}

	property var documentManager: null

	// The editor opens the pages of a saved product only: a new one has no tokens or tree yet
	function updateStoredProduct(){
		if (productController.view && productController.documentManager
					&& !productController.documentManager.documentIsNew(productController.documentId)
					&& representationModel.m_productId){
			productController.view.storedProductId = String(representationModel.m_productId)
		}
	}

	property Connections documentManagerConnections: Connections {
		target: productController.documentManager

		function onDocumentSaved(documentId){
			if (documentId === productController.documentId){
				productController.updateStoredProduct()
			}
		}
	}

	function updateRepresentationFromDocument(){
		startUpdateRepresentation(documentId, representationModel)

		getProductInput.m_id = documentId
		getProductInput.m_collectionId = "ConnectedProducts"
		getProductRequest.send(getProductInput)
	}

	function updateDocumentFromRepresentation(){
		startUpdateDocument(documentId)

		updateProductInput.m_documentId = documentId
		updateProductInput.m_product = representationModel
		return updateProductRequest.send(updateProductInput)
	}

	property DocumentId getProductInput: DocumentId {}
	property UpdateConnectedProductFromRepresentationInput updateProductInput: UpdateConnectedProductFromRepresentationInput {}

	property GqlSdlRequestSender getProductRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthConnectedProductCollectionDocumentServiceSdlCommandIds.s_getConnectedProductRepresentation
		sdlObjectComp: Component {
			ConnectedProductData {
				onFinished: {
					productController.representationModel.copyFrom(this)
					productController.updateStoredProduct()
					productController.representationUpdated(productController.documentId, productController.representationModel)
				}
			}
		}

		function onError(message, type){
			productController.updateRepresentationFailed(productController.documentId, message)
		}
	}

	property GqlSdlRequestSender updateProductRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthConnectedProductCollectionDocumentServiceSdlCommandIds.s_updateConnectedProductFromRepresentation
		requestType: 1
		sdlObjectComp: Component {
			DocumentOperationStatus {
				onFinished: {
					if (m_status === "Success"){
						productController.documentUpdated(productController.documentId)
					}
					else{
						productController.updateDocumentFailed(productController.documentId, m_message)
						productController.updateRepresentationFromDocument()
					}
				}
			}
		}

		function onError(message, type){
			productController.updateDocumentFailed(productController.documentId, message)
			productController.updateRepresentationFromDocument()
		}
	}
}
