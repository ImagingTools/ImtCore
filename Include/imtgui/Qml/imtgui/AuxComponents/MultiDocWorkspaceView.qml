import QtQuick 2.12
import Acf 1.0

Item {
    id: documentManager;

    anchors.fill: parent;

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem;

    property alias isCloseEnable: tabPanelInternal.isCloseEnable;

    property alias pagesCount: docsData.count;

    property string operation;

    TreeItemModel {
        id: documentsData;
    }

    function addDocument(document){
        let itemId = document["Id"];
        console.log("MultidocWorkspaceView addDocument", itemId, document["CommandsId"])

        let pageIndex = this.getDocumentIndexById(itemId);
        console.log("MultidocWorkspaceView pageIndex", pageIndex)
        if (pageIndex < 0){
            var index = documentsData.InsertNewItem();
            console.log("MultidocWorkspaceView addDocument index:", index)

            //TODO -> Uuid
            documentsData.SetData("Id", itemId, index);
            documentsData.SetData("CommandsId", document["CommandsId"], index);
            documentsData.SetData("Title", document["Name"], index);
            documentsData.SetData("Source", document["Source"], index);

            pageIndex = index;
        }

        tabPanelInternal.selectedIndex = pageIndex;
    }

    function closeDocument(itemId){
         let index = this.getDocumentIndexById(itemId);;
         if (index >= 0){
             documentsData.RemoveItem(index);

             if (tabPanelInternal.selectedIndex >= index && index > 0){
                 tabPanelInternal.selectedIndex--;
             }
         }
     }

    function setDocumentTitle(parameters){
        let itemId = parameters["Id"];
        let newTitle = parameters["Title"];

        let index = this.getDocumentIndexById(itemId);
        if (index >= 0){
            documentsData.SetData("Title", newTitle, index);
        }
    }

    function getDocumentIndexById(documentId){
        for (var i = 1; i < documentsData.GetItemsCount(); i++){
            var m_id = documentsData.GetData("Id", i);
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
        anchors.top: tabPanelInternal.bottom;
       // anchors.topMargin: 20;//thumbnailDecoratorContainer.mainMargin;
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
                if(docsDataDeleg.visible && dataLoader.item){
                    console.log("MultidocWorkspaceView onVisibleChanged", dataLoader.item);

                    documentManager.activeItem = dataLoader.item;
                    documentManager.activeItem.forceActiveFocus();
                }
            }

            Component.onDestruction: {
                console.log("Item onDestruction");
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                source: model.Source;

                Component.onCompleted: {
                    console.log("Loader onCompleted", model.Source);
                }

                Component.onDestruction: {
                    console.log("Loader onDestruction");
                }

                onLoaded: {
                    console.log("MultidocWorkspaceView onLoaded", model.CommandsId);

                    if(docsDataDeleg.visible){
                        documentManager.activeItem = dataLoader.item;
                        documentManager.activeItem.forceActiveFocus();
                    }
                    dataLoader.item.commandsId = model.CommandsId

                    documentsData.SetData("Item", dataLoader.item, model.index);
                }
            }
        }
    }
}
