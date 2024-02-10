import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtdocgui 1.0

Rectangle {
    id: multiDocPageView;

    anchors.fill: parent;

    property var startPageObj;

    onStartPageObjChanged: {
        let id = startPageObj["Id"];
        let name = startPageObj["Name"];
        let source = startPageObj["Source"];
        let typeId = startPageObj["CommandId"];

        MainDocumentManager.registerDocumentManager(typeId, documentManager);

        var startItemComp = Qt.createComponent(source);
        if (startItemComp.status !== Component.Ready) {
            console.log("Start component not ready!", startItemComp.errorString());

            return;
        }

        documentManager.addFixedView(startItemComp, name);
    }

    MultiDocWorkspaceView {
        id: documentManager;

        anchors.fill: parent;
    }
}
