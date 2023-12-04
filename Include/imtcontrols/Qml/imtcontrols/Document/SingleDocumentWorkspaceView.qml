import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtqml 1.0

DocumentManager {
    id: workspaceView;

    property alias alertPanelComp: alertPanel.sourceComponent;

    onDocumentAdded: {
        Events.sendEvent("CommandsClearModel");
    }

    Loader {
        id: alertPanel;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: alertPanel.status == Loader.Ready;
    }

    Loader {
        id: documentLoader;

        anchors.fill: parent;

        sourceComponent: model.DocumentComp;

        onLoaded: {
            console.log("Document onLoaded");

            if (item.uuid !== undefined){
                item.uuid = model.Uuid;
            }

            if (item.documentManagerPtr !== undefined){
                item.documentManagerPtr = workspaceView;
            }

            if (item.documentTypeId !== undefined){
                item.documentTypeId = model.TypeId;
            }

            if (item.documentModel !== undefined){
                item.documentModel = model.Model;
            }

            workspaceView.documentsModel.setProperty(model.index, "DocumentObj", item);

            if (item.startLoading !== undefined){
                item.startLoading.connect(documentLoading.start);
            }

            if (item.stopLoading !== undefined){
                item.stopLoading.connect(documentLoading.stop);
            }
        }

        onStatusChanged: {
            console.log("Document onStatusChanged", documentLoader.status);
            if (status === Loader.Error){
                console.error("Document loading was failed", modelData);
            }
        }
    }

    Connections {
        target: documentController;

        function onGetModelStateChanged(){
            let state = documentController.getModelState;
            if (state === "Loading"){
                loading.start();
            }
            else{
                loading.stop();
            }
        }

        function onSetModelStateChanged(){
            let state = documentController.setModelState;
            if (state === "Loading"){
                loading.start();
            }
            else{
                loading.stop();
            }
        }

        function onError(){
            loading.stop();
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;
    }
}
