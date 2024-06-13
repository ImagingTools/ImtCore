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

    function createDocumentModel(){
        if (root.documentModelComp){
            documentModel = root.documentModelComp.createObject(null);
        }
        else{
            console.error("Unable to create document model for DocumentDataController. Error: 'documentModelComp' is invalid")
        }
    }
}
