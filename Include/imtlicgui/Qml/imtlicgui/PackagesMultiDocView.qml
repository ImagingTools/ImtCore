import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: packagesMultiDocViewContainer;
    anchors.fill: parent;
    property alias firstElementImageSource: packagesMultiDocView.firstElementImageSource;
    property alias model: packagesMultiDocView.model;
    function menuActivated(menuId) {
        packagesMultiDocView.menuActivated(menuId);
    }

    function updateCommandId(){
        packagesMultiDocView.updateCommandId();
    }

    function commandsChanged(commandsId){
        console.log("PackagesMultiDocView commandsChanged!", commandsId);
        packagesMultiDocView.commandsChanged(commandsId);
    }

    onVisibleChanged: {
        if (packagesMultiDocViewContainer.visible && packagesMultiDocView.pagesCount === 0) {
            console.log("PackagesMultiDocView first page loaded !");
            packagesMultiDocView.addToHeadersArray("", "Packages", "../../imtlicgui/PackageCollectionView.qml", "Packages")
        }
    }

    MultiDocWorkspaceView {
        id: packagesMultiDocView;
            anchors.fill: parent;
//        Component.onCompleted: {
//            console.log("PackagesMultiDocView on completed");
//            packagesMultiDocView.addToHeadersArray("", "Packages", "../../imtlicgui/PackageCollectionView.qml", "Packages")
//        }

    }
}
