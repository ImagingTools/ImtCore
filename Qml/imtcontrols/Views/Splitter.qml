import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

Rectangle {
    id: splitterContainer;

    color: Style.backgroundColor;

    property int type: 0x1;
    property bool isPressed: false;
    property int leftLimit: 0;
    property int rightLimit: 1000000;

    MouseArea {
        id: splitterMA;

        anchors.fill: parent;

        hoverEnabled: true;
        acceptedButtons: Qt.LeftButton;
        cursorShape: Qt.SplitHCursor;

        onPressed: {
            splitterContainer.isPressed = true;
        }
        onReleased: {
            splitterContainer.isPressed = false;
        }

        onMouseXChanged: {
            if (splitterContainer.type !== 0x1){
                return;
            }

            if (splitterContainer.isPressed){
                let newX = splitterContainer.x + mouseX;
                if((newX > splitterContainer.x && newX < splitterContainer.rightLimit)||
                        (newX < splitterContainer.x && newX > splitterContainer.leftLimit)){

                    splitterContainer.x += mouseX;
                }
            }
        }
    }
}
