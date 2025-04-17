import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;
import imtcontrols 1.0

Item {
    id: barControl;

    property int orientation: Qt.Horizontal; //Qt.Vertical

    property string backgroundColor: "lightgray";
    property string controlColor: "lightgreen";

    property alias radius: controlRec.radius;

    property real value: 0.0;


    Rectangle {
        id: controlRec;

        anchors.fill: parent;
        radius: Math.min(height/2, width/2);

        gradient: barControl.orientation == Qt.Horizontal ? horizGrad : vertGrad;

        width: barControl.orientation == Qt.Horizontal ? barControl.value * barControl.width : barControl.width;
        height: barControl.orientation == Qt.Vertical ? barControl.value * barControl.height : barControl.height;

        Gradient{
            id : horizGrad;

            orientation: Gradient.Horizontal;

            GradientStop { position: 0.0; color: barControl.controlColor }
            GradientStop { position: barControl.value - 0.0001; color: barControl.controlColor }
            GradientStop { position: barControl.value; color: barControl.controlColor }
            GradientStop { position: barControl.value + 0.0001; color: barControl.backgroundColor }
            GradientStop { position: 1.0; color: barControl.backgroundColor}
        }

        Gradient{
            id : vertGrad;

            orientation: Gradient.Vertical;

            GradientStop { position: 0.0; color: barControl.backgroundColor }
            GradientStop { position: 1 - barControl.value - 0.0001; color: barControl.backgroundColor }
            GradientStop { position: 1 - barControl.value; color: barControl.backgroundColor }
            GradientStop { position: 1 - barControl.value + 0.0001; color: barControl.controlColor }
            GradientStop { position: 1.0; color: barControl.controlColor}
        }

    }


    MouseArea{
        id: ma;

        anchors.fill: parent;

        hoverEnabled: true;
        cursorShape: Qt.PointingHandCursor;
        property var startPoint: Qt.point(0,0);
        onPressed: {
            ma.startPoint = Qt.point(mouse.x, mouse.y);
        }
        onPositionChanged: {
            if(!pressed){
                return;
            }
            let val = -1;
            if(barControl.orientation == Qt.Horizontal){
                if(mouse.x >= 0 && mouse.x <= barControl.width && barControl.width !== 0){
                    //val = mouse.x / barControl.width;
                    val = barControl.value - (ma.startPoint.x - mouse.x) / barControl.width;

                }
            }
            else if(barControl.orientation == Qt.Vertical){
                if(mouse.y >= 0 && mouse.y <= barControl.height && barControl.height !== 0){
                    //val = (barControl.height - mouse.y) / barControl.height;
                    val = barControl.value + (ma.startPoint.y - mouse.y) / barControl.height;
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
            ma.startPoint = Qt.point(mouse.x, mouse.y);
        }

    }

}
