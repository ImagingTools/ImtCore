import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

DocumentManager {
    id: workspaceView;

//    property alias alertPanelComp: alertPanel.sourceComponent;

    property var fixedViews: [];
    property var fixedViewNames: [];

    onDocumentClosed: {
        if (documentIndex <= tabPanel.selectedIndex && documentIndex > 0){
            tabPanel.selectedIndex--;
        }
    }

    onDocumentAdded: {
        tabPanel.selectedIndex = documentIndex;
    }

    function addFixedView(viewComp, name){
        let viewName = name;
        if (!viewName || viewName == ""){
            viewName = defaultDocumentName;
        }

        documentsModel.append({
                                  "Uuid": UuidGenerator.generateUUID(),
                                  "Title": viewName,
                                  "DocumentViewComp": viewComp,
                                  "Fixed": true
                              });

        documentAdded(documentsModel.count - 1, "");
    }

//    function setAlertPanel(alertPanelComp){
//        alertPanel.sourceComponent = alertPanelComp;
//    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
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

    Component {
        id: alertPanelComp;

        Rectangle {
            anchors.fill: parent;

            color: Style.selectedColor;

            Image {
                id: icon;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                width: 20;
                height: 20;

                sourceSize.height: height;
                sourceSize.width: width;

                source: "../../../" + Style.getIconPath("Icons/Lamp", Icon.State.On, Icon.Mode.Normal);
            }

            BaseText {
                id: message;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: icon.right;
                anchors.leftMargin: 10;
                anchors.right: updateButton.left;

                text: qsTr("This document has been modified");
            }

            Button {
                id: updateButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                width: 70;
                height: 30;

                text: qsTr("Update");

                onClicked: {
                }
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
                id: alertPanel;

                anchors.top: parent.top;
                anchors.left: parent.left;
                anchors.right: parent.right;

                height: visible ? 40: 0;

                sourceComponent: alertPanelComp;

//                visible: alertPanel.status == Loader.Ready;
                visible: false;
            }

            Loader {
                id: documentLoader;

                anchors.top: alertPanel.bottom;
                anchors.left: parent.left;
                anchors.right: parent.right;
                anchors.bottom: parent.bottom;

                visible: tabPanel.selectedIndex === model.index;

                sourceComponent: model.DocumentViewComp;

                clip: true;

                onLoaded: {
                    console.log("onLoaded", item);
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
                    console.log("documentData", documentData);

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
