import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

DocumentManager {
    id: documentManager;

    property alias alertPanelComp: alertPanel.sourceComponent;

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

        addInitialItem(startItemComp, name);
    }

    onDocumentAdded: {
        let documentComp = documentsModel.get(documentIndex).DocumentViewComp;

        stackView.push(documentComp);
    }

    onDocumentClosed: {
        stackView.pop();
    }

    function addInitialItem(viewComp, name){
        documentsModel.append({
                                  "Uuid": UuidGenerator.generateUUID(),
                                  "Title": name,
                                  "DocumentViewComp": viewComp,
                                  "Fixed": true
                              });

        documentAdded(documentsModel.count - 1, "");
    }

    function setAlertPanel(alertPanelComp){
        alertPanel.sourceComponent = alertPanelComp;
    }

    Rectangle {
        id: background;

        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Loader {
        id: alertPanel;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: visible ? 40: 0;

        visible: alertPanel.status == Loader.Ready;
    }

    Item {
        id: buttonPanel;

        anchors.top: alertPanel.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: visible ? Style.size_mainMargin : 0;

        width: visible ? closeButton.width: 0;
        height: headersListView.height;

        visible: stackView.countPage > 1;

        ToolButton {
            id: closeButton;

            anchors.centerIn: buttonPanel;

            width: visible ? 25: 0;
            height: width;

            iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal);

            onClicked: {
                let index = documentManager.documentsModel.count - 1 ;
                if (index > 0){
                    let documentId = documentManager.documentsModel.get(index).Uuid;

                    Events.sendEvent(documentId + "CommandActivated", "Close")
                }
            }
        }
    }

    ListView {
        id: headersListView;

        anchors.top: alertPanel.bottom;
        anchors.left: buttonPanel.right;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: parent.right;

        height: 40;

        orientation: ListView.Horizontal;
        boundsBehavior: Flickable.StopAtBounds;

        model: documentManager.documentsModel;

        spacing: Style.size_mainMargin;

        delegate: Item {
            width: content.width;
            height: headersListView.height;

            Row {
                id: content;

                height: parent.height;

                spacing: Style.size_mainMargin;

                Text {
                    id: separator;

                    anchors.verticalCenter: content.verticalCenter;

                    font.pixelSize: Style.fontSize_title;
                    font.family: Style.fontFamily;

                    color: Style.titleColor;

                    text: "/"

                    visible: model.index > 0;
                }

                Text {
                    id: headerText;

                    anchors.verticalCenter: content.verticalCenter;

                    font.pixelSize: Style.fontSize_title;
                    font.family: Style.fontFamily;

                    color: Style.titleColor;

                    text: model.Title;
                }
            }

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                }
            }
        }
    }

    StackView {
        id: stackView;

        z: 10;

        anchors.top: headersListView.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
//        anchors.margins: Style.size_mainMargin;

        onItemAdded: {
            console.log("onItemAdded", index, item);
            let documentObj = documentManager.documentsModel.get(index);

            if (item.viewId !== undefined){
                item.viewId = documentObj.Uuid;
            }

            let documentData = documentObj.DocumentData;
            if (documentData){
                documentData.views.push(item);
                documentData.viewAdded(item);
            }
        }
    }
}
