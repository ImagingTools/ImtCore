import QtQuick 2.12
import Acf 1.0

Item {
    id: documentManager;

    anchors.fill: parent;

    property var startPageObj;

    onStartPageObjChanged: {

        console.log("onStartPageObjChanged");
        documentLoader.source = startPageObj["Source"];
        console.log("documentLoader.source", documentLoader.source);

         if (documentLoader.item){

             documentLoader.item.commandsId = startPageObj["CommandsId"];
         }

         console.log("documentLoader.item", documentLoader.item);
    }

    Loader {
        id: documentLoader;

        anchors.fill: parent;

        onLoaded: {
            console.log("onLoaded", source);
        }
    }

    function addDocument(document){

        if (documentLoader.item){
            documentLoader.item.addDocument(document);
        }
    }
}
