import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: multiDocPageView;

    anchors.fill: parent;

    property var startPageObj;
    property MainDocumentManager mainDocumentManager: null;
    property alias documentManager: documentManager_;

    onStartPageObjChanged: {
        console.log("multiDocPageView onStartPageObjChanged");
        documentManager_.addDocument(multiDocPageView.startPageObj, {}, false)

        if (multiDocPageView.mainDocumentManager != null){
            multiDocPageView.mainDocumentManager.registerDocumentManager(multiDocPageView.startPageObj["CommandsId"], documentManager_);
        }

        documentManager_.documentLoading = false;
    }

    function updateName(){

    }

    MultiDocWorkspaceView {
        id: documentManager_;

        anchors.fill: parent;

        mainDocumentManager: multiDocPageView.mainDocumentManager;
    }
}
