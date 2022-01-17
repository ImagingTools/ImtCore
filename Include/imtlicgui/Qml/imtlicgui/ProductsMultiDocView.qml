import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: productsMultiDocViewContainer;
    anchors.fill: parent;
    property alias firstElementImageSource: productsMultiDocView.firstElementImageSource;
//    property alias model: productsMultiDocView.model;

    function menuActivated(menuId) {
        productsMultiDocView.menuActivated(menuId);
    }

    function updateCommandId(){
        productsMultiDocView.updateCommandId();
    }

    function commandsChanged(commandsId){
        console.log("productsMultiDocView commandsChanged!", commandsId);
        productsMultiDocView.commandsChanged(commandsId);
    }

    onVisibleChanged: {
        if (productsMultiDocViewContainer.visible && productsMultiDocView.pagesCount === 0) {
            console.log("ProductsMultiDocView first page loaded !");
            productsMultiDocView.addToHeadersArray("", "Products", "../../imtlicgui/ProductCollectionView.qml", "Products")
        }
    }

    MultiDocWorkspaceView {
        id: productsMultiDocView;
        anchors.fill: parent;
//        Component.onCompleted: {
//            productsMultiDocView.addToHeadersArray("", "Products", "../../imtlicgui/ProductCollectionView.qml", "Products")
//        }

    }
}
