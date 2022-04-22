import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: adminPanelMultiDocViewContainer;
    anchors.fill: parent;

    visible: false;

    property Item rootItem;
    property alias firstElementImageSource: adminPanelMultiDocView.firstElementImageSource;
//    property alias model: adminPanelMultiDocView.model;

    function menuActivated(menuId) {
        adminPanelMultiDocView.menuActivated(menuId);
    }

    function updateCommandId(){
        adminPanelMultiDocView.updateCommandId();
    }

    function commandsChanged(commandsId){
        console.log("adminPanelMultiDocView commandsChanged!", commandsId);
        adminPanelMultiDocView.commandsChanged(commandsId);
    }

    function setFocus(){
        console.log("adminPanelMultiDocView setFocus");
        adminPanelMultiDocView.setFocus();
    }

    onVisibleChanged: {
        if (adminPanelMultiDocViewContainer.visible && adminPanelMultiDocView.pagesCount === 0) {
            console.log("adminPanelMultiDocView first page loaded !");
            adminPanelMultiDocView.addToHeadersArray("", "Users", "../../imtlicgui/UsersCollectionView.qml", "Users", "Open")
        }
    }

    MultiDocWorkspaceView {
        id: adminPanelMultiDocView;
        anchors.fill: parent;

        rootItem: adminPanelMultiDocViewContainer.rootItem;
    }
}
