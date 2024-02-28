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
        Events.sendEvent("MenuModelRequest", true);
    }

    MultiDocWorkspaceView {
        id: documentManager;

        anchors.fill: parent;

        Component.onDestruction: {
            Events.unSubscribeEvent("MenuModelChanged", documentManager.onMenuModelChanged);
        }

        Component.onCompleted: {
            Events.subscribeEvent("MenuModelChanged", documentManager.onMenuModelChanged);

        }

        function onMenuModelChanged(model){
            let id = multiDocPageView.startPageObj["Id"];
            for(let i = 0; i < model.GetItemsCount(); i++){
                let curr_id = model.GetData("Id",i);
                let curr_name = model.GetData("Name",i);
                if(curr_id == id){
                    documentManager.documentsModel.setProperty(0, "Title", curr_name);
                    break;
                }
            }
        }

    }
}
