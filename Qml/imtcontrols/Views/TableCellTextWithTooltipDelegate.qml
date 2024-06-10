import QtQuick 2.12
import Acf 1.0

TableCellDelegateBase {
    id: delegateContainer

    TableCellText {
        id: tableCellText;

        anchors.left: parent.left
        anchors.leftMargin: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textLeftMargin : 0
        anchors.right: parent.right
        anchors.rightMargin: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textRightMargin : 0
        anchors.verticalCenter: parent.verticalCenter

        rowDelegate: delegateContainer.rowDelegate
        text: delegateContainer.getValue()
        columnIndex: delegateContainer.columnIndex
    }


    MouseArea{
        id: ma;

        anchors.fill: tableCellText;

        hoverEnabled: true;

        visible: helpText.width > (delegateContainer.width - 2*helpText.x);

        onExited: {
            tooltip.closeTooltip();
        }
        onPositionChanged: {
            if(tooltip.text !== "" && enabled && ma.visible){
                if(mouseX >= 0 && mouseX <= width && mouseY >= 0 && mouseY <= height){
                    //tooltip.show(mouseX, mouseY);
                    tooltip.show(width/2, mouseY);
                }
                else{
                    tooltip.closeTooltip();
                }
            }
        }
    }

    Text{
        id: helpText;

        x: tableCellText.x;

        font.family: tableCellText.font.family;
        font.pixelSize: tableCellText.font.pixelSize;
        font.bold: tableCellText.font.bold;

        text: tableCellText.text;

        visible: false;

    }

    CustomTooltip {
        id: tooltip;

        text: tableCellText.text;
        fitToTextWidth: false;
        componentMinHeight: 30;

        fitToHCenter: true;

        function hide(){
            closeTooltip();
        }

        function show(xX, yY){
            if (text !== ""){
                openTooltip(xX, yY);
            }
        }
    }
}
