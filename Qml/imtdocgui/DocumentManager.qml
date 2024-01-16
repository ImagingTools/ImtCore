import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: documentManager;

    property string defaultDocumentName: "<no name>"

    property ListModel documentsModel: ListModel {
        dynamicRoles: true;
    }

    property GqlDocumentDataController documentController: GqlDocumentDataController {
        onError: {
            internal.m_closingDocuments = []

            Events.sendEvent("SendError", {"Message": message, "ErrorType": type})
        }
    }

    UuidGenerator {
        id: uuidGenerator;
    }

    signal documentClosed(int documentIndex, string documentUuid);
    signal documentAdded(int documentIndex, string documentUuid);


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


    function registerDocument(documentTypeId, viewComp)
    {
        if (documentTypeId === "" || !viewComp){
            return false;
        }

        internal.m_registeredView[documentTypeId] = viewComp;

        return true;
    }


    function getDocumentView(documentTypeId)
    {
        if (!documentIsRegistered(documentTypeId)){
            console.error("Unable to get a document view. View is unregistered!");

            return null;
        }

        return internal.m_registeredView[documentTypeId];
    }


    function documentIsRegistered(documentTypeId)
    {
        return documentTypeId in internal.m_registeredView;
    }


    function insertNewDocument(documentTypeId, properties)
    {
        console.log("insertNewDocument",documentTypeId);
        let documentData = createDocumentComponent(documentTypeId);
        if (!documentData){
            return false;
        }

        documentsModel.append({
                                  "Uuid": uuidGenerator.generateUUID(),
                                  "Title": defaultDocumentName,
                                  "TypeId": documentTypeId,
                                  "Model": createDocumentModel(),
                                  "DocumentComp": documentData,
                                  "Properties": properties,
                                  "DocumentObj": null
                              });

        documentAdded(documentsModel.count - 1, "");

        return true;
    }

    /*!
        Open document by documentId.

        \param    documentId             Document-ID
        \param    documentTypeId         Document Type-ID
        \param    properties             Properties in QML View (Object)
        \param    additionInputParams    Additional input param for the GQL request
    */
    function openDocument(documentId, documentTypeId, properties, additionInputParams)
    {
        console.log('openDocument', documentId, documentTypeId)

        let index = getDocumentIndexByDocumentId(documentId);
        if (index >= 0){
            // Document already opened

            documentAdded(index, "");

            return;
        }

        let documentData = createDocumentComponent(documentTypeId);
        if (!documentData){
            return;
        }

        let callback = function(documentModel){
            console.log("callback", callback, documentModel);
            if (documentModel){
                documentsModel.append({
                                          "Uuid": uuidGenerator.generateUUID(),
                                          "Title": "",
                                          "TypeId": documentTypeId,
                                          "Model": documentModel,
                                          "DocumentComp": documentData,
                                          "Properties": properties,
                                          "DocumentObj": null
                                      });

                documentAdded(documentsModel.count - 1, "");
            }
        }

        if (!additionInputParams){
            additionInputParams = {}
        }

        documentController.getData(documentTypeId, documentId, additionInputParams, callback)
    }

    /*!
        Save document by UUID.

        \param      documentUuid           UUID of the document
        \param      additionInputParams    Additional input param for the GQL request
    */
    function saveDocument(documentUuid, additionInputParams)
    {
        console.log("saveDocument", documentUuid, JSON.stringify(additionInputParams));

        let index = getDocumentIndexByUuid(documentUuid);
        if (index >= 0){
            let document = documentsModel.get(index).DocumentObj;
            if (document.isDirty){
                if (document.documentCanBeSaved()){

                    let documentId = document.documentId;
                    let documentModel = document.documentModel;
                    let documentTypeId = document.documentTypeId;

                    document.doUpdateModel();

                    let callBack = function(documentId, documentName){
                        onDocumentSaved(documentId, documentName, document, index);
                    }

                    if (!additionInputParams){
                        additionInputParams = {}
                    }

                    if (documentId === ""){
                        documentController.setData(document.gqlAddCommandId, documentId, documentModel, additionInputParams, callBack);
//                        documentController.setData(documentTypeId, documentId, documentModel, additionInputParams, callBack);
                    }
                    else{
//                        documentController.updateData(documentTypeId, documentId, documentModel, additionInputParams, callBack);
                        documentController.updateData(document.gqlUpdateCommandId, documentId, documentModel, additionInputParams, callBack);
                    }
                }
            }
        }
    }


    function onDocumentSaved(documentId, documentName, document, documentIndex)
    {
        document.documentId = documentId;
        document.documentName = documentName;

        document.saved();

        updateDocumentTitle(documentIndex);

        if (internal.m_closingDocuments.includes(document.uuid)){
            closeDocument(document.uuid);
        }
    }


    function getDocumentIndexByUuid(documentUuid){
        for (let i = 0; i < documentsModel.count; i++){
            let uuid = documentsModel.get(i).Uuid;
            if (String(uuid) === String(documentUuid)){
                return i;
            }
        }

        return -1;
    }


    function getDocumentIndexByDocumentId(documentId){
        for (let i = 0; i < documentsModel.count; i++){
            let documentObj = documentsModel.get(i).DocumentObj;
            if (documentObj && documentObj.documentId === documentId){
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

        let documentData = documentsModel.get(documentIndex).DocumentObj;
        let documentName = documentData.getDocumentName();
        if (documentName && documentName !== ""){
            title = documentName;
        }

        if (documentData.isDirty){
            title += " *";
        }

        return title;
    }


    function updateDocumentTitle(documentIndex){
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

        let documentData = documentsModel.get(documentIndex).DocumentObj;

        if (documentData.isDirty && !force){
            let callback = function(result){
                if (result == Enums.ButtonType.Yes){
                    internal.m_closingDocuments.push(documentData.uuid);
                    saveDocument(documentData.uuid);
                }
                else if (result == Enums.ButtonType.No){
                    documentData.isDirty = false;

                    closeDocumentByIndex(documentIndex);
                }

                modalDialogManager.finished.disconnect(callback);
            }

            modalDialogManager.openDialog(saveDialog, {}, "", callback);
        }
        else{
            documentsModel.remove(documentIndex);

            documentClosed(documentIndex, "");
        }
    }


    function closeDocument(documentUuid, force)
    {
        let index = getDocumentIndexByUuid(documentUuid, force);

        closeDocumentByIndex(index);
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
        id: treeItemModelComp;

        TreeItemModel {}
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

            buttonsModel: ListModel{
                ListElement{Id: Enums.ButtonType.Yes; Name:qsTr("Yes"); Enabled: true}
                ListElement{Id: Enums.ButtonType.No; Name:qsTr("No"); Enabled: true}
                ListElement{Id: Enums.ButtonType.Cancel; Name:qsTr("Cancel"); Enabled: true}
            }

            Component.onCompleted: {
//                buttonsModel.append({"Id":"Cancel", "Name":qsTr("Cancel"), "Enabled": true});
//                buttonsModel.append({Id: Enums.ButtonType.Cancel, Name:qsTr("Cancel"), Enabled: true});
            }

            onFinished: {
                if (buttonId === Enums.ButtonType.Yes){
                }
            }
        }
    }
}
