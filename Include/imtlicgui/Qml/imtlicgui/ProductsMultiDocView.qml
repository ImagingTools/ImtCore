import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    MultiDocWorkspaceView {
        id: packageMultiDocView;
            anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            packageMultiDocView.addToHeadersArray("", "Products", "../../imtlicgui/ProductCollectionView.qml")
        }

    }
}
