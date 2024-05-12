import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root;

    property alias text: tp.text;
    property MouseArea mouseArea: null;
    property alias tooltip: tp;

    property bool containsMouse: mouseArea ? mouseArea.containsMouse : false;

    onContainsMouseChanged: {
        console.log("onContainsMouseChanged", containsMouse, text);

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

    CustomTooltip {
        id: tp;

        text: root.text;
        componentMinHeight: 30;

        function openTooltip(xX, yY){
            var point = mapToItem(null, xX, yY);

            open(point.x, point.y)
        }
    }
}


