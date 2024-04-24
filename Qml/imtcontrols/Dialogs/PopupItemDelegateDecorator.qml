import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtcontrols 1.0

ItemDelegateDecorator {
    id: root;

    contentCentered: false;

    color: !baseElement ? "transparent" :
                              baseElement.highlighted ? Style.selectedColor :
                              baseElement.down || baseElement.checked ?
                                   Style.buttonPressedColor : root.selected ?
                                       Style.buttonHoverColor : "transparent";

    border.width: 0;

    property bool isSeparator: !root.baseElement ? false : root.baseElement.isSeparator;
    property bool selected: !root.baseElement ? false : root.baseElement.selected;

    onSelectedChanged: {
        console.log("Decorator onSelectedChanged", selected);
    }

    Rectangle {
        id: separator;

        anchors.left: root.left;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: root.right;
        anchors.rightMargin: Style.size_mainMargin;
        anchors.verticalCenter: root.verticalCenter;

        height: 1;

        color: Style.textColor;

        visible: root.isSeparator;
    }
}
