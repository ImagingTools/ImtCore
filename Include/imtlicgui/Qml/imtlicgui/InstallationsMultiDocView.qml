import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias firstElementImageSource: installationsMultiDocView.firstElementImageSource;
    property alias model: installationsMultiDocView.model;

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

    MultiDocWorkspaceView {
        id: installationsMultiDocView;
            anchors.fill: parent;
        Component.onCompleted: {
            installationsMultiDocView.addToHeadersArray("", "Installations", "../../imtlicgui/InstallationCollectionView.qml", "Installations")
        }

    }
}
