import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtqml 1.0
import imtgui 1.0

DocumentManager {
    id: documentManager;

    property alias alertPanelComp: alertPanel.sourceComponent;

    property var startPageObj;

    Component.onCompleted: {
        documentController.onSetModelStateChanged.connect(documentManager.onSetModelStateChanged);
        documentController.onGetModelStateChanged.connect(documentManager.onGetModelStateChanged);
        documentController.onUpdateModelStateChanged.connect(documentManager.onUpdateModelStateChanged);
    }

    onStartPageObjChanged: {
        console.log("onStartPageObjChanged", startPageObj["Source"]);
        documentLoader.source = startPageObj["Source"];

        if (documentLoader.item.commandId !== undefined){
            documentLoader.item.commandId = startPageObj["CommandId"];
        }

        if(documentLoader.item.documentManagerPtr !== undefined){
            documentLoader.item.documentManagerPtr = documentManager;
        }
    }

    onDocumentAdded: {
        let documentComp = documentManager.documentsModel.get(documentIndex).DocumentComp;

        stackView.push(documentComp);
    }

    onDocumentClosed: {
        console.log("onDocumentClosed", documentIndex);

        stackView.pop();
    }

    function checkState(state){
        if (state === "Loading"){
            Events.sendEvent("StartLoading");
        }
        else{
            Events.sendEvent("StopLoading");
        }
    }

    function onGetModelStateChanged(){
        checkState(documentController.getModelState);
    }

    function onSetModelStateChanged(){
        checkState(documentController.setModelState);
    }

    function onUpdateModelStateChanged(){
        checkState(documentController.updateModelState);
    }

    function setAlertPanel(alertPanelComp){
        console.log("DocumentManager setAlertPanel", alertPanelComp);

        alertPanel.sourceComponent = alertPanelComp;
    }

    Loader {
        id: documentLoader;

        anchors.fill: parent;

        visible: stackView.countPage == 0;

        onLoaded: {
            console.log("Document onLoaded");

            if (item.documentManagerPtr !== undefined){
                item.documentManagerPtr = documentManager;
            }
        }

        onStatusChanged: {
            console.log("Document onStatusChanged", documentLoader.status);
            if (status === Loader.Error){
                console.error("Document loading was failed", documentLoader.source);
            }
        }
    }

    Loader {
        id: alertPanel;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: alertPanel.status == Loader.Ready;
    }

    StackView {
        id: stackView;

        z: 10;

        anchors.top: alertPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        onItemAdded: {
            console.log("Document onItemAdded", item);

            let documentObj = documentManager.documentsModel.get(index);

            if (item.uuid !== undefined){
                item.uuid = documentObj.Uuid;
            }

            if (item.documentManagerPtr !== undefined){
                item.documentManagerPtr = documentManager;
            }

            if (item.documentTypeId !== undefined){
                item.documentTypeId = documentObj.TypeId;
            }

            if (item.documentModel !== undefined){
                item.documentModel = documentObj.Model;
            }

            documentManager.documentsModel.setProperty(index, "DocumentObj", item);
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;
    }
}
