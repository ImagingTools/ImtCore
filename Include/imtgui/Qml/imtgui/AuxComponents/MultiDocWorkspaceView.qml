import QtQuick 2.12
import Acf 1.0

Item {
    id: multiDocView;

    anchors.fill: parent;

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem;

    property alias pagesCount: docsData.count;

    property string operation;

    Component.onCompleted: {
        console.log("MultidocWorkspaceView onCompleted", tabPanelInternal.selectedIndex)
        docsData.anchors.topMargin = tabPanelInternal.height;
    }

    onFocusChanged: {
        console.log("MultidocWorkspaceView onFocusChanged", focus);

        if (focus){
            activeItem.forceActiveFocus();
        }
    }

    TreeItemModel {
        id: documentsData;
    }

    function addDocument(document){
        console.log("MultidocWorkspaceView addDocument", document)

        let itemId = document["Id"];

        let pageIndex = this.getDocumentIndexById(itemId);
        if (pageIndex < 0){
            var index = documentsData.InsertNewItem();

            //TODO -> Uuid
            documentsData.SetData("ItemId", itemId, index);
            documentsData.SetData("Title", document["Name"], index);
            documentsData.SetData("Source", document["Source"], index);
            documentsData.SetData("CommandsId", document["CommandsId"], index);

            pageIndex = index;
        }

        tabPanelInternal.selectedIndex = pageIndex;
    }

    function closeDocument(itemId){
         console.log("MultidocWorkspaceView closeDocument", itemId);
         let index = this.getDocumentIndexById(itemId);;
         if (index >= 0){
             documentsData.RemoveItem(index);

             if (tabPanelInternal.selectedIndex >= index && index > 0){
                 tabPanelInternal.selectedIndex--;
             }
         }
     }

    function setDocumentTitle(parameters){
        let itemId = parameters["ItemId"];
        let newTitle = parameters["Title"];

        let index = this.getDocumentIndexById(itemId);
        if (index >= 0){
            documentsData.SetData("Title", newTitle, index);
        }
    }

    function getDocumentIndexById(documentId){
        console.log("MultidocWorkspaceView getPageIndexById", documentId);

        for (var i = 1; i < documentsData.GetItemsCount(); i++){
            var m_id = documentsData.GetData("ItemId", i);
            if (m_id === documentId){
                return i;
            }
        }

        return -1;
    }

    TabPanel {
        id: tabPanelInternal;

        anchors.left: parent.left;
        anchors.right: parent.right;

        visible: true;
        model: documentsData;

        onCloseItem: {
            console.log("MultiDocWorkspaceView TabPanel onCloseItem", index)

            let item = documentsData.GetData("Item", index);
            item.commandsDelegate.commandHandle("Close");
        }

        onRightClicked: {
            if (tabPanelInternal.selectedIndex < documentsData.GetItemsCount() - 1) {
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
        model: documentsData;

        delegate: Item {
            id: docsDataDeleg;

            width: visible ? docsData.width : 0;
            height: docsData.height;

            visible: tabPanelInternal.selectedIndex === model.index;

            onVisibleChanged: {
                if(docsDataDeleg.visible){
                    activeItem = dataLoader.item;
                }
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                Component.onCompleted: {
                    console.log("dataLoader onCompleted", model.Source);
                    dataLoader.source = model.Source;
                }

                onLoaded: {
//                    dataLoader.item.itemId = model.ItemId
//                    dataLoader.item.itemName = model.Title

//                    dataLoader.item.model = model;

                    dataLoader.item.commandsId = model.CommandsId

                    documentsData.SetData("Item", dataLoader.item, model.index);
                }
            }
        }
    }
}
