import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: splitterContainer;

    color: Style.backgroundColor;

    property int type: Qt.Horizontal;

    MouseArea {
        id: splitterMA;

        anchors.fill: parent;

        hoverEnabled: true;
        acceptedButtons: Qt.LeftButton;
        cursorShape: Qt.SplitHCursor;

        onMouseXChanged: {
            console.log("MouseX =", mouseX);
            if (splitterContainer.type !== Qt.Horizontal){
                return;
            }

            if (splitterMA.pressed){
                splitterContainer.x += mouseX;
            }
        }

        onMouseYChanged: {
            console.log("MouseY =", mouseY);

            if (splitterContainer.type !== Qt.Vertical){
                return;
            }

            if (splitterMA.pressed){
                splitterContainer.y += mouseY;
            }
        }
    }

}
