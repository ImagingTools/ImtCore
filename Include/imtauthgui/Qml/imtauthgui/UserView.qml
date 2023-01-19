import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: container;

    anchors.fill: parent;

    property string title: qsTr("Users");
    property string username: qsTr("New User");

    property int mainMargin: 0;
    property int panelWidth: 150;

    onItemNameChanged: {
        if (container.documentModel.ContainsKey("Name")){
            let username = documentModel.GetData("Name");
            if (username != ""){
                container.username = username;
            }
        }
    }

    onDocumentModelChanged: {
        if (container.documentModel.ContainsKey("Name")){
            let username = documentModel.GetData("Name");
            if (username != ""){
                container.username = username;
            }
        }

        for (let index = 0; index < leftMenuModel.count; index++){
            let loader = bodyRepeater.itemAt(index);
            loader.item.undoRedoManager = undoRedoManager;
            loader.item.documentModel = documentModel;
        }

        undoRedoManager.registerModel(documentModel);
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: container.commandsId;

        onModelStateChanged: {
            container.updateGui();
        }
    }

    function updateGui(){
        for (let index = 0; index < leftMenuModel.count; index++){
            let loader = bodyRepeater.itemAt(index);
            loader.item.updateGui();
        }
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem;
        }
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Style.backgroundColor;
        Loader{
            id: backgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(backgroundDecoratorLoader.item){
                    backgroundDecoratorLoader.item.rootItem = background;
                }
            }
        }
    }

    Row {
        id: header;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 40

        spacing: 20;

        AuxButton {
            id: closeButton;

            anchors.verticalCenter: parent.verticalCenter;
            highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;
            width: 25;
            height: width;

            iconSource: "../../../Icons/" + Style.theme + "/Left_On_Normal.svg";

            onClicked: {
                Events.sendEvent("UserCommandActivated", "Close")
            }
        }

        Text {
            id: headerText;

            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            color: Style.titleColor;

            text: container.title + " / " + container.username;

            Loader{
                id: titleDecoratorLoader;

                sourceComponent: Style.titleDecorator !==undefined ? Style.titleDecorator: emptyDecorator;
                onLoaded: {
                    if(titleDecoratorLoader.item){
                        titleDecoratorLoader.item.rootItem = headerText;
                    }
                }
            }
        }
    }

    Rectangle {
        id: mainPanelBackground;

        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.topMargin: container.mainMargin;
        anchors.leftMargin: container.mainMargin;


        width: container.panelWidth;

        color: Style.backgroundColor;

        Loader{
            id: mainPanelBackgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(mainPanelBackgroundDecoratorLoader.item){
                    mainPanelBackgroundDecoratorLoader.item.rootItem = mainPanelBackground;
                }
            }
        }

        Item{
            id: columnContainer;
            width: parent.width;
            height: mainPanel.height + 2*mainPanel.anchors.topMargin;


            Loader{
                id: mainPanelFrameLoader;

                anchors.fill: parent;

                sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;

                onLoaded: {
                    if(mainPanelFrameLoader.item){
                        container.mainMargin = mainPanelFrameLoader.item.mainMargin;
                        container.panelWidth = mainPanelFrameLoader.item.panelWidth;
                    }
                }
            }

        }

        Column {
            id: mainPanel;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.topMargin: 10;

            property int selectedIndex: -1;

            spacing: 10;

            ListModel{
                id: leftMenuModel

                ListElement{
                    Id: "General";
                    Name: "General";
                    Source: "qrc:/qml/imtauthgui/UserEditor.qml"
                }

                ListElement{
                    Id: "Roles";
                    Name: "Roles";
                    Source: "qrc:/qml/imtauthgui/UserRoles.qml"
                }
            }


            Component{
                id: defaultButtonDecorator;

                CommonButtonDecorator{
                    height: 35;
                    color: Style.alternateBaseColor;
                }
            }

            Repeater {
                id: mainPanelRepeater;

                model: leftMenuModel;

                delegate:

                    BaseButton{
                    id: buttonContainer;

                    anchors.left: parent.left;
                    anchors.leftMargin: 10;
                    anchors.right: parent.right;
                    anchors.rightMargin: 10;

                    text:  model.Name;
                    selected: mainPanel.selectedIndex == model.index;

                    decorator: Style.commonButtonDecorator !==undefined ? Style.commonButtonDecorator : defaultButtonDecorator;

                    Component.onCompleted: {
                        if (model.index === 0){
                            clicked();
                        }
                    }

                    onClicked: {
                        if (mainPanel.selectedIndex !== model.index){
                            mainPanel.selectedIndex = model.index;
                        }
                    }
                }
            }//Repeater
        }
    }

    Rectangle {
        id: bodyAdministration;

        anchors.left: mainPanelBackground.right;
        anchors.top: header.bottom;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.topMargin: container.mainMargin;
        anchors.leftMargin: container.mainMargin;
        anchors.rightMargin: container.mainMargin;

        Repeater {
            id: bodyRepeater;

            model: leftMenuModel;

            delegate: Loader {
                id: bodyLoader;

                anchors.fill: parent;

                source: model.Source;

                visible: mainPanel.selectedIndex == model.index;
            }
        }
    }
}
