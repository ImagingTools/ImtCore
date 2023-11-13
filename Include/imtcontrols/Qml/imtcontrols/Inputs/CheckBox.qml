import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: checkBox;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.checkBoxDecorator: Decorators.checkBoxDecorator;

    property string text: "";

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

        onPositionChanged: {
            if(tooltip.text !== ""){
                tooltip.show(mouseX, mouseY);
            }
        }

        onExited: {
            if(tooltip.text !== ""){
                tooltip.hide();
            }
        }
    }

    CustomTooltip{
        id: tooltip;

        fitToTextWidth: true;
    }


}
