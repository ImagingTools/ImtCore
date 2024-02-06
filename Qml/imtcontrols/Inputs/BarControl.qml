import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

Item {
    id: barControl;

    property int orientation: Qt.Horizontal; //Qt.Vertical

    property string backgroundColor: "lightgray";
    property string controlColor: "lightgreen";
    property string cornerColor: Style.backgroundColor;

    property real value: 0.0;

    Rectangle{
        id: background;

        anchors.fill: parent;

        color: barControl.backgroundColor;
        radius: Math.min(height/2, width/2);

    }

    Rectangle {
        id: controlRec;

        anchors.left: parent.left;
        anchors.bottom: parent.bottom;

        color: barControl.controlColor;

        width: barControl.orientation == Qt.Horizontal ? barControl.value * barControl.width : barControl.width;
        height: barControl.orientation == Qt.Vertical ? barControl.value * barControl.height : barControl.height;


    }

    PaintedCorner{
        id: leftBottom;

        anchors.left: parent.left;
        anchors.bottom: parent.bottom;

        width: Math.min(parent.width/2, parent.height/2);
        height: width;

        mainColor: barControl.cornerColor
        lineColor: mainColor;
        r: background.radius;
        rotation: 0;

    }

    PaintedCorner{
        id: leftTop;

        anchors.left: parent.left;
        anchors.top: parent.top;

        width: Math.min(parent.width/2, parent.height/2);
        height: width;

        mainColor: barControl.cornerColor
        lineColor: mainColor;
        r: background.radius;
        rotation: 90;
    }

    PaintedCorner{
        id: rightTop;

        anchors.right: parent.right;
        anchors.top: parent.top;

        width: Math.min(parent.width/2, parent.height/2);
        height: width;

        mainColor: barControl.cornerColor
        lineColor: mainColor;
        r: background.radius;
        rotation: 180;

    }


    PaintedCorner{
        id: rightBottom;

        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        width: Math.min(parent.width/2, parent.height/2);
        height: width;

        mainColor: barControl.cornerColor
        lineColor: mainColor;
        r: background.radius;
        rotation: 270;
    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        hoverEnabled: true;
        cursorShape: Qt.PointingHandCursor;
        onPositionChanged: {
            if(!pressed){
                return;
            }
            let val = -1;
            if(barControl.orientation == Qt.Horizontal){
                if(mouse.x >= 0 && mouse.x <= barControl.width){
                    val = mouse.x / barControl.width;
                }
            }
            else if(barControl.orientation == Qt.Vertical){
                if(mouse.y >= 0 && mouse.y <= barControl.height){
                    val = (barControl.height - mouse.y) / barControl.height;
                }
            }

            if(val <= 0.01 && val >= 0){
                val = 0;
            }
            if(val >= 0.99){
                val = 1;
            }

            if(barControl.value !== val && val >= 0){
                barControl.value = val
                //console.log(barControl.value)
            }
        }

    }

}
