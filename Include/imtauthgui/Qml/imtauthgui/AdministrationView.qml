import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: container;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property Item documentManager: null;

    property TreeItemModel documentsData: null;

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

        color: Style.baseColor;

        Column {
            id: mainPanel;

            anchors.fill: parent;
            anchors.topMargin: 10;

            property int selectedIndex: -1;

            spacing: 5;

            onSelectedIndexChanged: {
                 headerText.text = qsTr("Administration") + " / " + leftMenuModel.get(mainPanel.selectedIndex).Id;
            }

            ListModel{
                id: leftMenuModel;

                Component.onCompleted: {
                    leftMenuModel.append({Id: "Roles",  Name: qsTr("Roles"), Source: "RoleCollectionView.qml"})
                    leftMenuModel.append({Id: "Users",  Name: qsTr("Users"), Source: "UserCollectionView.qml"})
                    leftMenuModel.append({Id: "Groups",  Name: qsTr("Groups"), Source: "UserGroupCollectionView.qml"})
                }

//                ListElement{
//                    Id: "Roles";
//                    Name: qsTr("Roles");
//                    Source: "RoleCollectionView.qml";
//                }

//                ListElement{
//                    Id: "Users";
//                    Name: qsTr("Users");
//                    Source: "UserCollectionView.qml";
//                }
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
                        console.log("onClicked", model)
                        if (mainPanel.selectedIndex !== model.index){
                            mainPanel.selectedIndex = model.index;
                        }
                    }

                    Rectangle {
                        anchors.top: parent.top;
                        anchors.bottom: parent.bottom;
                        anchors.left: parent.left;

                        width: 5;

                        color: Style.iconColorOnSelected;
                        visible: mainPanel.selectedIndex == model.index;
                    }
                }
            }
        }
    }

    property Item activePage: null;

    function updateGui(){
        bodyRepeater.model = 0;
        bodyRepeater.model = leftMenuModel;
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

                property Item documentManager: container.documentManager;

                onDocumentManagerChanged: {
                    if (bodyLoader.item){
                        bodyLoader.item.documentManager = documentManager;
                    }
                }

                property TreeItemModel documentsData: container.documentsData;
                onDocumentsDataChanged: {
                    if (bodyLoader.item){
                        bodyLoader.item.documentsData = documentsData;
                    }
                }

                onVisibleChanged: {
                    if (bodyLoader.visible){
                        console.log("onVisibleChanged model.Source", model.Source);
                        if (!bodyLoader.item){
                            bodyLoader.source = model.Source;
                        }

                        container.activePage = bodyLoader.item;
                    }
                }

                visible: mainPanel.selectedIndex == model.index;

                onLoaded: {
                    console.log("bodyRepeater onLoaded", model.Source);
                    bodyLoader.item.commandsId = model.Id;

                    if (documentManager != null && bodyLoader.item.documentManager !== undefined){
                        bodyLoader.item.documentManager = documentManager;
                    }

                    if (documentsData != null && bodyLoader.item.documentsData !== undefined){
                        bodyLoader.item.documentsData = documentsData;
                    }
                }
            }
        }
    }
}
