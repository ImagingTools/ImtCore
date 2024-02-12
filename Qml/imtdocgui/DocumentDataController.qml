import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel documentModel: TreeItemModel {}

    property string documentId;
    property string documentName;

    function getDocumentName(){
        return documentName;
    }

    function updateDocumentModel(){}

    function insertDocument(){
        console.log("DocumentDataController insertDocument", documentModel.toJSON());
    }

    function saveDocument(){
        console.log("DocumentDataController saveDocument", documentModel.toJSON());
    }
}
