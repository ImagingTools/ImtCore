import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ButtonDecorator {
    id: root;

    color: !root.baseElement ? "transparent" :
                               root.baseElement.highlighted ? Style.highlightColor :
                               root.baseElement.down || root.baseElement.checked ?
                                   Style.buttonPressedColor : root.baseElement.hovered ?
                                       Style.buttonHoverColor : Style.buttonColor

    maxTextWidth: root.baseElement ? root.baseElement.width - Style.sizeMainMargin : 100;
	backgroundItem.radius: 0
}


