import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ButtonDecorator{
    id: root;

    color: baseElement && baseElement.hovered  ? Style.buttonHoverColor : "transparent";
    border.width: 0;

    Component.onCompleted: {
        icon.width = width;
    }
}

