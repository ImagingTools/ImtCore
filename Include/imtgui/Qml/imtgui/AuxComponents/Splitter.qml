import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: splitterContainer;

    color: Style.backgroundColor;

    property int type: 0x1;

    MouseArea {
        id: splitterMA;

        anchors.fill: parent;

        hoverEnabled: true;
        acceptedButtons: Qt.LeftButton;
        cursorShape: Qt.SplitHCursor;

        onMouseXChanged: {
            console.log("MouseX =", mouseX);
            if (splitterContainer.type !== 0x1){
                return;
            }

            if (splitterMA.pressed){
                splitterContainer.x += mouseX;
            }
        }
    }
}
