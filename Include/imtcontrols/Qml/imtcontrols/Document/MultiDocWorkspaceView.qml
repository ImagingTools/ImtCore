import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtqml 1.0

DocumentManager {
    id: workspaceView;

    property alias alertPanelComp: alertPanel.sourceComponent;

    onDocumentClosed: {
        if (documentIndex <= tabPanel.selectedIndex && documentIndex > 0){
            tabPanel.selectedIndex--;
        }
    }

    onDocumentAdded: {
        tabPanel.selectedIndex = documentIndex;
    }

    Loader {
        id: alertPanel;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: alertPanel.status == Loader.Ready;
    }

    TabPanel {
        id: tabPanel;

        anchors.top: alertPanel.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        model: workspaceView.documentsModel;

        onCloseItem: {
            workspaceView.closeDocumentByIndex(index);
        }
    }

    Repeater {
        id: documentRepeater;

        anchors.top: tabPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        model: workspaceView.documentsModel;

        delegate: Loader {
            id: documentLoader;

            anchors.fill: documentRepeater;

            visible: tabPanel.selectedIndex === model.index;

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

                itemConnections.enabled = true;
            }

            onStatusChanged: {
                console.log("Document onStatusChanged", documentLoader.status);
                if (status === Loader.Error){
                    console.error("Document loading was failed", modelData);
                }
            }

            Connections {
                id: itemConnections;

                enabled: false;
                target: documentLoader.item;

                function onIsDirtyChanged(){
                    let isDirty = documentLoader.item.isDirty;
                    if (isDirty){
                        let title = model.Title;

                        workspaceView.setDocumentTitle(model.index, title + "*");
                    }
                    else{
                        let title = model.Title;

                        let lastSymbol = title.charAt(title.length - 1);

                        if (lastSymbol === '*'){
                            title = title.replace('*', '');
                        }

                        workspaceView.setDocumentTitle(model.index, title);
                    }
                }
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
