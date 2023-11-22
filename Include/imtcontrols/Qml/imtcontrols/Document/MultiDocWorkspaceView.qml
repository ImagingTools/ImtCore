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
            workspaceView.closeDocument(index);
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

            sourceComponent: model.DocumentPtr;

            onLoaded: {
                console.log("Document onLoaded");

                if (item.documentManagerPtr !== undefined){
                    item.documentManagerPtr = workspaceView;
                }
            }

            onStatusChanged: {
                console.log("Document onStatusChanged", documentLoader.status);
                if (status === Loader.Error){
                    console.error("Document loading was failed", modelData);
                }
            }
        }
    }
}
