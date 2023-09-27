import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0;

Item {
    id: container;

    width: container.text == "" ? checkRect.width : checkRect.width + checkBoxText.width + container.mainMargin;
    height: 13;

    property alias checkSize: checkRect.height;
    property alias borderWidth: checkRect.border.width;
    property string borderColor: "";
    property alias border: checkRect.border;
    property alias color: checkRect.color;
    property alias radius: checkRect.radius;

    property bool isActive: true;

    property int checkState: Qt.Unchecked;
    property alias imageSource: image.source;

    property string imageSourceActive: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
    property string imageSourceNotActive: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.Off, Icon.Mode.Disabled);

    property alias text: checkBoxText.text;
    property alias textObj: checkBoxText;

    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;
    property alias fontColor: checkBoxText.color;
    property bool isLeftText: false;

    property int mainMargin: 8;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    signal clicked();

    Rectangle {
        id: checkRect;


        anchors.left: parent.left;
        anchors.leftMargin: !container.isLeftText ? 0 : checkBoxText.width + container.mainMargin;
        anchors.verticalCenter: parent.verticalCenter;

        width: height;
        height: container.height;

        color: "transparent";

        border.width: 1;
        border.color: container.borderColor !=="" ? container.borderColor  : container.isActive ? Style.borderColor : Style.disabledInActiveTextColor;

        Image {
            id: image;

            anchors.centerIn: parent;

            height: checkRect.height - 1;
            width: height;

            sourceSize.width: width;
            sourceSize.height: height;

            visible: container.checkState != Qt.PartiallyChecked;
            source: container.isActive ? container.checkState == Qt.Checked ? container.imageSourceActive : "" :
                container.checkState == Qt.Checked ? container.imageSourceNotActive : "";
        }
    }

    Rectangle {
        id: rect;

        anchors.centerIn: checkRect;

        height: checkRect.height - 4;
        width: height;

        color: Style.textColor;

        visible: container.checkState == Qt.PartiallyChecked;
    }



    Text {
        id: checkBoxText;

        anchors.left: parent.left;
        anchors.leftMargin: container.isLeftText ? 0 : checkRect.width + container.mainMargin;
        anchors.verticalCenter: container.verticalCenter;

        color: container.enabled ? Style.buttonText : Style.inactive_buttonText;

        font.pixelSize: container.fontPixelSize;
        font.family: Style.fontFamily;
        font.bold: container.fontBold;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;
        hoverEnabled: true;

        cursorShape: Qt.PointingHandCursor;

        visible: container.isActive;

        onClicked: {
            container.clicked();
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
