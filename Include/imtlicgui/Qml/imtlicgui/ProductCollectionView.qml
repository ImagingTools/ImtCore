import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

CollectionView {
    id: productCollectionViewContainer;

    visibleMetaInfo: false;

    documentName: qsTr("Products");
    defaultSortHeaderIndex: 4;
    defaultOrderType: "DESC";

    Component.onCompleted: {
        productCollectionViewContainer.commandsDelegatePath = "../../imtlicgui/ProductCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("ProductId");

        productCollectionViewContainer.commandId = "Products";
    }

    onDocumentManagerPtrChanged: {
        if (documentManagerPtr){
            documentManagerPtr.registerDocument("Product", productDocumentComp);
        }
    }

    function selectItem(id){
        if (id === ""){
            documentManagerPtr.insertNewDocument("Product");
        }
        else{
            documentManagerPtr.openDocument(id, "Product");
        }
    }

    Component {
        id: productDocumentComp;

        ProductView {}
    }
}
