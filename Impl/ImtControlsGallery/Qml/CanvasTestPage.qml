import QtQuick 2.12
import imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: canvasPage;

    anchors.fill: parent;

    MovingArea{
        id: moving;

        anchors.fill: parent;

        onClicked: {
            if(!wasMoving){
                if(insideMovingItem){
                    canvas.filled = !canvas.filled;
                }
                else {
                    canvas.filled = false;
                }
                canvas.requestPaint();
            }
        }


        onWheelDeltaSignal: {
            if(wheelDelta > 0){//up
                canvas.scale += scaleStep
            }
            else{//down
                if(canvas.scale > scaleStep){
                    canvas.scale -= scaleStep
                }
            }
        }

        function movingFunction(delta){
            let withinBorders_ = withinBorders(delta);

            if(insideMovingItem && withinBorders_){
                canvas.mainRec_x += delta.x;
                canvas.mainRec_y += delta.y;
                canvas.requestPaint();
            }
        }

        function checkInsideMovingItem(){
            let ok = false

            ok = moving.coordPressed.x >= canvas.mainRec_x
                    && moving.coordPressed.y >= canvas.mainRec_y
                    && moving.coordPressed.x <= canvas.mainRec_x + canvas.mainRec_width
                    && moving.coordPressed.y <= canvas.mainRec_y + canvas.mainRec_height

            return ok
        }

        function withinBorders(delta){
            let ok = false
            ok =
                    canvas.mainRec_x + delta.x > 10
                    && canvas.mainRec_y + delta.y > 10
                    && canvas.mainRec_x + delta.x < moving.width - canvas.mainRec_width - 10
                    && canvas.mainRec_y + delta.y < moving.height - canvas.mainRec_height - 10

            return ok
        }
    }

    Canvas {

        id: canvas

        anchors.fill: parent

        scale: 1

        antialiasing: true

        property color strokeStyle:  "#ff6600"
        property color fillStyle: "#ff6600"
        property int lineWidth: 2
        property bool fill: true
        property bool stroke: true

        //variants
        property real mainRec_width: 350;
        property real mainRec_height: 60;
        property real mainRec_x: canvas.width/2 - mainRec_width/2;
        property real mainRec_y: canvas.height/2 - mainRec_height/2;
        property int radius_: 4;
        property string fillColor: filled ? "#90EE90" : "#ffffff";
        property bool filled: false;
        //variants

        onWidthChanged: {
            requestPaint()
        }

        onHeightChanged: {
            requestPaint()
        }


        onPaint: {
            var ctx = canvas.getContext('2d');
            ctx.lineCap = "round"
            ctx.lineJoin = "round"
            ctx.clearRect(0, 0, canvas.width, canvas.height)
            ctx.strokeStyle = "#000000"
            ctx.lineWidth = canvas.lineWidth;

            //main rectangle
            ctx.roundedRect(mainRec_x, mainRec_y, mainRec_width, mainRec_height, radius_, radius_);
            ctx.stroke();


            //inner rectangle
            let borderShift = 6;
            ctx.strokeStyle = "#ff6600"
            ctx.fillStyle = canvas.fillColor;
            ctx.beginPath()
            ctx.roundedRect(mainRec_x + borderShift, mainRec_y + borderShift, mainRec_width - 2 * borderShift, mainRec_height - 2 * borderShift, radius_, radius_);
            ctx.fill();
            ctx.stroke();

            //text
            ctx.strokeStyle = "#000000"
            ctx.fillStyle = "#000000"
            ctx.lineWidth = 1;
            ctx.font = "20px sans-serif";
            ctx.beginPath()
            let textStr = "text"
            let textWidth = ctx.measureText("text").width
            ctx.fillText(textStr, mainRec_x + mainRec_width/2 + borderShift - textWidth/2, mainRec_y + mainRec_height/2  + borderShift);
            ctx.strokeText(textStr, mainRec_x + mainRec_width/2 + borderShift - textWidth/2, mainRec_y + mainRec_height/2  + borderShift);

        }

    }


}
