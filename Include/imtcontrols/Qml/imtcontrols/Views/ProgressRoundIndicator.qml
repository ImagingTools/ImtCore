import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item{
    id: roundIndicator;

    property real percent: 0;

    property int lineWidth: 3;
    property string mainColor: "green";

    property bool hasText: true;
    property bool hasIcon: false;

    property string imageSource: "../../../" +Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal);

    property bool indeterminate: false;
    property string indetMode: "Increase";


    Component.onCompleted: {
        canvas.requestPaint();
    }

    onPercentChanged: {
        canvas.requestPaint();
    }

    onIndeterminateChanged: {
         canvas.requestPaint();
    }

    Canvas {

        id: canvas

        anchors.centerIn: parent;

        width: parent.width - 2 * roundIndicator.lineWidth;
        height: width;

        antialiasing: true;


        property real increaseVal: 0;
        property real rotationVal: 0;
        property real decreaseVal: 0;


        //property var ctx: canvas.getContext('2d');

        onWidthChanged: {
            requestPaint()
        }

        onHeightChanged: {
            requestPaint()
        }
        onIncreaseValChanged: {
            requestPaint();
        }
        onRotationValChanged: {
            requestPaint();
        }

        onDecreaseValChanged: {
            requestPaint();
        }



        onPaint: {
            var ctx = canvas.getContext('2d');
            ctx.clearRect(0, 0, canvas.width, canvas.height)

            if(!roundIndicator.indeterminate){
                drawProgress(ctx);
            }
            else if(roundIndicator.indetMode == "Increase"){
                drawIncrease(ctx);
            }
            else if(roundIndicator.indetMode == "Rotation"){
                drawRotation(ctx);
            }
            else if(roundIndicator.indetMode == "Decrease"){
                drawDecrease(ctx);
            }

        }

        function drawProgress(ctx){
            ctx.clearRect(0, 0, canvas.width, canvas.height)
            ctx.lineJoin = "round"
            ctx.lineWidth = roundIndicator.lineWidth;

            ctx.strokeStyle = roundIndicator.mainColor;
            ctx.fillStyle = roundIndicator.mainColor;

            ctx.beginPath()
            ctx.arc(canvas.width/2, canvas.height/2, canvas.width/2 - roundIndicator.lineWidth,  - Math.PI/2, (roundIndicator.percent/100) * 2 * Math.PI - Math.PI/2, false);
            ctx.stroke();
        }

        function drawIncrease(ctx){
            ctx.clearRect(0, 0, canvas.width, canvas.height)
            ctx.lineJoin = "round"
            ctx.lineWidth = roundIndicator.lineWidth;

            ctx.strokeStyle = roundIndicator.mainColor;
            ctx.fillStyle = roundIndicator.mainColor;

            ctx.beginPath()
            ctx.arc(canvas.width/2, canvas.height/2, canvas.width/2 - roundIndicator.lineWidth,  - Math.PI/2, (canvas.increaseVal/100) * 2 * Math.PI - Math.PI/2 , false);
            ctx.stroke();


        }
        function drawRotation(ctx){
            ctx.clearRect(0, 0, canvas.width, canvas.height)
            ctx.lineJoin = "round"
            ctx.lineWidth = roundIndicator.lineWidth;

            ctx.strokeStyle = roundIndicator.mainColor;
            ctx.fillStyle = roundIndicator.mainColor;

            ctx.beginPath()
            ctx.arc(canvas.width/2, canvas.height/2, canvas.width/2 - roundIndicator.lineWidth,  - Math.PI/2 + Math.PI * canvas.rotationVal / 180, (canvas.increaseVal/100) * 2 * Math.PI - Math.PI/2 + Math.PI * canvas.rotationVal / 180 , false);
            ctx.stroke();

        }

        function drawDecrease(ctx){
            ctx.clearRect(0, 0, canvas.width, canvas.height)
            ctx.lineJoin = "round"
            ctx.lineWidth = roundIndicator.lineWidth;

            ctx.strokeStyle = roundIndicator.mainColor;
            ctx.fillStyle = roundIndicator.mainColor;

            ctx.beginPath()
            ctx.arc(canvas.width/2, canvas.height/2, canvas.width/2 - roundIndicator.lineWidth, (canvas.decreaseVal/100) * 2 * Math.PI /*- Math.PI/2*/, 2 * Math.PI - Math.PI/2, false);
            ctx.stroke();

        }

    }

    Text{
        anchors.centerIn: parent;

        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_subtitle;
        color: Style.textColor;
        visible: !roundIndicator.indeterminate && roundIndicator.hasText && !roundIndicator.hasIcon && roundIndicator.percent > 0;

        text: Math.round(roundIndicator.percent) + "%";

    }

    Image{
        anchors.centerIn: parent;

        width: parent.width/2;
        height: parent.height/2;
        sourceSize.width: width;
        sourceSize.height: height;
        source: roundIndicator.imageSource;

        visible: !roundIndicator.indeterminate && roundIndicator.hasIcon && !roundIndicator.hasText;
    }
    
    Timer{
        id: timer;

        running: roundIndicator.indeterminate;
        triggeredOnStart: true;
        onTriggered: {
            if(roundIndicator.indeterminate){
                roundIndicatorAnimIncrease.start();
            }
        }
        interval: 1500;
        repeat: true;
    }


    NumberAnimation {
        id: roundIndicatorAnimIncrease;

        target: canvas;
        property: "increaseVal";
        duration: 400;
        from: 0; to: 87;
        onStarted: {
            roundIndicator.indetMode = "Increase";
        }
        onFinished: {
            if(roundIndicator.indeterminate){
                roundIndicatorAnimRotation.start();
            }

        }
    }
    NumberAnimation {
        id: roundIndicatorAnimRotation;

        target: canvas;
        property: "rotationVal";
        duration: 200;
        from: 0; to: 45;
        onStarted: {
            if(roundIndicator.indeterminate){
                roundIndicator.indetMode = "Rotation";
            }
        }
        onFinished: {
            roundIndicatorAnimDecrease.start();
        }
    }

    NumberAnimation {
        id: roundIndicatorAnimDecrease;

        target: canvas;
        property: "decreaseVal";
        duration: 400;
        from: 0; to: 75;
        onStarted: {
            roundIndicator.indetMode = "Decrease";
        }
        onFinished: {
        }
    }
}

