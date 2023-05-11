import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: multiDocPageView;

    anchors.fill: parent;

    property var startPageObj;
    property MainDocumentManager mainDocumentManager: null;

    onStartPageObjChanged: {
        console.log("multiDocPageView onStartPageObjChanged");
        documentManager.addDocument(multiDocPageView.startPageObj, {}, false)

        if (multiDocPageView.mainDocumentManager != null){
            multiDocPageView.mainDocumentManager.registerDocumentManager(multiDocPageView.startPageObj["CommandsId"], documentManager);
        }

        documentManager.documentLoading = false;
    }

    MultiDocWorkspaceView {
        id: documentManager;

        anchors.fill: parent;

        mainDocumentManager: multiDocPageView.mainDocumentManager;
    }
}
