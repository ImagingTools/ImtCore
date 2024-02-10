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

    function insertDocument(){}

    function saveDocument(){}
}
