import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property var documentModel;

    property string documentId;
    property string documentName;

    property bool hasRemoteChanges: false;

    signal saved(string id, string name);
    signal error(string message, string type);

    function getDocumentName(){
        return documentName;
    }

    function updateDocumentModel(){
        documentModelChanged();
    }

    function insertDocument(){
        console.log("DocumentDataController insertDocument", documentModel.ToJson());
    }

    function saveDocument(){
        console.log("DocumentDataController saveDocument", documentModel.ToJson());
    }
}
