import QtQuick 2.12
import Acf 1.0

Item {
    id: workspaceView;

    anchors.fill: parent;

    property TabPanel tabPanel: tabPanelInternal;

    property Item activeItem: null;

    property CollectionView mainCollectionView: null;

    property alias isCloseEnable: tabPanelInternal.isCloseEnable;

    property alias pagesCount: docsData.count;

    property string operation;

    property TreeItemModel documentsData: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("DocumentSaved", workspaceView.documentSaved);
        Events.subscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
        Events.subscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DocumentSaved", workspaceView.documentSaved);
        Events.unSubscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
        Events.unSubscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);
    }

    onVisibleChanged: {
        if (workspaceView.visible){
            Events.subscribeEvent("DocumentSaved", workspaceView.documentSaved);
            Events.subscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
            Events.subscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);
        }
        else{
            Events.unSubscribeEvent("DocumentSaved", workspaceView.documentSaved);
            Events.unSubscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
            Events.unSubscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);
        }
    }

    onDocumentsDataChanged: {
        if(dataLoader.item && dataLoader.item.documentsData !==undefined){
            dataLoader.item.documentsData = workspaceView.documentsData;
        }
    }

    function documentIsDirtyChanged(isDirty){
        let currentTitle = workspaceView.documentsData.GetData("Title", tabPanelInternal.selectedIndex);
        if (isDirty){
            let lastSymbol = currentTitle.charAt(currentTitle.length - 1);
            if (lastSymbol !== '*'){
                currentTitle = currentTitle + '*';
            }
        }
        else{
            currentTitle = currentTitle.replace('*', '');
        }

        workspaceView.documentsData.SetData("Title", currentTitle, tabPanelInternal.selectedIndex);
    }

    function documentSaved(parameters){
        let documentId = parameters["Id"];
        let documentName = parameters["Name"];

        workspaceView.documentsData.SetData("Id", documentId, tabPanelInternal.selectedIndex);
        workspaceView.documentsData.SetData("Title", documentName, tabPanelInternal.selectedIndex);

        let documentBase = workspaceView.documentsData.GetData("Item", tabPanelInternal.selectedIndex);

        if (workspaceView.mainCollectionView != null){
            workspaceView.mainCollectionView.updateGui();
        }
    }

    function newDocument(data){
        let index = documentsData.InsertNewItem();

        documentsData.SetData("Id", "", index);
        documentsData.SetData("Title", "<new item>", index);
        documentsData.SetData("CommandsId", data["CommandsId"], index);
        documentsData.SetData("Source", data["Source"], index);

        tabPanelInternal.selectedIndex = index;
    }

    function addDocument(document){
        let itemId = document["Id"];
        console.log("MultidocWorkspaceView addDocument", itemId, document["CommandsId"])

        if (itemId === ""){
            console.log("MultidocWorkspaceView new document")
            workspaceView.newDocument(document);

            return;
        }

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

    function closeCurrentDocument(){
        console.log("MultidocWorkspaceView closeCurrentDocument")
         if (tabPanelInternal.selectedIndex >= 0){
             workspaceView.documentsData.RemoveItem(tabPanelInternal.selectedIndex);

             if (tabPanelInternal.selectedIndex > 0){
                 tabPanelInternal.selectedIndex--;
             }
         }
     }

    function closeDocument(itemId){
         let index = this.getDocumentIndexById(itemId);
         if (index >= 0){
             workspaceView.documentsData.RemoveItem(index);

             if (tabPanelInternal.selectedIndex >= index && index > 0){
                 tabPanelInternal.selectedIndex--;
             }
         }
     }

    function saveDocument(index){
    }

    function saveDirtyDocuments(){
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
            tabPanelInternal.selectedIndex = index;

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
                    workspaceView.activeItem = dataLoader.item;
                }
            }

            Loader {
                id: dataLoader;

                anchors.fill: parent;

                source: model.Source;

                onLoaded: {
                    console.log("MultidocWorkspaceView onLoaded", model.CommandsId);
                    if (model.index === 0){
                        workspaceView.mainCollectionView = dataLoader.item;
                    }

                    if(docsDataDeleg.visible){
                        workspaceView.activeItem = dataLoader.item;
                    }

                    if(dataLoader.item.documentsData !==undefined){
                        dataLoader.item.documentsData = workspaceView.documentsData;
                    }

                    if(dataLoader.item.documentManager !==undefined){
                        dataLoader.item.documentManager = workspaceView;
                    }

                    //C
                    dataLoader.item.itemId = model.Id;
                    //

                    dataLoader.item.commandsId = model.CommandsId

                    workspaceView.documentsData.SetData("Item", dataLoader.item, model.index);
                }
            }
        }
    }
}
