import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: multiDocView;

    height: 100;
    width: 100;

    color: "transparent";

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem;
    property Item activeCollectionItem;
    property Item rootItem;

    property var pagesSources: [];
    property var pagesItems: [];

    property alias pagesCount: docsData.count;
    property alias firstElementImageSource: tabPanelInternal.firstElementImageSource;

    property string operation;

    Component.onCompleted: {
        console.log("MultidocWorkspaceView onCompleted", tabPanelInternal.selectedIndex)
        docsData.anchors.topMargin = tabPanelInternal.height;
    }

    function menuActivated(menuId){
        multiDocView.activeItem.menuActivated(menuId);
    }

    function updateCommandId(){
        console.log("MultidocWorkspaceView updateCommandId", tabPanelInternal.selectedIndex)
        if (tabPanelInternal.selectedIndex > -1 && multiDocView.rootItem){
            var commandsId = pagesData.GetData("CommandsId", tabPanelInternal.selectedIndex);
            var itemId = pagesData.GetData("ItemId", tabPanelInternal.selectedIndex);
            multiDocView.rootItem.changeCommandsId(commandsId);
        }
    }

    function commandsChanged(commandsId){
        console.log("MultidocWorkspaceView commandsChanged")
        multiDocView.activeItem.commandsChanged(commandsId);
    }

    function tabIsOpened(itemId) {
        for (var i = 0; i < pagesData.GetItemsCount(); i++) {
            var pageId = pagesData.GetData("ItemId", i);
            if (pageId === itemId) {
                tabPanelInternal.selectedIndex = i;
                return true;
            }
        }
        return false;
    }

    function addToHeadersArray(itemId, title, source, commandsId, operation){
        console.log("MultidocWorkspaceView addToHeadersArray", title, source, itemId, commandsId, operation)
        if (operation) {
            multiDocView.operation = operation;
        }
        var findPage = false;
        if (itemId !== "" && multiDocView.tabIsOpened(itemId)) {
            findPage = true;
        }
        if (!findPage) {
//            multiDocView.activeItem = null
            var index = pagesData.InsertNewItem();
            console.log("MultidicWorkspaceView addToHeadersArray index", index);
            pagesData.SetData("ItemId", itemId, index);
            pagesData.SetData("Title", title, index);
            pagesData.SetData("Source", source, index);
            pagesData.SetData("CommandsId", commandsId, index);
//            pagesData.SetData("ItemSelectedIndex", -1, index);

//            tabPanelInternal.model = 0;
//            tabPanelInternal.model = pagesData;
            tabPanelInternal.selectedIndex = pagesData.GetItemsCount() - 1;
        }
    }

    function closeTab(index) {
        console.log("MultiDocWorkspaceView closeTab", index);

        pagesData.RemoveItem(index)
        if (tabPanelInternal.selectedIndex >= index){
            tabPanelInternal.selectedIndex--;
        }
    }

    function updateTitleTab(itemId, title, index) {
        console.log("MultidocWorkspaceView updateTitleTab()", index, title);
        pagesData.SetData("Title", title, index);
        pagesData.SetData("ItemId", itemId, index);
    }

    TreeItemModel {
        id: pagesData;
    }

    TabPanel {
        id: tabPanelInternal;

        anchors.left: parent.left;
        anchors.right: parent.right;

        visible: true;
        model: pagesData;

        onCloseItem: {
            console.log("MultiDocWorkspaceView TabPanel onCloseItem", index)
            multiDocView.closeTab(index);
        }

        onSelectedIndexChanged: {
            console.log("MultiDocWorkspaceView TabPanel onSelectedIndexChanged", tabPanelInternal.selectedIndex);

//            if(multiDocView.activeItem) {
//                multiDocView.updateCommandId();
//            }

        }

        onRightClicked: {
            if (tabPanelInternal.selectedIndex < pagesData.GetItemsCount() - 1) {
                tabPanelInternal.selectedIndex++;
                tabPanelInternal.viewTabInListView(tabPanelInternal.selectedIndex);
            }
        }

        onLeftClicked: {
            if (tabPanelInternal.selectedIndex > 0) {
                tabPanelInternal.selectedIndex--;
                tabPanelInternal.viewTabInListView(tabPanelInternal.selectedIndex);
            }
        }
    }

    ListView {
        id: docsData;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.topMargin: tabPanelInternal.height;
        anchors.bottom: parent.bottom;

        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        orientation: ListView.Horizontal;
        model: pagesData;

        property int  currentIndex: -1;

        delegate: Rectangle {
            id: docsDataDeleg;

            width: visible ? docsData.width : 0;
            height: docsData.height;

            color: "transparent";
            visible: tabPanelInternal.selectedIndex === model.index;

            onVisibleChanged: {
                console.log("MultiDocView ListView onVisibleChanged", this.visible);

                if(this.visible){
                    console.log("multiDocView.activeItem", multiDocView.activeItem);
                    console.log("dataLoader.item", dataLoader.item);
                    multiDocView.activeItem = dataLoader.item;
                    multiDocView.activeItem.refresh();
                    multiDocView.updateCommandId();
                }
            }

            Component.onCompleted: {
                console.log("MultiDocView ListView onCompleted", tabPanelInternal.selectedIndex);
            }

            function setModeMenuButton(commandId, mode){
                console.log("MultiDocView ListView setModeMenuButton", commandId, mode);
                multiDocView.rootItem.setModeMenuButton(commandId, mode);

            }

            function updateTitleTab(itemId, name) {
                console.log("MultiDocView ListView updateTitleTab",itemId,  name);
                multiDocView.updateTitleTab(itemId, name, tabPanelInternal.selectedIndex);
            }

            function closeTab() {
                multiDocView.closeTab(tabPanelInternal.selectedIndex);
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                Component.onCompleted: {
                    console.log("MultidocWorkspaceView model index ", dataLoader.source, model.Source)
                    dataLoader.source = model.Source
                    console.log("MultidocWorkspaceView model index 2", dataLoader.source, model.Source)
                }

                onItemChanged: {
                    console.log("MultidocWorkspaceView dataLoader onItemChanged", dataLoader.source, docsDataDeleg)
                    if (dataLoader.item && dataLoader.source != ""){

                        dataLoader.item.rootItem = docsDataDeleg;
                        dataLoader.item.itemId = model.ItemId
                        dataLoader.item.itemName = model.Title
                        dataLoader.item.operation = multiDocView.operation

                        docsData.currentIndex = model.index;

                        var dataModelLocal
                        if (pagesData.ContainsKey("DocsData",model.index)){
                            dataModelLocal = pagesData.GetData("DocsData",model.index);
                        }
                        else {
                            dataModelLocal = pagesData.AddTreeModel("DocsData",model.index)
                        }

                        dataLoader.item.multiDocViewItem = multiDocView;
                        dataLoader.item.model = dataModelLocal
                        dataLoader.item.rootItem = docsDataDeleg;

                        if (tabPanelInternal.selectedIndex === model.index) {
                            multiDocView.activeItem = dataLoader.item;
                            multiDocView.updateCommandId();
                        }
                    }
                }

            }
        }
    }

}
