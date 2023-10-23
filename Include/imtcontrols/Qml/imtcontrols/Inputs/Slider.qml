import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: slider;

    width: decorator ? decorator.width : 0
    height: decorator ? decorator.height : 0
    rotation: orientation == Qt.Vertical ? -90 : 0;

    property Component decoratorComponent;
    property var decorator : null;

    property int backgroundWidth: width;
    property int backgroundHeight: 6;
    property string backgroundColor: "#ffc0cb";
    property string controlColor:  "#CB4154";//"#F7B6AE";

    property real backgroundOpacity: 1.;

    property real controlRecX: 0;
    property real controlWidth: height > 0 ? height :20;

    property int orientation: Qt.Horizontal;
    property real position: controlRecX/(width - controlWidth);


    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    onDecoratorComponentChanged: {
        if(!decoratorComponent){
            return;
        }
        if(decorator){
            decorator.destroy()
        }
        decorator = decoratorComponent.createObject(slider)
        decorator.baseElement = slider
        bindWidth.target = decorator
        bindHeight.target = decorator
        bindPosition.target = decorator

        controlRecX = position * (width - controlWidth)
    }

    Binding {
        id: bindWidth
        property: "width"
        value: slider.width;
    }

    Binding {
        id: bindHeight
        property: "height"
        value: slider.height;
    }

    Binding {
        id: bindPosition;
        property: "value"
        value: slider.position;
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
                //slider.controlRecX = slider.checked * (slider.backgroundWidth - slider.controlWidth);
            }
            ma.canClick = true;

            if(mouse.x - slider.controlWidth/2 <= 0){
                slider.controlRecX = 0;
            }
            else if(mouse.x - slider.controlWidth/2 >= slider.backgroundWidth - slider.controlWidth){
                slider.controlRecX = slider.backgroundWidth - slider.controlWidth;
            }
            else {
                slider.controlRecX = mouse.x - slider.controlWidth/2;
            }

        }

        onPositionChanged:  {
            if(ma.canDrag){
                ma.canClick = false;
                //console.log(mouse.x)
                if(mouse.x - slider.controlWidth/2 <= 0){
                    slider.controlRecX = 0;
                }
                else if(mouse.x - slider.controlWidth/2 >= slider.backgroundWidth - slider.controlWidth){
                    slider.controlRecX = slider.backgroundWidth - slider.controlWidth;
                }
                else {
                    slider.controlRecX = mouse.x - slider.controlWidth/2;
                }

                slider.position = slider.controlRecX/(slider.width - slider.controlWidth);
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
