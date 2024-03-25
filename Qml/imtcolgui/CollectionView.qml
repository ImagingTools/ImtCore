import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

CollectionViewBase {
    id: container;

    property bool visibleMetaInfo: false;

    property Component dataControllerComp;
    property var dataController: null;

    onDataControllerCompChanged: {
        if (dataControllerComp){
            if (dataController){
                dataController.destroy();
            }

            dataController = dataControllerComp.createObject(container);
        }
    }

    commandsDelegateComp: Component {
        CollectionViewCommandsDelegateBase {
            collectionView: container;
        }
    }

    Connections {
        target: container.dataController;

        function onBeginUpdate(){
            container.loading.start();
        }

        function onEndUpdate(){
            container.loading.stop();
        }

        function onHeadersModelChanged(){
            container.doUpdateGui();
        }

        function onNotificationModelChanged(){
            if (!dataController){
                return;
            }

            let notificationModel = dataController.notificationModel;
            if (notificationModel.ContainsKey("TotalCount")){
                let totalCount = notificationModel.GetData("TotalCount")

                container.pagination.countAllElements = totalCount;
            }

            if (notificationModel.ContainsKey("PagesCount")){
                let pagesCount = notificationModel.GetData("PagesCount")

                container.pagination.pagesSize = pagesCount;
            }
        }

        function onFilterableHeadersModelChanged(){
            if (!dataController){
                return;
            }

            container.collectionFilter.setFilteringInfoIds(dataController.filterableHeadersModel);
        }
    }

    QtObject {
        id: internal;

        property bool guiUpdateRequired: false

        function filterMenuActivate(){
            if (container.hasFilter){
                container.filterMenuVisible = !container.filterMenuVisible;
            }
        }

        function onBeginUpdate(){
            container.loading.start();
        }

        function onEndUpdate(){
            container.loading.stop();
        }

        function onHeadersModelChanged(){
            container.doUpdateGui();
        }

        function onNotificationModelChanged(){
            let notificationModel = container.dataController.notificationModel;
            if (notificationModel.ContainsKey("TotalCount")){
                let totalCount = notificationModel.GetData("TotalCount")

                container.pagination.countAllElements = totalCount;
            }

            if (notificationModel.ContainsKey("PagesCount")){
                let pagesCount = notificationModel.GetData("PagesCount")

                container.pagination.pagesSize = pagesCount;
            }
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("FilterActivated", internal.filterMenuActivate);
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
        Events.unSubscribeEvent("FilterActivated", internal.filterMenuActivate);
    }

    onDataControllerChanged: {
        if (container.dataController){
            bindHeaders.target = container.table;
            bindElements.target = container.table;

            //            container.dataController.beginUpdate.connect(internal.onBeginUpdate);
            //            container.dataController.endUpdate.connect(internal.onEndUpdate);
            //            container.dataController.headersModelChanged.connect(internal.onHeadersModelChanged);
            //            container.dataController.notificationModelChanged.connect(internal.onNotificationModelChanged);
        }
    }

    onRightButtonMouseClicked: {
        openPopupMenu(mouseX, mouseY);
    }

    onDoubleClicked: {
        onEdit()
    }

    function onEdit() {
        if (container.commandsDelegate){
            container.commandsDelegate.commandHandle("Edit");
        }
    }

    Binding {
        id: bindHeaders;
        property: "headers";

        value: container.dataController.headersModel;
    }

    Binding {
        id: bindElements;
        property: "elements";
        value: container.dataController.elementsModel;
    }

    //    Connections {
    //        target: container.dataController;

    //        function onFilterableHeadersModelChanged(){
    //            container.collectionFilter.setFilteringInfoIds(container.dataController.filterableHeadersModel);
    //        }
    //    }

    onVisibleChanged: {
        if (container.visible){
            if (internal.guiUpdateRequired){
                internal.guiUpdateRequired = false;
                container.doUpdateGui();
            }

            Events.subscribeEvent("FilterActivated", container.filterMenuActivate);
        }
        else{
            Events.unSubscribeEvent("FilterActivated", container.filterMenuActivate);
        }
    }

    function updateGui(){
        console.log("CollectionView updateGui");
        if (!container.visible){
            internal.guiUpdateRequired = true;

            return;
        }

        let count = 0;
        let offset = -1;

        if (container.hasPagination){
            count = pagination.countElements;
            offset = pagination.currentIndex * count;
        }

        if (container.dataController){
            console.log("CollectionView updateElements");
            container.dataController.updateElements(count, offset, container.collectionFilter.filterModel);
        }
    }

    function openPopupMenu(x, y){
        console.log("openPopupMenu", x, y);
        if (container.commandsDelegate){
            let contextMenuModel = container.commandsDelegate.getContextMenuModel();
            console.log("contextMenuModel", contextMenuModel.toJSON());

            let offset = 26 * contextMenuModel.GetItemsCount();
            modalDialogManager.openDialog(popupMenuDialog, {"x": x, "y": y - offset, "model": contextMenuModel});
        }
    }

    function removeElement(elementIndex){
        if (container.dataController){
            return container.dataController.removeElement(elementIndex);
        }
    }

    function setElementName(elementIndex, name){
        if (container.dataController){
            return container.dataController.setElementName(elementIndex, name);
        }
    }

    function setElementDescription(elementIndex, description){
        if (container.dataController){
            return container.dataController.setElementDescription(elementIndex, description);
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

        visible: container.visibleMetaInfo;
    }

    Component {
        id: popupMenuDialog;

        PopupMenuDialog {
            onFinished: {
                container.commandsDelegate.commandHandle(commandId);
            }
        }
    }
}

