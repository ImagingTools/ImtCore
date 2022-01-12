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

    function menuActivated(menuId){
        multiDocView.activeItem.menuActivated(menuId);
    }

////    signal changeCommandsId(string commandsId);
//    function changeCommandsId(commandsId) {
//        console.log("multiDocView changeCommandsId",commandsId)
//        pagesDeleg.changeCommandsId(commandsId)
//    }

    function addToHeadersArray(itemId, title, source, commandsId){
        console.log("addToHeadersArray", title,source,itemId)
//        multiDocView.pagesItems.push(itemId);
//        multiDocView.pagesSources.push(source);
//        tabPanelInternal.addToHeadersArray(title);

        var index = pages.InsertNewItem();
        pages.SetData("ItemId",itemId,index);
        pages.SetData("Title",title,index);
        pages.SetData("Source",source,index);
        pages.SetData("CommandsId",commandsId,index);
        tabPanelInternal.selectedIndex = pages.GetItemsCount() - 1;
        pages.Refresh();
        pagesDeleg.changeCommandsId(commandsId);
        console.log("addToHeadersArray", itemId, index)
//        tabPanelInternal.model = 0;
//        tabPanelInternal.model = pages;
    }

    Component.onCompleted: {
       docsData.anchors.topMargin =  tabPanelInternal.height;
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
//            console.log("multiDocView changeCommandsId", commandsId)
            var commandsId = pages.GetData("CommandsId");
            pagesDeleg.changeCommandsId(commandsId)
        }
    }

    ListView {
        id: docsData;
//            anchors.fill: parent;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.topMargin: tabPanelInternal.height;
        anchors.bottom: parent.bottom;
        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        orientation: ListView.Horizontal;
//            spacing: 0;
//        model: tabPanelInternal.count;
        model: pages;
        delegate: Rectangle {
//            id: pagesDeleg;
//            anchors.top: tabPanelInternal.bottom;
//            anchors.bottom: multiDocView.bottom;
            width: visible ? docsData.width : 0;
//            width: 500;
            height: docsData.height;
            color: "transparent";
            visible: tabPanelInternal.selectedIndex === model.index;
            onVisibleChanged: {
                if(this.visible){
                    multiDocView.activeItem = loader.item;
                }
            }

            Loader {
                id: loader;
                anchors.fill: parent;
                Component.onCompleted: {
                    console.log("MultidocWorkspaceView model index ",model.index)
//                    loader.source = multiDocView.pagesSources[model.index];
                    loader.source = model.Source
                    console.log("MultidocWorkspaceView source",loader.source)
                }
                onItemChanged: {
                    if (loader.item && loader.source != ""){
//                        loader.item.itemId = multiDocView.pagesItems[model.index];
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

                        loader.item.model = dataModelLocal

//                        loader.item.gqlModelInfo = multiDocView.pagesInfo
//                        loader.item.gqlModelItems = multiDocView.pagesItems
                    }
                }

            }
        }
    }

}
