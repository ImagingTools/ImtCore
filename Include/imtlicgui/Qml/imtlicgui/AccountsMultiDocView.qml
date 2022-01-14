import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias firstElementImageSource: accountsMultiDocView.firstElementImageSource;
    property alias model: accountsMultiDocView.model;

    function menuActivated(menuId) {
        accountsMultiDocView.menuActivated(menuId);
    }

    function updateCommandId(){
        accountsMultiDocView.updateCommandId();
    }

    function commandsChanged(commandsId){
        console.log("accountsMultiDocView commandsChanged!", commandsId);
        accountsMultiDocView.commandsChanged(commandsId);
    }

    MultiDocWorkspaceView {
        id: accountsMultiDocView;
        anchors.fill: parent;
        Component.onCompleted: {
            accountsMultiDocView.addToHeadersArray("", "Accounts", "../../imtlicgui/AccountCollectionView.qml", "Accounts")
        }

    }
}
