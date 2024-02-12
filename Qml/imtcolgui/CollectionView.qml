import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

CollectionViewBase {
    id: collectionView;

    property bool visibleMetaInfo: false;

    property var dataController: null;

//    property var templateContextMenuModel: [
//        {"Id": "Edit"},
//        {"Id": ""},
//        {"Id": "Rename"},
//        {"Id": "SetDescription"}
//    ]

    QtObject {
        id: internal;

        property bool guiUpdateRequired: false

        function filterMenuActivate(){
            if (collectionView.hasFilter){
                collectionView.filterMenuVisible = !collectionView.filterMenuVisible;
            }
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("FilterActivated", internal.filterMenuActivate);
        Events.subscribeEvent("OnLocalizationChanged", collectionView.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", collectionView.onLocalizationChanged);
        Events.unSubscribeEvent("FilterActivated", internal.filterMenuActivate);
    }

    onDataControllerChanged: {
        if (collectionView.dataController){
            bindHeaders.target = collectionView.table;
            bindElements.target = collectionView.table;
        }
    }

    onRightButtonMouseClicked: {
        openPopupMenu(mouseX, mouseY);
    }

    onDoubleClicked: {
        onEdit()
    }

    function onEdit() {
        if (collectionView.commandsDelegate){
            collectionView.commandsDelegate.commandHandle("Edit");
        }
    }

    Binding {
        id: bindHeaders;
        property: "headers";

        value: collectionView.dataController.headersModel;
    }

    Binding {
        id: bindElements;
        property: "elements";
        value: collectionView.dataController.elementsModel;
    }

    Connections {
        target: collectionView.dataController;

        function onBeginUpdate(){
            collectionView.loading.start();
        }

        function onEndUpdate(){
            collectionView.loading.stop();
        }

        function onHeadersModelChanged(){
            collectionView.doUpdateGui();
        }

        function onNotificationModelChanged(){
            let notificationModel = collectionView.dataController.notificationModel;
            if (notificationModel.ContainsKey("TotalCount")){
                let totalCount = notificationModel.GetData("TotalCount")

                collectionView.pagination.countAllElements = totalCount;
            }

            if (notificationModel.ContainsKey("PagesCount")){
                let pagesCount = notificationModel.GetData("PagesCount")

                collectionView.pagination.pagesSize = pagesCount;
            }
        }
    }

    onVisibleChanged: {
        if (collectionView.visible){
            if (internal.guiUpdateRequired){
                internal.guiUpdateRequired = false;
                collectionView.doUpdateGui();
            }

            Events.subscribeEvent("FilterActivated", collectionView.filterMenuActivate);
        }
        else{
            Events.unSubscribeEvent("FilterActivated", collectionView.filterMenuActivate);
        }
    }

    function fillContextMenuModel(){}

    function updateGui(){
        console.log("CollectionView updateGui");
        if (!collectionView.visible){
            internal.guiUpdateRequired = true;

            return;
        }

        let count = 0;
        let offset = -1;

        if (collectionView.hasPagination){
            count = pagination.countElements;
            offset = pagination.currentIndex * count;
        }

        if (collectionView.dataController){
            collectionView.dataController.updateElements(count, offset, collectionView.collectionFilter.filterModel);
        }
    }

    function openPopupMenu(x, y){
        let offset = 26 * collectionView.commandsController.contextMenuCommandsModel.GetItemsCount();
        modalDialogManager.openDialog(popupMenuDialog, {"x": x, "y": y - offset, "model": collectionView.commandsController.contextMenuCommandsModel});
    }

    function removeElement(elementIndex){
        if (collectionView.dataController){
            return collectionView.dataController.removeElement(elementIndex);
        }
    }

    function setElementName(elementIndex, name){
        if (collectionView.dataController){
            return collectionView.dataController.setElementName(elementIndex, name);
        }
    }

    function setElementDescription(elementIndex, description){
        if (collectionView.dataController){
            return collectionView.dataController.setElementDescription(elementIndex, description);
        }
    }

    function setMetaInfoModel(metaInfoModel){
        collectionMetaInfo.metaInfoModel = metaInfoModel;
    }

    MetaInfo {
        id: collectionMetaInfo;

        anchors.right: parent.right;

        width: visible ? 200 : 1;
        height: parent.height;

        visible: collectionView.visibleMetaInfo;
    }

    Component {
        id: popupMenuDialog;

        PopupMenuDialog {
            onFinished: {
                console.log("collectionView", collectionView);
                collectionView.commandsDelegate.commandHandle(commandId);
            }
        }
    }
}

