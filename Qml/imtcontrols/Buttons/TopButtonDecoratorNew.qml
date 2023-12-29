import QtQuick 2.12
// import Acf 1.0
// import QtGraphicalEffects 1.0
// import imtgui 1.0



Item {
    id: topButtonDecorator;

    width: isEmpty ? 20 : 60;
    height: 28;

    property var baseButton;

    property string textColor: Style.textColor;
    property string textColorDisabled: "gray";
    property real fontSize: Style.fontSize_small;
    property int radius: 4;
    property bool isEmpty: baseButton.text === "";

    Rectangle{
        id: button;

        anchors.top: parent.top;
        anchors.topMargin: 6;
        anchors.left: parent.left;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width;
        height: parent.height;

        radius: topButtonDecorator.radius;

        color: baseButton.down || baseButton.checked ? Style.buttonPressedColor : baseButton.hovered ?  Style.buttonHoverColor : "transparent"

        border.width: Style.buttonBorderWidth
        border.color: baseButton.focus ? Style.borderFocusColor : "transparent"

        Image {
            id: image;

            anchors.centerIn: parent;

            height: parent.height - 8;
            width: height;

            source: baseButton.iconSource;

            sourceSize.width: width;
            sourceSize.height: height;
        }
    }

    Text {
        id: description;

        anchors.top: button.bottom;
        anchors.topMargin: 4;
        anchors.horizontalCenter: parent.horizontalCenter;

        color: topButtonDecorator.enabled ? topButtonDecorator.textColor : topButtonDecorator.textColorDisabled;
        font.pixelSize: topButtonDecorator.fontSize;

        font.family: Style.fontFamily;

        text: baseButton.text;
    }


}

