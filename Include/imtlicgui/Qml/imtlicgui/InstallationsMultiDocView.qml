import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: installationsMultiDocViewContainer;
    anchors.fill: parent;

    visible: false;

    property Item rootItem;
    property alias firstElementImageSource: installationsMultiDocView.firstElementImageSource;
//    property alias model: installationsMultiDocView.model;

    function menuActivated(menuId) {
        installationsMultiDocView.menuActivated(menuId);
    }

    function updateCommandId(){
        installationsMultiDocView.updateCommandId();
    }

    function commandsChanged(commandsId){
        console.log("installationsMultiDocView commandsChanged!", commandsId);
        installationsMultiDocView.commandsChanged(commandsId);
    }

    function setFocus(){
        console.log("installationsMultiDocView setFocus");
        installationsMultiDocView.setFocus();
    }

    onVisibleChanged: {
        if (installationsMultiDocViewContainer.visible && installationsMultiDocView.pagesCount === 0) {
            console.log("InstallationsMultiDocView first page loaded !");
            installationsMultiDocView.addToHeadersArray("", "Installations", "../../imtlicgui/InstallationCollectionView.qml", "Installations", "Open")
        }
    }

    MultiDocWorkspaceView {
        id: installationsMultiDocView;
        anchors.fill: parent;

        rootItem: installationsMultiDocViewContainer.rootItem;
//        Component.onCompleted: {
//            installationsMultiDocView.addToHeadersArray("", "Installations", "../../imtlicgui/InstallationCollectionView.qml", "Installations")
//        }

    }
}
