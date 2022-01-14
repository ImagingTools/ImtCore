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
    property alias firstElementImageSource: tabPanelInternal.firstElementImageSource;
    property TreeItemModel model;
    property var pagesSources: [];
    property var pagesItems: [];
    property alias pagesCount: docsData.count;

    function menuActivated(menuId){
        multiDocView.activeItem.menuActivated(menuId);
    }

    function updateCommandId(){
        var commandsId = pages.GetData("CommandsId", tabPanelInternal.selectedIndex);
        var itemId = pages.GetData("ItemId", tabPanelInternal.selectedIndex);
        console.log("multiDocView tabPanel onSelectedIndexChanged commandsId = ", commandsId, " itemId = ", itemId, " selectedIndex = ", tabPanelInternal.selectedIndex)
        pagesDeleg.changeCommandsId(commandsId);
    }

    function commandsChanged(commandsId){
        multiDocView.activeItem.commandsChanged(commandsId);
    }

    function addToHeadersArray(itemId, title, source, commandsId){
        console.log("MultidicWorkspaceView addToHeadersArray", title, source, itemId, commandsId)
        var index = pages.InsertNewItem();
        console.log("MultidicWorkspaceView addToHeadersArray index", index);
        pages.SetData("ItemId", itemId, index);
        pages.SetData("Title", title, index);
        pages.SetData("Source", source, index);
        pages.SetData("CommandsId", commandsId, index);
        pagesDeleg.changeCommandsId(commandsId);
        tabPanelInternal.selectedIndex = pages.GetItemsCount() - 1;
        pages.Refresh();
        console.log("TEST", itemId, index, commandsId)
    }

    Component.onCompleted: {
       docsData.anchors.topMargin = tabPanelInternal.height;
    }

    TreeItemModel {
        id: pages;
    }

    TabPanel {
        id: tabPanelInternal;
        anchors.left: parent.left;
        anchors.right: parent.right;
        visible: true;
        model: pages;

        onCloseItem: {
            console.log("tabPanelInternal onCloseItem", index)
            pages.RemoveItem(index)
            if (tabPanelInternal.selectedIndex >= index){
                tabPanelInternal.selectedIndex--;
            }
            pages.Refresh()
        }

        onSelectedIndexChanged: {
            multiDocView.updateCommandId();
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
        model: pages;
        delegate: Rectangle {
            id: docsDataDeleg;
            width: visible ? docsData.width : 0;
            height: docsData.height;
            color: "transparent";
//            visible: false;
            visible: tabPanelInternal.selectedIndex === model.index;
            onVisibleChanged: {
                console.log("MultiDocView onVisibleChanged", this.visible);
                if(this.visible){
                    multiDocView.activeItem = loader.item;
                }
            }

            Component.onCompleted: {
                console.log("tabPanelInternal.selectedIndex", tabPanelInternal.selectedIndex);
                console.log("model.index", model.index);
            }

            function setModeMenuButton(commandId, mode){
                pagesDeleg.setModeMenuButton(commandId, mode);
            }

            Loader {
                id: loader;
                anchors.fill: parent;
                Component.onCompleted: {
                    console.log("MultidocWorkspaceView model index ",model.index)
                    loader.source = model.Source
//                    docsDataDeleg.visible = tabPanelInternal.selectedIndex === model.index;
                    console.log("MultidocWorkspaceView source",loader.source)
                }
                onItemChanged: {
                    if (loader.item && loader.source != ""){
                        loader.item.itemId = model.ItemId
                        var dataModelLocal
                        if (pages.ContainsKey("DocsData",model.index)){
                            dataModelLocal = pages.GetData("DocsData",model.index);
                        }
                        else {
                            dataModelLocal = pages.AddTreeModel("DocsData",model.index)
                            console.log("MultidocWorkspaceView onItemChanged", dataModelLocal)
                        }
                        console.log("MultidocWorkspaceView onItemChanged", loader.source)
                        if (tabPanelInternal.selectedIndex === model.index)
                            multiDocView.activeItem = loader.item;
                        loader.item.model = dataModelLocal

                    }
                }

            }
        }
    }

}
