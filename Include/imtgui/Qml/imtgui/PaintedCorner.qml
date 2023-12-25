import QtQuick 2.0


Rectangle {
    id: paintedCorner;

    width: 20;
    height: 20;
    color: "transparent";
    property real r: 20;
    property string mainColor: "red";
    property string lineColor: "red";
    property real lineWidth: 1;

    onMainColorChanged: {
        canvas.requestPaint();
    }


    Canvas {
        id:canvas;

        anchors.centerIn: parent;
        width: parent.width;
        height: parent.height;
        property color strokeStyle: paintedCorner.lineColor;
        property color fillStyle: paintedCorner.mainColor;
        property real lineWidth: paintedCorner.lineWidth;

        onPaint: {
            var ctx = canvas.getContext('2d');
            ctx.lineCap = "butt";
            ctx.lineJoin = "bevel";
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.strokeStyle = canvas.strokeStyle;
            ctx.fillStyle = canvas.fillStyle;
            ctx.lineWidth = canvas.lineWidth;

            ctx.beginPath();

            //            ctx.arcTo(0, canvas.height, canvas.width, 0, 5);

            ctx.arc(canvas.width,  0,  r, 0, 90*Math.PI/180, true);
            //                ctx.strokeStyle = canvas.strokeStyle;
            ctx.lineTo(0, canvas.height);
            ctx.lineTo(0, 0);
            ctx.stroke();
            ctx.fill();
            ctx.closePath();

        }

    }

}

