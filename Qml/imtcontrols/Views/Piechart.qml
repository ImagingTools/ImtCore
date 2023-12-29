import QtQuick 2.0

import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


Item {
    id: pieChart;

    width: 200;
    height: 200;

    property real percent: 50;
    property string pieColor: "#4682B4";
    property string pieColor_second:  "#ff8a3d"; //"black"
    property bool isRing: true;
    property bool clockwise: false;
    property bool visibleText: false;
    property real rotationAngle: 0;
    property int fontSize: 20;
    property string fontFamily: Style.fontFamilyBold;
    property bool fontBold: false;
    property real textPositionCoeff: 0.6;
    property real ringSizeCoeff: 0.6;
    property alias textPositive: positive.text;
    property alias textNegative: negative.text;

    onPercentChanged: {canvas.requestPaint();}
    //visible: (percent===0) ? false: true

    Rectangle{
        id: whiteCircle;

        anchors.centerIn: parent;

        visible: pieChart.isRing;
        radius : parent.width * pieChart.ringSizeCoeff;
        height: parent.height * pieChart.ringSizeCoeff;
        width:  parent.width * pieChart.ringSizeCoeff;
        color: "white";
        z:1;

    }

    Canvas {
        id: canvas;

        anchors.centerIn: parent;

        width: parent.width;
        height: parent.height;
        rotation: pieChart.rotationAngle;
        property color strokeStyle: pieChart.pieColor; //"#ff8a3d"
        property color fillStyle: pieChart.pieColor; //"#ff8a3d"
        property real lineWidth: 1;


        onPaint: {
            var ctx = canvas.getContext('2d');
            ctx.lineCap = "butt";
            ctx.lineJoin = "bevel";
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.strokeStyle = canvas.strokeStyle;
            ctx.fillStyle = canvas.fillStyle;
            ctx.lineWidth = canvas.lineWidth;

            var r = canvas.width/2 - canvas.lineWidth*2;

            if ((pieChart.percent != 100)&&(pieChart.percent != 0))
            {
                if(! pieChart.clockwise){
                    ctx.beginPath();
                    ctx.arc(canvas.width/2,  canvas.height/2, r, 0, - pieChart.percent * 3.6 * Math.PI/180, true);
                    ctx.strokeStyle = canvas.strokeStyle;
                    ctx.lineTo(canvas.width/2, canvas.height/2)
                    ctx.lineTo(canvas.width - canvas.lineWidth*2, canvas.height/2)
                    ctx.stroke();
                    ctx.fill();
                    ctx.closePath();


                    ctx.beginPath();
                    ctx.strokeStyle = pieChart.pieColor_second; //"black";
                    ctx.fillStyle = pieChart.pieColor_second; //"black"
                    ctx.arc(canvas.width/2,  canvas.height/2, r, 2*Math.PI,  (100 - pieChart.percent) * 3.6 * Math.PI/180, false);

                    ctx.lineTo(canvas.width/2, canvas.height/2);
                    ctx.lineTo(canvas.width - canvas.lineWidth*2, canvas.height/2);
                    ctx.stroke();
                    ctx.fill();
                    ctx.closePath();
                }

                else{

                    ctx.beginPath();
                    ctx.arc(canvas.width/2,  canvas.height/2, r, 2*Math.PI,  pieChart.percent * 3.6 * Math.PI/180, false);
                    ctx.strokeStyle = canvas.strokeStyle;
                    ctx.lineTo(canvas.width/2, canvas.height/2);
                    ctx.lineTo(canvas.width - canvas.lineWidth*2, canvas.height/2);
                    ctx.stroke();
                    ctx.fill();
                    ctx.closePath();


                    ctx.beginPath();
                    ctx.strokeStyle = pieChart.pieColor_second; //"black";
                    ctx.fillStyle = pieChart.pieColor_second; //"black"
                    ctx.arc(canvas.width/2,  canvas.height/2, r, 0, - (100 - pieChart.percent) * 3.6 * Math.PI/180, true);

                    ctx.lineTo(canvas.width/2, canvas.height/2);
                    ctx.lineTo(canvas.width - canvas.lineWidth*2, canvas.height/2);
                    ctx.stroke();
                    ctx.fill();
                    ctx.closePath();

                }

            } else if (pieChart.percent ==0)
            {

                ctx.beginPath();
                ctx.strokeStyle = pieChart.pieColor_second; //"black";
                ctx.fillStyle = pieChart.pieColor_second; //"black"
                ctx.arc(canvas.width/2,  canvas.height/2, r, 0,  2*Math.PI, true);
                ctx.stroke();
                ctx.fill();
                ctx.closePath();

            } else if (pieChart.percent ==100)

            {
                ctx.beginPath();
                ctx.strokeStyle = canvas.strokeStyle;
                ctx.fillStyle = canvas.fillStyle;
                ctx.arc(canvas.width/2,  canvas.height/2, r, 0,  2*Math.PI, true);
                ctx.stroke();
                ctx.fill();
                ctx.closePath();

            }

        }


    }

    ////////////text///////////////

    Text {
        id: positive;

        visible: ((pieChart.percent/100 * 360)/2 * Math.PI/180 !==0) && ((pieChart.percent/100 * 360)/2 * Math.PI/180 !== Math.PI)  && pieChart.visibleText;
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        font.pixelSize: pieChart.fontSize;
        font.bold: pieChart.fontBold;
        font.family: pieChart.fontFamily;
        color: "black";

        text: Math.floor(pieChart.percent);

        property real sign: pieChart.clockwise ? -1 : 1;
        property real r: parent.width/2;
        property real angle: (pieChart.percent/100 * 360)/2 * Math.PI/180 - positive.sign * pieChart.rotationAngle * Math.PI/180;  //от 0 до 180 градусов
        property real shiftX: positive.r - positive.width/2;
        property real shiftY: positive.r - positive.height/2;
        property real shiftR: positive.r * pieChart.textPositionCoeff;

        property real x_anticlockwise:  (positive.shiftX + positive.shiftR * Math.cos(positive.angle));
        property real y_anticlockwise : (positive.shiftY - positive.shiftR * Math.sin(positive.angle));
        property real x_clockwise: (positive.shiftX + positive.shiftR * Math.cos(positive.angle));
        property real y_clockwise: (positive.shiftY + positive.shiftR * Math.sin(positive.angle));



        x: pieChart.clockwise ? positive.x_clockwise : positive.x_anticlockwise;
        y: pieChart.clockwise ? positive.y_clockwise : positive.y_anticlockwise;

    }

    Text {
        id: negative;

        visible: (((100 - pieChart.percent)/100 * 360)/2 * Math.PI/180 !==0) && (((100 - pieChart.percent)/100 * 360)/2 * Math.PI/180 !== Math.PI) && pieChart.visibleText;
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        color: "black";//"red"
        font.pixelSize: pieChart.fontSize;
        font.bold: pieChart.fontBold;
        font.family: pieChart.fontFamily;

        text: (100 - Math.floor(pieChart.percent));

        property real sign: pieChart.clockwise ? -1 : 1;
        property real r: parent.width/2;
        property real angle: ((100 - pieChart.percent)/100 * 360)/2 * Math.PI/180 + negative.sign * pieChart.rotationAngle * Math.PI/180;   //от 0 до 180 градусов
        property real shiftX: negative.r - negative.width/2;
        property real shiftY: negative.r - negative.height/2;
        property real shiftR: negative.r * pieChart.textPositionCoeff;


        property real x_anticlockwise: (negative.shiftX + negative.shiftR * Math.cos(negative.angle));
        property real y_anticlockwise: (negative.shiftY + negative.shiftR * Math.sin(negative.angle));

        property real x_clockwise:  (negative.shiftX + negative.shiftR * Math.cos(negative.angle));
        property real y_clockwise: (negative.shiftY - negative.shiftR * Math.sin(negative.angle));

        x: pieChart.clockwise ? negative.x_clockwise : negative.x_anticlockwise;
        y: pieChart.clockwise ? negative.y_clockwise : negative.y_anticlockwise;



    }
//
    Text {
        id: positive_abs;

        anchors.centerIn: parent;

        visible: pieChart.percent == 100  && pieChart.visibleText;
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        font.pixelSize: pieChart.fontSize;
        font.bold: pieChart.fontBold;
        font.family: pieChart.fontFamily;
        color: "black";

        text: positive.text;

    }

    Text {
        id: negative_abs;

        anchors.centerIn: parent;

        visible: pieChart.percent == 0  && pieChart.visibleText;
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        color: "black";//"red"
        font.pixelSize: pieChart.fontSize;
        font.bold: pieChart.fontBold;
        font.family: pieChart.fontFamily;

        text: negative.text;

    }

}



//NumberAnimation {
//    id: anim
//    target: piechart
//    property: "percent"
//    duration: 2000
//    from: 0; to:100
//    easing.type: Easing.InOutQuad
//    onStopped: piechart.percent = 0//start()
//}

//AUX.Piechart{
//    id: piechart
//    anchors.centerIn: parent
//    visible: true
//    percent: 0
//    isRing: true
//    visibleText: true
//    textPositionCoeff: 0.8
//    clockwise: true
//    rotationAngle: -90
//    MouseArea{anchors.fill: parent
//        hoverEnabled: true
//        cursorShape:containsMouse ? Qt.PointingHandCursor :Qt.ArrowCursor
//        onClicked: anim.start()
//    }

//}



