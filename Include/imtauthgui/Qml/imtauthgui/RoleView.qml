import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: container;

//    commandsDelegate: RoleViewCommandsDelegate {}
//    commandsDelegatePath: "../../imtauthgui/RoleViewCommandsDelegate.qml";

    commandsDelegateSourceComp: RoleViewCommandsDelegate {}

    property TreeItemModel rolesModel; // Collection of the all roles

    property int selectedIndex: -1;

    property alias pageIndex: mainPanel.selectedIndex;

    property Item includedRolesTable;

    property string productId: "";
    property string title: productId + " / " + qsTr("Roles");

    onDocumentModelChanged: {
        headerText.text = title + " / " + documentModel.GetData("Name");

        removeElementFromRolesModel();
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: container.commandsId;
        editorItem: container;

        onModelParsed: {
            updateGui();
        }
    }

    onSelectedIndexChanged: {
        console.log("RoleView onSelectedIndexChanged", selectedIndex);
    }

    function removeElementFromRolesModel(){
        console.log("removeElementFromRolesModel");
        let roleId = documentModel.GetData("Id");
        let productId = documentModel.GetData("ProductId");
        for (let i = 0; i < rolesModel.GetItemsCount(); i++){
            let currentProductId = rolesModel.GetData("Id", i);
            if (productId == currentProductId){
                let roles = rolesModel.GetData("Roles", i);

                for (let j = 0; j < roles.GetItemsCount(); j++){
                    let currentRoleId = roles.GetData("Id", j);

                    if (roleId == currentRoleId){

                        console.log("RemoveData", j);
                        roles.RemoveData(j);
                        break;
                    }
                }

                break;
            }
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
        color: Style.backgroundColor;
    }

    Row {
        id: header;

        z: 10;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        height: 40;

        spacing: 10;

        clip: true;

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

        color: Style.backgroundColor;

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
                    Source: "qrc:/qml/imtauthgui/RoleEditor.qml"
                }

                ListElement{
                    Id: "Permissions";
                    Name: "Permissions";
                    Source: "qrc:/qml/imtauthgui/RolePermissions.qml"
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

        z: 5;

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
