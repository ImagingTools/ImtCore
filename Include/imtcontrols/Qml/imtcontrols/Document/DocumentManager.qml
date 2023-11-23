import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtcontrols 1.0


Item {
    id: documentManager;

    property ListModel documentsModel: ListModel {
        dynamicRoles: true;
    }

    property GqlDocumentDataController documentController: GqlDocumentDataController {}

    UuidGenerator {
        id: uuidGenerator;
    }

    signal documentClosed(int documentIndex, string documentUuid);
    signal documentAdded(int documentIndex, string documentUuid);


    function getDocumentsCount()
    {
        return documentsModel.count;
    }


    function getActiveView()
    {
        return privateFields.m_activeViewPtr;
    }


    function setActiveView(viewPtr)
    {
        if (privateFields.m_activeViewPtr !== viewPtr){
            privateFields.m_activeViewPtr = viewPtr;
        }
    }


    function getDocumentTypeId(document)
    {
        let documentsCount = getDocumentsCount();
        for (let documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
            let info = privateMethods.getSingleDocumentData(documentIndex);
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

        privateFields.m_registeredView[documentTypeId] = viewComp;

        return true;
    }


    function getDocumentView(documentTypeId)
    {
        if (!documentIsRegistered(documentTypeId)){
            console.error("Unable to get a document view. View is unregistered!");

            return null;
        }

        return privateFields.m_registeredView[documentTypeId];
    }


    function documentIsRegistered(documentTypeId)
    {
        return documentTypeId in privateFields.m_registeredView;
    }


    function insertNewDocument(documentTypeId)
    {
        let documentData = createDocumentComponent(documentTypeId);
        if (!documentData){
            return false;
        }

        documentsModel.append({
                                  "Uuid": uuidGenerator.generateUUID(),
                                  "Title": "<new item>",
                                  "TypeId": documentTypeId,
                                  "Model": createDocumentModel(),
                                  "DocumentComp": documentData,
                                  "DocumentObj": null
                              });

        documentAdded(documentsModel.count - 1, "");

        return true;
    }


    function openDocument(documentId, documentTypeId)
    {
        let documentData = createDocumentComponent(documentTypeId);
        if (!documentData){
            return false;
        }

        let callback = function(documentModel){
            if (documentModel){
                let documentName = ""
                if (documentModel.ContainsKey("Name")){
                    documentName = documentModel.GetData("Name");
                }

                documentsModel.append({
                                          "Uuid": uuidGenerator.generateUUID(),
                                          "Title": documentName,
                                          "TypeId": documentTypeId,
                                          "Model": documentModel,
                                          "DocumentComp": documentData,
                                          "DocumentObj": null
                                      });

                documentAdded(documentsModel.count - 1, "");

                documentController.result.disconnect(callback);
            }
        }

        documentController.getData(documentId, {}, documentTypeId, callback)
    }


    function saveDocument(documentUuid)
    {
        let index = getDocumentIndexByUuid(documentUuid);
        if (index >= 0){
            let document = documentsModel.get(index).DocumentObj;
            if (document.isDirty){
                if (document.documentCanBeSaved()){

                    let documentId = document.documentId;
                    let documentModel = document.documentModel;

                    if (documentId === ""){
                        let callBack = function(documentId, documentName){
                            document.documentId = documentId;
                            document.isDirty = false;
                            document.saved();

                            documentController.documentAdded.disconnect(callBack);
                        }

                        documentController.setData(documentId, documentModel, {}, callBack);
                    }
                    else{
                        let callBack = function(documentId, documentName){
                            document.documentId = documentId;
                            document.isDirty = false;
                            document.saved();

                            documentController.documentUpdated.disconnect(callBack);
                        }

                        documentController.updateData(documentId, documentModel, {}, callBack);
                    }
                }
            }
        }
    }


    function getDocumentIndexByUuid(documentUuid){
        for (let i = 0; i < documentsModel.count; i++){
            let uuid = documentsModel.get(i).Uuid;
            console.log("uuid", uuid);
            if (String(uuid) === String(documentUuid)){
                return i;
            }
        }

        return -1;
    }


    function setDocumentTitle(documentIndex, title)
    {
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            return;
        }

        documentsModel.setProperty(documentIndex, "Title", title);
    }


    function saveDirtyDocuments(beQuiet, ignoredPtr)
    {

    }


    function closeDocumentByIndex(documentIndex)
    {
        console.log("closeDocumentByIndex", documentIndex);
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            return;
        }

        let documentData = documentsModel.get(documentIndex).DocumentObj;
        if (!documentData.isDirty){
            documentsModel.remove(documentIndex);

            documentClosed(documentIndex, "");
        }
        else{
            modalDialogManager.openDialog(saveDialog, {});
        }
    }


    function closeDocument(documentUuid)
    {
        console.log("closeDocument", documentUuid);

        let index = getDocumentIndexByUuid(documentUuid);

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
        id: privateMethods;

        function getSingleDocumentData(index)
        {
            if (index < 0 || index >= privateFields.m_documentInfos.length){
                return null;
            }

            return privateFields.m_documentInfos[index];
        }

        function getDocumentInfoFromView(view)
        {
            let documentsCount = documentManager.getDocumentsCount();
            for (let documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
                let info = privateMethods.getSingleDocumentData(documentIndex);

                for (let viewIndex = 0; viewIndex < documentsCount; ++viewIndex){

                }
            }

            return privateFields.m_activeViewPtr[index];
        }
    }

    QtObject {
        id: privateFields;

        property var m_registeredView: ({});
        property var m_documentInfos: [];
        property var m_activeViewPtr;
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
                console.log("saveDialog onFinished", buttonId);
                if (buttonId === Enums.ButtonType.Ok){
                    console.log("Ok");
                }
            }
        }
    }
}
