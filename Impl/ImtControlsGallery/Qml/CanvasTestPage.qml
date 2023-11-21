import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: canvasPage;

    anchors.fill: parent;

    Rectangle{
        id: mainContainer;

        anchors.centerIn: parent;

        width: parent.width * canvas.scaleCoeff;
        height:  parent.height * canvas.scaleCoeff;
        color: "transparent";
        border.color: "lightgrey";
        border.width: 2;
        radius: 4;

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

            onPressed: {
                insideMovingItem = checkInsideMovingItem(canvas.mainRec_x, canvas.mainRec_y, canvas.mainRec_width, canvas.mainRec_height);
            }

            onWheelDeltaSignal: {
                if(wheelDelta > 0){//up
                    canvas.scaleCoeff += scaleStep
                }
                else{//down
                    if(canvas.scaleCoeff > scaleStep){
                        canvas.scaleCoeff -= scaleStep
                    }
                }
            }

            function movingFunction(delta){
                let withinBorders_ = withinBorders(delta, canvas.mainRec_x, canvas.mainRec_y, canvas.mainRec_width, canvas.mainRec_height);

                if(insideMovingItem && withinBorders_){

                    canvas.mainRec_x_norm = (canvas.mainRec_x + delta.x)/canvas.width;
                    canvas.mainRec_y_norm = (canvas.mainRec_y + delta.y)/canvas.height;
                    canvas.requestPaint();
                }
            }

        }

        Canvas {

            id: canvas

            anchors.fill: parent

            antialiasing: true

            property color strokeStyle:  "#ff6600"
            property color fillStyle: "#ff6600"
            property int lineWidth: 2 * scaleCoeff;

            property real scaleCoeff: 1.;

            //variables
            property real mainRec_width: 250 * scaleCoeff;
            property real mainRec_height: 60 * scaleCoeff;

            //
            property real mainRec_x: canvas.width * mainRec_x_norm
            property real mainRec_y: canvas.height * mainRec_y_norm

            property real mainRec_x2:  canvas.width * mainRec_x2_norm
            property real mainRec_y2: canvas.height * mainRec_y2_norm

            property real mainRec_x_norm:  0.3
            property real mainRec_y_norm:  0.3

            property real mainRec_x2_norm:  0.6
            property real mainRec_y2_norm:  0.6
            //

            property int fontSize: 20 * scaleCoeff;
            property int fontSizeSmall: 14 * scaleCoeff;
            property int radius_: 2 * scaleCoeff;
            property int borderShift: 4 * scaleCoeff;

            property string imageUrl_1: "../../../" + Style.theme + "/Icons" + "/AppIcon.svg"
            property string imageUrl_2: "../../../" + Style.theme + "/Icons" + "/Sickle.svg"
            property real imageSize: 20 * scaleCoeff;
            property real imageMargin: 4 * scaleCoeff;

            property string fillColor: filled ? "#90EE90" : "#ffffff";
            property bool filled: false;
            //variables

            Component.onCompleted: {
                loadImage(imageUrl_1);
                loadImage(imageUrl_2);
            }

            onWidthChanged: {
                requestPaint()
            }

            onHeightChanged: {
                requestPaint()
            }

            onImageLoaded: {
                //console.log("Image loaded")
                requestPaint()
            }

            onScaleCoeffChanged: {
                requestPaint()
            }

            function drawObject(ctx, x_, y_, mainText, secondText){

                ctx.lineCap = "round"
                ctx.lineJoin = "round"

                //width calculation
                ctx.lineWidth = 1;
                let fontStr_main = String(fontSize) + "px sans-serif"
                ctx.font = fontStr_main; //"20px sans-serif";
                let textStr_main = mainText
                let textWidth_main = ctx.measureText(mainText).width

                ctx.lineWidth = 0.5;
                let fontStr_second = String(fontSizeSmall) + "px sans-serif"
                ctx.font = fontStr_second;
                let textStr_second = secondText;
                let textWidth_second = ctx.measureText(secondText).width

                let add = 2*imageSize + 2*imageMargin + 2*borderShift + 30;
                let mainRecWidth = Math.max(textWidth_main + add, textWidth_second + add, mainRec_width)
                //width calculation
                //TO DO: mainRecWidth to model --> checkInsideMovingItem

                //main rectangle
                ctx.lineWidth = canvas.lineWidth;
                ctx.strokeStyle = "#ff6600"
                ctx.fillStyle = canvas.fillColor;
                ctx.beginPath()
                ctx.roundedRect(x_, y_, mainRecWidth, mainRec_height, radius_, radius_);
                ctx.fill();
                ctx.stroke();

                //inner rectangle
                ctx.strokeStyle = "#808080"

                ctx.beginPath()
                ctx.roundedRect(x_ + borderShift, y_ + borderShift, mainRecWidth - 2 * borderShift, mainRec_height - 2 * borderShift, radius_, radius_);
                ctx.stroke();

                //Main text
                ctx.strokeStyle = "#000000"
                ctx.fillStyle = "#000000"
                ctx.lineWidth = 1;
                let fontStr = String(fontSize) + "px sans-serif"
                ctx.font = fontStr; //"20px sans-serif";
                let textStr = mainText
                let textWidth = ctx.measureText(mainText).width

                let text_x = x_ + borderShift + 8 * scaleCoeff;
                let text_y = y_ + borderShift + 22 * scaleCoeff;

                ctx.beginPath()
                ctx.fillText(textStr, text_x, text_y);
                ctx.strokeText(textStr, text_x, text_y);

                //Second text
                ctx.strokeStyle = "#808080"
                ctx.fillStyle = "#808080"
                ctx.lineWidth = 0.5;
                let fontStr2 = String(fontSizeSmall) + "px sans-serif"
                ctx.font = fontStr2;
                let textStr2 = secondText;
                let textWidth2 = ctx.measureText(secondText).width

                let text_x2 = x_ + borderShift + 8 * scaleCoeff;
                let text_y2 = y_ + mainRec_height - borderShift - 8 * scaleCoeff;

                ctx.beginPath()
                ctx.fillText(textStr2, text_x2, text_y2);
                ctx.strokeText(textStr2, text_x2, text_y2);

                //images
                let image1_x = x_ + mainRecWidth - borderShift - imageSize - imageMargin;
                let image1_y = text_y - imageSize + imageMargin;
                let image2_x = image1_x - imageSize - imageMargin;
                let image2_y = text_y - imageSize + imageMargin;
                ctx.beginPath()
                ctx.drawImage(imageUrl_1, image1_x, image1_y, imageSize, imageSize);
                ctx.drawImage(imageUrl_2, image2_x, image2_y, imageSize, imageSize);

            }

            function drawLink(ctx,x1, y1, x2, y2){
                ctx.lineCap = "round"
                ctx.lineJoin = "round"
                ctx.lineWidth = 2;
                ctx.strokeStyle = "#ff6600"
                ctx.fillStyle = "#ff6600";

                ctx.beginPath()
                ctx.moveTo(x1, y1);
                ctx.lineTo(x2, y2);
                ctx.stroke();
            }

            onPaint: {
                var ctx = canvas.getContext('2d');
                ctx.clearRect(0, 0, canvas.width, canvas.height)

                drawLink(ctx, mainRec_x2 + mainRec_width/2, mainRec_y2 + mainRec_height/2,
                         mainRec_x + mainRec_width/2, mainRec_y + mainRec_height/2)

                //drawObject(ctx, mainRec_x, mainRec_y, "Main text Main text Main text", "Second text");
                drawObject(ctx, mainRec_x, mainRec_y, "Main text", "Second text");
                drawObject(ctx, mainRec_x2 , mainRec_y2, "Main text 2", "Second text 2");

            }//onPaint

        }
    }

}
