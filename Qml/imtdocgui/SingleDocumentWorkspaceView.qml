import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtqml 1.0
import imtgui 1.0

DocumentManager {
    id: documentManager;

    property alias alertPanelComp: alertPanel.sourceComponent;

    property var startPageObj;

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

        visible: stackView.countPage == 0;

        onLoaded: {
            console.log("Document onLoaded");

            if (item.uuid !== undefined){
                item.uuid = model.Uuid;
            }

            if (item.documentManagerPtr !== undefined){
                item.documentManagerPtr = documentManager;
            }

            if (item.documentTypeId !== undefined){
                item.documentTypeId = model.TypeId;
            }

            if (item.documentModel !== undefined){
                item.documentModel = model.Model;
            }

            documentManager.documentsModel.setProperty(model.index, "DocumentObj", item);
        }

        onStatusChanged: {
            console.log("Document onStatusChanged", documentLoader.status);
            if (status === Loader.Error){
                console.error("Document loading was failed", documentLoader.source);
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

//        function onError(){
//            console.error("Unable to open document:", message);

//            loading.stop();
//        }
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
