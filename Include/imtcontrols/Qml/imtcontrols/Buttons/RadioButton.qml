import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: customRadioButton;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.radioButtonDecorator: Decorators.radioButtonDecorator;

    property string text: "";

    property int checkState: checked ? Qt.Checked : Qt.Unchecked;
    property bool checked: false;

    property int checkSize: 20;
    property int fontPixelSize: Style.fontSize_common;
    property bool fontBold: false;

    property int mainMargin: 8;

    property string selectedColor: "#000000";

    property bool autoExclusive: true;

    property alias tooltipText: tooltip.text;
    property alias tooltipItem: tooltip;

    signal clicked();

    Component.onCompleted: {
        Events.subscribeEvent("RadioButtonUnchecked", customRadioButton.onRadioButtonUnchecked)

    }

    Component.onDestruction: {
        Events.unSubscribeEvent("RadioButtonUnchecked", customRadioButton.onRadioButtonUnchecked)
    }

    onDecoratorComponentChanged: {
        if(decorator.checked !==undefined){
            decorator.checked = checked;
        }
    }

    onClicked: {
        clickedFuntion();
    }

    function clickedFuntion(){
        if(!autoExclusive){
            checked = !checked;
        }
        else if(!checked){
            checked = true;
        }
        if(checked){
            let obj = {}
            obj["Self"] = this;
            obj["Parent"] = this.parent;
            Events.sendEvent("RadioButtonUnchecked", obj);
        }

    }

    function onRadioButtonUnchecked(object_){
        if(autoExclusive){
            var self_ = object_["Self"];
            var parent_ = object_["Parent"];
            if(customRadioButton !== self_ && customRadioButton.parent == parent_ && checked){
                checked = false;
            }
        }
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;
        hoverEnabled: true;

        cursorShape: Qt.PointingHandCursor;

        visible: customRadioButton.enabled;

        onClicked: {
            customRadioButton.clicked();
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
