import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topPanel;
    height: 55;
    width: parent.width;
    color: "#e5e5e7";
    property string fontName: "";
    property string activePageId;
    property alias title: titleText.text;

    gradient: Gradient {
             GradientStop { position: 0.0; color: "#e5e5e7"; }
             GradientStop { position: 0.7; color: "#d9d9db"; }
             GradientStop { position: 0.98; color: "#d2d2d4"; }
             GradientStop { position: 1.0; color: "#a4a5a6"; }
         }

    onActivePageIdChanged: {
        console.log("onActivePageIdChanged", topPanel.activePageId);
        commandsModel.updateModel();
    }

    onTitleChanged: {
        console.log("onTitleChanged", topPanel.title);
        console.log("onActivePageIdChanged", topPanel.activePageId);

    }

    Button {
        id: prevStack;
        anchors.verticalCenter: parent.verticalCenter;
        x: 10;
        width: 30;
        height: 30;
        iconSource:"../../Icons/Left.svg";
        onClicked: {
            console.log("Left ckicked");
        }
    }

    Button {
        id: nextStack;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: prevStack.right;
        anchors.leftMargin: 10;
        width: 30;
        height: 30;
        iconSource:"../../Icons/Right.svg";
    }

    Text {
        id: titleText;
        anchors.left: nextStack.right;
        anchors.leftMargin: 10;
        anchors.verticalCenter: parent.verticalCenter;

        text: qsTr("Products");
        font.family: "Helvetica";
        font.pixelSize: 25;
    }

    Item {
        anchors.left: nextStack.right;
        anchors.leftMargin: 150;
        anchors.right: parent.right;
        height: parent.height;
        ListView {
            id: lvButtons;
//            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height;
            width: contentWidth > parent.width ? parent.width : contentWidth;
            model: 4;
            clip: true;
            orientation: ListView.Horizontal;
            delegate: TopButton {
                text: model[CommandEnum.NAME];
//                imageSource: model[CommandEnum.ICON];
                fontName: topPanel.fontName;
            }
        }
    }

    GqlModel {
        id: commandsModel;

        function updateModel() {
            console.log( "updateModel");

            var query = Gql.GqlRequest("query", "CommandsData");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField(PageEnum.ID);
            inputParams.InsertFieldArgument(PageEnum.ID, topPanel.activePageId);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField(CommandEnum.ID);
            queryFields.InsertField(CommandEnum.NAME);
            queryFields.InsertField(CommandEnum.ICON);
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
//            console.log(gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, commandsModel);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey("CommandsData")){
                    dataModelLocal = dataModelLocal.GetData("CommandsData");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
//                        console.log("items",dataModelLocal);
                        lvButtons.model = dataModelLocal;
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

