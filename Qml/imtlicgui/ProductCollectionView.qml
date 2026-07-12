import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtlicProductsSdl 1.0

RemoteCollectionView {
	id: productCollectionViewContainer;
	
	visibleMetaInfo: false;

	// Requires a page host (e.g. CollectionWorkspacePageView) to be
	// instantiated somewhere with pageId == "Products", so that
	// MainDocumentService.getDocumentService("Products") below resolves to
	// a real DocumentService instead of undefined.
	collectionId: "Products";
	documentCollectionFilter: null
	additionalFieldIds: [ProductItemTypeMetaInfo.s_productId]
	
	commandsDelegateComp: Component {ProductCollectionViewCommandsDelegate {
			collectionView: productCollectionViewContainer;

			Component.onCompleted: {
				registerDocumentType("Product", qsTr("Product"))
				addDocumentView("Product", "ProductEditor", productDocumentComp, productControllerComp)
			}

			onDocumentManagerChanged: {
				if (documentManager){
					documentManager.backend.registerDocumentDataController("Product", dataControllerComp)
				}
			}
		}
	}
	
	Component {
		id: productDocumentComp;
		
		ProductView {
			id: productEditor;
			
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
		id: productControllerComp

		InProcessDocumentRepresentationController {
			backend: MainDocumentService.getDocumentService(productCollectionViewContainer.collectionId).backend
		}
	}

	Component {
		id: dataControllerComp;

		GqlRequestDocumentDataController {
			id: requestDocumentDataController

			property ProductData productData: documentModel

			gqlGetCommandId: ImtlicProductsSdlCommandIds.s_productItem;
			gqlUpdateCommandId: ImtlicProductsSdlCommandIds.s_productUpdate;
			gqlAddCommandId: ImtlicProductsSdlCommandIds.s_productAdd;
			
			typeId: "Product";
			documentName: productData ? productData.m_productName: "";
			documentDescription: productData ? productData.m_description: "";
			
			documentModelComp: Component {
				ProductData {}
			}
		}
	}
}
