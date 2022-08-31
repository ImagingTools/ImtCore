import QtQuick 2.12
import Acf 1.0

Item {
    id: singleDocumentView;

    anchors.fill: parent;

    property var startPageObj;

    onStartPageObjChanged: {
        documentLoader.source = startPageObj["Source"];

        documentLoader.item.commandsId = startPageObj["CommandsId"];
    }

    Loader {
        id: documentLoader;

        anchors.fill: parent;

        onLoaded: {
//            documentLoader.item.commandsId = model.CommandsId
        }
    }
}
