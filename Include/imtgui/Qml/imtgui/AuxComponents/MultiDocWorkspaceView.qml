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
    property alias firstElementImageSource: tabPanelInternal.firstElementImageSource;
//    property TreeItemModel model;
    property var pagesSources: [];
    property var pagesItems: [];
    property alias pagesCount: docsData.count;

    property string typeOperation;

    onActiveItemChanged: {

    }

    function menuActivated(menuId){
        multiDocView.activeItem.menuActivated(menuId);
    }

    function updateCommandId(){
        console.log("MultidocWorkspaceView updateCommandId", tabPanelInternal.selectedIndex, multiDocView, packagesMultiDocView)
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

    function addToHeadersArray(itemId, title, source, commandsId, typeOperation){
        console.log("MultidocWorkspaceView addToHeadersArray", title, source, itemId, commandsId, typeOperation)
        if (typeOperation) {
            multiDocView.typeOperation = typeOperation;
        }
        var findPage = false;
        if (itemId !== "" && multiDocView.tabIsOpened(itemId)) {
            findPage = true;
        }
        if (!findPage) {
            multiDocView.activeItem = null
            var index = pagesData.InsertNewItem();
            console.log("MultidicWorkspaceView addToHeadersArray index", index);
            pagesData.SetData("ItemId", itemId, index);
            pagesData.SetData("Title", title, index);
            pagesData.SetData("Source", source, index);
            pagesData.SetData("CommandsId", commandsId, index);
//            tabPanelInternal.model = 0;
//            tabPanelInternal.model = pages;
            tabPanelInternal.selectedIndex = pagesData.GetItemsCount() - 1;
        }
    }

    function closeTab(index) {
        pagesData.RemoveItem(index)
        if (tabPanelInternal.selectedIndex >= index){
            tabPanelInternal.selectedIndex--;
        }
    }

    function updateTitleTab(itemId, title, index) {
        console.log("MultidocWorkspaceView updateTitleTab()", index, title);
        pages.SetData("Title", title, index);
        pages.SetData("ItemId", itemId, index);
        console.log("Tab after updating ItemId = " , pages.GetData("ItemId", index), "Title = ", pages.GetData("Title", index))
    }

    Component.onCompleted: {
       docsData.anchors.topMargin = tabPanelInternal.height;
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
            if(multiDocView.activeItem)
                multiDocView.updateCommandId();
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
        delegate: Rectangle {
            id: docsDataDeleg;
            width: visible ? docsData.width : 0;
            height: docsData.height;
            color: "transparent";
            visible: tabPanelInternal.selectedIndex === model.index;
            onVisibleChanged: {
                console.log("MultiDocView ListView onVisibleChanged", this.visible);
                if(this.visible){
                    multiDocView.activeItem = loader.item;
                    multiDocView.activeItem.refresh();
                }
            }

            Component.onCompleted: {
                console.log("MultiDocView ListView onCompleted", tabPanelInternal.selectedIndex);
            }

            function setModeMenuButton(commandId, mode){
                console.log("MultiDocView ListView setModeMenuButton", commandId, mode);
//                pagesDeleg.setModeMenuButton(commandId, mode);

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
                id: loader;
                anchors.fill: parent;
                Component.onCompleted: {
                    console.log("MultidocWorkspaceView model index ", model.Source)
                    loader.source = model.Source
                }
                onItemChanged: {
                    console.log("MultidocWorkspaceView Loader onItemChanged", loader.source, docsDataDeleg)
                    if (loader.item && loader.source != ""){
                        loader.item.rootItem = docsDataDeleg;
                        console.log("ItemId =", model.ItemId, "ItemName =", model.Title, "TypeOperation = ", multiDocView.typeOperation);
                        loader.item.itemId = model.ItemId
                        loader.item.itemName = model.Title

//                        if (loader.item.typeOperation) {
//                            console.log("loader.item.typeOperation exist!");
//                            loader.item.typeOperation = multiDocView.typeOperation;
//                        }

                        var dataModelLocal
                        if (pagesData.ContainsKey("DocsData",model.index)){
                            dataModelLocal = pagesData.GetData("DocsData",model.index);
                        }
                        else {
                            dataModelLocal = pagesData.AddTreeModel("DocsData",model.index)
                            console.log("MultidocWorkspaceView onItemChanged", dataModelLocal)
                        }
                        console.log("MultidocWorkspaceView Loader onItemChanged", loader.source)
                        loader.item.rootItem = docsDataDeleg;
                        loader.item.multiDocViewItem = multiDocView;
                        if (tabPanelInternal.selectedIndex === model.index) {
                            multiDocView.activeItem = loader.item;
                            multiDocView.updateCommandId();
                        }
                        console.log("loader.item.rootItem = ", docsDataDeleg)

                        loader.item.model = dataModelLocal

                    }
                }

            }
        }
    }

}
