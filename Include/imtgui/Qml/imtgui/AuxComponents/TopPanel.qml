import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topPanel;
    height: 55;
    width: parent.width;
    color: Style.baseColor;
    property string fontName: "";
    property string activeCommandsModelId;
    property string title;

    signal menuActivatedSignal(string menuId);
    signal commandsChangedSignal(string commandsId);

    function setModeMenuButton(commandId, mode) {
        console.log("TopPanel setModeMenuButton!", commandId, mode);
        var buttonsModelLocal = buttonsModelItem.GetData(topPanel.activeCommandsModelId);
        //console.log("buttonsModelItem GetItemsCount!", buttonsModelLocal.GetItemsCount());
        for (var i = 0; i < buttonsModelLocal.GetItemsCount(); i++) {
            var id = buttonsModelLocal.GetData(CommandEnum.ID, i);
            //lvButtons.model.Refresh()
            if (id === commandId) {
                 buttonsModelLocal.SetData("Mode", mode, i);
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

    gradient: Gradient {
             GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
             GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
             GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
             GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
         }

    onWidthChanged: {
        if (thubnailDecoratorContainer.dialogIsActive()) {
            thubnailDecoratorContainer.closeDialog();
        }
    }

    onHeightChanged: {
        if (thubnailDecoratorContainer.dialogIsActive()) {
            thubnailDecoratorContainer.closeDialog();
        }
    }

    onActiveCommandsModelIdChanged: {
        console.log("onActiveCommandsModelIdChanged", topPanel.activeCommandsModelId);
        if (!buttonsModelItem.ContainsKey(topPanel.activeCommandsModelId)) {
            commandsModel.updateModel();
        } else {
            updateTimer.model = buttonsModelItem.GetData(topPanel.activeCommandsModelId);
        }
        topPanel.commandsChangedSignal(topPanel.activeCommandsModelId);
    }

    onTitleChanged: {
        console.log("onTitleChanged", topPanel.title);
        console.log("activeCommandsModelId", topPanel.activeCommandsModelId);
    }

    TreeItemModel {
        id: buttonsModelItem;
    }

    AuxButton {
        id: preferenceButton;
        z: 100;
        anchors.top: parent.top;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
        anchors.topMargin: 10;
        width: 24;
        height: 24;
        iconSource: "../../../Icons/" + Style.theme + "/Settings_On_Normal.svg";
        onClicked: {
            console.log("Preference button clicked !");
            var source = "AuxComponents/Preference.qml";
            var parameters = {};
            thubnailDecoratorContainer.openDialog(source, parameters);
        }
    }

    Item {
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: menuButton.left;
        anchors.rightMargin: 10;
        height: parent.height;

        ListView {
            id: lvButtons;
            anchors.horizontalCenter: parent.horizontalCenter;
            height: parent.height;
            width: contentWidth > parent.width ? parent.width : contentWidth;
            clip: true;
            orientation: ListView.Horizontal;
            boundsBehavior: Flickable.StopAtBounds;
            delegate: TopButton {
                text: model[CommandEnum.NAME];
                isEmpty: model[CommandEnum.NAME] === "";
                imageSource: "../../../" + "Icons/" + Style.theme + "/" + model[CommandEnum.ICON] + "_" + "Off" + "_" + model["Mode"] + ".svg";
                fontName: Style.fontFamily;
                checkable: model["Mode"] === "Normal";
                visible: x + width <= lvButtons.width;

                onVisibleChanged: {
                    console.log("TopPanel onVisibleChanged",text, visible);
                    if (!visible && lvButtons.width !== 0) {
                        for (var i = 0; i < modelButtons.count; i++) {
                            if (modelButtons.get(i).id !== "" && modelButtons.get(i).id === model[CommandEnum.ID]) {
                               return;
                            }
                        }
                        console.log("ModelButtons added id ", model[CommandEnum.ID]);
                        modelButtons.append({"id": model[CommandEnum.ID], "imageSource": imageSource, "name": text, "mode": model["Mode"]});
                    } else if (visible && lvButtons.width !== 0) {
                        var j;
                        for (var i = 0; i < modelButtons.count; i++) {
                            if (modelButtons.get(i).id === model[CommandEnum.ID]) {
                                console.log("ModelButtons removed id ", modelButtons.get(i).id);
                                modelButtons.remove(i)
                                j = i;
                                break;
                            }
                        }
                        for (var i = j; i < modelButtons.count; i++) {
                            console.log("ModelButtons removed id ", modelButtons.get(i).id);
                            modelButtons.remove(i)
                        }
                    }
                }

                onClicked: {
                    console.log("TopPanel menuActivited", model[CommandEnum.ID])
                    topPanel.menuActivatedSignal(model.Id);
                }
            }
        }
    }

    TopButton {
        id: menuButton;

        anchors.right: preferenceButton.left;
        anchors.rightMargin: 10;
        anchors.verticalCenter: parent.verticalCenter;

        height: 56;
        width: 35;

        text: "";
        checkable: true;

        visible: modelButtons.count > 0;

        imageSource: "../../../" + "Icons/" + Style.theme + "/Right_On_Normal.svg";

        onClicked: {
            console.log("TopPanel TopButton onClicked");
            var source = "AuxComponents/PopupMenuDialog.qml";
            var parameters = {};
            parameters["model"] = modelButtons;
            parameters["backgroundOpacity"] = 0;
            parameters["resultItem"] = topPanel;
            parameters["itemHeight"] = 35;
            parameters["itemWidth"] = 150;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }

    }

    ListModel {
        id: modelButtons;
    }

    Timer {
        id: updateTimer;
        interval: 10;
        property TreeItemModel model;
        onModelChanged: {
            modelButtons.clear();
            updateTimer.start();
        }

        onTriggered: {
           lvButtons.model = updateTimer.model;
        }
    }

    GqlModel {
        id: commandsModel;

        property bool isFirst: true;

        function updateModel() {

            var query = Gql.GqlRequest("query", "CommandsData");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField(PageEnum.ID);
            inputParams.InsertFieldArgument(PageEnum.ID, topPanel.activeCommandsModelId);
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
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey("CommandsData")){
                    dataModelLocal = dataModelLocal.GetData("CommandsData");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        var pageId = dataModelLocal.GetData("information").GetData("CommandsModelId");
                        dataModelLocal = dataModelLocal.GetData("items");
                        buttonsModelItem.SetExternTreeModel(pageId, dataModelLocal);
                        updateTimer.model = dataModelLocal;
                        commandsModel.isFirst = !commandsModel.isFirst
                        topPanel.commandsChangedSignal(topPanel.activeCommandsModelId);
                    }
                    else if(commandsModel.ContainsKey("errors")){
                        var errorsModel = commandsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey("CommandsData")){
                            console.log("message", errorsModel.GetData("CommandsData").GetData("message"));
                        }
                    }
                }
            }
        }
    }
}

