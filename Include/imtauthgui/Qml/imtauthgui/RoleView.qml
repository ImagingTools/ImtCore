import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: container;

    commandsDelegateSourceComp: RoleViewCommandsDelegate {}

    property TreeItemModel rolesModel; // Collection of the all roles

    property int selectedIndex: -1;

    property alias pageIndex: mainPanel.selectedIndex;

    property BasicTableView includedRolesTable;

    property string productId: "";
    property string title;
    property string roleName: qsTr("New Role");

    onItemNameChanged: {
        if (documentModel.ContainsKey("Name")){
            let roleName = documentModel.GetData("Name");

            if (roleName != ""){
                container.roleName = roleName;
            }
        }
    }

    onProductIdChanged: {
        console.log("onProductIdChanged", productId);
        title = container.productId + " / " + qsTr("Roles");
    }

    onDocumentModelChanged: {
        console.log("onDocumentModelChanged", documentModel.toJSON());
        for (let index = 0; index < leftMenuModel.count; index++){
            let loader = bodyRepeater.itemAt(index);
            loader.item.documentModel = documentModel;
            loader.item.undoRedoManager = undoRedoManager;
        }

        if (documentModel.ContainsKey("Name")){
            let roleName = documentModel.GetData("Name");

            if (roleName != ""){
                container.roleName = roleName;
            }
        }

        if (documentModel.ContainsKey("ProductId")){

            container.productId = documentModel.GetData("ProductId")
            console.log("productId", productId);
        }

        undoRedoManager.registerModel(documentModel);
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: container.commandsId;

        onModelStateChanged: {
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
                Events.sendEvent("RoleCommandActivated", "Close")
            }
        }

        Text {
            id: headerText;

            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            color: Style.titleColor;

            text: container.title + " / " + container.roleName;
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
