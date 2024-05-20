import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

DocumentManager {
    id: workspaceView;

    onDocumentClosed: {
        if (documentIndex <= tabPanel.selectedIndex && documentIndex > 0){
            tabPanel.selectedIndex--;
        }
    }

    onDocumentAdded: {
        tabPanel.selectedIndex = documentIndex;
    }

    function addFixedView(viewComp, name, index){
        let viewName = name;
        if (!viewName || viewName == ""){
            viewName = defaultDocumentName;
        }

        if (!index){
            index = documentsModel.count;
        }

        documentsModel.insert(index, {
                                  "Uuid": UuidGenerator.generateUUID(),
                                  "Title": viewName,
                                  "DocumentViewComp": viewComp,
                                  "Fixed": true
                              })

        documentAdded(index, "");
    }

    function setAlertPanel(alertPanelComp){
        alertPanel.sourceComponent = alertPanelComp;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor2;
    }

    TabPanel {
        id: tabPanel;

        anchors.top: parent.top;
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

    Loader {
        id: alertPanel;

        anchors.top: tabPanel.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: alertPanel.item != null && alertPanel.item !== undefined;
    }

    Repeater {
        id: documentRepeater;

        anchors.top: alertPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        model: workspaceView.documentsModel;

        clip: true;

        delegate: Item {
            anchors.fill: documentRepeater;

            Loader {
                id: documentLoader;

                anchors.top: parent.top;
                anchors.left: parent.left;
                anchors.right: parent.right;
                anchors.bottom: parent.bottom;

                visible: tabPanel.selectedIndex === model.index;

                sourceComponent: model.DocumentViewComp;

                clip: true;

                onLoaded: {
                    if (item.viewId !== undefined){
                        item.viewId = model.Uuid;
                    }

                    if (model.Fixed !== undefined && model.Fixed){
                        return;
                    }

                    if (model.Properties){
                        for (let key in model.Properties){
                            if (item[key] !== undefined){
                                item[key] = model.Properties[key];
                            }
                        }
                    }

                    let documentData = workspaceView.documentsModel.get(model.index).DocumentData;
                    if (documentData){
                        documentData.views.push(item);
                        documentData.viewAdded(item);
                    }

                    workspaceView.updateDocumentTitle(model.index);
                }

                onStatusChanged: {
                    console.log("Document onStatusChanged", documentLoader.status);
                    if (status === Loader.Error){
                        console.error("Document loading was failed");
                    }
                }
            }
        }
    }
}
