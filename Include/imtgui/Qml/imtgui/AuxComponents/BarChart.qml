import QtQuick 2.12
import Acf 1.0

Item {

    id: barChart;

    height: 250;
    width: visibleElements ? elementWidth : 0;

    property bool visibleElements: true;
    property real maxBarHeight: 90;
    property real maxValue: 100;
    property real negativeValue;
    property real positiveValue;
    property real elementWidth: barWidth;
    property real barWidth:30;

    property int addToValue: 30;


    property string color_positive: "#ff8a3d";
    property string color_negative: "#000000";

    property bool hasTooltip: true;
    property bool isPositiveTooltip: true;
    property bool isNegativeTooltip: false;

    property alias toolTipColor: tooltip.color;
    property alias toolTipFontColor: tooltip.fontColor;
    property alias toolTipFontSize: tooltip.fontPixelSize;

    property real parentY: 0;

    onParentYChanged: {
        if(tooltip.openST){
            tooltip.closeTooltip();
        }
    }

    //Rectangle{anchors.fill: parent;color: "red"; opacity: 0.2}



    Rectangle{
        id: barDown;//positive

        anchors.bottom:  parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: barChart.barWidth;
        height: (barChart.positiveValue !==0)?
                    barChart.maxBarHeight/barChart.maxValue*barChart.positiveValue : 1;
        radius: width;//Math.min(width, height)
        color: barChart.color_positive;
        visible: !barChart.visible ? false : (barChart.positiveValue == 0)? false : barChart.visibleElements;

    }

    Rectangle{
        id: barDown_topRec;//positive

        anchors.top:  barDown.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: barChart.barWidth;
        height: (barChart.positiveValue !==0)? barDown.height/2 : 1;
                    //Math.min(barChart.maxBarHeight/barChart.maxValue*barChart.positiveValue, width/2) : 1
        color: barChart.color_positive;
        visible: !barChart.visible ? false : (barChart.positiveValue == 0)? false : (barChart.negativeValue == 0) ? false : barChart.visibleElements;

    }

    Rectangle{
        id: barUp;//negative

        anchors.bottom: barDown.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: barChart.barWidth;
        height: (barChart.negativeValue !==0)?
                    barChart.maxBarHeight/barChart.maxValue*barChart.negativeValue : 1;
        radius: width//Math.min(width, height)
        color: barChart.color_negative;
        visible: !barChart.visible ? false : (barChart.negativeValue == 0)? false : barChart.visibleElements;

    }

    Rectangle{
        id: barUp_downRec;//negative

        anchors.bottom: barUp.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: barChart.barWidth;
        height: (barChart.negativeValue !==0)? barUp.height/2 : 1;
                    //Math.min(barChart.maxBarHeight/barChart.maxValue*barChart.negativeValue, width/2) : 1
        color: barChart.color_negative;
        visible: !barChart.visible ? false : (barChart.negativeValue == 0) ? false : (barChart.positiveValue == 0) ? false : barChart.visibleElements;

    }

    CustomTooltip{
        id: tooltip;

        fitToTextWidth: true;

        componentHeight: 30;
        fontPixelSize:  Style.fontSize_common;

        color: Style.color_buttonText;
        fontColor: "#ffffff";

        borderColor: Style.color_elementBorder;
        property string shownValPos: Math.trunc((barChart.positiveValue + barChart.addToValue)*100)/100;
        property string shownValNeg: Math.trunc((barChart.negativeValue + barChart.addToValue)*100)/100;
        property string shownVal: barChart.isPositiveTooltip ? shownValPos : barChart.isNegativeTooltip ? shownValNeg : "";

        text: shownVal;
    }

    PauseAnimation {
        id: pauseTooltip;

        duration: tooltip.waitingDuration;
        onFinished: {
            //tooltip.openTooltip(ma.mouseX, ma.mouseY);
            tooltip.openTooltipWithCoord(barChart.barWidth/2 - tooltip.tooltipWidth/2,  barDown.y - tooltip.componentHeight - 5);

        }
    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        visible: true;
        hoverEnabled: visible;
        cursorShape: Qt.PointingHandCursor;

        onEntered: {
            if(barChart.hasTooltip){
                if(tooltip.text !== ""){
                    pauseTooltip.stop();
                    pauseTooltip.start();

                }
            }
        }

        onExited: {
            if(barChart.hasTooltip){
                if(tooltip.text !== ""){
                    pauseTooltip.stop();
                    tooltip.closeTooltip();
                }
            }
        }

    }


}

