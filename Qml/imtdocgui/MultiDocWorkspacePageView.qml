import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtdocgui 1.0

Rectangle {
    id: multiDocPageView;

    anchors.fill: parent;

    color: Style.backgroundColor2;

    property var startPageObj;

    property bool startItemIsLoaded: false;
    property alias documentManager: documentManager_;

    onStartPageObjChanged: {
        let typeId = startPageObj["CommandId"];

        MainDocumentManager.registerDocumentManager(typeId, documentManager_);
    }

    function loadStartItem(){
        if (startItemIsLoaded){
            return;
        }

        let id = startPageObj["Id"];
        let name = startPageObj["Name"];
        let source = startPageObj["Source"];

        var startItemComp = Qt.createComponent(source);
        if (startItemComp.status !== Component.Ready) {
            console.log("Start component not ready!", startItemComp.errorString());

            return;
        }

        documentManager_.addFixedView(startItemComp, name, 0);

        startItemIsLoaded = true;
    }

    MultiDocWorkspaceView {
        id: documentManager_;

        anchors.fill: parent;

        Component.onCompleted: {
            Events.subscribeEvent("MenuModelChanged", documentManager_.onMenuModelChanged);
            Events.subscribeEvent("SetAlertPanel", setAlert);
        }

        Component.onDestruction: {
            Events.unSubscribeEvent("MenuModelChanged", documentManager_.onMenuModelChanged);
            Events.unSubscribeEvent("SetAlertPanel", setAlert);
        }

        function onMenuModelChanged(model){
            let id = multiDocPageView.startPageObj["Id"];
            for(let i = 0; i < model.GetItemsCount(); i++){
                let curr_id = model.GetData("Id",i);
                let curr_name = model.GetData("Name",i);
                if(curr_id == id){
                    documentManager_.documentsModel.setProperty(0, "Title", curr_name);
                    break;
                }
            }
        }

        function setAlert(parameters){
            if (!parameters){
                console.error("Unable to set alert panel, 'parameters' is invalid");
                return;
            }

            let id = parameters["Id"];
            let alertPanelComp = parameters["AlertPanelComp"];

            if (id === multiDocPageView.startPageObj["Id"]){
                setAlertPanel(alertPanelComp);
            }
        }
    }
}
