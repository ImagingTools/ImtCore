import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias firstElementImageSource: packagesMultiDocView.firstElementImageSource;
    property alias model: packagesMultiDocView.model;
    function menuActivated(menuId) {
        packagesMultiDocView.menuActivated(menuId);
    }

    MultiDocWorkspaceView {
        id: packagesMultiDocView;
            anchors.fill: parent;
//            color: "red";
        Component.onCompleted: {
            packagesMultiDocView.addToHeadersArray("", "Packages", "../../imtlicgui/PackageCollectionView.qml")
        }

//        onChangeCommandsId: {
//            console.log("packagesMultiDocView changeCommandsId",commandsId)
//            pagesDeleg.changeCommandsId(commandsId)
//        }

    }
}
