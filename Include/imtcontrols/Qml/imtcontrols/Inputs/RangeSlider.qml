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

    property real controlWidth: height > 0 ? height :20;

    property int orientation: Qt.Horizontal;

    property real from: 0.0;
    property real to: 1.0;

    property real controlRecXFirst: 0;
    property real positionFirst: controlRecXFirst/(width - controlWidth);
    property real valueFirst: positionFirst * (to - from) + from;

    property real controlRecXSecond: controlWidth;
    property real positionSecond: controlRecXSecond/(width - controlWidth);
    property real valueSecond: positionSecond * (to - from) + from;


    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;


    Component.onCompleted: {
        positionFirst = (valueFirst - from)/(to - from)
        controlRecXFirst = positionFirst * (width - controlWidth)

        positionSecond = (valueSecond - from)/(to - from)
        controlRecXSecond = positionSecond * (width - controlWidth)

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

        if(decorator.first !== undefined && decorator.second !== undefined){
            var firstVal = slider.valueFirst;
            var secondVal = slider.valueSecond;
            console.log(firstVal, secondVal)
            decorator.setValues(firstVal, secondVal);
        }


        bindWidth.target = decorator
        bindHeight.target = decorator

        controlRecXFirst = positionFirst * (width - controlWidth)
        controlRecXSecond = positionSecond * (width - controlWidth)

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
        property bool canMoveFirst: false;
        property bool canMoveSecond: false;

        onPressed: {
            ma.canDrag = true;
            if(tooltip.text !== ""){
                tooltip.closeTooltip();
            }

            if(mouse.x >= slider.controlRecXFirst && mouse.x <= slider.controlRecXFirst + slider.controlWidth){
                ma.canMoveFirst = true;
            }
            else if(mouse.x >= slider.controlRecXSecond && mouse.x <= slider.controlRecXSecond + slider.controlWidth){
                ma.canMoveSecond = true;
            }

        }
        onReleased: {
            ma.canDrag = false;
            ma.canMoveFirst = false;
            ma.canMoveSecond = false;
            if(ma.canClick){
            }
            ma.canClick = true;

            //            if(mouse.x - slider.controlWidth <= 0){
            //                slider.controlRecXFirst = 0;
            //            }
            //            else if(mouse.x - slider.controlWidth >= slider.backgroundWidth - slider.controlWidth){
            //                slider.controlRecXFirst = slider.backgroundWidth - slider.controlWidth;
            //            }
            //            else {
            //                slider.controlRecXFirst = mouse.x - slider.controlWidth;
            //            }

            //            slider.positionFirst = slider.controlRecXFirst/(slider.backgroundWidth - slider.controlWidth);

        }

        onPositionChanged:  {
            if(ma.canDrag){
                ma.canClick = false;
                if(mouse.x <= slider.controlRecXSecond && ma.canMoveFirst){//first
                    if(mouse.x <= 0){
                        slider.controlRecXFirst = 0;
                    }
                    else if(mouse.x >= slider.controlRecXSecond - slider.controlWidth){
                        slider.controlRecXFirst = slider.controlRecXSecond - slider.controlWidth;
                    }
                    else {
                        slider.controlRecXFirst = mouse.x;
                    }

                    //                    if(slider.controlRecXSecond - slider.controlRecXFirst < slider.controlWidth/2){
                    //                        slider.controlRecXFirst = slider.controlRecXSecond - slider.controlWidth;
                    //                    }

                    slider.positionFirst = slider.controlRecXFirst/(slider.backgroundWidth - slider.controlWidth);
                    if(slider.positionFirst < 0.5){
                        slider.positionFirst = 0.
                    }
                }

                else if(ma.canMoveSecond){//second
                    if(mouse.x - slider.controlWidth <= slider.controlRecXFirst + slider.controlWidth){
                        slider.controlRecXSecond = slider.controlRecXFirst + slider.controlWidth;
                    }
                    else if(mouse.x - slider.controlWidth >= slider.backgroundWidth - slider.controlWidth){
                        slider.controlRecXSecond = slider.backgroundWidth - slider.controlWidth;
                    }
                    else {
                        slider.controlRecXSecond = mouse.x - slider.controlWidth;
                    }

                    slider.positionSecond = slider.controlRecXSecond/(slider.backgroundWidth - slider.controlWidth);
                    if(slider.positionSecond > 0.95){
                        slider.positionSecond = 1.
                    }

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
