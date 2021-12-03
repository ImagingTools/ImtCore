import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: container;
    property string styleColor: "Light";
//    property color foneColor: styleColor == "Dark"? "#2b2d2d" : "white";
    property color foneColor: Style.mainColor;
//    property color fontColor: styleColor == "Dark"? "#edefef" : "black";
    //property color textColor: Style.textColor;
    property int radiusValue: 5;

    radius: radiusValue;
    color: Style.baseColor;
    border.color: "#2b2d2d";

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: body;
        anchors.fill: parent;
//       width: parent.width - 5;
//       height: parent.height - 5;
       anchors.horizontalCenter: parent.horizontalCenter;
       anchors.verticalCenter: parent.verticalCenter;
       clip: true;
       color: Style.baseColor;
       anchors.margins: 10;

       Rectangle {
           id: topPanel;
           width: parent.width;
           height: 30;
          // anchors.bottomMargin: 50;
           color: Style.baseColor;

           Rectangle {
               id: iconPref;
               width: 20;
               height: 20;
               color: container.styleColor == "Dark"? "white" : "#4C514A";
               anchors.left: topPanel.left;
               anchors.verticalCenter: topPanel.verticalCenter;
               radius: radiusValue;
           }

           Text {
               id: name;
               anchors.left: iconPref.right;
               anchors.leftMargin: 10;
               anchors.verticalCenter: topPanel.verticalCenter;
               text: "Preferences";
               color: Style.textColor;

           }

//           Button {

//           }

           Rectangle {
               id: exit;
               anchors.right: topPanel.right;
               anchors.verticalCenter: topPanel.verticalCenter;
               width: 20;
               height: 20;
               color: "red";
               MouseArea {
                   anchors.fill: parent;
                   onClicked: {
//                       container.visible = false;
                       preference.visible = false;
                   }
               }
           }
       }

       Rectangle {
           id: versionBlock;
           width: parent.width;
           height: 100;
           anchors.top: topPanel.bottom;
           anchors.topMargin: 30;
           color: Style.baseColor;
           Text {
               id: versionTitle;
               anchors.left: versionBlock.left;
               anchors.leftMargin: 30;
               anchors.top: versionBlock.top;
               text: "Version";
               color: Style.textColor;
           }

           Rectangle {
               id: line0;
               anchors.topMargin: 10;
               anchors.top: versionTitle.bottom;
               width: parent.width;
               height: 1;
               color: "black";
           }

           Text {
               id: versionValue;
               anchors.leftMargin: 30;
               anchors.topMargin: 10;
               anchors.left: versionBlock.left;
               anchors.top: line0.bottom;
               text: "MIO Version";
               color: "#A5A5A5";
           }
       }

       Rectangle {
           id: languageUnitBlock;
           z: 2;
           width: parent.width;
           height: 100;
           anchors.top: versionBlock.bottom;
           //anchors.topMargin: 30;
           color: Style.baseColor;
           Text {
               id: languageText;
               anchors.left: languageUnitBlock.left;
               anchors.leftMargin: 30;
               anchors.top: languageUnitBlock.top;
               text: "Language";
               color: Style.textColor;
           }

           Text {
               id: unitText;
               x: parent.width / 2 + 30;
               anchors.top: versionBlock.top;
               text: "Unit";
               color: Style.textColor;
           }

           Rectangle {
               id: line;
               anchors.topMargin: 10;
               anchors.top: languageText.bottom;
               width: parent.width;
               height: 1;
               color: "black";
           }

           ComboBox {
               id: cbLang;
               anchors.leftMargin: 30;
               anchors.topMargin: 10;
               anchors.top: line.bottom;
               anchors.left: languageUnitBlock.left;
               model: languageModel;
           }

           ComboBox {
               id: cbUnit;
               x: unitText.x;
               anchors.topMargin: 10;
               anchors.top: line.bottom;
               model: unitModel;
           }
       }

       Rectangle {
           id: modeBlock;
           z: 1;
           width: parent.width;
           height: 100;
           anchors.top: languageUnitBlock.bottom;
          // anchors.bottomMargin: 30;
           color: Style.baseColor;
           Text {
               id: modeText;
               anchors.leftMargin: 30;
               anchors.left: modeBlock.left;
               //anchors.bottomMargin: 10;
               anchors.top: modeBlock.top;
               text: "Mode";
               color: Style.textColor;
           }

           Rectangle {
               id: line2;
               anchors.topMargin: 10;
               anchors.top: modeText.bottom;
               width: parent.width;
               height: 1;
               color: "black";
           }

           Rectangle {
               id: darkMode;
               width: 90;
               height: 60;
               anchors.top: line2.bottom;
               anchors.left: modeBlock.left;
               anchors.leftMargin: 30;
               anchors.topMargin: 10;
               color: "black";
               radius: radiusValue;
               border.color: container.styleColor == "Dark"? "#1560BD" : "transparent";
               border.width: 2;

               Text {
                   id: darkText;
                   text: "Dark";
                   anchors.top: darkMode.bottom;
                   anchors.topMargin: 10;
                   color: Style.textColor;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        container.styleColor = "Dark";
                        stylesModel.getStyle("Dark");
                    }
               }
           }

           Rectangle {
               id: lightMode;

               width: darkMode.width;
               height: darkMode.height;
               anchors.top: line2.bottom;
               anchors.left: darkMode.right;

               anchors.topMargin: 10;
               anchors.leftMargin: 20;
               color: "white";
               border.color: container.styleColor == "Light"? "#1560BD" : "black";
               border.width: 2;
               radius: radiusValue;

               Text {
                   id: lightText;
                    text: "Light";
                    anchors.top: lightMode.bottom;
                    anchors.topMargin: 10;
                    color: Style.textColor;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {

                        stylesModel.getStyle("Light");
                        container.styleColor = "Light";
                    }
               }
           }
       }
    }

    function getThemeColor(colorType, colorKey, themeModel) {
        var colorPalette = this.themeModel.GetData("Style").GetData(colorType).GetData(colorKey);
        return themeModel.GetData("ColorPalette").GetData(colorPalette);
    }

    function parseStyleTheme(themeModel) {
        var activeColors = themeModel.GetData("Style").GetData("ActiveColors");
        var colorPalette = themeModel.GetData("ColorPalette");
        var count = activeColors.GetItemsCount();

        Style.imagingToolsGradient1 = themeModel.GetData("ColorPalette").GetData("ImagingToolsGradient1");
        Style.imagingToolsGradient2 = themeModel.GetData("ColorPalette").GetData("ImagingToolsGradient2");
        Style.imagingToolsGradient3 = themeModel.GetData("ColorPalette").GetData("ImagingToolsGradient3");
        Style.imagingToolsGradient4 = themeModel.GetData("ColorPalette").GetData("ImagingToolsGradient4");
        Style.baseColor = preference.getThemeColor("ActiveColors", "Base", themeModel);
        Style.backgroundColor = preference.getThemeColor("ActiveColors", "Background", themeModel);
        Style.textColor = preference.getThemeColor("ActiveColors", "Text", themeModel);

    }

    GqlModel {
        id: stylesModel;

        function getStyle(theme) {
            console.log( "getStyle");

            var query = Gql.GqlRequest("query", "GetStyle");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("theme");
            inputParams.InsertFieldArgument("theme", theme);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("style");
            queryFields.InsertField("theme");
            queryFields.InsertField("source");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log(gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, stylesModel);
            if (this.state === "Ready") {
                var dataModelLocal = this.GetData("data");
                console.log(dataModelLocal);
                if(dataModelLocal.ContainsKey("GetStyle")) {
                    dataModelLocal = dataModelLocal.GetData("GetStyle");
                }
                if(dataModelLocal !== null && dataModelLocal.ContainsKey("source")){
                    dataModelLocal = dataModelLocal.GetData("source");
                    preference.parseStyleTheme(dataModelLocal);
                }
                else if(stylesModel.ContainsKey("errors")){
                    var errorsModel = stylesModel.GetData("errors");
                    if(errorsModel !== null && errorsModel.ContainsKey("GetStyle")){
                        console.log("message", errorsModel.GetData("GetStyle").GetData("message"));
                    }
                }

            }
        }
    }

    ListModel {
        id: languageModel;
        ListElement {
            text: "Русский";
        }

        ListElement {
            text: "English";
        }
    }

    ListModel {
        id: unitModel;
        ListElement {
            text: "Millimeters";
        }

        ListElement {
            text: "Centimeters";
        }
    }


}
