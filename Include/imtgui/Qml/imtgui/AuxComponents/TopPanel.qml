import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topPanel;

    color: Style.backgroundColor;
//        color: "red";
    property string fontName: "";
    property string activeCommandsModelId;
    property string title;

    signal menuActivatedSignal(string idMenu);
    signal commandsChangedSignal(string commandsId);

    function updateModels(){
        commandsModel.updateModel();
    }

    function setModeMenuButton(commandId, mode) {
        console.log("TopPanel setModeMenuButton!", commandId, mode, topPanel.activeCommandsModelId);

        if (buttonsModelItem.ContainsKey(topPanel.activeCommandsModelId)) {
            var buttonsModelLocal = buttonsModelItem.GetData(topPanel.activeCommandsModelId);
            var itemsCount = buttonsModelLocal.GetItemsCount();

            for (var i = 0; i < itemsCount; i++) {
                var id = buttonsModelLocal.GetData(CommandEnum.ID, i);

                if (id === commandId) {
                    buttonsModelLocal.SetData("Mode", mode, i);
                    decoratorLoader.item.centerPanel.lvButtons.model = 0
                    decoratorLoader.item.centerPanel.lvButtons.model =  buttonsModelLocal
                    console.log("Command", commandId, " change mode", mode);
                    topPanel.updateModeModelMenuButtons(id, mode);
                    break;
                }
            }
        }
    }

    function updateModeModelMenuButtons(id, mode) {
        for (var i = 0; i < modelButtons.count; i++) {
            if (modelButtons.get(i).id === id) {
                modelButtons.get(i).mode = mode;
                modelButtons.get(i).imageSource = "../../../Icons/" + Style.theme + "/" + modelButtons.get(i).name + "_Off" + "_" + mode + ".svg";
                break;
            }
        }
    }

    function dialogResult(parameters) {
        console.log("TopPanel dialogResult", parameters["status"]);
        topPanel.menuActivatedSignal(parameters["status"]);
    }

    function getMenuButtonsX() {
        return menuButton.x - 100;
    }

    function getMenuButtonsY() {
        return menuButton.y + menuButton.width;
    }

    onWidthChanged: {
         if (decoratorLoader.item){
             decoratorLoader.item.width = topPanel.width;
         }
    }

    onHeightChanged: {
        if (thubnailDecoratorContainer.dialogIsActive()) {
            //thubnailDecoratorContainer.closeDialog();
        }
    }

    onActiveCommandsModelIdChanged: {
        console.log("onActiveCommandsModelIdChanged", topPanel.activeCommandsModelId);
        if (topPanel.activeCommandsModelId == "") {
            return;
        }
        commandsModel.updateModel();

//        if (!buttonsModelItem.ContainsKey(topPanel.activeCommandsModelId)) {
//            commandsModel.updateModel();
//        } else {
//            updateTimer.model = buttonsModelItem.GetData(topPanel.activeCommandsModelId);
//        }
        topPanel.commandsChangedSignal(topPanel.activeCommandsModelId);
    }

    TreeItemModel {
        id: buttonsModelItem;
    }

    Loader {
        id: decoratorLoader;

        source: Style.topPanelDecoratorPath;

        onItemChanged: {
            if (decoratorLoader.item){
                console.log("decoratorLoader", topPanel.width, topPanel.height);
                decoratorLoader.item.width = topPanel.width;
                decoratorLoader.item.height = topPanel.height;
                decoratorLoader.item.topPanel = topPanel;
            }
        }
    }

//    TopLeftPanel {
//        id: leftPanel;
//        anchors.top: parent.top;
//        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
//        anchors.left: parent.left;
//        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;

//        gradient: parent.gradient;

//        height: parent.height - thubnailDecoratorContainer.mainMargin;
//        width: 150;

//        decoratorSource: Style.topLeftPanelDecoratorPath;

//        topPanel: topPanel;
//    }

//    TopCenterPanel {
//        id: centerPanel;

//        anchors.top: parent.top;
//        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
//        anchors.left: leftPanel.right;
//        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
//        anchors.right: rightPanel.left;
//        anchors.rightMargin: thubnailDecoratorContainer.mainMargin;

//        gradient: parent.gradient;

//        height: parent.height - thubnailDecoratorContainer.mainMargin;

//        decoratorSource: Style.topCenterPanelDecoratorPath;

//        topPanel: topPanel;
//    }

//    TopRightPanel {
//        id: rightPanel;

//        anchors.top: parent.top;
//        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
//        anchors.right: parent.right;
//        anchors.rightMargin: thubnailDecoratorContainer.mainMargin;

//        gradient: parent.gradient;

//        height: parent.height - thubnailDecoratorContainer.mainMargin;
//        width: parent.width * 0.1;

//        decoratorSource: Style.topRightPanelDecoratorPath;

//        topPanel: topPanel;
//    }


//    AuxButton {
//        id: preferenceButton;
//        z: 100;

//        anchors.top: parent.top;
//        anchors.right: parent.right;
//        anchors.rightMargin: 10;
//        anchors.topMargin: 10;

//        width: 24;
//        height: 24;

//        iconSource: "../../../Icons/" + Style.theme + "/Settings_On_Normal.svg";

//        onClicked: {
//            console.log("Preference button clicked !", thubnailDecoratorContainer.localSettings);
//            var source = "AuxComponents/PreferenceDialog.qml";
//            var parameters = {};
//            parameters["localSettings"] = thubnailDecoratorContainer.localSettings;
//            thubnailDecoratorContainer.openDialog(source, parameters);

////            thubnailDecoratorContainer.refreshText();
//        }
//    }

