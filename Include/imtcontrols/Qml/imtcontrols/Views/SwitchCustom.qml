import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: switchCustom;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.switchDecorator : Decorators.switchDecorator;

    property string text: "";

    property int backgroundWidth: decorator ? decorator.backgroundWidth : 0;
    property int backgroundHeight: 14;
    property string backgroundColor: "#E1E1E1";
    property string controlColor: "#C4C4C4";
    property string backgroundColor_checked: "#ffc0cb";
    property string controlColor_checked: "#F7B6AE";

    property real backgroundOpacity: 1.;

    property bool checked: false;

    property real controlRecX: 0;
    property real controlWidth: height > 0 ? height :20;

    property bool isLeftText: false;
    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;

    property int mainMargin: 8;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    onDecoratorComponentChanged: {
        setChecked(switchCustom.checked)
    }

    function setChecked(checkedVal){
        switchCustom.checked = checkedVal;
        switchCustom.controlRecX = switchCustom.checked * (switchCustom.backgroundWidth - switchCustom.controlWidth);
    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        property int mouseX_prev: 0;
        property bool canDrag: false;
        property bool canClick: true;

        onPressed: {
            ma.canDrag = true;
            if(tooltip.text !== ""){
                tooltip.closeTooltip();
            }

        }
        onReleased: {
            ma.canDrag = false;
            if(ma.canClick){
                switchCustom.checked = !switchCustom.checked;
                switchCustom.controlRecX = switchCustom.checked * (switchCustom.backgroundWidth - switchCustom.controlWidth);
            }
            ma.canClick = true;

            if(switchCustom.controlRecX >= switchCustom.backgroundWidth/2 - switchCustom.controlWidth/2){
                switchCustom.controlRecX = switchCustom.backgroundWidth - switchCustom.controlWidth;
                switchCustom.checked = true;

            }
            else {
                switchCustom.controlRecX = 0;
                switchCustom.checked = false;
            }


        }
        onPositionChanged:  {
            if(ma.canDrag){
                ma.canClick = false;
                //console.log(mouse.x)
                if(mouse.x <= 0){
                    switchCustom.controlRecX = 0;
                    switchCustom.checked = false;
                }
                else if(mouse.x >= switchCustom.backgroundWidth - switchCustom.controlWidth){
                    switchCustom.controlRecX = switchCustom.backgroundWidth - switchCustom.controlWidth;
                    switchCustom.checked = true;
                }
                else {
                    switchCustom.controlRecX = mouse.x;
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
