import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtqml 1.0
import imtgui 1.0

DocumentManager {
    id: workspaceView;

    property alias alertPanelComp: alertPanel.sourceComponent;

    Component.onCompleted: {
        documentController.setModelStateChanged.connect(workspaceView.onSetModelStateChanged);
        documentController.getModelStateChanged.connect(workspaceView.onGetModelStateChanged);
        documentController.updateModelStateChanged.connect(workspaceView.onUpdateModelStateChanged);
    }

    onDocumentClosed: {
        if (documentIndex <= tabPanel.selectedIndex && documentIndex > 0){
            tabPanel.selectedIndex--;
        }
    }

    onDocumentAdded: {
        tabPanel.selectedIndex = documentIndex;
    }

    function checkState(state){
        if (state === "Loading"){
            Events.sendEvent("StartLoading");
//            loading.start();
        }
        else{
            Events.sendEvent("StopLoading");
//            loading.stop();
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
        id: alertPanel;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: alertPanel.status == Loader.Ready;

        onStatusChanged: {
            console.log("alertPanel onStatusChanged", status);
        }
    }

    TabPanel {
        id: tabPanel;

        anchors.top: alertPanel.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        model: workspaceView.documentsModel;

        clip: true;

        onCloseItem: {
            workspaceView.closeDocumentByIndex(index);
        }

        onRightClicked: {
            if (tabPanel.selectedIndex < workspaceView.documentsModel.count - 1){
                tabPanel.selectedIndex++;
            }
        }

        onLeftClicked: {
            if (tabPanel.selectedIndex > 0){
                tabPanel.selectedIndex--;
            }
        }
    }

    Repeater {
        id: documentRepeater;

        anchors.top: tabPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        model: workspaceView.documentsModel;

        clip: true;

        delegate: Item {
            anchors.fill: documentRepeater;

            Loader {
                id: documentLoader;

                anchors.fill: parent;

                visible: tabPanel.selectedIndex === model.index;

                sourceComponent: model.DocumentComp;

                clip: true;

                onLoaded: {
                    console.log("Document onLoaded");

                    if (model.Properties){
                        for (let key in model.Properties){
                            if (item[key] !== undefined){
                                item[key] = model.Properties[key];
                            }
                        }
                    }

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

                    if (documentLoader.item.isDirty !== undefined){
                        documentLoader.item.onIsDirtyChanged.connect(documentLoader.onIsDirtyChanged);
                    }

                    if (workspaceView.documentsModel.count >= model.index){
                        workspaceView.documentsModel.setProperty(model.index, "DocumentObj", item);
                    }

                    workspaceView.updateDocumentTitle(model.index);
                }

                onStatusChanged: {
                    console.log("Document onStatusChanged", documentLoader.status);
                    if (status === Loader.Error){
                        console.error("Document loading was failed", modelData);
                    }
                }

                function onIsDirtyChanged(){
                    console.log("DocumentManager onIsDirtyChanged");

                    workspaceView.updateDocumentTitle(model.index);
                }
            }
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;
    }
}
