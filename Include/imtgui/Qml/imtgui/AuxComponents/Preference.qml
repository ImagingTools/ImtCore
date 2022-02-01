import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: preferenceContainer;

    width: 450;
    height: 450;

    property string styleColor: "Light";
    property int radiusValue: 5;

    radius: radiusValue;
    color: Style.baseColor;
//    border.color: "#2b2d2d";
    property real backgroundOpacity: 0.4;
    property bool centered: true;

    Component.onCompleted: {
//        preferenceContainer.changeThemeIcons("Light");
    }

    function changeThemeIcons(theme)
    {
//        var type1 = ["On", "Off"];
//        var type2 = ["Active", "Disabled", "Normal", "Selected"];
//        for (var i = 0; i < IconStyle.iconNames.length; i++)
//        {
//            for (var j = 0; j < type1.length; j++)
//            {
//                for (var k = 0; k < type2.length; k++)
//                {
//                    var propName = "icon_" + IconStyle.iconNames[i] + "_" + type1[j] + "_" + type2[k];
//                    var fileName = IconStyle.iconNames[i] + "_" + type1[j] + "_" + type2[k];
//                    var command = "IconStyle." + propName + " = \"/Icons/" + theme + "/" + fileName + ".svg\"";
//                    eval(command);
//                }
//            }
//        }
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            cbLang.menuVisible = false;
//            cbUnit.menuVisible = false;
        }
    }

    Rectangle {
        id: body;
        anchors.fill: parent;
//       anchors.horizontalCenter: parent.horizontalCenter;
//       anchors.verticalCenter: parent.verticalCenter;
       clip: true;
       color: Style.baseColor;
       anchors.margins: 10;

       Rectangle {
           id: topPan;
           width: parent.width;
           height: 30;
           color: Style.baseColor;

           Image {
               id: iconPref;
               width: 20;
               height: 20;
               anchors.left: topPan.left;
               anchors.verticalCenter: topPan.verticalCenter;
               source: "../../../" + "Icons/" + Style.theme + "/" + "Settings" + "_" + "On" + "_" + "Normal" + ".svg";
              // source: "../../../" + Style.getImageSource("Settings", Style.theme, "On", "Active");
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

           AuxButton {
               id: exit;
               anchors.right: topPan.right;
               anchors.verticalCenter: topPan.verticalCenter;
               width: 17;
               height: 17;
               radius: radiusValue;
               iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";
              MouseArea {
                  anchors.fill: parent;
                  onClicked: {
                      cbLang.menuVisible = false;
//                      preferenceContainer.visible = false;
                      loaderDialog.closeItem();
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
               font.pixelSize: Style.fontSize_common;
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
               font.pixelSize: Style.fontSize_common;
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
               font.pixelSize: Style.fontSize_common;
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
       }

       Rectangle {
           id: modeBlock;
           z: 1;
           width: parent.width;
           height: 100;
           anchors.top: languageUnitBlock.bottom;
           color: Style.baseColor;
           Text {
               id: modeText;
               anchors.leftMargin: 30;
               anchors.left: modeBlock.left;
               anchors.top: modeBlock.top;
               text: "Mode";
               color: Style.textColor;
               font.family: Style.fontFamily;
               font.pixelSize: Style.fontSize_common;
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
               border.color: preferenceContainer.styleColor == "Dark"? "#1560BD" : "transparent";
               border.width: 2;

               Text {
                   id: darkText;
                   text: "Dark";
                   anchors.top: darkMode.bottom;
                   anchors.topMargin: 10;
                   color: Style.textColor;
                   font.family: Style.fontFamily;
                   font.pixelSize: Style.fontSize_common;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        preferenceContainer.styleColor = "Dark";

                        Style.camera = "../Icons/Dark/Camera.svg";
                        stylesModel.getStyle("Dark");
//                        preferenceContainer.changeThemeIcons("Dark");
                        Style.theme = "Dark";
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
               border.color: preferenceContainer.styleColor == "Light"? "#1560BD" : "black";
               border.width: 2;
               radius: radiusValue;

               Text {
                   id: lightText;
                    text: "Light";
                    anchors.top: lightMode.bottom;
                    anchors.topMargin: 10;
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        stylesModel.getStyle("Light");
//                        preferenceContainer.changeThemeIcons("Light");
                        preferenceContainer.styleColor = "Light";
                        Style.theme = "Light";

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
        Style.baseColor = preferenceContainer.getThemeColor("ActiveColors", "Base", themeType);
        Style.alternateBaseColor = preferenceContainer.getThemeColor("ActiveColors", "AlternateBase", themeType);
        Style.backgroundColor = preferenceContainer.getThemeColor("ActiveColors", "Background", themeType);
        Style.textColor = preferenceContainer.getThemeColor("ActiveColors", "Text", themeType);
        Style.textSelected = preferenceContainer.getThemeColor("ActiveColors", "TextSelectedBackground", themeType);
        console.log("Style.textSelected ", Style.textSelected);
        Style.selectedColor = preferenceContainer.getThemeColor("ActiveColors", "ItemSelected", themeType);
        Style.buttonColor = preferenceContainer.getThemeColor("ActiveColors", "HeaderBorder", themeType);
        Style.buttonBorderColor = preferenceContainer.getThemeColor("ActiveColors", "ButtonBorder", themeType);

        Style.disabledInActiveTextColor = preferenceContainer.getThemeColor("DisabledInActiveColors", "Text", themeType);

        Style.hover = preferenceContainer.getThemeColor("ActiveColors", "Hover", themeType);

        Style.imagingToolsGradient1 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient1");
        Style.imagingToolsGradient2 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient2");
        Style.imagingToolsGradient3 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient3");
        Style.imagingToolsGradient4 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient4");

        Style.iconColorOnSelected = preferenceContainer.getThemeColor("IconColor", "OnSelected", themeType);
        Style.tabSelectedColor = preferenceContainer.getThemeColor("ActiveColors", "TabSelected", themeType);
        Style.errorTextColor = preferenceContainer.getThemeColor("ActiveColors", "ErrorText", themeType);
    }

    GqlModel {
        id: stylesModel;

        function getStyle(theme) {
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
            console.log("Preference GqlModel getStyle query ", gqlData);
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
                    preferenceContainer.parseStyleTheme(dataModelLocal);
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
