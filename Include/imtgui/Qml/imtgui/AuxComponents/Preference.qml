import QtQuick 2.0


Rectangle {
    id: container;
    property string styleMode;
    property string styleColor: "Dark";
    property color foneColor: styleColor == "Dark"? "#2b2d2d" : "white";
    property color fontColor: styleColor == "Dark"? "#edefef" : "black";
    property int radiusValue: 5;

    radius: radiusValue;
    color: foneColor;
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
       color: container.color;
       anchors.margins: 10;

       Rectangle {
           id: topPanel;
           width: parent.width;
           height: 30;
          // anchors.bottomMargin: 50;
           color: container.color;

           Rectangle {
               id: iconPref;
               width: 20;
               height: 20;
               color: styleColor == "Dark"? "white" : "#4C514A";
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
               color: fontColor;

           }

           Button {
               id: exit;
               anchors.right: topPanel.right;
               anchors.verticalCenter: topPanel.verticalCenter;
               width: 20;
               height: 20;
               color: "red";
               onClicked: {
                   container.visible = false;
               }
           }

       }

       Rectangle {
           id: versionBlock;
           width: parent.width;
           height: 100;
           anchors.top: topPanel.bottom;
           anchors.topMargin: 30;
           color: container.color;
           Text {
               id: versionTitle;
               anchors.left: versionBlock.left;
               anchors.leftMargin: 30;
               anchors.top: versionBlock.top;
               text: "Version";
               color: fontColor;
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
           //anchors.topMargin: 30
           color: container.color;
           Text {
               id: languageText;
               anchors.left: languageUnitBlock.left;
               anchors.leftMargin: 30;
               anchors.top: languageUnitBlock.top;
               text: "Language";
               color: fontColor;
           }

           Text {
               id: unitText;
               x: parent.width / 2 + 30;
               anchors.top: versionBlock.top;
               text: "Unit";
               color: fontColor;
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
               foneColor: container.foneColor;
               fontColor: container.fontColor;
               borderColor: styleColor == "Dark"? "white" : "#4C514A";
           }

           ComboBox {
               id: cbUnit;
               x: unitText.x;
               anchors.topMargin: 10;
               anchors.top: line.bottom;
               model: unitModel;
               foneColor: container.foneColor;
               fontColor: container.fontColor;
               borderColor: styleColor == "Dark"? "white" : "#4C514A";
           }
       }

       Rectangle {
           id: modeBlock;
           z: 1;
           width: parent.width;
           height: 100;
           anchors.top: languageUnitBlock.bottom;
          // anchors.bottomMargin: 30;
           color: container.color;
           Text {
               id: modeText;
               anchors.leftMargin: 30;
               anchors.left: modeBlock.left;
               //anchors.bottomMargin: 10;
               anchors.top: modeBlock.top;
               text: "Mode";
               color: fontColor;
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
               border.color: styleColor == "Dark"? "#1560BD" : "transparent";
               border.width: 2;

               Text {
                   id: darkText;
                   text: "Dark";
                   anchors.top: darkMode.bottom;
                   anchors.topMargin: 10;
                   color: fontColor;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        styleColor = "Dark";
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
               border.color: styleColor == "Light"? "#1560BD" : "black";
               border.width: 2;
               radius: radiusValue;

               Text {
                   id: lightText;
                    text: "Light";
                    anchors.top: lightMode.bottom;
                    anchors.topMargin: 10;
                    color: fontColor;
               }

               MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        styleColor = "Light";
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
