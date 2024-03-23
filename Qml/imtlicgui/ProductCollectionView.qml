import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
    id: productCollectionViewContainer;

    visibleMetaInfo: false;

    collectionId: "Products";

    additionalFieldIds: ["ProductId"]

    commandsDelegateComp: Component {ProductCollectionViewCommandsDelegate {
        collectionView: productCollectionViewContainer;

        documentTypeId: "Product";
        viewTypeId: "ProductEditor";
    }
    }

    Component.onCompleted: {
        let documentManager = MainDocumentManager.getDocumentManager(productCollectionViewContainer.collectionId);
        if (documentManager){
            productCollectionViewContainer.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Product", "ProductEditor", productDocumentComp);
            documentManager.registerDocumentDataController("Product", dataControllerComp);
        }
    }

    Component {
        id: productDocumentComp;

        ProductView {
            id: productEditor;

            commandsControllerComp: Component {CommandsRepresentationProvider {
                commandId: "Product";
                uuid: productEditor.viewId;
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

        GqlDocumentDataController {
            gqlGetCommandId: "ProductItem";
            gqlUpdateCommandId: "ProductUpdate";
            gqlAddCommandId: "ProductAdd";
        }
    }
}
