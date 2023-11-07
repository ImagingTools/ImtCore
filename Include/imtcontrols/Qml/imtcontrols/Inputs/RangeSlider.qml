import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: slider;

    rotation: orientation == Qt.Vertical ? -90 : 0;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.rangeSliderDecorator: Decorators.rangeSliderDecorator;

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

    function decoratorChangedFunc(){
        console.log("RangeSlider: redefinition of base function");
    }

    onDecoratorComponentChanged: {
        decoratorChangedFuncBase();

        if(decorator.to !== undefined){
            decorator.to = to;
        }
        if(decorator.from !== undefined){
            decorator.from = from;
        }

        if(decorator.first !== undefined && decorator.second !== undefined){
            var firstVal = positionFirst * (to - from) + from;//slider.valueFirst;
            var secondVal = positionSecond * (to - from) + from;//slider.valueSecond;
            decorator.setValues(firstVal, secondVal);
        }
        if(decorator.compl !== undefined){
            decorator.compl = true;
        }

        setBindTargets();

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

            let x_ = mouse.x;
            let isIntersection = slider.controlRecXFirst + slider.controlWidth > slider.controlRecXSecond;

            if(isIntersection && !ma.canMoveFirst && !ma.canMoveSecond){
                ma.canMoveFirst = true;
            }

            //in first handle
            if(x_ >= slider.controlRecXFirst && x_ <= slider.controlRecXFirst + slider.controlWidth){
                if(!isIntersection){
                    ma.canMoveFirst = true;
                    ma.canMoveSecond = false
                }
            }
            //in second handle
            else if(x_ >= slider.controlRecXSecond && x_ <= slider.controlRecXSecond + slider.controlWidth){
                if(!isIntersection){
                    ma.canMoveFirst = false;
                    ma.canMoveSecond = true;
                }
            }
            //to the left of the first handle
            else if(x_ < slider.controlRecXFirst){
                slider.controlRecXFirst = x_;

                slider.positionFirst = slider.controlRecXFirst/(slider.backgroundWidth - slider.controlWidth);
                slider.valueFirst = slider.positionFirst * (slider.to - slider.from) + slider.from;
                ma.canMoveFirst = true;
                ma.canMoveSecond = false
            }
            //to the right of the second handle
            else if(x_ > slider.controlRecXSecond){
                if(x_ >= slider.backgroundWidth - slider.controlWidth){
                    slider.controlRecXSecond = slider.backgroundWidth - slider.controlWidth;
                }
                else {
                    slider.controlRecXSecond = x_;
                }

                slider.positionSecond = slider.controlRecXSecond/(slider.backgroundWidth - slider.controlWidth);
                slider.valueSecond = slider.positionSecond * (slider.to - slider.from) + slider.from;

                ma.canMoveFirst = false;
                ma.canMoveSecond = true;
            }
            //between the handles
            else {
                if(x_ - (slider.controlRecXFirst + slider.controlWidth) <= slider.controlRecXSecond - x_){
                    slider.controlRecXFirst = x_ - slider.controlWidth;

                    slider.positionFirst = slider.controlRecXFirst/(slider.backgroundWidth - slider.controlWidth);
                    slider.valueFirst = slider.positionFirst * (slider.to - slider.from) + slider.from;

                    ma.canMoveFirst = true;
                    ma.canMoveSecond = false
                }
                else {
                    slider.controlRecXSecond = x_;

                    slider.positionSecond = slider.controlRecXSecond/(slider.backgroundWidth - slider.controlWidth);
                    slider.valueSecond = slider.positionSecond * (slider.to - slider.from) + slider.from;

                    ma.canMoveFirst = false;
                    ma.canMoveSecond = true;
                }

            }


        }
        onReleased: {
            ma.canDrag = false;

            let isIntersection = slider.controlRecXFirst + slider.controlWidth > slider.controlRecXSecond;
            if(!isIntersection){
                ma.canMoveFirst = false;
                ma.canMoveSecond = false;
            }
            if(ma.canClick){
            }
            ma.canClick = true;

        }

        onPositionChanged:  {
            if(ma.canDrag){
                ma.canClick = false;

                let add = -slider.controlWidth/2;
                let x_ = mouse.x + add;

                if(ma.canMoveFirst){//first
                    if(x_ <= 0){
                        slider.controlRecXFirst = 0;
                    }
                    else if(x_ >= slider.controlRecXSecond){
                        slider.controlRecXFirst = slider.controlRecXSecond;
                    }
                    else {
                        slider.controlRecXFirst = x_;
                    }

                    if(slider.controlRecXSecond - slider.controlRecXFirst < slider.controlWidth/2){
                        slider.controlRecXFirst = slider.controlRecXSecond;
                    }

                    slider.positionFirst = slider.controlRecXFirst/(slider.backgroundWidth - slider.controlWidth);
                    if(slider.positionFirst < 0.05){
                        slider.positionFirst = 0.
                    }
                    slider.valueFirst = slider.positionFirst * (slider.to - slider.from) + slider.from;

                }

                else if(ma.canMoveSecond){//second
                    if(x_ <= slider.controlRecXFirst){
                        slider.controlRecXSecond = slider.controlRecXFirst;
                    }
                    else if(x_  >= slider.backgroundWidth - slider.controlWidth){
                        slider.controlRecXSecond = slider.backgroundWidth - slider.controlWidth;
                    }
                    else {
                        slider.controlRecXSecond = x_;
                    }

                    slider.positionSecond = slider.controlRecXSecond/(slider.backgroundWidth - slider.controlWidth);
                    if(slider.positionSecond > 0.95){
                        slider.positionSecond = 1.
                    }
                    slider.valueSecond = slider.positionSecond * (slider.to - slider.from) + slider.from;

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
