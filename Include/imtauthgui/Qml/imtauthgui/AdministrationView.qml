import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: container;

    anchors.fill: parent;

    Row {
        id: header;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 50

        spacing: 20;

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
                    Id: "Users";
                    Name: "Users";
                }

                ListElement{
                    Id: "Roles";
                    Name: "Roles";
                }
            }

            Repeater {
                id: mainPanelRepeater;

                model: leftMenuModel

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

                    backgroundColor: Style.mainColor

                    borderColor: mainPanel.selectedIndex == model.index ? Style.iconColorOnSelected : Style.buttonColor;

                    Component.onCompleted: {
                        if (model.index === 0){
                            clicked();
                        }
                    }

                    onClicked: {
                        if (mainPanel.selectedIndex !== model.index){
                            mainPanel.selectedIndex = model.index;

                            bodyLoader.source = "qrc:/qml/imtgui/AuxComponents/CollectionView.qml";

                            if (bodyLoader.item){
                                bodyLoader.item.commandsId = model.Id;
                                headerText.text = model.Id;
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle{
        id: bodyAdministration;

        anchors.left: mainPanelBackground.right;
        anchors.top: header.bottom;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;

        Loader {
            id: bodyLoader;

            anchors.fill: parent;

            onItemChanged: {
            }
        }
    }
}
