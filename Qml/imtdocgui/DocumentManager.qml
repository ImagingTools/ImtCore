import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: documentManager;

    property string defaultDocumentName: qsTr("<no name>");

    property ListModel documentsModel: ListModel {
        dynamicRoles: true;
    }

    signal documentClosed(int documentIndex, string documentId);
    signal documentAdded(int documentIndex, string documentId);


    function openErrorDialog(message){
        modalDialogManager.openDialog(errorDialogComp, {"message": message});
    }


    function getDocumentsCount()
    {
        return documentsModel.count;
    }


    function getActiveView()
    {
        return internal.m_activeViewPtr;
    }


    function setActiveView(viewPtr)
    {
        if (internal.m_activeViewPtr !== viewPtr){
            internal.m_activeViewPtr = viewPtr;
        }
    }


    function getDocumentTypeId(document)
    {
        let documentsCount = getDocumentsCount();
        for (let documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
            let info = internal.getSingleDocumentData(documentIndex);
            if (info.documentPtr === document){
                return info.documentTypeId;
            }
        }

        return String();
    }


    function registerDocumentValidator(documentTypeId, validatorComp){
        if (!documentIsRegistered(documentTypeId)){
            console.error("Unable to register validator. Document with documentTypeId: ", documentTypeId, " unregistered!");

            return false;
        }

        internal.m_registeredValidators[documentTypeId] = validatorComp;

        return true;
    }


    function registerDocumentDataController(documentTypeId, dataControllerComp){
        internal.m_registeredDataControllers[documentTypeId] = dataControllerComp;
        console.log("registerDocumentDataController", documentTypeId, dataControllerComp)

        return true;
    }

    function unRegisterDocumentDataController(documentTypeId){
        delete internal.m_registeredDataControllers[documentTypeId]

        return true;
    }


    function registerDocumentView(documentTypeId, viewTypeId, viewComp)
    {
        if (documentTypeId === "" || viewTypeId === "" || !viewComp){
            return false;
        }

        if (documentIsRegistered(documentTypeId)){
            if (documentViewIsRegistered(viewTypeId)){
                console.error("View with type-ID: ", viewTypeId, " already registered!");

                return false;
            }

            internal.m_registeredView[documentTypeId].push({"ViewTypeId": viewTypeId, "ViewComp": viewComp});
        }
        else{
            internal.m_registeredView[documentTypeId] = [{"ViewTypeId": viewTypeId, "ViewComp": viewComp}]
        }

        return true;
    }

    function unRegisterDocumentView(documentTypeId, viewTypeId)
    {
        let registeredViewList = internal.m_registeredView[documentTypeId];
        for (let i = 0; i < registeredViewList.length; i++){
            let registeredViewObj = registeredViewList[i];
            if (registeredViewObj["ViewTypeId"] == viewTypeId){
                registeredViewList.splice(i,1)

                return true;
            }
        }

        return false;
    }

    function getDocumentView(documentTypeId, viewTypeId)
    {
        if (!documentViewIsRegistered(documentTypeId, viewTypeId)){
            console.error("Unable to get a document view. View is unregistered!");

            return null;
        }

        let registeredViewList = internal.m_registeredView[documentTypeId];
        for (let i = 0; i < registeredViewList.length; i++){
            let registeredViewObj = registeredViewList[i];
            if (registeredViewObj["ViewTypeId"] == viewTypeId){
                return registeredViewObj["ViewComp"];
            }
        }

        return null;
    }


    function getDocumentValidator(documentTypeId){
        if (documentTypeId in internal.m_registeredValidators){
            let validatorComp = internal.m_registeredValidators[documentTypeId];

            return validatorComp.createObject(documentManager);
        }

        return null;
    }


    function documentIsRegistered(documentTypeId)
    {
        return documentTypeId in internal.m_registeredView;
    }


    function documentViewIsRegistered(documentTypeId, viewTypeId)
    {
        if (documentIsRegistered(documentTypeId)){
            let registeredViewList = internal.m_registeredView[documentTypeId];
            for (let i = 0; i < registeredViewList.length; i++){
                let registeredViewObj = registeredViewList[i];
                if (registeredViewObj["ViewTypeId"] == viewTypeId){
                    return true;
                }
            }
        }

        return false;
    }


    function dataControllerIsRegistered(documentTypeId)
    {
        return documentTypeId in internal.m_registeredDataControllers;
    }


    function getDocumentDataController(documentTypeId)
    {
        console.log("getDocumentDataController", documentTypeId);
        if (!dataControllerIsRegistered(documentTypeId)){
            console.error("Data controller for documents with type-ID: ", documentTypeId, " is unregistered!");

            return defaultDataController.createObject(documentManager);
        }

        let dataControllerComp = internal.m_registeredDataControllers[documentTypeId];
        console.log("dataControllerComp", dataControllerComp);
        console.log("documentManager", documentManager);

        return dataControllerComp.createObject(documentManager);
    }


    function insertNewDocument(documentTypeId, viewTypeId)
    {
        console.log("insertNewDocument", documentTypeId, viewTypeId);
        let documentData = createTemplateDocument("", documentTypeId, createDocumentModel(null));
        if (!documentData){
            return false;
        }

        let documentViewComp = getDocumentView(documentTypeId, viewTypeId);
        if (!documentViewComp){
            return false;
        }

        documentData.documentIndex = documentsModel.count;

        documentsModel.append({
                                  "Uuid": documentData.documentId,
                                  "Title": defaultDocumentName,
                                  "TypeId": documentTypeId,
                                  "DocumentData": documentData,
                                  "DocumentViewComp": documentViewComp,
                                  "IsNew": true
                              });

        documentAdded(documentsModel.count - 1, "");

        return true;
    }


    function createTemplateDocument(documentId, documentTypeId, documentModel){
        console.log("createTemplateDocument", documentId, documentTypeId, documentModel);

        let singleDocumentData = singleDocumentDataComp.createObject(documentManager);
        if (singleDocumentData){
            if (documentId === ""){
                singleDocumentData.documentId = UuidGenerator.generateUUID();
            }
            else{
                singleDocumentData.documentId = documentId;
            }

            let documentDataController = getDocumentDataController(documentTypeId);
            if (!documentDataController){
                return false;
            }

            documentDataController.documentId = singleDocumentData.documentId;

            singleDocumentData.documentDataController = documentDataController;
            singleDocumentData.documentTypeId = documentTypeId;

            let documentValidator = getDocumentValidator(documentTypeId);
            if (documentValidator){
                singleDocumentData.documentValidator = documentValidator;
            }

            if (documentModel){
                documentDataController.documentModel = documentModel;
            }
            else{
                documentDataController.updateDocumentModel();
            }

            return singleDocumentData;
        }

        return null;
    }


    function openDocument(documentId, documentTypeId, viewTypeId)
    {
        console.log("openDocument", documentId, documentTypeId, viewTypeId);

        let index = getDocumentIndexByDocumentId(documentId);
        if (index >= 0){
            // already opened
            documentAdded(index, documentId);

            return;
        }

        let documentData = createTemplateDocument(documentId, documentTypeId);
        if (!documentData){
            return false;
        }

        let documentViewComp = getDocumentView(documentTypeId, viewTypeId);
        if (!documentViewComp){
            return false;
        }

//        Events.sendEvent("StartLoading");

        documentData.documentIndex = documentsModel.count;
        documentsModel.append({
                                  "Uuid": documentData.documentId,
                                  "Title": defaultDocumentName,
                                  "TypeId": documentTypeId,
                                  "DocumentData": documentData,
                                  "DocumentViewComp": documentViewComp,
                                  "IsNew": false
                              });

        documentAdded(documentsModel.count - 1, documentId);

        return true;
    }

    /*!
        Save document by UUID.

        \param      documentId           UUID of the document
    */
    function saveDocument(documentId)
    {
        console.log("DocumentManager saveDocument")
        Events.sendEvent("StartLoading");

        let index = getDocumentIndexByDocumentId(documentId);
        if (index >= 0){
            let isNew = documentsModel.get(index).IsNew;
            let document = documentsModel.get(index).DocumentData;
            if (document.isDirty){
                for (let i = 0; i < document.views.length; i++){
                    if (!document.views[i].readOnly){
                        document.views[i].doUpdateModel();
                    }
                }

                if (document.documentValidator){
                    let data = {}
                    let documentIsValid = document.documentValidator.isValid(data);
                    if (!documentIsValid){
                        openErrorDialog(data.message);
                        Events.sendEvent("StopLoading");

                        return;
                    }
                }

                console.log("documentDataController", document.documentDataController)

                if (document.documentDataController){
                    if (isNew){
                        document.documentDataController.insertDocument();
                    }
                    else{
                        document.documentDataController.saveDocument();
                    }
                }
            }
        }
    }


    function onDocumentSaved(documentId, documentIndex)
    {
        Events.sendEvent("StopLoading");

        if (internal.m_closingDocuments.includes(documentId)){
            closeDocument(documentId);
        }

        if (documentIndex >= 0){
            if (documentsModel.get(documentIndex).IsNew){
                documentsModel.setProperty(documentIndex, "IsNew", false);
            }
        }
    }


    function getDocumentIndexByDocumentId(documentId){
        for (let i = 0; i < documentsModel.count; i++){
            let documentData = documentsModel.get(i).DocumentData;
            if (documentData && documentData.documentId === documentId){
                return i;
            }
        }

        return -1;
    }


    function getDocumentTitle(documentIndex)
    {
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            return "";
        }

        return documentsModel.get(documentIndex).Title;
    }


    function generateDocumentTitle(documentIndex){
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            return "";
        }

        let title = defaultDocumentName;

        let documentData = documentsModel.get(documentIndex).DocumentData;

        let documentName = "";

        if (documentData){
            console.log("getDocumentName", documentData.getDocumentName());
            documentName = documentData.getDocumentName();
        }

        if (documentName && documentName !== ""){
            title = documentName;
        }

        if (documentData && documentData.isDirty){
            title += " *";
        }

        return title;
    }


    function updateDocumentTitle(documentIndex){
        console.log("updateDocumentTitle", documentIndex);
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            return;
        }

        let documentTitle = generateDocumentTitle(documentIndex);
        setDocumentTitle(documentIndex, documentTitle);
    }


    function setDocumentTitle(documentIndex, title)
    {
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            return;
        }

        documentsModel.setProperty(documentIndex, "Title", title);
    }


    // soon
    function saveDirtyDocuments(beQuiet, ignoredPtr)
    {
    }


    /*!
        Close document by document index.

        \param      documentIndex    index of the document from the model of all open documents
        \param      force            if true - document will be closed without isDirty checking
    */
    function closeDocumentByIndex(documentIndex, force)
    {
        console.log("closeDocumentByIndex", documentIndex);
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            console.error("Unable to close document with index: ", documentIndex);

            return;
        }

        if (!force){
            force = false;
        }

        let documentData = documentsModel.get(documentIndex).DocumentData;

        if (documentData.isDirty && !force){
            let callback = function(result){
                if (result == Enums.yes){
                    internal.m_closingDocuments.push(documentData.documentId);
                    documentManager.saveDocument(documentData.documentId);
                }
                else if (result == Enums.no){
                    documentData.isDirty = false;

                    documentManager.closeDocumentByIndex(documentIndex);
                }

                modalDialogManager.finished.disconnect(callback);
            }

            modalDialogManager.openDialog(saveDialog, {}, "", callback);
        }
        else{
            let index = internal.m_closingDocuments.indexOf(documentData.documentId);
            if (index !== -1) {
                internal.m_closingDocuments.splice(index, 1);
            }

            documentData.destroy();
            documentsModel.remove(documentIndex);

            documentClosed(documentIndex, "");
        }
    }


    function closeDocument(documentId, force)
    {
        let index = getDocumentIndexByDocumentId(documentId);

        closeDocumentByIndex(index, force);
    }


    function createDocumentComponent(documentTypeId)
    {
        let viewComp = getDocumentView(documentTypeId);
        if (!viewComp){
            console.error("Unable to create a document component. View component is invalid!");

            return false;
        }

        return viewComp;
    }


    function createDocumentModel(parent)
    {
        return treeItemModelComp.createObject(parent);
    }


    Component {
        id: singleDocumentDataComp;

        QtObject {
            id: singleDocumentData;

            // index of the document in document manager
            property int documentIndex: -1;
            property string documentId;
            property string documentName;
            property string documentTypeId;
            property DocumentDataController documentDataController: DocumentDataController {};
            property DocumentValidator documentValidator: DocumentValidator {};
            property UndoRedoManager undoManager: UndoRedoManager {
                onUndo: {
                    singleDocumentData.checkDocumentModel();
                }

                onRedo: {
                    singleDocumentData.checkDocumentModel()
                }

                onModelChanged: {
                    let undoSteps = getAvailableUndoSteps();
                    let redoSteps = getAvailableRedoSteps();

                    for (let i = 0; i < singleDocumentData.views.length; i++){
                        if (singleDocumentData.views[i].commandsController){
                            singleDocumentData.views[i].commandsController.setCommandIsEnabled("Undo", undoSteps > 0);
                            singleDocumentData.views[i].commandsController.setCommandIsEnabled("Redo", redoSteps > 0);

                            singleDocumentData.views[i].doUpdateGui();
                        }
                    }
                }
            };

            property var views: [];
            property bool isDirty: false;

            property Connections connections: Connections {
                function onSaved(documentId, documentName){
                    singleDocumentData.documentId = documentId;
                    singleDocumentData.documentName = documentName;

                    singleDocumentData.isDirty = false;

                    let documentModel = singleDocumentData.documentDataController.documentModel;
                    if (singleDocumentData.undoManager){
                        singleDocumentData.undoManager.setStandardModel(documentModel);
                    }

                    documentManager.updateDocumentTitle(singleDocumentData.documentIndex);

                    documentManager.onDocumentSaved(documentId, singleDocumentData.documentIndex);
                }

                function onDocumentModelChanged(){
                    let documentModel = singleDocumentData.documentDataController.documentModel;

                    if (documentModel.ContainsKey("Id")){
                        singleDocumentData.documentId = documentModel.GetData("Id");
                    }

                    if (documentModel.ContainsKey("Name")){
                        singleDocumentData.documentName = documentModel.GetData("Name");
                    }

                    singleDocumentData.blockingUpdateModel = true;
                    for (let i = 0; i < singleDocumentData.views.length; i++){
                        singleDocumentData.views[i].model = documentModel;
                        if (documentManager.documentsModel.get(singleDocumentData.documentIndex).IsNew){
                            singleDocumentData.views[i].doUpdateModel();
                        }
                    }
                    singleDocumentData.blockingUpdateModel = false;

                    documentManager.updateDocumentTitle(singleDocumentData.documentIndex);

                    singleDocumentData.undoManager.registerModel(documentModel);
                    singleDocumentData.documentValidator.documentModel = documentModel;

                    Events.sendEvent("StopLoading");
                }

                function onError(){
                    console.log("OnError");
                    Events.sendEvent("StopLoading");
                }
            }

            property Connections modelConnections: Connections {
                target: singleDocumentData.documentDataController.documentModel

                function onDataChanged(){
                    console.log("Connections onModelChanged", singleDocumentData.blockingUpdateModel);
                    if (singleDocumentData.blockingUpdateModel){
                        return;
                    }

                    singleDocumentData.isDirty = true;
                }
            }

            property bool blockingUpdateModel: false;

            onDocumentDataControllerChanged: {
                console.log("onDocumentDataControllerChanged", documentDataController);
                if (documentDataController){
                    singleDocumentData.connections.target = documentDataController;
                }
            }

            onBlockingUpdateModelChanged: {
                if (undoManager){
                    undoManager.setBlockingUpdateModel(blockingUpdateModel);
                }
            }

            signal viewAdded(var view);
            signal viewRemoved(var view);

            Component.onDestruction: {
                Events.unSubscribeEvent(documentId + "CommandActivated", commandHandle);
                if (singleDocumentData.undoManager){
                    Events.unSubscribeEvent(documentId + "CommandActivated", singleDocumentData.undoManager.commandHandle);
                }
            }

            onDocumentIdChanged: {
                Events.subscribeEvent(documentId + "CommandActivated", commandHandle);

                if (singleDocumentData.undoManager){
                    Events.subscribeEvent(documentId + "CommandActivated", singleDocumentData.undoManager.commandHandle);
                }

                if (documentDataController){
                    documentDataController.documentId = documentId;
                }
            }

            onViewAdded: {
                console.log("onViewAdded", singleDocumentData.documentIndex);
                if (singleDocumentData.documentDataController){
                    singleDocumentData.blockingUpdateModel = true;
                    view.model = singleDocumentData.documentDataController.documentModel;

                    if (documentManager.documentsModel.get(singleDocumentData.documentIndex).IsNew){
                        view.doUpdateModel();
                    }
                    singleDocumentData.blockingUpdateModel = false;
                }
            }

            onViewRemoved: {}

            onIsDirtyChanged: {
                for (let i = 0; i < singleDocumentData.views.length; i++){
                    if (singleDocumentData.views[i].commandsController){
                        singleDocumentData.views[i].commandsController.setCommandIsEnabled("Save", isDirty);
                    }
                }

                documentManager.updateDocumentTitle(documentIndex);
            }

            function getDocumentName(){
                if (singleDocumentData.documentDataController){
                    return singleDocumentData.documentDataController.getDocumentName();
                }
            }

            function commandHandle(commandId){
                console.log("DicumentManager commandHandle, documentId", documentId);

                if (commandId === "Close"){
                    documentManager.closeDocument(documentId);
                }
                else if (commandId === "Save"){
                    documentManager.saveDocument(documentId);
                }
                else if (commandId === "History"){
                    modalDialogManager.openDialog(documentHistoryDialogComp,
                                                  {
                                                      "documentId": documentId,
                                                      "documentTypeId": documentTypeId
                                                  });
                }
            }

            function checkDocumentModel(){
                let currentStateModel = undoManager.getStandardModel();
                if (currentStateModel){
                    let documentModel = singleDocumentData.documentDataController.documentModel
                    let isEqual = currentStateModel.IsEqualWithModel(documentModel);
                    isDirty = !isEqual;
                }
            }

            property Shortcut shortcut: Shortcut {
                sequence: "Ctrl+S";
                enabled: true;
                onActivated: {
                    if (!singleDocumentData.isDirty){
                        return
                    }

                    singleDocumentData.commandHandle("Save");
                }
            }

            property Component documentHistoryDialogComp: Component {
                DocumentHistoryDialog {
                    title: qsTr("Document history");

                    onStarted: {
                        updateModel();
                    }
                }
            }
        }
    }

    Component {
        id: treeItemModelComp;

        TreeItemModel {}
    }

    Component {
        id: defaultDataController;

        DocumentDataController {
        }
    }

    QtObject {
        id: internal;

        function getSingleDocumentData(index)
        {
            if (index < 0 || index >= internal.m_documentInfos.length){
                return null;
            }

            return internal.m_documentInfos[index];
        }

        function getDocumentInfoFromView(view)
        {
            let documentsCount = documentManager.getDocumentsCount();
            for (let documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
                let info = internal.getSingleDocumentData(documentIndex);

                for (let viewIndex = 0; viewIndex < documentsCount; ++viewIndex){

                }
            }

            return internal.m_activeViewPtr[index];
        }

        property var m_registeredView: ({});
        property var m_registeredDataControllers: ({});
        property var m_registeredValidators: ({});
        property var m_documentInfos: [];
        property var m_activeViewPtr;
        property var m_closingDocuments: [];
    }

    Component {
        id: errorDialogComp;

        ErrorDialog {
        }
    }

    Component {
        id: saveDialog;

        MessageDialog {
            title: qsTr("Save document");

            message: qsTr("Save all changes ?")

            Component.onCompleted: {
                buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
            }

            onFinished: {
                if (buttonId === Enums.yes){
                }
            }
        }
    }
}
