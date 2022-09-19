import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: container;

    anchors.fill: parent;

    onDocumentModelChanged: {
        headerText.text = documentModel.GetData("Name");
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: container.commandsId;
        editorItem: container;

        onModelParsed: {
            updateGui();
        }
    }

    function updateGui(){
        for (let index = 0; index < leftMenuModel.count; index++){
            let loader = bodyRepeater.itemAt(index);
            loader.item.updateGui();
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: Style.baseColor;
    }

    Row {
        id: header;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 50

        spacing: 20;

        AuxButton {
            id: closeButton;

            anchors.verticalCenter: parent.verticalCenter;

            width: 25;
            height: width;

            iconSource: "../../../Icons/" + Style.theme + "/Left_On_Normal.svg";

            onClicked: {
                stackView.pop();
            }
        }

        Text {
            id: headerText;

            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            color: Style.titleColor;
        }
    }

    Rectangle {
        id: mainPanelBackground;

        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;

        width: 150;

        color: Style.alternateBaseColor;

        Column {
            id: mainPanel;

            anchors.fill: parent;
            anchors.topMargin: 10;

            property int selectedIndex: -1;

            spacing: 5;

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

                ListElement{
                    Id: "Permissions";
                    Name: "Permissions";
                    Source: "qrc:/qml/imtauthgui/UserPermissions.qml"
                }
            }

            Repeater {
                id: mainPanelRepeater;

                model: leftMenuModel;

                delegate: AuxButton {

                    anchors.left: parent.left;
                    anchors.leftMargin: 10;
                    anchors.right: parent.right;
                    anchors.rightMargin: 10;

                    height: 35;

                    radius: 3;

                    hasText: true;
                    hasIcon: false;

                    textButton: model.Name;

                    backgroundColor: Style.alternateBaseColor;

                    borderColor: mainPanel.selectedIndex == model.index ? Style.iconColorOnSelected : Style.buttonColor;

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
            }
        }
    }

    Rectangle {
        id: bodyAdministration;

        anchors.left: mainPanelBackground.right;
        anchors.top: header.bottom;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;

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
