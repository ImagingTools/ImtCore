import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
    id: root;

    property alias text: tp.text;
    property MouseArea mouseArea: null;
    property alias tooltip: tp;

    property bool containsMouse: mouseArea ? mouseArea.containsMouse : false;
    property alias interval: timer.interval;

    onContainsMouseChanged: {
        if (!containsMouse){
            tp.closeTooltip();
        }
        else{
            if (!tp.openST && tp.text !== "" ){
                tp.openTooltip(root.mouseArea.mouseX, -tp.componentHeight);
            }
        }
    }

    onMouseAreaChanged: {
        if (mouseArea){
            mouseArea.hoverEnabled = true;
        }
    }

    Timer {
        id: timer;
        interval: 3000;
        onTriggered: {
            tp.closeTooltip();
        }
    }

    CustomTooltip {
        id: tp;

        text: root.text;
		componentMinHeight: Style.controlHeightM;

        function openTooltip(xX, yY){
            var point = mapToItem(null, xX, yY);

            open(point.x, point.y)

            timer.restart();
        }
    }
}


