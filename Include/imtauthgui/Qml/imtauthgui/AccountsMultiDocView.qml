import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: accountsMultiDocViewContainer;
    anchors.fill: parent;

    visible: false;
    property Item rootItem;
    property alias firstElementImageSource: accountsMultiDocView.firstElementImageSource;

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

    onVisibleChanged: {
        if (accountsMultiDocViewContainer.visible && accountsMultiDocView.pagesCount === 0) {
            console.log("AccountsMultiDocView first page loaded !");
            accountsMultiDocView.addToHeadersArray("", "Accounts", "../../imtauthgui/AccountCollectionView.qml", "Accounts", "Open")
        }
    }

    MultiDocWorkspaceView {
        id: accountsMultiDocView;
        anchors.fill: parent;

        rootItem: accountsMultiDocViewContainer.rootItem;
//        Component.onCompleted: {
//            accountsMultiDocView.addToHeadersArray("", "Accounts", "../../imtlicgui/AccountCollectionView.qml", "Accounts")
//        }
    }
}
