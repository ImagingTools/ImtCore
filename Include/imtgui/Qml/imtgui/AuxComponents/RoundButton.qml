import QtQuick 2.12

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

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

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
        onClicked: {
            roundButton.clicked();
        }
        onPressed: {
            roundButton.scale = 0.95;
            tooltip.closeTooltip();

        }
        onReleased: {
            roundButton.scale = 1;
        }
        onEntered: {
            roundButton.containsMouse = true;
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                pauseTooltip.start();

            }
        }
        onExited: {
            roundButton.containsMouse = false;
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                tooltip.closeTooltip();
            }
        }
        onDoubleClicked: {}
    }

    CustomTooltip{
        id: tooltip;
    }

    PauseAnimation {
        id: pauseTooltip;

        duration: tooltip.waitingDuration;
        onFinished: {
            tooltip.openTooltip(ma.mouseX, ma.mouseY);
        }
    }

}
