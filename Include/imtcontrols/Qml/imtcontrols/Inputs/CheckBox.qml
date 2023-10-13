import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: checkBox;

    width: decorator ? decorator.width : 0
    height: decorator ? decorator.height : 0

    property Component decoratorComponent;
    property var decorator : null;

    property string text: "";
//    property alias checkSize: checkRect.height;
//    property alias borderWidth: checkRect.border.width;
//    property alias border: checkRect.border;
//    property alias color: checkRect.color;
//    property alias radius: checkRect.radius;
//    property alias imageSource: image.source;
//    property alias text: checkBoxText.text;
//    property alias textObj: checkBoxText;
//    property alias fontColor: checkBoxText.color;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    property string borderColor: "";
    property bool isActive: true;

    property int checkState: Qt.Unchecked;

    property string imageSourceActive: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
    property string imageSourceNotActive: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.Off, Icon.Mode.Disabled);


    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;
    property bool isLeftText: false;

    property int mainMargin: 8;


    signal clicked();

    onDecoratorComponentChanged: {
        if(!decoratorComponent){
            return;
        }
        if(decorator){
            decorator.destroy()
        }
        decorator = decoratorComponent.createObject(checkBox)
        decorator.baseElement = checkBox
        bindWidth.target = decorator
        bindHeight.target = decorator
    }

    Binding {
        id: bindWidth
        property: "width"
        value: checkBox.width;
    }

    Binding {
        id: bindHeight
        property: "height"
        value: checkBox.height;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;
        hoverEnabled: true;

        cursorShape: Qt.PointingHandCursor;

        visible: checkBox.isActive;

        onClicked: {
            checkBox.clicked();
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
