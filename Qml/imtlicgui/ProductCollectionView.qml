import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtlicProductsSdl 1.0
import imtlicProductCollectionDocumentServiceSdl 1.0

RemoteCollectionView {
	id: productCollectionViewContainer;

	visibleMetaInfo: false;

	// The collection is the one of the page showing the view (the page ID): products are shown
	// by several pages, a default here would register this view's editor with another page.
	// The list of the product schema, whatever collection of products the page shows.
	gqlGetListCommandId: ImtlicProductsSdlCommandIds.s_productsList;
	documentCollectionFilter: null

	// Set when the page holds products of one category only (new products get it, the editor does not offer it).
	property string fixedCategoryId: ""

	// Editor of a product; a page can replace it with its own.
	property Component productEditorComp: productDocumentComp
	additionalFieldIds: [ProductItemTypeMetaInfo.s_productId]
	
	commandsDelegateComp: Component {ProductCollectionViewCommandsDelegate {
			collectionView: productCollectionViewContainer;

			Component.onCompleted: {
				registerDocumentType("Product", qsTr("Product"))
				addDocumentView("Product", "ProductEditor", productCollectionViewContainer.productEditorComp, dataControllerComp)
			}
		}
	}
	
	Component {
		id: productDocumentComp;
		
		ProductView {
			id: productEditor;

			fixedCategoryId: productCollectionViewContainer.fixedCategoryId
			
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "Product";
				}
			}
			
			commandsDelegateComp: Component {ProductViewCommandsDelegate {
					view: productEditor;
				}
			}
		}
	}
	
	Component {
		id: dataControllerComp

		DocumentRepresentationController {
			id: dataController

			representationModel: ProductData {}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				documentIdInput.m_id = documentId
				documentIdInput.m_collectionId = productCollectionViewContainer.collectionId
				getRepresentationRequest.send(documentIdInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateInput.m_collectionId = productCollectionViewContainer.collectionId
				updateInput.m_documentId = documentId
				updateInput.m_productData = representationModel

				return updateRequest.send(updateInput)
			}

			property DocumentId documentIdInput: DocumentId {}
			property GqlSdlRequestSender getRepresentationRequest: GqlSdlRequestSender {
				gqlCommandId: ImtlicProductCollectionDocumentServiceSdlCommandIds.s_getProductRepresentation
				sdlObjectComp: Component {
					ProductData {
						onFinished: {
							dataController.representationModel.copyFrom(this)
							dataController.representationUpdated(dataController.documentId, dataController.representationModel)
						}
					}
				}

				function onError(message, type){
					dataController.updateRepresentationFailed(dataController.documentId, message)
				}
			}

			property UpdateProductInput updateInput: UpdateProductInput {}
			property GqlSdlRequestSender updateRequest: GqlSdlRequestSender {
				gqlCommandId: ImtlicProductCollectionDocumentServiceSdlCommandIds.s_updateProductFromRepresentation
				requestType: 1
				sdlObjectComp: Component {
					DocumentOperationStatus {
						onFinished: {
							if (m_status === "Success"){
								dataController.documentUpdated(dataController.documentId)
							}
							else{
								if (m_message){
									PopupManager.addErrorMessage(m_message)
								}
								dataController.updateDocumentFailed(dataController.documentId, "")
								dataController.updateRepresentationFromDocument()
							}
						}
					}
				}

				function onError(message, type){
					dataController.updateDocumentFailed(dataController.documentId, message)
					dataController.updateRepresentationFromDocument()
				}
			}
		}
	}
}
