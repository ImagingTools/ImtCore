import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: container;
    property string styleColor: "Light";
//    property color foneColor: styleColor == "Dark"? "#2b2d2d" : "white";
//    property color foneColor: Style.mainColor;
//    property color fontColor: styleColor == "Dark"? "#edefef" : "black";
    //property color textColor: Style.textColor;
    property int radiusValue: 5;

    radius: radiusValue;
    color: Style.baseColor;
    border.color: "#2b2d2d";

    function changeThemeIcons(theme)
    {
        for (var i = 0; i < IconStyle.iconNames.length; i++)
        {
            var fileName =  IconStyle.iconNames[i].slice(5) + ".svg";
            var command = "IconStyle." + IconStyle.iconNames[i] + " = \"/Icons/" + theme + "/" + fileName + "\"";
            console.log("eval command " + command);
            eval(command);
        }
    }


    Component.onCompleted: {

//        var iconNames = Style.iconNames;
//        var type1 = ["On", "Off"];
//        var type2 = ["Active", "Disabled", "Normal", "Selected"];
//        for (var i = 0; i < iconNames.length; i++)
//        {
//            for (var j = 0; j < type1.length; j++)
//            {
//                for (var k = 0; k < type2.length; k++)
//                {
//                    var propName = "icon" + iconNames[i] + "_" + type1[j] + "_" + type2[k];
//                    var path = "/Icons/" + styleColor + "/" + iconNames[i] + "_" + type1[j] + "_" + type2[k]
//                    console.log("Name: ", propName);
//                    console.log("Path: ", path);
//                    var obj = Object.defineProperty(style2, propName,
//                     {
//                         enumerable: true,
//                         configurable: true,
//                         writable: true,
//                         value: path
//                     })
//                }
//            }
//        }

//        console.log("Style iconCamera_On_Active: ", style2.iconCamera_On_Active);
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            cbLang.menuVisible = false;
            cbUnit.menuVisible = false;
        }
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
           id: topPan;
           width: parent.width;
           height: 30;
          // anchors.bottomMargin: 50;
           color: Style.baseColor;

           Rectangle {
               id: iconPref;
               width: 20;
               height: 20;
//               color: container.styleColor == "Dark"? "white" : "#4C514A";
               color: Style.textColor;
               anchors.left: topPan.left;
               anchors.verticalCenter: topPan.verticalCenter;
               radius: radiusValue;
           }

           Text {
               id: name;
               anchors.left: iconPref.right;
               anchors.leftMargin: 10;
               anchors.verticalCenter: topPan.verticalCenter;
               text: "Preferences";
               color: Style.textColor;
               font.family: Style.fontFamily;
               font.pixelSize: Style.fontSize_subtitle;
           }

           Rectangle {
               id: exit;
               anchors.right: topPan.right;
               anchors.verticalCenter: topPan.verticalCenter;
               width: 20;
               height: 20;
               color: Style.textColor;
               radius: radiusValue;
               MouseArea {
                   anchors.fill: parent;
                   onClicked: {
                       cbLang.menuVisible = false;
                       cbUnit.menuVisible = false;
                       preference.visible = false;
                   }
               }
           }
       }

       Rectangle {
           id: versionBlock;
           width: parent.width;
           height: 100;
           anchors.top: topPan.bottom;
           anchors.topMargin: 30;
           color: Style.baseColor;
           Text {
               id: versionTitle;
               anchors.left: versionBlock.left;
               anchors.leftMargin: 30;
               anchors.top: versionBlock.top;
               text: "Version";
               color: Style.textColor;
               font.family: Style.fontFamily;
               font.pixelSize: Style.fontSize_subtitle;
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
               font.family: Style.fontFamily;
               font.pixelSize: Style.fontSize_subtitle;
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
               font.family: Style.fontFamily;
               font.pixelSize: Style.fontSize_subtitle;
           }

           Text {
               id: unitText;
               x: parent.width / 2 + 30;
               anchors.top: versionBlock.top;
               text: "Unit";
               color: Style.textColor;
               font.family: Style.fontFamily;
               font.pixelSize: Style.fontSize_subtitle;
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
               font.family: Style.fontFamily;
               font.pixelSize: Style.fontSize_subtitle;
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
               border.color: preference.styleColor == "Dark"? "#1560BD" : "transparent";
               border.width: 2;

               Text {
                   id: darkText;
                   text: "Dark";
                   anchors.top: darkMode.bottom;
                   anchors.topMargin: 10;
                   color: Style.textColor;
                   font.family: Style.fontFamily;
                   font.pixelSize: Style.fontSize_subtitle;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        preference.styleColor = "Dark";

                        Style.camera = "../Icons/Dark/Camera.svg";
                        stylesModel.getStyle("Dark");
                        preference.changeThemeIcons("Dark");
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
               border.color: preference.styleColor == "Light"? "#1560BD" : "black";
               border.width: 2;
               radius: radiusValue;

               Text {
                   id: lightText;
                    text: "Light";
                    anchors.top: lightMode.bottom;
                    anchors.topMargin: 10;
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_subtitle;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {
//                        console.log("IconStyle.icon_Axis_On_Active = ", IconStyle.icon_Axis_On_Active);
//                        var color = "IconStyle.icon_Axis_On_Active = \"test\" ";
//                        eval(color)

//                        console.log("IconStyle.icon_Axis_On_Active = ", IconStyle.icon_Axis_On_Active);
                        stylesModel.getStyle("Light");
                        preference.changeThemeIcons("Light");
//                        IconStyle.icon_Axis_On_Active = "/Icons/Light/Axis_On_Active.svg";
                        //"/Icons/Dark/Axis_On_Active.svg"
                        preference.styleColor = "Light";
                    }
               }
           }
       }
    }

    function getThemeColor(colorType, colorKey, themeType) {
        var colorPalette = themeType.GetData("Style").GetData(colorType).GetData(colorKey);
        return themeType.GetData("ColorPalette").GetData(colorPalette);

    }

    function parseStyleTheme(themeType) {
        Style.baseColor = preference.getThemeColor("ActiveColors", "Base", themeType);
        Style.backgroundColor = preference.getThemeColor("ActiveColors", "Background", themeType);
        Style.textColor = preference.getThemeColor("ActiveColors", "Text", themeType);

        Style.selectedColor = preference.getThemeColor("ActiveColors", "ItemSelected", themeType);
        Style.buttonColor = preference.getThemeColor("ActiveColors", "HeaderBorder", themeType);
        Style.buttonBorderColor = preference.getThemeColor("ActiveColors", "ButtonBorder", themeType);
        Style.imagingToolsGradient1 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient1");
        Style.imagingToolsGradient2 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient2");
        Style.imagingToolsGradient3 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient3");
        Style.imagingToolsGradient4 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient4");
    }

    GqlModel {
        id: stylesModel;

        function getStyle(theme) {
//            console.log( "getStyle");

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
//            console.log(gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, stylesModel);
            if (this.state === "Ready") {
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey("GetStyle")) {
                    dataModelLocal = dataModelLocal.GetData("GetStyle");
                }
                if(dataModelLocal !== null && dataModelLocal.ContainsKey("source")){
                    dataModelLocal = dataModelLocal.GetData("source");
//                    console.log("dataModelLocal", dataModelLocal);
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
