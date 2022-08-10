import QtQuick 2.12
import Acf 1.0

Item {
    id: multiDocView;

    anchors.fill: parent;

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem;
    property Item rootItem;

    property alias pagesCount: docsData.count;
    property alias firstElementImageSource: tabPanelInternal.firstElementImageSource;

    property string operation;

    property int mainMargin: 0;

    Component.onCompleted: {
        console.log("MultidocWorkspaceView onCompleted", tabPanelInternal.selectedIndex)
        docsData.anchors.topMargin = tabPanelInternal.height;
    }

    onFocusChanged: {
        console.log("MultidocWorkspaceView onFocusChanged", multiDocView.focus);

        if (multiDocView.focus){
            multiDocView.activeItem.forceActiveFocus();
        }
    }

    function setFocus(){
        console.log("MultidocWorkspaceView setFocus");
        multiDocView.forceActiveFocus();
    }

    function addPage(page){
        console.log("MultidocWorkspaceView addPage", page)

        let itemId = page["Id"];

        let pageIndex = multiDocView.getPageIndexById(itemId);
        if (pageIndex < 0){
            var index = pagesData.InsertNewItem();
            pagesData.SetData("ItemId", itemId, index);
            pagesData.SetData("Title", page["Name"], index);
            pagesData.SetData("Source", page["Source"], index);
            pagesData.SetData("CommandsId", page["CommandsId"], index);

            pageIndex = index;
        }

        tabPanelInternal.selectedIndex = pageIndex;
    }

     function closePage(itemId){
         console.log("MultidocWorkspaceView closePage", itemId);
         let index;
         if (itemId == ""){
             console.log("Empty");
             index = tabPanelInternal.selectedIndex;
         }
         else{
             index = multiDocView.getPageIndexById(itemId);
         }

         if (index >= 0){
             pagesData.RemoveItem(index);

             if (tabPanelInternal.selectedIndex >= index && index > 0){
                 tabPanelInternal.selectedIndex--;
             }
         }
     }

     function updatePageId(parameters){
         let oldId = parameters["OldId"];
         let newId = parameters["NewId"];

         let index;
         if (oldId === ""){
             index = tabPanelInternal.selectedIndex;
         }
         else{
             index = multiDocView.getPageIndexById(oldId);
         }

         if (index >= 0){
             pagesData.SetData("ItemId", newId, index);
         }
     }

    function updatePageTitle(parameters){
        let itemId = parameters["ItemId"];
        let newTitle = parameters["Title"];

        let index;
        if (itemId === ""){
            index = tabPanelInternal.selectedIndex;
        }
        else{
            index = multiDocView.getPageIndexById(itemId);
        }

        if (index >= 0){
            pagesData.SetData("Title", newTitle, index);
        }
    }

    function getPageIndexById(pageId){
        console.log("MultidocWorkspaceView getPageIndexById", pageId);

        for (var i = 1; i < pagesData.GetItemsCount(); i++){
            var m_id = pagesData.GetData("ItemId", i);
            if (m_id === pageId){
                return i;
            }
        }

        return -1;
    }

    TreeItemModel {
        id: pagesData;
    }

    TabPanel {
        id: tabPanelInternal;

        anchors.left: parent.left;
        anchors.leftMargin: multiDocView.mainMargin;
        anchors.right: parent.right;
        anchors.rightMargin: multiDocView.mainMargin;

        visible: true;
        model: pagesData;

        onCloseItem: {
            console.log("MultiDocWorkspaceView TabPanel onCloseItem", index)

            let item = pagesData.GetData("Item", index);
            item.commands.commandActivated("Close");
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

        delegate: Item {
            id: docsDataDeleg;

            width: visible ? docsData.width : 0;
            height: docsData.height;

            visible: tabPanelInternal.selectedIndex === model.index;

            onVisibleChanged: {
                if(docsDataDeleg.visible){
                    multiDocView.activeItem = dataLoader.item;
                }
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                Component.onCompleted: {
                    console.log("dataLoader onCompleted", model.Source);
                    dataLoader.source = model.Source;
                }

                onItemChanged: {
                    console.log("MultidocWorkspaceView dataLoader onItemChanged", dataLoader.source, docsDataDeleg)
                    if (dataLoader.item){
                        dataLoader.item.rootItem = multiDocView;
                        dataLoader.item.itemId = model.ItemId
                        dataLoader.item.itemName = model.Title
                        dataLoader.item.commandsId = model.CommandsId

                        pagesData.SetData("Item", dataLoader.item, model.index);
                        docsData.currentIndex = model.index;
                    }
                }
            }
        }
    }
}
