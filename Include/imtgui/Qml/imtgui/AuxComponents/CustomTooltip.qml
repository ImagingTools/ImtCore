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

    property bool openST: false;
    property int waitingDuration: 500;


    function openTooltip(xX, yY){
        if(!customTooltip.openST){
            console.log("openTooltip", xX, yY);

            //var point = mapToItem(thumbnailDecoratorContainer, xX, yY);
            var point = mapToItem(null, xX, yY);
            if(point.x > modalDialogManager.width*2/3){
                if(customTooltip.fitToTextWidth){
                    point.x = point.x - forWidthText.width - 2*customTooltip.textMargin - customTooltip.componentMargin;

                }
                else{
                    point.x = point.x - customTooltip.componentWidth - customTooltip.componentMargin;
                }
            }
            else{
                point.x = point.x + customTooltip.componentMargin;
            }

            if(point.y > modalDialogManager.height*2/3){
                point.y = point.y - customTooltip.componentHeight - customTooltip.componentMargin;
            }
            else{
                point.y = point.y + customTooltip.componentMargin;
            }

            modalDialogManager.openDialog(hint, {"x": point.x, "y": point.y});

            modalDialogManager.backgroundItem.visible = false;

            customTooltip.openST = true;
        }
    }

    function closeTooltip(){
        if(customTooltip.openST){
            console.log("closeTooltip");

            modalDialogManager.closeDialog();

            customTooltip.openST = false;
        }

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

                horizontalAlignment: customTooltip.fitToTextWidth ? Text.AlignLeft: customTooltip.horizontalAlignment;
                verticalAlignment: Text.AlignVCenter;

                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;


                text: qsTr(customTooltip.text);

            }
        }
    }

}
