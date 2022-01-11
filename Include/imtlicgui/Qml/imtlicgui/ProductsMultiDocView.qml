import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias firstElementImageSource: productsMultiDocView.firstElementImageSource;
    property alias model: productsMultiDocView.model;

    function menuActivated(menuId) {
        productsMultiDocView.menuActivated(menuId);
    }

    MultiDocWorkspaceView {
        id: productsMultiDocView;
            anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            productsMultiDocView.addToHeadersArray("", "Products", "../../imtlicgui/ProductCollectionView.qml")
        }

    }
}
