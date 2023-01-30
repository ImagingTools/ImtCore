import QtQuick 2.12
import Acf 1.0

Item {
    id: workspaceView;

    anchors.fill: parent;

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem;

    property alias isCloseEnable: tabPanelInternal.isCloseEnable;

    property alias pagesCount: docsData.count;

    property string operation;

    property TreeItemModel documentsData: TreeItemModel {}

//    property var openedDocuments: []

//    function openDocument(document){
//        const comp = Qt.createComponent("SingleDocumentData.qml");
//        if (comp.status === Component.Ready){
//            let documentData = comp.createObject(workspaceView, { documentId: document["Id"], commandId: document["CommandsId"]});

//            workspaceView.openedDocuments.push(documentData);

//            var index = documentsData.InsertNewItem();

//            documentsData.SetData("Id", document["Id"], index);
//            documentsData.SetData("CommandsId", document["CommandsId"], index);
//            documentsData.SetData("Title", document["Name"], index);
//            documentsData.SetData("Source", document["Source"], index);

//            tabPanelInternal.selectedIndex = index;
//        }
//    }

    function addDocument(document){
        let itemId = document["Id"];
        console.log("MultidocWorkspaceView addDocument", itemId, document["CommandsId"])

        let pageIndex = this.getDocumentIndexById(itemId);
        console.log("MultidocWorkspaceView pageIndex", pageIndex)
        if (pageIndex < 0){
            var index = documentsData.InsertNewItem();
            console.log("MultidocWorkspaceView addDocument index:", index)

            documentsData.SetData("Id", itemId, index);
            documentsData.SetData("CommandsId", document["CommandsId"], index);
            documentsData.SetData("Title", document["Name"], index);
            documentsData.SetData("Source", document["Source"], index);

            pageIndex = index;
        }

        tabPanelInternal.selectedIndex = pageIndex;
    }

//    function closeDocument(documentKey){
//        for (let i = 0; i < openedDocuments.length; i++){
//            let documentData = openedDocuments[i]

//            if (documentData.key === documentKey){
//                openedDocuments.splice(i, 1);

//                documentsData.RemoveItem(i + 1);

//                if (tabPanelInternal.selectedIndex >= i + 1 && i + 1 > 0){
//                    tabPanelInternal.selectedIndex--;
//                }

//                break;
//            }
//        }
//    }

    function closeDocument(itemId){
         let index = this.getDocumentIndexById(itemId);;
         if (index >= 0){
             workspaceView.documentsData.RemoveItem(index);

             if (tabPanelInternal.selectedIndex >= index && index > 0){
                 tabPanelInternal.selectedIndex--;
             }
         }
     }

    function saveOpenedDocuments(){

    }

    function setDocumentTitle(parameters){
        let itemId = parameters["Id"];
        let newTitle = parameters["Title"];

        let index = this.getDocumentIndexById(itemId);
        if (index >= 0){
            workspaceView.documentsData.SetData("Title", newTitle, index);
        }
    }

//    function getDocumentIndexById(documentId){
//        for (let i = 0; i < openedDocuments.length; i++){
//            let documentData = openedDocuments[i]

//            if (documentData.documentId === documentId){
//                return i;
//            }
//        }

//        return -1;
//    }

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
        model: workspaceView.documentsData;

        onCloseItem: {
            let item = workspaceView.documentsData.GetData("Item", index);
            item.commandsDelegate.commandHandle("Close");
        }

        onRightClicked: {
            if (tabPanelInternal.selectedIndex < workspaceView.documentsData.GetItemsCount() - 1) {
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
        model: workspaceView.documentsData;

        delegate: Item {
            id: docsDataDeleg;

            width: visible ? docsData.width : 0;
            height: docsData.height;

            visible: tabPanelInternal.selectedIndex === model.index;

            onVisibleChanged: {
                if(docsDataDeleg.visible && dataLoader.item){
                    console.log("MultidocWorkspaceView onVisibleChanged", dataLoader.item);

                    workspaceView.activeItem = dataLoader.item;
                    workspaceView.activeItem.forceActiveFocus();
                }
            }

            Component.onDestruction: {
                console.log("Item onDestruction");
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                source: model.Source;

                onLoaded: {
                    console.log("MultidocWorkspaceView onLoaded", model.CommandsId);

                    if(docsDataDeleg.visible){
                        workspaceView.activeItem = dataLoader.item;
                        workspaceView.activeItem.forceActiveFocus();
                    }

                    if(dataLoader.item.documentsData !==undefined){
                        dataLoader.item.documentsData = workspaceView.documentsData;
                    }

                    dataLoader.item.commandsId = model.CommandsId

                    workspaceView.documentsData.SetData("Item", dataLoader.item, model.index);
                }
            }
        }
    }
}
