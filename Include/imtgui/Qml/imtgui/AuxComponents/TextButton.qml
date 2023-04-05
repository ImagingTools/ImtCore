import QtQuick 2.12
import Acf 1.0

Item {
    id: textButton;

    width: legendText.width;
    height: legendText.height + activeIndicator.height + activeIndicator.anchors.topMargin;
    visible: true;
    property bool active: false;
    property string legend;
    property int textWidth: legendText.width;
    property string fontFamily: Style.fontFamilyBold;
    property int fontPixelSize: Style.fontSize_subtitle;
    property bool fontBold: false;
    property alias indicatorHeight: activeIndicator.height;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;


    signal clicked();

    Text {
        id: legendText;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;

        width: contentWidth;
        height: contentHeight;

        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignTop;
        wrapMode: Text.NoWrap;
        font.pixelSize: textButton.fontPixelSize;
        font.bold: textButton.fontBold;
        font.family: textButton.fontFamily;
        color: textButton.active ? Style.color_text_titles : Style.color_text_notActive;

        text: textButton.legend;
    }

    Rectangle{
        id: activeIndicator;

        anchors.top: legendText.bottom;
        anchors.topMargin: 5;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: legendText.width;
        height: 3;

        radius: 5;
        color: Style.color_text_titles;
        visible: textButton.active;
    }

    MouseArea{
        id: ma;

        anchors.fill: parent;

        visible: textButton.visible;
        enabled: visible;
        hoverEnabled: enabled;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        onClicked: {
            textButton.clicked();
        }
        onPressed: {
            if(tooltip.text !== ""){
                tooltip.closeTooltip();
            }
        }
        onEntered: {
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                pauseTooltip.start();

            }

        }

        onExited: {
            if(tooltip.text !== ""){
                pauseTooltip.stop();
                tooltip.closeTooltip();
            }
        }

    }

    CustomTooltip{
        id: tooltip;
    }

    PauseAnimation {
        id: pauseTooltip;

        duration: tooltip.waitingDuration;
        onFinished: {
            tooltip.openTooltip(ma.mouseX, ma.mouseY);
        }
    }
}
