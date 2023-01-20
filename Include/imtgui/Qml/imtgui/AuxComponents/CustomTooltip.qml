import QtQuick 2.12
import Acf 1.0


Item {
    id: customTooltip;

    property string text: "";

    property int componentWidth: 200;
    property int componentMinHeight: 40;
    property int componentMargin: 12;

    property bool fitToTextWidth: false;

    property int componentHeight: Math.max(customTooltip.componentMinHeight, forHeightText.height + 2*customTooltip.textMargin);


    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;
    property string fontColor: Style.textColor;
    property int textMargin: 8;
    property int horizontalAlignment: Text.AlignHCenter;

    property int radius: 4;
    property string color: "#ffffff";
    property int borderWidth: 1;
    property string borderColor: "lightgray";

    function openTooltip(xX, yY){
        console.log("openTooltip", xX, yY);

        var point = mapToItem(thumbnailDecoratorContainer, xX, yY);
        if(point.x > thumbnailDecoratorContainer.width*2/3){
            point.x = point.x - customTooltip.componentWidth - customTooltip.componentMargin;
        }
        else{
            point.x = point.x + customTooltip.componentMargin;
        }

        if(point.y > thumbnailDecoratorContainer.height*2/3){
            point.y = point.y - customTooltip.componentHeight - customTooltip.componentMargin;
        }
        else{
            point.y = point.y + customTooltip.componentMargin;
        }

        modalDialogManager.openDialog(hint, {"x": point.x, "y": point.y});

        if(modalDialogManager.count == 1){
            modalDialogManager.backgroundItem.opacity = 0;
            modalDialogManager.backgroundItem.backgroundAreaItem.visible = false;
        }
    }

    function closeTooltip(){

        console.log("closeTooltip");
        if(modalDialogManager.count == 1){
            modalDialogManager.backgroundItem.opacity = 1;
            modalDialogManager.backgroundItem.backgroundAreaItem.visible = true;
        }
        modalDialogManager.closeDialog();

    }

    Text{
        id: forHeightText;

        visible: false;

        width: customTooltip.componentWidth - 2*customTooltip.textMargin;

        font.family: Style.fontFamily;
        font.pixelSize:  customTooltip.fontPixelSize;
        font.bold: customTooltip.fontBold;
        color: customTooltip.fontColor;

        horizontalAlignment: customTooltip.horizontalAlignment;
        verticalAlignment: Text.AlignVCenter;

        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;

        text: qsTr(customTooltip.text);

    }

    Text{
        id: forWidthText;

        visible: false;

        font.family: Style.fontFamily;
        font.pixelSize:  customTooltip.fontPixelSize;
        font.bold: customTooltip.fontBold;
        color: customTooltip.fontColor;

        text: qsTr(customTooltip.text);

    }

    Component{
        id: hint;

        Rectangle{
            id: compRec;

            width: customTooltip.fitToTextWidth ? forWidthText.width + 2*customTooltip.textMargin : customTooltip.componentWidth;
            height: customTooltip.componentHeight;

            color: customTooltip.color;
            border.color: customTooltip.borderColor;
            border.width: customTooltip.borderWidth;
            radius: customTooltip.radius;

            property Item root;

            Text{
                id: compText;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: customTooltip.textMargin;

                width: parent.width - 2*customTooltip.textMargin;

                font.family: Style.fontFamily;
                font.pixelSize:  customTooltip.fontPixelSize;
                font.bold: customTooltip.fontBold;
                color: customTooltip.fontColor;

                horizontalAlignment: customTooltip.horizontalAlignment;
                verticalAlignment: Text.AlignVCenter;

                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;


                text: qsTr(customTooltip.text);

            }
        }
    }

    //////////////////example///////////////////////////////////

//    Rectangle{
//        id: rec;
//        x: 100;
//        y: 700;
//        width: 100;
//        height: width;
//        color:"red";

//        CustomTooltip{
//            id: tooltip;
//            text: "tooltip tooltip tooltip tooltip tooltip tooltip";
//            fitToTextWidth: false;
//            horizontalAlignment: Text.AlignHCenter;
//        }

//        MouseArea{
//            anchors.fill: parent;
//            cursorShape: Qt.PointingHandCursor;
//            hoverEnabled: true;
//            onEntered: {
//                tooltip.openTooltip(mouseX, mouseY);
//            }
//            onExited: {
//                tooltip.closeTooltip();
//            }
//        }
//    }


}
