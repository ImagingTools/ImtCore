import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: root;

    color: Style.backgroundColor;

    function start(){
        console.log("Loading start");

        root.stop();

        root.visible = true;
    }

    function stop(){
        console.log("Loading stop");
        root.visible = false;
    }

    Item { // square
        id: square;

        anchors.centerIn: parent
        property double minimum: Math.min(root.width, root.height);
        width: 50;
        height: 50;

        Repeater {
            id: repeater;

            model: 8;

            delegate: Rectangle{
                color: Style.textColor;

                property double b: 0.1;
                property double a: 0.25;

                width: ((b - a) / repeater.count * index + a) * square.height;  height: width;
                radius: 0.5 * height;

                x: 0.5 * square.width  + 0.5 * (square.width  - width )  * Math.cos(2 * Math.PI / repeater.count * model.index) - 0.5 * width;
                y: 0.5 * square.height - 0.5 * (square.height - height)  * Math.sin(2 * Math.PI / repeater.count * model.index) - 0.5 * height;
            }
        }
    }

    Timer {
        interval: 10;
        running: root.visible;
        repeat:  true;

        onTriggered: {
            square.rotation += 2; // degrees
        }
    }
}
