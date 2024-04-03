import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: commonButtonDecorator

    width: Math.max(iconObj.width + textObj.width + Style.paddingSmall * 3, widthDefault)
    height: Style.buttonHeight;
    clip: true;

    property int widthDefault: 0;
    property alias icon: iconObj
    property alias font: textObj.font
    property alias textColor: textObj.color
    property alias color: background.color
    property alias radius: background.radius
    property alias border: background.border

    property bool contentCentered: true;

    property string tooltipText: baseElement ? baseElement.tooltipText : "";

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
        }
    }

    onMousePositionChanged: {
        if(tooltip.text !== ""){
            tooltip.show(mouseX, mouseY);
        }
    }

    onMouseExited: {
        if(tooltip.text !== ""){
            tooltip.hide();
        }
    }

    Rectangle {
        id: background

        anchors.fill: parent
        radius: Style.buttonRadius
        color: !commonButtonDecorator.baseElement ? "transtarent" :
                                                    commonButtonDecorator.baseElement.down || commonButtonDecorator.baseElement.checked ?
                                                        Style.buttonPressedColor : commonButtonDecorator.baseElement.hovered ?
                                                            Style.buttonHoverColor : Style.buttonColor
        border.width: Style.buttonBorderWidth
        border.color: !commonButtonDecorator.baseElement ? "transtarent" : commonButtonDecorator.baseElement.focus ? Style.borderFocusColor : Style.borderColor
    }

    Row {
        id: content;

        anchors.horizontalCenter: commonButtonDecorator.horizontalCenter;
        anchors.verticalCenter: commonButtonDecorator.verticalCenter

        height: Math.max(iconObj.height, textObj.height)

        spacing: Style.size_mainMargin;

        Image {
            id: iconObj

            width: source == "" ? 0 : Style.iconSizeSmall
            height: width

            sourceSize.width: width
            sourceSize.height: height
            source: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.iconSource
        }

        Text {
            id: textObj

            color: !commonButtonDecorator.baseElement ? "transtarent" : commonButtonDecorator.baseElement.enabled ? Style.textColor : Style.inactive_textColor

            font.pixelSize: Style.fontSize_common
            font.family: Style.fontFamily

            text: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.text
        }
    }

    CustomTooltip{
        id: tooltip;

        text: commonButtonDecorator.tooltipText;
        fitToTextWidth: false;
        componentMinHeight: 30;

        fitToHCenter: true;
    }
}


