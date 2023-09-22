import QtQuick 2.12

Rectangle {
    id: switchCustom;

    width: 72;
    height: 40;
    property int backgroundHeight: 25;
    property string backgroundColor: "#E1E1E1";
    property string controlColor: "#C4C4C4";
    property string backgroundColor_checked: "#ffc0cb";
    property string controlColor_checked: "#F7B6AE";

    property real backgroundOpacity: 1.;

    property bool checked: false;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    function setChecked(checkedVal){
        switchCustom.checked = checkedVal;
        controlRec.x = switchCustom.checked * (switchCustom.width - controlRec.width);
    }

    Rectangle{
        id: backgroundRec;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        height: switchCustom.backgroundHeight;
        color: switchCustom.checked ? switchCustom.backgroundColor_checked :
                                      switchCustom.backgroundColor;
        radius: height;

        opacity: switchCustom.backgroundOpacity;

    }

    Rectangle{
        id: controlRec;

        anchors.verticalCenter: parent.verticalCenter;

        x: 0;//switchCustom.checked ? switchCustom.width - width : 0;
        width: switchCustom.height;
        height: width;
        radius: width;
        color: switchCustom.checked ? switchCustom.controlColor_checked :
                                      switchCustom.controlColor;
    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        property int mouseX_prev: 0;
        property bool canDrag: false;
        property bool canClick: true;
//        onClicked: {

//        }
        onPressed: {
//            ma.canDrag = true;
            if(tooltip.text !== ""){
                tooltip.closeTooltip();
            }

        }
        onReleased: {
            ma.canDrag = false;
            if(ma.canClick){
                switchCustom.checked = !switchCustom.checked;
                controlRec.x = switchCustom.checked * (switchCustom.width - controlRec.width);
            }
            ma.canClick = true;
        }
        onPositionChanged:  {
            if(ma.canDrag){
                ma.canClick = false;
                //console.log(mouse.x)
                if(mouse.x <= 0){
                    controlRec.x = 0;
                    switchCustom.checked = false;
                }
                else if(mouse.x >= switchCustom.width - controlRec.width){
                    controlRec.x = switchCustom.width - controlRec.width;
                    switchCustom.checked = true;
                }
                else{
                    controlRec.x = mouse.x;
                }
            }

        }

        onEntered: {
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                pauseTooltip.start();

            }

        }

        onExited: {
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                tooltip.closeTooltip();
            }
        }
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
