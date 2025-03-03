import QtQuick 2.12
import Acf 1.0
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
	
	collectionId: "Products";
	
	additionalFieldIds: [ProductItemTypeMetaInfo.s_productId]
	
	commandsDelegateComp: Component {ProductCollectionViewCommandsDelegate {
			collectionView: productCollectionViewContainer;
			
			documentTypeIds: ["Product"]
			documentViewTypeIds: ["ProductEditor"]
			documentViewsComp: [productDocumentComp]
			documentDataControllersComp: [dataControllerComp]
		}
	}
	
	Component.onCompleted: {
	}
	
	Component {
		id: productDocumentComp;
		
		ProductView {
			id: productEditor;
			
			commandsControllerComp: Component {CommandsPanelController {
					commandId: "Product";
					uuid: productEditor.viewId;
					commandsView: productEditor.commandsView;
				}
			}
			
			commandsDelegateComp: Component {ProductViewCommandsDelegate {
					view: productEditor;
				}
			}
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
			
			payloadModel: ProductDataPayload {
				onFinished: {
					requestDocumentDataController.documentModel = m_productData;
				}
			}
		}
	}
}
