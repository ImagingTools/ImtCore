import QtQuick 2.12
import Acf 1.0

ButtonDecorator {
    property bool isEmpty: baseElement.text === "";
    width: isEmpty ? 50 : contentWidth + 2 * Style.sizeMainMargin;
    maxTextWidth: 200;
    color: baseElement && baseElement.mouseArea.containsMouse ? Style.baseColor : Style.backgroundColor2;
    icon.width: icon.visible ? 20 : 0;
    border.width: baseElement && baseElement.mouseArea.containsMouse ? 1 : 0;
    contentSpacing: Style.sizeSmallMargin;
}


