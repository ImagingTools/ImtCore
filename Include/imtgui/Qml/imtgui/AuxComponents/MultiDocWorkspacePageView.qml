import QtQuick 2.12
import Acf 1.0

Item {
    id: multiDocPageView;

    anchors.fill: parent;

    property var startPageObj;

    onStartPageObjChanged: {
        console.log("multiDocPageView onStartPageObjChanged");
        documentManager.addDocument(multiDocPageView.startPageObj)
    }

    MultiDocWorkspaceView {
        id: documentManager;

        anchors.fill: parent;
    }
}
