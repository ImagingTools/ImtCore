import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtcontrols 1.0

Item {
    id: documentManager;

    property ListModel documentsModel: ListModel {}

    signal documentClosed(int documentIndex, string documentUuid);
    signal documentAdded(int documentIndex, string documentUuid);

    UuidGenerator {
        id: uuidGenerator;
    }

    function getUndoManagerForDocument(documentPtr)
    {
        let documentsCount = getDocumentsCount();
        for (let documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
            let info = privateMethods.getSingleDocumentData(documentIndex);
            if (info.documentPtr === documentPtr){
                return info.undoManagerPtr;
            }
        }

        return null;
    }


    function getDocumentsCount()
    {
        return privateFields.m_documentInfos.length;
    }


    function getDocumentFromIndex(index, documentInfoPtr)
    {
        if (index < 0 || index >= privateFields.m_documentInfos.length){
            return null;
        }

        let infoPtr = privateFields.m_documentInfos[index];

        if (documentInfoPtr){
            documentInfoPtr = infoPtr;
        }

        return infoPtr.documentPtr;
    }


    function getViewsCount(index)
    {
        if (index < 0 || index >= privateFields.m_documentInfos.length){
            return -1;
        }

        let infoPtr = privateFields.m_documentInfos[index];

        return infoPtr.views.length;
    }


    function getViewFromIndex(documentIndex, viewIndex)
    {
        if (documentIndex < 0 || documentIndex >= privateFields.m_documentInfos.length){
            return -1;
        }

        if (viewIndex < 0 || viewIndex >= getViewsCount(documentIndex)){
            return -1;
        }

        let infoPtr = privateFields.m_documentInfos[index];

        let viewInfo = infoPtr.views[viewIndex];

        return viewInfo.viewPtr;
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


    function getDocumentFromView(view, documentInfoPtr)
    {
        let infoPtr = getDocumentInfoFromView(view);
        if (infoPtr){
            if (documentInfoPtr){
                documentInfoPtr = infoPtr;
            }

            return infoPtr.documentPtr;
        }

        return null;
    }


    function getDocumentInfos(){
        return privateFields.m_documentInfos;
    }


    function addViewToDocument(document, viewTypeId)
    {

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

        documentsModel.append({"Title": "<new item>", "DocumentPtr": documentData});

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
                documentData.documentModel = documentModel;

                let documentName = ""
                if (documentModel.ContainsKey("Name")){
                    documentName = documentModel.GetData("Name");
                }

                documentsModel.append({"Title": documentName, "DocumentPtr": documentData});

                documentAdded(documentsModel.count - 1, "");
            }
        }

        documentController.getData(documentId, {}, documentTypeId, callback)
    }


    function saveDocument(
        documentIndex,
        requestFileName,
        savedMapPtr,
        beQuiet,
        ignoredPtr,
        progressManagerPtr)
    {

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


    function closeDocument(documentIndex)
    {
        if (documentIndex < 0 || documentIndex >= documentsModel.count){
            return;
        }

        let documentData = documentsModel.get(documentIndex).DocumentPtr;
        if (!documentData.isDirty){
            documentsModel.remove(documentIndex);

            documentClosed(documentIndex, "");
        }
    }


    function closeView(viewPtr, beQuiet, ignoredPtr)
    {

    }

//    Component {
//        id: documentComp;

//        QtObject {
//            property string uuid;
//            property string title;
//            property DocumentData documentPtr;
//        }
//    }

    function createDocumentComponent(documentTypeId)
    {
        let viewComp = getDocumentView(documentTypeId);
        if (!viewComp){
            console.error("Unable to create a document component. View component is invalid!");

            return false;
        }

//        let viewObject = viewComp.createObject(documentManager);
//        if (!viewObject){
//            console.error("Unable to create a document component.");

//            return false;
//        }

//        viewObject.uuid = uuidGenerator.generateUUID();

        return viewComp;
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

    GqlDocumentDataController {
        id: documentController;
    }
}
