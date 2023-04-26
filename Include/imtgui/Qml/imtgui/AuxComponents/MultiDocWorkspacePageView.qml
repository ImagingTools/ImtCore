import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: multiDocPageView;

    anchors.fill: parent;

    property var startPageObj;
    property MainDocumentManager mainDocumentManager: null;

    onStartPageObjChanged: {
        console.log("multiDocPageView onStartPageObjChanged");
        documentManager.addDocument(multiDocPageView.startPageObj)

        if (multiDocPageView.mainDocumentManager != null){
            multiDocPageView.mainDocumentManager.registerDocumentManager(multiDocPageView.startPageObj["CommandsId"], documentManager);
        }
    }

    MultiDocWorkspaceView {
        id: documentManager;

        anchors.fill: parent;

        mainDocumentManager: multiDocPageView.mainDocumentManager;
    }
}
