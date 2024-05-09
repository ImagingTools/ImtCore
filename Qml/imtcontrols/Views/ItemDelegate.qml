import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Button {
    id: root;

    width: parent ? parent.width : 0;
    height: 30;

    decorator: Style.itemDelegateDecorator;

    property bool highlighted: false;
}
