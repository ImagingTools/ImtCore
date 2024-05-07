import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: commonButtonDecorator

    width: Math.max(Style.iconSizeSmall + textItem.width + Style.size_mainMargin * 2, widthDefault)
    height: Style.buttonHeight;

    clip: true;

    property int maxTextWidth: 80;
    property int minTextWidth: 40;

    property int widthDefault: 0;
    property alias icon: iconObj
    property alias font: textObj.font
    property alias textColor: textObj.color
    property alias color: background.color
    property alias radius: background.radius
    property alias border: background.border

    property alias backgroundItem: background

    property bool contentCentered: true;
    property bool textIsCropped: textObj.text !== "" && helperText.width > textObj.width;

    property string tooltipText: baseElement && baseElement.tooltipText !== "" ? baseElement.tooltipText : (textIsCropped ? textObj.text : "");
    property bool enabled: baseElement ? baseElement.enabled : false;

    signal mouseEntered(real mouseX, real mouseY);
    signal mouseExited(real mouseX, real mouseY);
    signal mousePositionChanged(real mouseX, real mouseY);

    onContentCenteredChanged: {
        if (contentCentered){
            content.anchors.left = undefined;
            content.anchors.horizontalCenter = commonButtonDecorator.horizontalCenter;
        }
        else{
            content.anchors.horizontalCenter = undefined;
            content.anchors.left = commonButtonDecorator.left;
            content.anchors.leftMargin = Style.size_mainMargin;
        }
    }

    onBaseElementChanged: {
        if(baseElement){
            baseElement.exited.connect(commonButtonDecorator.mouseExited);
            baseElement.entered.connect(commonButtonDecorator.mouseEntered);
            baseElement.positionChanged.connect(commonButtonDecorator.mousePositionChanged);
            baseElement.closeTooltip.connect(commonButtonDecorator.closeTooltip);
        }
    }

    onMousePositionChanged: {
        if(tooltip.text !== "" && enabled){
            if(mouseX >= 0 && mouseX <= baseElement.width && mouseY >= 0 && mouseY <= baseElement.height){
                tooltip.show(mouseX, mouseY);
            }
            else{
                closeTooltip();
            }
        }
    }

    onMouseExited: {
        closeTooltip();
    }

    onEnabledChanged: {
        if (!enabled){
            closeTooltip()
        }
    }

    Rectangle {
        id: background

        anchors.fill: parent
        radius: Style.buttonRadius
        color: !commonButtonDecorator.baseElement ? "transparent" :
                                                    commonButtonDecorator.baseElement.down || commonButtonDecorator.baseElement.checked ?
                                                    Style.buttonPressedColor : commonButtonDecorator.baseElement.hovered ?
                                                    Style.buttonHoverColor : Style.buttonColor
        border.width: Style.buttonBorderWidth
        border.color: !commonButtonDecorator.baseElement ? "transparent" :
                                                    commonButtonDecorator.baseElement.focus ? Style.buttonBorderFocusColor :
                                                    Style.borderColor
    }

    Row {
        id: content;

        anchors.horizontalCenter: commonButtonDecorator.horizontalCenter;
        anchors.verticalCenter: commonButtonDecorator.verticalCenter

        height: Math.max(iconObj.height, textItem.height)

        spacing: Style.size_mainMargin;

        Image {
            id: iconObj

            width: source == "" ? 0 : Style.iconSizeSmall
            height: width

            sourceSize.width: width
            sourceSize.height: height
            source: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.iconSource
        }

        Item {
            id: textItem;

            width: helperText.width > commonButtonDecorator.maxTextWidth ? commonButtonDecorator.maxTextWidth : helperText.width;
            height: textObj.height;

            visible: textObj.text !== "";

            Text {
                id: textObj

                width: parent.width;

                color: !commonButtonDecorator.baseElement ? "transparent" : commonButtonDecorator.baseElement.enabled ? Style.textColor : Style.inactive_textColor

                font.pixelSize: Style.fontSize_common
                font.family: Style.fontFamily

                text: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.text
                elide: Text.ElideRight;
            }

            Text {
                id: helperText;

                font.pixelSize: Style.fontSize_common
                font.family: Style.fontFamily

                text: textObj.text;

                visible: false;
            }
        }
    }

    function closeTooltip(){
        if(tooltip && tooltip.openST){
            tooltip.closeTooltip();
        }
    }

    CustomTooltip {
        id: tooltip;

        text: commonButtonDecorator.tooltipText;
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


