import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root;

    property alias text: tooltip.text;
    property MouseArea mouseArea: null;

    Connections {
        target: root.mouseArea;

        function onContainsMouseChanged(){
            if (root.mouseArea.containsMouse && root.text !== ""){
                tooltip.openTooltip(root.mouseArea.mouseX, root.mouseArea.mouseY);
            }
            else{
                tooltip.closeTooltip();
            }
        }
    }

    CustomTooltip {
        id: tooltip;

        text: root.text;
        componentMinHeight: 30;

        function openTooltip(xX, yY){
            var point = mapToItem(null, xX, yY);

            modalDialogManager.openDialog(tooltip.tooltipContentComp, {"x": point.x, "y": point.y});
            modalDialogManager.backgroundItem.visible = false;
            tooltip.openST = true;
        }
    }
}


