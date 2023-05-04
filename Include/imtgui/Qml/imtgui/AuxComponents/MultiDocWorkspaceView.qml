import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

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

    property bool documentLoading: false;

    property MainDocumentManager mainDocumentManager: null;

    Component.onCompleted: {
        Events.subscribeEvent("DocumentSaved", workspaceView.documentSaved);
        Events.subscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
        Events.subscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);

        Events.subscribeEvent("DocumentUpdating", workspaceView.documentUpdating);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DocumentSaved", workspaceView.documentSaved);
        Events.unSubscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
        Events.unSubscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);

        Events.unSubscribeEvent("DocumentUpdating", workspaceView.documentUpdating);
    }

    onVisibleChanged: {
        if (workspaceView.visible){
            Events.subscribeEvent("DocumentSaved", workspaceView.documentSaved);
            Events.subscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
            Events.subscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);

            Events.subscribeEvent("DocumentUpdating", workspaceView.documentUpdating);
        }
        else{
            Events.unSubscribeEvent("DocumentSaved", workspaceView.documentSaved);
            Events.unSubscribeEvent("CloseDocument", workspaceView.closeCurrentDocument);
            Events.unSubscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);

            Events.unSubscribeEvent("DocumentUpdating", workspaceView.documentUpdating);
        }
    }

    onDocumentsDataChanged: {
        if(dataLoader.item && dataLoader.item.documentsData !==undefined){
            dataLoader.item.documentsData = workspaceView.documentsData;
        }
    }

    onDocumentLoadingChanged: {
        loading.visible = workspaceView.documentLoading;
    }

    function documentUpdating(updatingFlag){
        if (!updatingFlag){
            workspaceView.documentLoading = false;
        }
    }

    function getTypeId(){
        if (workspaceView.mainCollectionView != null){
            return workspaceView.mainCollectionView.commandsId;
        }

        return "";
    }

    function getMainCollectionView(){
        return workspaceView.mainCollectionView;
    }

    function documentIsDirtyChanged(parameters){
        let documentId = parameters["Id"];
        let isDirty = parameters["IsDirty"];

        let documentIndex = workspaceView.getDocumentIndexById(documentId);
        if (documentIndex < 0){
            documentIndex = tabPanelInternal.selectedIndex;
        }

        if (documentIndex > 0){
            let currentTitle = workspaceView.documentsData.GetData("Title", documentIndex);
            if (isDirty){
                let lastSymbol = currentTitle.charAt(currentTitle.length - 1);
                if (lastSymbol !== '*'){
                    currentTitle = currentTitle + '*';
                }
            }
            else{
                currentTitle = currentTitle.replace('*', '');
            }

            workspaceView.documentsData.SetData("Title", currentTitle, documentIndex);
        }
    }

    function dirtyDocumentsExists(){
        for (let i = 1; i < workspaceView.documentsData.GetItemsCount(); i++){
            let documentBase = workspaceView.documentsData.GetData("Item", i);
            if (documentBase.isDirty){
                return true;
            }
        }

        return false;
    }

    function documentSaved(parameters){
        let documentId = parameters["Id"];
        let documentName = parameters["Name"];

        workspaceView.documentsData.SetData("Id", documentId, tabPanelInternal.selectedIndex);

        let documentBase = workspaceView.documentsData.GetData("Item", tabPanelInternal.selectedIndex);
        workspaceView.setDocumentTitle({"Id": documentId, "Title": documentName});

        documentBase.itemId = documentId;
        documentBase.itemName = documentName;

        documentBase.isDirty = false;

        documentBase.saved();

        if (documentBase.closingFlag){
            workspaceView.closeDocument(documentBase.itemId);
        }

        if (workspaceView.mainCollectionView != null){
            workspaceView.mainCollectionView.updateGui();
        }
    }

    function newDocument(data){
        let index = documentsData.InsertNewItem();

        documentsData.SetData("Id", "", index);
        //documentsData.SetData("Title", "<new item>", index);
        documentsData.SetData("CommandsId", data["CommandsId"], index);
        documentsData.SetData("Source", data["Source"], index);

        tabPanelInternal.selectedIndex = index;
    }

    function addDocument(document){
        let itemId = document["Id"];

        if (itemId === ""){
            workspaceView.newDocument(document);

            return;
        }

        let documentIndex = this.getDocumentIndexById(itemId);
        if (documentIndex < 0){
            var index = documentsData.InsertNewItem();

            documentsData.SetData("Id", itemId, index);
            documentsData.SetData("CommandsId", document["CommandsId"], index);
            documentsData.SetData("Title", document["Name"], index);
            documentsData.SetData("Source", document["Source"], index);

            documentIndex = index;
        }

        tabPanelInternal.selectedIndex = documentIndex;
    }

    function closeDocument(itemId, force){
        if (force === undefined){
            force = false;
        }

         let index = this.getDocumentIndexById(itemId);
         if (index < 0){
             index = tabPanelInternal.selectedIndex;
         }

         if (index !== 0){
             let documentBase = workspaceView.documentsData.GetData("Item", index);

             if (documentBase.isDirty && !force){
                 tabPanelInternal.selectedIndex = index;

                 modalDialogManager.openDialog(saveDialog, {"message": qsTr("Save all changes ?")});
             }
             else{
                 if (tabPanelInternal.selectedIndex >= index && index > 0){
                     tabPanelInternal.selectedIndex--;
                 }

                 workspaceView.documentsData.RemoveItem(index);
             }
         }
     }

    function insertNewDocument(documentId, params){
        workspaceView.documentLoading = true;

        let index = documentsData.InsertNewItem();

        documentsData.SetData("Id", documentId, index);

        let commandId = workspaceView.mainCollectionView.getEditorCommandId();
        documentsData.SetData("CommandsId", commandId, index);

        let source = workspaceView.mainCollectionView.getEditorPath();
        documentsData.SetData("Source", source, index);

        documentController.documentTypeId = commandId;
        documentController.getData(documentId, params);

        return index;
    }

    function openDocument(documentId, params){
        let documentIndex = this.getDocumentIndexById(documentId);
        if (documentIndex < 0){
            documentIndex = workspaceView.insertNewDocument(documentId, params);
        }

        tabPanelInternal.selectedIndex = documentIndex;
    }

    function saveDocument(documentId){
        let documentIndex = -1;

        let isNew = false;
        if (documentId === ""){
            documentIndex = tabPanelInternal.selectedIndex;
            isNew = true;
        }
        else{
            documentIndex = this.getDocumentIndexById(documentId);
        }

        if (documentIndex >= 0){
            let commandId = workspaceView.mainCollectionView.getEditorCommandId();
            documentController.documentTypeId = commandId;

            let documentBase = workspaceView.documentsData.GetData("Item", documentIndex);

            if (!documentBase.documentCanBeSaved()){
                if (documentBase.closingFlag){
                    documentBase.closingFlag = false;
                }

                return;
            }

            documentBase.updateModel();

            let documentData = documentBase.documentModel;

            if (isNew){
                if (!documentBase.nameOutsideEditor){
                    documentController.setData(documentId, documentData);
                }
                else{
                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the document:")});
                }
            }
            else{
                documentController.updateData(documentId, documentData);
            }
        }
    }

    function updateDocumentModel(documentId, params){
        let documentIndex = this.getDocumentIndexById(documentId);
        if (documentIndex >= 0){
            let commandId = workspaceView.mainCollectionView.getEditorCommandId();
            documentController.documentTypeId = commandId;
            documentController.getData(documentId, params);
        }
    }

    Component {
        id: saveDialog;
        MessageDialog {
            Component.onCompleted: {
                buttons.addButton({"Id":"Cancel", "Name":"Cancel", "Enabled": true});
            }

            onFinished: {
                let documentBase = workspaceView.documentsData.GetData("Item", tabPanelInternal.selectedIndex);
                if (buttonId == "Yes"){
                    documentBase.closingFlag = true;

                    workspaceView.saveDocument(documentBase.itemId);
                }
                else if (buttonId == "No"){
                    documentBase.isDirty = false;
                    workspaceView.closeDocument(documentBase.itemId);
                }
            }
        }
    }

    Component {
        id: inputDialog;
        InputDialog {
            title: qsTr("Entering a name");
            onFinished: {
                if (buttonId == "Ok"){
                    let documentBase = workspaceView.documentsData.GetData("Item", tabPanelInternal.selectedIndex);
                    documentBase.onEntered(inputValue);

                    documentController.setData(documentBase.itemId, documentBase.documentModel);
                }
            }
        }
    }

    Component {
        id: savingErrorDialog;

        ErrorDialog {
            onFinished: {
            }
        }
    }

    function saveDirtyDocuments(){
    }

    function setDocumentTitle(parameters){
        let itemId = parameters["Id"];
        let newTitle = parameters["Title"];

        let index = this.getDocumentIndexById(itemId);
        if (index < 0){
            index = tabPanelInternal.selectedIndex;
        }

        if (index > 0){
            workspaceView.documentsData.SetData("Title", newTitle, index);

            let document = workspaceView.documentsData.GetData("Item", index);
            if (document.isDirty){
                workspaceView.documentIsDirtyChanged({"Id": document.itemId, "IsDirty": true});
            }
        }
    }

    function getDocumentIndexById(documentId){
        if (documentId !== ""){
            for (var i = 0; i < documentsData.GetItemsCount(); i++){
                var id = documentsData.GetData("Id", i);
                if (id === documentId){
                    return i;
                }
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

    GqlDocumentDataController {
        id: documentController;

        onDocumentModelChanged: {
            if (documentController.documentModel != null){
                let documentId = documentController.documentModel.GetData("Id");
                if (documentId !== ""){
                    for (let i = 0; i < workspaceView.documentsData.GetItemsCount(); i++){
                        let id = workspaceView.documentsData.GetData("Id", i);
                        if (id === documentId){
                            if (workspaceView.documentsData.ContainsKey("Item", i)){
                                let item = workspaceView.documentsData.GetData("Item", i);
                                item.documentModel = documentController.documentModel;

                                break;
                            }
                        }
                    }
                }
                else{
                    let item = workspaceView.documentsData.GetData("Item", tabPanelInternal.selectedIndex);
                    item.documentModel = documentController.documentModel;
                }
            }
        }

        onDocumentAdded: {
            workspaceView.documentSaved({"Id":documentId, "Name":documentName});
        }

        onDocumentUpdated: {
            workspaceView.documentSaved({"Id":documentId, "Name":documentName});
        }

        onSavingError: {
            workspaceView.openErrorDialog(message);

//            if (container.documentBase.closingFlag){
//                container.documentBase.closingFlag = false;
//            }
        }
    }

    function openErrorDialog(message){
        modalDialogManager.openDialog(savingErrorDialog, {"message": message});
    }

    GqlDocumentObserver {
        id: documentObserver;

        observedGetModel: documentController.gqlGetModel;
        observedSetModel: documentController.gqlSetModel;
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
                    workspaceView.documentsData.SetData("Item", dataLoader.item, model.index);

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

                    if(dataLoader.item.mainDocumentManager !== undefined){
                        dataLoader.item.mainDocumentManager = workspaceView.mainDocumentManager;
                    }

                    //C
                    dataLoader.item.itemId = model.Id;
                    //

                    dataLoader.item.commandsId = model.CommandsId
                }
            }
        }
    }

    Loading {
        id: loading;

        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        visible: false;

        onVisibleChanged: {
            Events.sendEvent("CommandsDecoratorSetVisible", !loading.visible);
        }
    }
}
