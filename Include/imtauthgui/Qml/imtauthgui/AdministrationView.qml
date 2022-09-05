import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3

Rectangle {
    id: container;
    Rectangle {
        id: mainPanelBackground;
        anchors.top: parent.top;
        anchors.left: parent.left;

        width: 150;
        height: parent.height;

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
                    Name: "Users"
                }
                ListElement{
                    Name: "Roles"
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
                        }
                    }
                }
            }
        }
    }
    Rectangle{
        id: bodyAdministration
        anchors.left: mainPanelBackground.right;
        anchors.top: parent.top;
        width: parent.width - mainPanelBackground.width
        height: parent.height
        CollectionView {
            id: userCollectionViewContainer;

            Component.onCompleted: {
                console.log("UserCollection onCompleted", model.index);
                userCollectionViewContainer.commandUpdateGui = "UserCollectionUpdateGui";
                userCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserCollectionViewCommandsDelegate.qml";
            }
        }
    }

}
