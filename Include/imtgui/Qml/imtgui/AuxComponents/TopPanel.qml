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
        console.log("buttonsModelItem GetItemsCount!", buttonsModelLocal.GetItemsCount());
        for (var i = 0; i < buttonsModelLocal.GetItemsCount(); i++) {
            var id = buttonsModelLocal.GetData(CommandEnum.ID, i);
            //lvButtons.model.Refresh()
            if (id === commandId) {
                 buttonsModelLocal.SetData("Mode", mode, i);
                 break;
            }
        }
    }


    gradient: Gradient {
             GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
             GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
             GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
             GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
         }

    onActiveCommandsModelIdChanged: {
        console.log("onActiveCommandsModelIdChanged", topPanel.activeCommandsModelId);
        if (!buttonsModelItem.ContainsKey(topPanel.activeCommandsModelId)) {
            commandsModel.updateModel();
        } else {
            updateTimer.model = buttonsModelItem.GetData(topPanel.activeCommandsModelId);
//            topPanel.commandsChangedSignal(topPanel.activeCommandsModelId);
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
            preference.visible = true;

        }
    }

    Item {
        anchors.left: parent.left;
        anchors.right: preferenceButton.left;
        height: parent.height;

        ListView {
            id: lvButtons;
            anchors.horizontalCenter: parent.horizontalCenter;
            height: parent.height;
            width: contentWidth > parent.width ? parent.width : contentWidth;
            clip: true;
            orientation: ListView.Horizontal;
            interactive: false;
            delegate: TopButton {
                text: model[CommandEnum.NAME];
                isEmpty: model[CommandEnum.NAME] === "";
                imageSource: "../../../" + "Icons/" + Style.theme + "/" + model[CommandEnum.ICON] + "_" + "On" + "_" + model["Mode"] + ".svg";
                fontName: Style.fontFamily;
                onClicked: {
                    console.log("TopPanel menuActivited", model[CommandEnum.ID])
                    topPanel.menuActivatedSignal(model.Id);
                }
            }
        }
    }


    Timer {
        id: updateTimer;
        interval: 10;
        property TreeItemModel model;
        onModelChanged: {
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

