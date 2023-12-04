import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: multiDocPageView;

    anchors.fill: parent;

    property var startPageObj;
//    property MainDocumentManager mainDocumentManager: null;

    Component.onCompleted: {
        console.log("multiDocPageView onCompleted");
    }

    onStartPageObjChanged: {
        console.log("multiDocPageView onStartPageObjChanged");

        let source = startPageObj["Source"];
        let typeId = startPageObj["CommandId"];

        console.log("source", source);

        var startItemComp = Qt.createComponent(source);
        if (startItemComp.status !== Component.Ready) {
            console.log("Start component not ready!", startItemComp.errorString());

            return;
        }

        documentManager.registerDocument(typeId, startItemComp);
        documentManager.insertNewDocument(typeId);
    }

    MultiDocWorkspaceView {
        id: documentManager;

        anchors.fill: parent;
    }
}
