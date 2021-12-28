import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topPanel;
    height: 55;
    width: parent.width;
    color: Style.baseColor;
    property string fontName: "";
    property string activePageId;
    property string title;


    gradient: Gradient {
             GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
             GradientStop { position: 0.7; color: Style.imagingToolsGradient2; }
             GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
             GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
         }

    onActivePageIdChanged: {
        console.log("onActivePageIdChanged", topPanel.activePageId);
        commandsModel.updateModel();
    }

    onTitleChanged: {
        console.log("onTitleChanged", topPanel.title);
        console.log("onActivePageIdChanged", topPanel.activePageId);
    }

//    AuxButton {
//        id: prevStack;
//        anchors.verticalCenter: parent.verticalCenter;
//        x: 10;
//        width: 24;
//        height: 24;
//        iconSource: "../../../" + "Icons/" + Style.theme + "/Left_On_Normal.svg";
//        onClicked: {
//            console.log("Left ckicked!");
//        }
//    }

//    AuxButton {
//        id: nextStack;
//        anchors.verticalCenter: parent.verticalCenter;
//        anchors.left: prevStack.right;
//        anchors.leftMargin: 10;
//        width: 24;
//        height: 24;
//        iconSource: "../../../Icons/" + Style.theme + "/Right_On_Normal.svg";

//        MouseArea {
//            id: leftButtonMA;
//            anchors.fill: parent;
//        }
//    }

    AuxButton {
        id: preferenceButton;
        z: 100;
        anchors.top: parent.top;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
        anchors.topMargin: 10;
        width: 24;
        height: 24;
//        iconSource: "../../../" + Style.getImageSource("Settings", Style.theme, "On", "Active");
        iconSource: "../../../Icons/" + Style.theme + "/Settings_On_Normal.svg";
        onClicked: {
            console.log("Preference button clicked !");
            preference.visible = true;

        }
    }

//    Text {
//        id: titleText;
//        anchors.left: nextStack.right;
//        anchors.leftMargin: 10;
//        anchors.verticalCenter: parent.verticalCenter;

//        text: qsTr("Products");
//        color: Style.textColor;
//        font.family: Style.fontFamily;
//        font.pixelSize: Style.fontSize_title;
//    }

    Item {
        anchors.left: parent.left;
        anchors.leftMargin: 200;
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
                isEmpty: model[CommandEnum.NAME] === "";
                imageSource: "../../../" + "Icons/" + Style.theme + "/" + model[CommandEnum.ICON] + "_" + "On" + "_" + "Normal" + ".svg";
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

