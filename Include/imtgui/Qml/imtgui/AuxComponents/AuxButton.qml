import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: auxButtonContainer;

    radius: 2;

    border.width: 1;
    border.color: highlighted ? Style.iconColorOnSelected : borderColor;

    color: !enabled ? Style.inactive_buttonColor : auxButtonContainer.pressed && auxButtonContainer.highlighted ? Style.hover :
                                        auxButtonContainer.highlighted ? Style.selectedColor :
                                                                         backgroundColor;

    property bool highlighted: ma.containsMouse;
    property bool hasIcon: auxButtonContainer.iconSource !== "";
    property bool hasText: false;
    property bool fontBold: false;
    property bool borderExist: false;
    property bool enabled: true;
    property bool pressed: false;

    property string borderColor: "transparent";
    property string backgroundColor: "transparent";
    property var textButton;

    property int iconWidth: auxButtonContainer.width - 3;
    property int iconHeight: auxButtonContainer.height - 3;
    property int fontPixelSize: Style.fontSize_common;

    property alias iconSource: image.source;
    property alias fontColor: text.color;

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

        height: auxButtonContainer.iconHeight;
        width: auxButtonContainer.iconWidth;

        sourceSize.width: width;
        sourceSize.height: height;

        visible: auxButtonContainer.hasIcon;
    }

    Text {
        id: text;

        anchors.horizontalCenter: auxButtonContainer.horizontalCenter;
        anchors.verticalCenter: auxButtonContainer.verticalCenter;

        color: auxButtonContainer.enabled ? Style.buttonText : Style.inactive_buttonText;

        font.pixelSize: auxButtonContainer.fontPixelSize;
        font.family: Style.fontFamily;
        font.bold: auxButtonContainer.fontBold;
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
