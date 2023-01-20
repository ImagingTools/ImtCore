import QtQuick 2.0

Rectangle {
    id: roundButton;

    width: 60;
    height: width;
    radius: width;
    color: "#F18C16";
    clip: true;
    property string imageSource: "../../Icons/whiteCross.png";

    property bool containsMouse: false;

    property bool mouseOnButton: ma.containsMouse;

    property real imageDecrease: 2;
    property real imageDecrease_horizontal: imageDecrease;
    property real imageDecrease_vertical: imageDecrease;
    property int verticalOffset: 0;
    property bool enabled:  true;

    signal clicked();

    Image {
        id: image;

        anchors.centerIn:  parent;
        anchors.verticalCenterOffset: roundButton.verticalOffset;

        width: roundButton.width/roundButton.imageDecrease_horizontal/2*2;
        height: roundButton.height/roundButton.imageDecrease_vertical/2*2;
        sourceSize.width:  width;
        sourceSize.height: height;

        source: roundButton.imageSource;
        //fillMode: Image.PreserveAspectFit;

    }
    MouseArea{
        id: ma;

        anchors.fill: parent;

        visible: parent.enabled && parent.visible;
        enabled: visible;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        onClicked: {roundButton.clicked()}
        onPressed: {roundButton.scale = 0.95}
        onReleased: {roundButton.scale = 1}
        onEntered: {roundButton.containsMouse = true}
        onExited: {roundButton.containsMouse = false}
        onDoubleClicked: {}
    }

}
