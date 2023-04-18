import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: container;

    commandsDelegateSourceComp: roleViewCommandsDelegate;

    property TreeItemModel rolesModel; // Collection of the all roles

    property int selectedIndex: -1;

    property BasicTableView includedRolesTable;

    property string productId: "";
    property string title;
    property string roleName: qsTr("New Role");

    property int mainMargin: 0;
    property int panelWidth: 150;

    onIncludedRolesTableChanged: {
        console.log("onIncludedRolesTableChanged", container.includedRolesTable);
    }

    onItemNameChanged: {
        if (container.documentModel.ContainsKey("Name")){
            let roleName = container.documentModel.GetData("Name");

            if (roleName != ""){
                container.roleName = roleName;
            }
        }
    }

    onProductIdChanged: {
        console.log("onProductIdChanged", container.productId);
        container.title = container.productId + " / " + qsTr("Roles");
    }

    onDocumentModelChanged: {
        console.log("onDocumentModelChanged", container.documentModel.toJSON());

        for (let index = 0; index < leftMenuModel.count; index++){
            let loader = bodyRepeater.itemAt(index);
            if (!loader.item)
                continue;
            loader.item.documentModel = container.documentModel;
            loader.item.undoRedoManager = undoRedoManager;
            if(loader.item.documentBase !==undefined){
                loader.item.documentBase = container;
            }
        }

        if (container.documentModel.ContainsKey("Name")){
            let roleName = container.documentModel.GetData("Name");

            if (roleName != ""){
                container.roleName = roleName;
            }
        }

        if (container.documentModel.ContainsKey("ProductId")){

            container.productId = container.documentModel.GetData("ProductId")
            console.log("productId", container.productId);
        }

        undoRedoManager.registerModel(container.documentModel);

        container.updateGui();
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;
    }

    function updateGui(){
        loading.visible = true;
        for (let index = 0; index < leftMenuModel.count; index++){
            let loader = bodyRepeater.itemAt(index);
            if (loader.item){
                loader.item.updateGui();
            }
        }

        loading.visible = false;
    }

    function updateModel(){
        for (let index = 0; index < leftMenuModel.count; index++){
            let loader = bodyRepeater.itemAt(index);
            if (loader.item){
                loader.item.updateModel();
            }
        }
    }

    Component {
        id: roleViewCommandsDelegate;
        RoleViewCommandsDelegate {}
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: container.documentUuid;
        documentBase: container;
        commandsDelegate: container.commandsDelegate;

        onModelStateChanged: {
            container.updateGui();
        }
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
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
            highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;
            width: 25;
            height: width;

            iconSource: "../../../Icons/" + Style.theme + "/Left_On_Normal.svg";

            onClicked: {
//                Events.sendEvent("RoleCommandActivated", "Close")

                Events.sendEvent(container.documentUuid + "CommandActivated", "Close")
            }
        }

        Text {
            id: headerText;

            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: Style.fontSize_title;
            font.family: Style.fontFamily;

            color: Style.titleColor;

            text: container.title + " / " + container.roleName;

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
//                        container.mainMargin = mainPanelFrameLoader.item.mainMargin;
//                        container.panelWidth = mainPanelFrameLoader.item.panelWidth;
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
                Component.onCompleted: {
                    leftMenuModel.append({Id: "General", Name: "General", Source: "qrc:/qml/imtauthgui/RoleEditor.qml"})
                    leftMenuModel.append({Id: "Permissions", Name: "Permissions", Source: "qrc:/qml/imtauthgui/RolePermissions.qml"})
                }

//                ListElement{
//                    Id: "General";
//                    Name: "General";
//                    Source: "qrc:/qml/imtauthgui/RoleEditor.qml"
//                }

//                ListElement{
//                    Id: "Permissions";
//                    Name: "Permissions";
//                    Source: "qrc:/qml/imtauthgui/RolePermissions.qml"
//                }
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
            }
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

        z: 5;

        Repeater {
            id: bodyRepeater;

            model: leftMenuModel;

            delegate: Loader {
                id: bodyLoader;

                anchors.fill: parent;

                source: model.Source;
                visible: mainPanel.selectedIndex == model.index;
                onLoaded: {
                    if (item){
                        item.documentModel = container.documentModel;
                        item.undoRedoManager = undoRedoManager;
                        if(item.documentBase !==undefined){
                            item.documentBase = container;
                        }
                    }
                }
            }
        }
    }

}
