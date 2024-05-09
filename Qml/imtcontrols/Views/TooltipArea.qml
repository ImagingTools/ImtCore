import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root;

    property alias text: tp.text;
    property MouseArea mouseArea: null;
    property alias tooltip: tp;

    onMouseAreaChanged: {
        if (mouseArea){
            mouseArea.hoverEnabled = true;
        }
    }

    Connections {
        id: conn;

        target: root.mouseArea;
        enabled: root.mouseArea;

        function onPositionChanged(){
            if(tp.text !== "" && root.mouseArea.enabled){
                if(root.mouseArea.containsMouse){
                    if (!tp.openST){
                        tp.openTooltip(root.mouseArea.mouseX, root.mouseArea.height);
                    }
                }
            }
        }

        function onContainsMouseChanged(){
            if (tp.openST && !root.mouseArea.containsMouse){
                tp.closeTooltip();
            }
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


