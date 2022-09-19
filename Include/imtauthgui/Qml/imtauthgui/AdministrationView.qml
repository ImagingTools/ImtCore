import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: container;

    anchors.fill: parent;

    color: Style.backgroundColor;

    function addHeader(header){
        headerText.text += "/ " + header;
    }

    Row {
        id: header;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 40

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

            onSelectedIndexChanged: {
                 headerText.text =  "Administration" + " / " + leftMenuModel.get(selectedIndex).Id;
            }

            ListModel{
                id: leftMenuModel

                ListElement{
                    Id: "Users";
                    Name: "Users";
                    Source: "UserCollectionView.qml";
                }

                ListElement{
                    Id: "Roles";
                    Name: "Roles";
                    Source: "RoleCollectionView.qml";
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

    Rectangle{
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

                onVisibleChanged: {
                    if (visible){
                        if (!bodyLoader.item){
                            bodyLoader.source = model.Source;
                        }
                    }
                }

                visible: mainPanel.selectedIndex == model.index;

                onLoaded: {
                    bodyLoader.item.commandsId = model.Id;
                }
            }
        }
    }
}
