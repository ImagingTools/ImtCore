import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


ControlBase {
    id: customTooltip;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.tooltipDecorator: Decorators.tooltipDecorator;

    property string text: "";

    property int componentWidth: 200;
    property int tooltipWidth: customTooltip.fitToTextWidth ? forWidthText.width + 2*customTooltip.textMargin : customTooltip.componentWidth;

    property int componentMinHeight: 40;
    property int componentMargin: 12;

    property bool fitToTextWidth: false;
    property bool fitToHCenter: false;

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
    property int waitingDuration: delay;
    property int delay: 500;
    property int timeout: -1;

    property real lineHeight: 1;

    property Component tooltipContentComp: hint;



    Component.onDestruction: {
        customTooltip.closeTooltip();
    }

    function openTooltipWithCoord(xX, yY){
        if(!customTooltip.openST){

            var point = mapToItem(null, xX, yY);

            modalDialogManager.openDialog(customTooltip.tooltipContentComp, {"x": point.x, "y": point.y});

            modalDialogManager.backgroundItem.visible = false;

            customTooltip.openST = true;
        }

    }

    function openTooltip(xX, yY){
        if(!customTooltip.openST){
            console.log("openTooltip", xX, yY);

            var point = mapToItem(null, xX, yY);
            var centeredAdd = customTooltip.fitToTextWidth ? customTooltip.fitToHCenter * (forWidthText.width/2 + customTooltip.textMargin + customTooltip.componentMargin) :
                                                             customTooltip.fitToHCenter * (customTooltip.componentWidth/2 + customTooltip.componentMargin);
            if(point.x > modalDialogManager.width*2/3){
                if(customTooltip.fitToTextWidth){
                    point.x = point.x - forWidthText.width - 2*customTooltip.textMargin - customTooltip.componentMargin + centeredAdd;
                }
                else{
                    point.x = point.x - customTooltip.componentWidth - customTooltip.componentMargin + centeredAdd;
                }
            }
            else{
                point.x = point.x + customTooltip.componentMargin - centeredAdd;
            }

            if(point.y > modalDialogManager.height*2/3){
                point.y = point.y - customTooltip.componentHeight - customTooltip.componentMargin;
            }
            else{
                point.y = point.y + 2*customTooltip.componentMargin;
            }

            modalDialogManager.openDialog(customTooltip.tooltipContentComp, {"x": point.x, "y": point.y});

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

       // width: customTooltip.componentWidth - 2*customTooltip.textMargin;

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
            opacity: 0;
            border.color: customTooltip.borderColor;
            border.width: customTooltip.borderWidth;
            radius: customTooltip.radius;

            property Item root: null;

            Component.onCompleted: {
               opacityAnimTo.start();
            }

            function closeAnimFunc(){
                opacityAnimFrom.start();
            }

            NumberAnimation {
                id: opacityAnimTo;

                target: compRec;
                property: "opacity";
                duration: 200
                from: 0; to: 1;
            }

            NumberAnimation {
                id: opacityAnimFrom;

                target: compRec;
                property: "opacity";
                duration: 200
                from: 1; to: 0;
            }

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

                wrapMode: customTooltip.fitToTextWidth ? Text.NoWrap : Text.WrapAtWordBoundaryOrAnywhere;

                text: qsTr(customTooltip.text);

                lineHeight: customTooltip.lineHeight;
            }
        }
    }

    PauseAnimation {
        id: pauseOpenTooltip;

        duration: customTooltip.delay;
        property real x: 0;
        property real y: 0;
        onFinished: {
            if(customTooltip.decorator && customTooltip.decorator.show !== undefined){
                customTooltip.decorator.show(customTooltip.text, customTooltip.timeout);
            }
            else {
                customTooltip.openTooltip(pauseOpenTooltip.x, pauseOpenTooltip.y);
            }
        }
    }

    PauseAnimation {
        id: pauseCloseTooltip;

        duration: customTooltip.timeout > 0 ? customTooltip.timeout : 1;
        onFinished: {
            customTooltip.closeTooltip();
        }
    }


    PauseAnimation {
        id: closeAnim;

        duration: 200;
        onFinished: {
            closeTooltip();
        }
    }

    function show(xX, yY){
        pauseOpenTooltip.x = xX;
        pauseOpenTooltip.y = yY;
        pauseCloseTooltip.stop();
        if(customTooltip.timeout > 0){
            pauseCloseTooltip.start();
        }
        pauseOpenTooltip.restart();
    }

    function hide(){
        pauseOpenTooltip.stop();
        pauseCloseTooltip.stop();

        if(customTooltip.decorator && customTooltip.decorator.hide !== undefined){
            customTooltip.decorator.hide();
        }
        else {
            let item = modalDialogManager.topItem;
            if(item && item.closeAnimFunc !== undefined){
                item.closeAnimFunc()
                closeAnim.start();
            }
            else{
                closeTooltip();
            }
        }
    }

}
