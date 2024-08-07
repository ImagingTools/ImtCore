import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


ControlBase {
    id: customTooltip;

    decorator: Style.tooltipDecorator

    property string text: "";

    property int componentWidth: 200;
    property int tooltipWidth: customTooltip.fitToTextWidth ? forWidthText.width + 2.5*customTooltip.textMargin : Math.min(customTooltip.componentWidth, forWidthText.width + 2.5*customTooltip.textMargin);

    property int componentMinHeight: 40;
    property int componentMargin: Style.size_mainMargin;

    property bool fitToTextWidth: false;
    property bool fitToHCenter: false;

    property int componentHeight: Math.max(customTooltip.componentMinHeight, forHeightText.height + 2*customTooltip.textMargin);


    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;
    property string fontColor: Style.textColor;
    property int textMargin: Style.size_smallMargin;
    property int horizontalAlignment: Text.AlignHCenter;

    property int radius: 4;
    property string color: Style.baseColor;
    property int borderWidth: 1;
    property string borderColor: Style.borderColor;

    property bool openST: false;
    property int waitingDuration: delay;
    property int delay: 500;
    property int timeout: -1;

    // Index this dialog from ModalDialogManager
    property int dialogIndex: -1;

    property real lineHeight: 1;

    property Component tooltipContentComp: hint;

    Component.onDestruction: {
        customTooltip.closeTooltip();
    }

    function openTooltipWithCoord(xX, yY){
        if(!customTooltip.openST){

            var point = mapToItem(null, xX, yY);

            open(xX, yY);
        }
    }

    function open(xX, yY){
        customTooltip.dialogIndex = ModalDialogManager.count;

        ModalDialogManager.openDialog(customTooltip.tooltipContentComp, {"x": xX, "y": yY});
        if (ModalDialogManager.backgroundItem){
            ModalDialogManager.backgroundItem.visible = false;
        }

        customTooltip.openST = true;
    }

    function openTooltip(xX, yY){
        if(!customTooltip.openST){
            if (!ModalDialogManager.activeView){
                return;
            }

            var point = mapToItem(null, xX, yY);
            var centeredAdd = customTooltip.fitToTextWidth ? customTooltip.fitToHCenter * (forWidthText.width/2 + customTooltip.textMargin + customTooltip.componentMargin) :
                                                             customTooltip.fitToHCenter * (customTooltip.tooltipWidth/2 + customTooltip.componentMargin);
            if(point.x > ModalDialogManager.activeView.width*2/3){
                if(customTooltip.fitToTextWidth){
                    point.x = point.x - forWidthText.width - 2*customTooltip.textMargin - customTooltip.componentMargin + centeredAdd;
                }
                else{
                    point.x = point.x - customTooltip.tooltipWidth - customTooltip.componentMargin + centeredAdd;
                }
            }
            else{
                point.x = point.x + customTooltip.componentMargin - centeredAdd;
            }

            if(point.y > ModalDialogManager.activeView.height*2/3){
                point.y = point.y - customTooltip.componentHeight - customTooltip.componentMargin;
            }
            else{
                point.y = point.y + 2*customTooltip.componentMargin;
            }

            open(point.x, point.y);
        }
    }

    function closeTooltip(){
        if(customTooltip.openST){
            ModalDialogManager.closeByComp(customTooltip.tooltipContentComp);

            customTooltip.openST = false;
        }
    }

    Text{
        id: forHeightText;

        visible: false;

        width: customTooltip.tooltipWidth - 2*customTooltip.textMargin;

        font.family: Style.fontFamily;
        font.pixelSize:  customTooltip.fontPixelSize;
        font.bold: customTooltip.fontBold;
        color: customTooltip.fontColor;

        horizontalAlignment: customTooltip.horizontalAlignment;
        verticalAlignment: Text.AlignVCenter;

        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;

        text: customTooltip.text;
    }

    Text{
        id: forWidthText;

        visible: false;

        font.family: Style.fontFamily;
        font.pixelSize:  customTooltip.fontPixelSize;
        font.bold: customTooltip.fontBold;
        color: customTooltip.fontColor;

        text: customTooltip.text;
    }

    Component{
        id: hint;

        Rectangle{
            id: compRec;

            width: customTooltip.tooltipWidth;
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

                wrapMode: customTooltip.fitToTextWidth ? Text.NoWrap : Text.WordWrap;

                text: customTooltip.text;

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
            if(customTooltip.decorator_ && customTooltip.decorator_.show !== undefined){
                customTooltip.decorator_.show(customTooltip.text, customTooltip.timeout);
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
            customTooltip.hide();
        }
    }


    PauseAnimation {
        id: closeAnim;

        duration: 200;
        onFinished: {
            customTooltip.closeTooltip();
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

        if(customTooltip.decorator_ && customTooltip.decorator_.hide !== undefined){
            customTooltip.decorator_.hide();
        }
        else {
            let item = ModalDialogManager.topItem;
            if(item && item.closeAnimFunc !== undefined){
                item.closeAnimFunc()
                closeAnim.start();
            }
            else{
                customTooltip.closeTooltip();
            }
        }
    }

}
