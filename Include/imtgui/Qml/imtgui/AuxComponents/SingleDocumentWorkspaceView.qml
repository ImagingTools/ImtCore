import QtQuick 2.12
import Acf 1.0

Item {
    id: documentManager;

    anchors.fill: parent;

    property var startPageObj;

    onStartPageObjChanged: {
        documentLoader.source = startPageObj["Source"];

        documentLoader.item.commandsId = startPageObj["CommandsId"];
    }

    Loader {
        id: documentLoader;

        anchors.fill: parent;
    }

    function addDocument(document){

        if (documentLoader.item){
            documentLoader.item.addDocument(document);
        }
    }
}
