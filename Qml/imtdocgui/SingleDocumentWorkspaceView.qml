import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

DocumentManager {
    id: documentManager;

    property var startPageObj;

    property bool startItemIsLoaded: false;

    onStartPageObjChanged: {
        let typeId = startPageObj["CommandId"];

        MainDocumentManager.registerDocumentManager(typeId, documentManager);
    }

    onDocumentAdded: {
        let documentComp = documentsModel.get(documentIndex).DocumentViewComp;

        stackView.push(documentComp);
    }

    onDocumentClosed: {
        stackView.pop();
    }

    Component.onCompleted: {
        Events.subscribeEvent("MenuModelChanged", documentManager.onMenuModelChanged);
        Events.subscribeEvent("SetAlertPanel", setAlert);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("MenuModelChanged", documentManager.onMenuModelChanged);
        Events.unSubscribeEvent("SetAlertPanel", setAlert);
    }


    function setAlert(parameters){
        if (!parameters){
            console.error("Unable to set alert panel, 'parameters' is invalid");
            return;
        }

        let id = parameters["Id"];
        let alertPanelComp = parameters["AlertPanelComp"];

        if (id === documentManager.startPageObj["Id"]){
            setAlertPanel(alertPanelComp);
        }
    }

    function loadStartItem(){
        if (startItemIsLoaded){
            return;
        }

        let name = startPageObj["Name"];
        let source = startPageObj["Source"];

        var startItemComp = Qt.createComponent(source);
        if (startItemComp.status !== Component.Ready) {
            console.log("Start component not ready!", startItemComp.errorString());

            return;
        }

        addInitialItem(startItemComp, name);
    }

    function onMenuModelChanged(model){
        if(!documentManager.startPageObj) return

        let id = documentManager.startPageObj["Id"];
        for(let i = 0; i < model.GetItemsCount(); i++){
            let curr_id = model.GetData("Id",i);
            let curr_name = model.GetData("Name",i);
            if(curr_id == id){
                documentManager.documentsModel.setProperty(0, "Title", curr_name);
                break;
            }
        }
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

        color: Style.backgroundColor2;
    }

    Loader {
        id: alertPanel;

        height: visible ? 40: 0;

        visible: false;
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
                    let uuid = documentManager.documentsModel.get(index).Uuid;

                    Events.sendEvent(uuid + "CommandActivated", "Close")
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

        onItemAdded: {
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
