import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: auxButtonContainer;

    radius: 2;

    border.width: 1;
    border.color: auxButtonContainer.borderColor;

    color: auxButtonContainer.pressed && auxButtonContainer.highlighted ? Style.hover :
                                        auxButtonContainer.highlighted ? Style.selectedColor :
                                                                         auxButtonContainer.backgroundColor;

    property bool highlighted: ma.containsMouse;
    property bool hasIcon: auxButtonContainer.iconSource !== "";
    property bool hasText: false;
    property bool borderExist: false;
    property bool enabled: true;
    property bool pressed: false;

    property string borderColor: "transparent";
    property string backgroundColor: "transparent";
    property var textButton;

    property int iconWidth: auxButtonContainer.width;
    property int iconHeight: auxButtonContainer.height;
    property int fontPixelSize: Style.fontSize_common;

    property alias iconSource: image.source;

    signal clicked;

    onFocusChanged: {
        auxButtonContainer.highlighted  = auxButtonContainer.focus;
    }

    Keys.onPressed: {
        console.log("AuxButton Key pressed!")
        if (event.key === Qt.Key_Return){
            auxButtonContainer.clicked();
        }
     }

    Image {
        id: image;

        anchors.centerIn: parent;

        height: auxButtonContainer.iconHeight - 3;
        width: auxButtonContainer.iconWidth - 3;

        sourceSize.width: width;
        sourceSize.height: height;

        visible: auxButtonContainer.hasIcon;
    }

    Text {
        anchors.horizontalCenter: auxButtonContainer.horizontalCenter;
        anchors.verticalCenter: auxButtonContainer.verticalCenter;

        color: auxButtonContainer.enabled ? Style.textColor : Style.disabledInActiveTextColor;

        font.pixelSize: auxButtonContainer.fontPixelSize;
        font.family: Style.fontFamily;
        text: auxButtonContainer.textButton;

        visible: auxButtonContainer.hasText;
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;

        hoverEnabled: true;

        cursorShape: Qt.PointingHandCursor;

        visible: auxButtonContainer.enabled;

        onClicked: {
            auxButtonContainer.clicked();
        }

        onPressed: {
            auxButtonContainer.pressed = true;
        }

        onReleased: {
            auxButtonContainer.pressed = false;
        }
    }
}
