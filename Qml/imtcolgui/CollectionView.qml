import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

Item {
    id: root;

    property alias viewId: container.viewId;

    property alias commandsControllerComp: container.commandsControllerComp;
    property alias commandsController: container.commandsController;

    property alias dataControllerComp: container.dataControllerComp;
    property alias dataController: container.dataController;

    property alias commandsDelegateComp: container.commandsDelegateComp;
    property alias commandsDelegate: container.commandsDelegate;

    property alias collectionFilter: container.collectionFilter;
    property alias table: container.table;

    property alias readOnly: container.readOnly;
    property alias filterMenu: container.filterMenu;
    property alias filterMenuVisible: container.filterMenuVisible;
    property alias hasFilter: container.hasFilter;
    property alias hasSort: container.hasSort;
    property alias hasPagination: container.hasPagination;
    property alias elementsCount: container.elementsCount;
    property alias pagination: container.pagination;
    property alias metaInfoView: collectionMetaInfo;

    property bool visibleMetaInfo: false;

    signal selectedIndexChanged(int index);
    signal elementsChanged();
    signal headersChanged();
    signal selectionChanged(var selection);
    signal checkedItemsChanged();
    signal rightButtonMouseClicked(int mouseX, int mouseY);

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged)
    }

    function onLocalizationChanged(language){
        if (root.dataController){
            root.dataController.updateModel();
        }
    }

    function doubleClicked(id, index){
        container.doubleClicked(id, index)
    }

    function doUpdateGui(){
        console.log("CollectionView doUpdateGui");
        container.doUpdateGui();
    }

    function doUpdateModel(){
        container.doUpdateModel();
    }

    function removeElement(elementIndex){
        container.removeElement(elementIndex);
    }

    function setElementName(elementIndex, name){
        container.setElementName(elementIndex, name);
    }

    function setElementDescription(elementIndex, description){
        container.setElementDescription(elementIndex, description);
    }

    function setMetaInfoModel(metaInfoModel){
        collectionMetaInfo.metaInfoModel = metaInfoModel;
    }

    function onEdit(id, index) {
        if (container.commandsDelegate){
            container.commandsDelegate.commandHandle("Edit");
        }
    }

    function onFilterChanged(filterId, filterValue){
        container.collectionFilter.setTextFilter(filterValue);

        container.doUpdateGui();
    }

    Connections {
        target: container;

        function onSelectedIndexChanged(index){
            root.selectedIndexChanged(index);
        }

        function onElementsChanged(){
            root.elementsChanged();
        }

        function onHeadersChanged()
        {
            root.headersChanged();
        }

        function onSelectionChanged(selection){
            root.selectionChanged(selection);
        }

        function onRightButtonMouseClicked(mouseX, mouseY){
            root.rightButtonMouseClicked(mouseX, mouseY)
        }
    }

    CollectionViewBase {
        id: container;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: collectionMetaInfo.left;

        property Component dataControllerComp;
        property var dataController: null;

        onDataControllerCompChanged: {
            if (dataControllerComp){
                if (dataController){
                    dataController.destroy();
                }

                dataController = dataControllerComp.createObject(container);
                console.log("*DEBUG* dataController.additionalFieldIds", dataController.additionalFieldIds)
            }
        }

        commandsDelegateComp: Component {
            CollectionViewCommandsDelegateBase {
                collectionView: container;
            }
        }

        onCheckedItemsChanged: {
            root.checkedItemsChanged();
        }

        Connections {
            target: container.dataController;

            function onBeginUpdate(){
                container.error.visible = false;
                container.loading.start();
            }

            function onEndUpdate(){
                container.loading.stop();
            }

            function onHeadersModelChanged(){
                container.doUpdateGui();
            }

            function onError(){
                container.error.visible = true;
            }
            function onNotificationModelChanged(){
                if (!container.dataController){
                    return;
                }

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

            function onFilterableHeadersModelChanged(){
                if (!container.dataController){
                    return;
                }

                container.collectionFilter.setFilteringInfoIds(container.dataController.filterableHeadersModel);
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
            }
        }

        onRightButtonMouseClicked: {
            openPopupMenu(mouseX, mouseY);
        }

        onDoubleClicked: {
            root.onEdit(id, index)
        }

        onFilterChanged: {
            root.onFilterChanged(filterId, filterValue);
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
            console.log("CollectionView updateGui", container.visible);

            if (container.dataController){
                console.log("ID", container.dataController.collectionId);
            }

            if (!container.visible){
                internal.guiUpdateRequired = true;

                return;
            }

            let count = -1;
            let offset = 0;

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
                console.log("contextMenuModel", contextMenuModel.ToJson());

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

        Component {
            id: popupMenuDialog;

            PopupMenuDialog {
                onFinished: {
                    container.commandsDelegate.commandHandle(commandId);
                }
            }
        }
    }

    MetaInfo {
        id: collectionMetaInfo;

        anchors.right: parent.right;

        width: visible ? 200 : 0;
        height: parent.height;

        visible: root.visibleMetaInfo;
    }
}


