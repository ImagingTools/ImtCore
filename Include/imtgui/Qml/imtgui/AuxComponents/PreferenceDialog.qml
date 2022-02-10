import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: preferenceContainer;

    width: 800;
    height: 500;

    color: Style.backgroundColor;

    property real backgroundOpacity: 0.4;
    property bool centered: true;

    Rectangle {
        id: topPan;

        width: parent.width;
        height: 30;

        color: Style.baseColor;

        Image {
            id: iconPref;

            anchors.left: topPan.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: topPan.verticalCenter;

            width: 20;
            height: 20;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Settings" + "_" + "On" + "_" + "Normal" + ".svg";
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
            anchors.rightMargin: 10;
            anchors.verticalCenter: topPan.verticalCenter;

            width: 17;
            height: 17;

            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

            MouseArea {
               anchors.fill: parent;
               onClicked: {
                   loaderDialog.closeItem();
               }
            }
        }
    }

   Column {
       id: mainPanelColumn;

       anchors.top: topPan.bottom;
       anchors.topMargin: 10;
       anchors.left: parent.left;
       anchors.leftMargin: 10;
       anchors.bottom: parent.bottom;
       anchors.bottomMargin: 10;

       width: 200;

       Rectangle {
           anchors.fill: parent;
           color: Style.baseColor;
       }

       Repeater {
           id: mainPanelRepeater;
       }
   }

   Column {
       id: dependentPanelColumn;

       anchors.left: mainPanelColumn.right;
       anchors.leftMargin: 20;
       anchors.right: preferenceContainer.right;
       anchors.rightMargin: 20;
       anchors.top: topPan.bottom;
       anchors.topMargin: 20;
       anchors.bottom: preferenceContainer.bottom;
       anchors.bottomMargin: 20;

       Repeater {
           id: dependentPanelRepeater;
       }
   }
}
