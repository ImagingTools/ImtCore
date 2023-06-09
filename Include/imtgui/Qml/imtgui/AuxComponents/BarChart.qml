import QtQuick 2.0

Item {

    id: barChart;

    height: 250;
    width: visibleElements ? elementWidth : 0;

    property bool visibleElements: true;
    property real maxBarHeight: 90;
    property real maxValue: 100;
    property real negativeValue;
    property real positiveValue;
    property real elementWidth:barWidth;
    property real barWidth:30;


    property string color_positive: "#ff8a3d";
    property string color_negative: "#000000";


    Rectangle{
        id: barDown;//positive

        anchors.bottom:  parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: barChart.barWidth;
        height: (barChart.positiveValue !==0)?
                    barChart.maxBarHeight/barChart.maxValue*barChart.positiveValue : 1;
        radius: width;//Math.min(width, height)
        color: barChart.color_positive;
        visible: (barChart.positiveValue == 0)? false : barChart.visibleElements;

    }

    Rectangle{
        id: barDown_topRec;//positive

        anchors.top:  barDown.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: barChart.barWidth;
        height: (barChart.positiveValue !==0)? barDown.height/2 : 1;
                    //Math.min(barChart.maxBarHeight/barChart.maxValue*barChart.positiveValue, width/2) : 1
        color: barChart.color_positive;
        visible: (barChart.positiveValue == 0)? false : (barChart.negativeValue == 0) ? false : barChart.visibleElements;

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
        visible: (barChart.negativeValue == 0)? false : barChart.visibleElements;

    }

    Rectangle{
        id: barUp_downRec;//negative

        anchors.bottom: barUp.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: barChart.barWidth;
        height: (barChart.negativeValue !==0)? barUp.height/2 : 1;
                    //Math.min(barChart.maxBarHeight/barChart.maxValue*barChart.negativeValue, width/2) : 1
        color: barChart.color_negative;
        visible: (barChart.negativeValue == 0) ? false : (barChart.positiveValue == 0) ? false : barChart.visibleElements;

    }

}

