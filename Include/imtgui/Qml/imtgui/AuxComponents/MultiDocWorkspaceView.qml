import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0

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
    property bool showLoading: true;

    property MainDocumentManager mainDocumentManager: null;

    property alias alertPanelComp: alertPanel.sourceComponent;

    Component.onCompleted: {
        Events.subscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);
        Events.subscribeEvent("DocumentUpdating", workspaceView.documentUpdating);
        Events.subscribeEvent("OnLocalizationChanged", workspaceView.onLocalizationChanged);
        Events.subscribeEvent("PageNameChanged", workspaceView.onPageNameChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);
        Events.unSubscribeEvent("DocumentUpdating", workspaceView.documentUpdating);
        Events.unSubscribeEvent("OnLocalizationChanged", workspaceView.onLocalizationChanged);
        Events.unSubscribeEvent("PageNameChanged", workspaceView.onPageNameChanged);
    }

    onVisibleChanged: {
        if (workspaceView.visible){
            Events.subscribeEvent("DocumentIsDirtyChanged", workspaceView.documentIsDirtyChanged);
            Events.subscribeEvent("DocumentUpdating", workspaceView.documentUpdating);
        }
        else{
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
        console.log("onDocumentLoadingChanged", workspaceView.documentLoading);
        if (workspaceView.showLoading){
            loading.visible = workspaceView.documentLoading;
        }
    }

    onMainCollectionViewChanged: {
        if (workspaceView.mainCollectionView != null){
            workspaceView.mainCollectionView.elementsChanged.connect(workspaceView.onMainCollectionUpdated);
        }
    }

    function onLocalizationChanged(language){
        console.log("workspaceView onLocalizationChanged", language);
    }

    function onMainCollectionUpdated(){
        if (workspaceView.mainCollectionView != null){
            let notificationModel = workspaceView.mainCollectionView.notificationModel;
            if (notificationModel){
                if (notificationModel.ContainsKey("TotalCount")){
                    let totalCount = notificationModel.GetData("TotalCount");

                    let collectionTitle = workspaceView.documentsData.GetData("Name");

                    collectionTitle += " (" + totalCount + ")"

                    workspaceView.documentsData.SetData("Title", collectionTitle);
                }
            }
        }
    }

    function onPageNameChanged(parameters){
        let pageId = parameters["Id"]
        let pageName = parameters["Name"]

        if (workspaceView.mainCollectionView != null){
            let collectionItemId = String(workspaceView.mainCollectionView.itemId);
            if (pageId == collectionItemId){
                let newName = String(pageName);

                workspaceView.documentsData.SetData("Name", newName);

                onMainCollectionUpdated();
            }
        }
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

    function closeAllDocuments(force){
        for (let i = 1; i < workspaceView.documentsData.GetItemsCount(); i++){
            let documentBase = workspaceView.documentsData.GetData("Item", i);
            workspaceView.closeDocument(documentBase.itemId, force);
        }
    }

    function getDocumentIDs(){
        let ids = []

        for (let i = 1; i < workspaceView.documentsData.GetItemsCount(); i++){
            let documentBase = workspaceView.documentsData.GetData("Item", i);
            ids.push(documentBase.itemId);
        }

        return ids;
    }

    // TODO: Rename Item
    function documentIsDirty(documentId){
        let index = getDocumentIndexById(documentId);
        if (index >= 0){
            let documentBase = workspaceView.documentsData.GetData("Item", index);
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
//            workspaceView.mainCollectionView.updateGui();
        }
    }

    UuidGenerator {
        id: uuidGenerator;
    }

    // isRequested - if true, then the document model will be requested from the server
    function addDocument(documentObj, params, isRequested){
        console.log();
        if (isRequested === undefined){
            isRequested = false;
        }

        workspaceView.documentLoading = true;

        let itemId = documentObj["Id"];
        let commandId = documentObj["CommandsId"];
        let name = documentObj["Name"];
        let source = documentObj["Source"];
        let parent = documentObj["Parent"];
        let addition = documentObj["addition"];

        let index = workspaceView.documentsData.InsertNewItem();

        workspaceView.documentsData.SetData("Id", itemId, index);
        workspaceView.documentsData.SetData("Parent", parent, index);
        workspaceView.documentsData.SetData("CommandsId", commandId, index);
        workspaceView.documentsData.SetData("Name", name, index);
        workspaceView.documentsData.SetData("Title", name, index);
        workspaceView.documentsData.SetData("addition", addition, index);
        workspaceView.documentsData.SetData("Source", source, index);

        tabPanelInternal.selectedIndex = index;

        if (isRequested){
            documentController.getData(itemId, params, commandId);
        }


        return index;
    }

    function openDocument(documentId, params){
        let documentIndex = workspaceView.getDocumentIndexById(documentId);
        if (documentIndex < 0){
            documentIndex = workspaceView.addDocument(params, params, true);
        }

        tabPanelInternal.selectedIndex = documentIndex;
    }

    QtObject {
        id: cacheData;

        property var callback;
    }

    function closeDocument(itemId, force, callback){
        console.log("closeDocument", itemId, force, callback);

        if (force === undefined){
            force = false;
        }

        let index = this.getDocumentIndexByUuid(itemId);
        if (index < 0){
            index = tabPanelInternal.selectedIndex;
        }

        if (index !== 0){
            let documentBase = workspaceView.documentsData.GetData("Item", index);

            if (documentBase.isDirty && !force){
                tabPanelInternal.selectedIndex = index;

                cacheData.callback = callback;

                modalDialogManager.openDialog(saveDialog, {});
            }
            else{
                if (tabPanelInternal.selectedIndex >= index && index > 0){
                    tabPanelInternal.selectedIndex--;
                }

                workspaceView.documentsData.RemoveItem(index);

                if (callback){
                    callback(true);
                }
            }
        }
    }

    function saveDocument(documentId){
        console.log("saveDocument", documentId);
        let documentIndex = -1;

        let isNew = false;
        if (documentId === ""){
            documentIndex = tabPanelInternal.selectedIndex;
            isNew = true;
        }
        else{
            documentIndex = this.getDocumentIndexById(documentId);
        }

        console.log("documentIndex", documentIndex);

        if (documentIndex >= 0){
            let commandId = workspaceView.mainCollectionView.getEditorCommandId();
            console.log("_DEBUG_commandId", commandId)

//            documentController.documentTypeId = commandId;

            let documentBase = workspaceView.documentsData.GetData("Item", documentIndex);

            if (!documentBase.documentCanBeSaved()){
                if (documentBase.closingFlag){
                    documentBase.closingFlag = false;
                }

                return;
            }

            documentBase.updateModel();

            let documentData = documentBase.documentModel;
            let additionInputParams = documentBase.additionInputParams;

            if (isNew){
                if (!documentBase.nameOutsideEditor){
                    documentController.setData(documentId, documentData, additionInputParams);
                }
                else{
                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the document:")});
                }
            }
            else{
                documentController.updateData(documentId, documentData, additionInputParams);
            }
        }
    }

    Component {
        id: saveDialog;
        MessageDialog {
            title: qsTr("Save document");

            message: qsTr("Save all changes ?")

            Component.onCompleted: {
                buttons.addButton({"Id":"Cancel", "Name":qsTr("Cancel"), "Enabled": true});
            }

            onFinished: {
                let documentBase = workspaceView.documentsData.GetData("Item", tabPanelInternal.selectedIndex);
                if (buttonId == "Yes"){
                    documentBase.closingFlag = true;

                    workspaceView.saveDocument(documentBase.itemId);
                }
                else if (buttonId == "No"){
                    documentBase.isDirty = false;
                    workspaceView.closeDocument(documentBase.itemId, false, cacheData.callback);
                }

                if (cacheData.callback){
                    cacheData.callback(false);
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

//                    documentBase.itemId = inputValue;

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

    function getDocumentIndexByUuid(uuid){
        if (uuid !== ""){
            for (var i = 0; i < documentsData.GetItemsCount(); i++){
                let document = workspaceView.documentsData.GetData("Item", i);
                let documentUuid = document.documentUuid;
                if (documentUuid === uuid){
                    return i;
                }
            }
        }

        return -1;
    }

    function showAlertMessage(message){
        alertMessage.message = message;

//        alertMessage.visible = true;
    }

    function hideAlertMessage(){
        alertMessage.visible = false;
    }

    AlertMessage {
        id: alertMessage;

        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: false;
    }

    Loader {
        id: alertPanel;

        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: alertPanel.status == Loader.Ready;
    }

    TabPanel {
        id: tabPanelInternal;

        anchors.top: alertPanel.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        visible: true;
        model: workspaceView.documentsData;

        onCloseItem: {
            console.log("onCloseItem", index);
            let documentBase = workspaceView.documentsData.GetData("Item", index);
            console.log("documentBase.itemId", documentBase.documentUuid);
            workspaceView.closeDocument(documentBase.documentUuid);
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
                let item = workspaceView.documentsData.GetData("Item", tabPanelInternal.selectedIndex);
                if (item !== null){
                    item.documentModel = documentController.documentModel;
                    documentController.documentModel = null
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
        }

        onError: {
            if (loading.visible){
                loading.stop();
            }

            workspaceView.openErrorDialog(message);
        }

        onSetModelStateChanged: {
            if (setModelState === "Loading"){
                loading.start();
            }
            else{
                loading.stop();
            }
        }
    }

    function openErrorDialog(message){
        modalDialogManager.openDialog(savingErrorDialog, {"message": message});
    }

    GqlDocumentObserver {
        id: documentObserver;
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

                source: model.Source ? model.Source : "";

                onLoaded: {
                    workspaceView.documentsData.SetData("Item", dataLoader.item, model.index);

                    if (model.index === 0){
                        workspaceView.mainCollectionView = dataLoader.item;
                    }

                    if(docsDataDeleg.visible){
                        workspaceView.activeItem = dataLoader.item;
                    }

                    if(dataLoader.item.documentsData !== undefined){
                        dataLoader.item.documentsData = workspaceView.documentsData;
                    }

                    if(dataLoader.item.additionInputParams !== undefined){
                        dataLoader.item.additionInputParams = model.addition
                    }

                    if(dataLoader.item.documentManager !== undefined){
                        dataLoader.item.documentManager = workspaceView;
                    }

                    if(dataLoader.item.mainDocumentManager !== undefined){
                        dataLoader.item.mainDocumentManager = workspaceView.mainDocumentManager;
                    }

                    dataLoader.item.itemId = model.Id;
                    dataLoader.item.commandsId = model.CommandsId

                    if (dataLoader.item.documentUuid !== undefined){
                        workspaceView.documentsData.SetData("DocumentUuid", dataLoader.item.documentUuid);
                    }

                    if (dataLoader.item.parentRef !== undefined){
                        dataLoader.item.parentRef = model.Parent;
                    }

                    if (documentController.documentModel != null){
                        dataLoader.item.documentModel = documentController.documentModel;
                        documentController.documentModel = null
                    }
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
            Events.sendEvent("SetCommandsVisible", !loading.visible);
        }
    }
}