//    Item {
//        anchors.left: parent.left;
//        anchors.leftMargin: 10;
//        anchors.right: menuButton.left;
//        anchors.rightMargin: 10;

//        height: parent.height;

//        ListView {
//            id: lvButtons;

//            anchors.horizontalCenter: parent.horizontalCenter;

//            height: parent.height;
//            width: contentWidth > parent.width ? parent.width : contentWidth;

//            clip: true;
//            orientation: ListView.Horizontal;
//            boundsBehavior: Flickable.StopAtBounds;

//            delegate: TopButton {
//                id: topButtonDelegate;

//                text: model[CommandEnum.NAME];
//                isEmpty: model[CommandEnum.NAME] === "";
//                imageSource: "../../../" + "Icons/" + Style.theme + "/" + model[CommandEnum.ICON] + "_" + "Off" + "_" + model["Mode"] + ".svg";
//                fontName: Style.fontFamily;
//                checkable: model["Mode"] === "Normal";
//                visible: x + width <= lvButtons.width;

//                onVisibleChanged: {
//                    console.log("TopPanel onVisibleChanged",topButtonDelegate.text, topButtonDelegate.visible);
////                    if (!topButtonDelegate.visible && lvButtons.width !== 0) {
////                        for (var i = 0; i < modelButtons.count; i++) {
////                            if (modelButtons.get(i).id !== "" && modelButtons.get(i).id === model[CommandEnum.ID]) {
////                               return;
////                            }
////                        }
////                        modelButtons.append({"id": model[CommandEnum.ID], "imageSource": topButtonDelegate.imageSource, "name": topButtonDelegate.text, "mode": model["Mode"]});
////                    }
////                    else if (topButtonDelegate.visible && lvButtons.width !== 0) {
////                        var j;
////                        for (var i = 0; i < modelButtons.count; i++) {
////                            if (modelButtons.get(i).id === model[CommandEnum.ID]) {
////                                modelButtons.remove(i)
////                                j = i;
////                                break;
////                            }
////                        }
////                        for (var i = j; i < modelButtons.count; i++) {
////                            modelButtons.remove(i)
////                        }
////                    }
//                }

//                onClicked: {
//                    console.log("TopPanel menuActivited", model.Id)
//                    topPanel.menuActivatedSignal(model.Id);
//                }
//            }
//        }
//    }

//    TopButton {
//        id: menuButton;

//        anchors.right: preferenceButton.left;
//        anchors.rightMargin: 10;
//        anchors.verticalCenter: parent.verticalCenter;

//        height: 56;
//        width: 35;

//        text: "";
//        checkable: true;

//        visible: modelButtons.count > 0;

//        imageSource: "../../../" + "Icons/" + Style.theme + "/Right_On_Normal.svg";

//        onClicked: {
//            console.log("TopPanel TopButton onClicked");
//            var source = "AuxComponents/PopupMenuDialog.qml";
//            var parameters = {};
//            parameters["model"] = modelButtons;
//            parameters["backgroundOpacity"] = 0;
//            parameters["resultItem"] = topPanel;
//            parameters["itemHeight"] = 35;
//            parameters["itemWidth"] = 150;
//            parameters["styleColor"] = Style.theme == "Dark" ? "Light" : "Dark";
//            thubnailDecoratorContainer.openDialog(source, parameters);
//        }
//    }

//    ListModel {
//        id: modelButtons;
//    }

    Timer {
        id: updateTimer;
        interval: 10;
        property TreeItemModel model;

        onModelChanged: {
            modelButtons.clear();
            updateTimer.start();
        }

        onTriggered: {
           decoratorLoader.item.centerPanel.lvButtons.model = updateTimer.model;
        }
    }

    GqlModel {
        id: commandsModel;

        property bool isFirst: true;

        function updateModel() {

            console.log("commandsModel topPanel.activeCommandsModelId", topPanel.activeCommandsModelId);
            var query = Gql.GqlRequest("query", "CommandsData");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField(PageEnum.ID);
            inputParams.InsertFieldArgument(PageEnum.ID, topPanel.activeCommandsModelId);

            inputParams.InsertField("LanguageId");
            inputParams.InsertFieldArgument("LanguageId", Style.language)
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField(CommandEnum.ID);
            queryFields.InsertField(CommandEnum.NAME);
            queryFields.InsertField(CommandEnum.ICON);
            queryFields.InsertField(CommandEnum.MODE);
            query.AddField(queryFields);
            queryFields = Gql.GqlObject("information");
            queryFields.InsertField("CommandsModelId");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("commandsModel updateModel", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, commandsModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (commandsModel.ContainsKey("errors")){
                    decoratorLoader.item.centerPanel.showCustomLoader();
                    return;
                }

                if (commandsModel.ContainsKey("data")){
                    dataModelLocal = commandsModel.GetData("data")

                    if(dataModelLocal.ContainsKey("CommandsData")){
                        dataModelLocal = dataModelLocal.GetData("CommandsData");

                        if(dataModelLocal.ContainsKey("items")){
                            var pageId = dataModelLocal.GetData("information").GetData("CommandsModelId");
                            dataModelLocal = dataModelLocal.GetData("items");
                            buttonsModelItem.SetExternTreeModel(pageId, dataModelLocal);
                            decoratorLoader.item.centerPanel.lvButtons.model = dataModelLocal;
                            decoratorLoader.item.centerPanel.showButtons();
                            commandsModel.isFirst = !commandsModel.isFirst
                            topPanel.commandsChangedSignal(topPanel.activeCommandsModelId);
                        }
                    }
                }
            }
        }
    }
}

