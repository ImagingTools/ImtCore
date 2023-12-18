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


   Component.onCompleted: {
       canvas.requestPaint();
   }

   onPercentChanged: {
       canvas.requestPaint();
   }

   Canvas {

       id: canvas

       anchors.centerIn: parent;

       width: parent.width - 2 * roundIndicator.lineWidth;
       height: width;

       antialiasing: true;

       onWidthChanged: {
           requestPaint()
       }

       onHeightChanged: {
           requestPaint()
       }

       onPaint: {
           var ctx = canvas.getContext('2d');
           ctx.clearRect(0, 0, canvas.width, canvas.height)

           drawProgress(ctx);

       }

       function drawProgress(ctx){
           ctx.lineJoin = "round"
           ctx.lineWidth = roundIndicator.lineWidth;

           ctx.strokeStyle = roundIndicator.mainColor;
           ctx.fillStyle = roundIndicator.mainColor;

           ctx.beginPath()
           ctx.arc(canvas.width/2, canvas.height/2, canvas.width/2 - roundIndicator.lineWidth, -Math.PI/2, (roundIndicator.percent/100) * 2 * Math.PI - Math.PI/2, false);
           ctx.stroke();
       }

       function draw(ctx, startAngle, finishAngle){
           ctx.lineJoin = "round"
           ctx.lineWidth = roundIndicator.lineWidth;

           ctx.strokeStyle = roundIndicator.mainColor;
           ctx.fillStyle = roundIndicator.mainColor;

           ctx.beginPath()
           ctx.arc(canvas.width/2, canvas.height/2, canvas.width/2 - roundIndicator.lineWidth, startAngle, finishAngle, false);
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
    
}

