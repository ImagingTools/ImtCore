import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: slider;

    width: decorator ? decorator.width : 150
    height: decorator ? decorator.height : 30

    rotation: orientation == Qt.Vertical ? -90 : 0;

    property Component decoratorComponent;
    property var decorator : null;

    property int backgroundWidth: width;
    property int backgroundHeight: 6;
    property string backgroundColor: "#ffc0cb";
    property string controlColor:  "#CB4154";

    property real backgroundOpacity: 1.;

    property real controlRecX: 0;
    property real controlWidth: height > 0 ? height :20;

    property int orientation: Qt.Horizontal;
    property real position: controlRecX/(width - controlWidth);

    property real from: 0.0;
    property real to: 1.0;
    property real value: position * (to - from) + from;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    Component.onCompleted: {
        if(value !== from){
            position = (value - from)/(to - from)
            controlRecX = position * (width - controlWidth)
        }
    }

    onDecoratorComponentChanged: {
        if(!decoratorComponent){
            return;
        }
        if(decorator){
            decorator.destroy()
        }

        var width_ = width;

        decorator = decoratorComponent.createObject(slider)
        decorator.baseElement = slider

        if(decorator.to !== undefined){
            decorator.to = to;
        }
        if(decorator.from !== undefined){
            decorator.from = from;
        }

        if(decorator.value !== undefined){
            decorator.value = position * (to - from) + from;
        }

        if(decorator.compl !== undefined){
            decorator.compl = true;
        }

        bindWidth.target = decorator
        bindHeight.target = decorator

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
            }
            ma.canClick = true;

            let x_ = mouse.x;

            if(x_ - slider.controlWidth <= 0){
                slider.controlRecX = 0;
            }
            else if(x_ - slider.controlWidth >= slider.backgroundWidth - slider.controlWidth){
                slider.controlRecX = slider.backgroundWidth - slider.controlWidth;
            }
            else {
                slider.controlRecX = x_ - slider.controlWidth;
            }

            slider.position = slider.controlRecX/(slider.backgroundWidth - slider.controlWidth);
            slider.value = slider.position * (slider.to - slider.from) + slider.from;
        }

        onPositionChanged:  {
            if(ma.canDrag){
                ma.canClick = false;

                let x_ = mouse.x;

                if(x_ - slider.controlWidth <= 0){
                    slider.controlRecX = 0;
                }
                else if(x_ - slider.controlWidth >= slider.backgroundWidth - slider.controlWidth){
                    slider.controlRecX = slider.backgroundWidth - slider.controlWidth;
                }
                else {
                    slider.controlRecX = x_ - slider.controlWidth;
                }

                slider.position = slider.controlRecX/(slider.backgroundWidth - slider.controlWidth);
                if(slider.position > 0.95){
                    slider.position = 1.
                }
                if(slider.position < 0.05){
                    slider.position = 0.
                }
                slider.value = slider.position * (slider.to - slider.from) + slider.from;
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
