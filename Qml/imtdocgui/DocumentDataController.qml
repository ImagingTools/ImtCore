import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property var documentModel;

    property string documentId;
    property string documentName;

    property bool hasRemoteChanges: false;

    property Component documentModelComp;

    signal saved(string id, string name);
    signal error(string message, string type);
    signal modelChanged();

    onDocumentModelChanged: {
        console.log("DocumentDataController onDocumentModelChanged");

        setupDocumentInfo();

        hasRemoteChanges = false;
        modelChanged();
    }

    onSaved: {
        documentId = id;
        documentName = name;
    }

    function getDocumentId(){
        return documentId;
    }

    function getDocumentName(){
        return documentName;
    }

    function updateDocumentModel(){
        modelChanged();
    }

    function insertDocument(){
        console.log("DocumentDataController insertDocument", documentModel.toJson());
    }

    function saveDocument(){
        console.log("DocumentDataController saveDocument", documentModel.toJson());
    }

    // For setted documentId/documentName from documentModel
    function setupDocumentInfo(){}

    function createDocumentModel(){
        if (root.documentModelComp){
            documentModel = root.documentModelComp.createObject(null);
        }
        else{
            console.error("Unable to create document model for DocumentDataController. Error: 'documentModelComp' is invalid")
        }
    }
}
