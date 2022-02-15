import QtQuick 2.0
import Acf 1.0
import 'AuxComponents'
import imtauthgui 1.0
import imtlicgui 1.0
import imtqml 1.0;
import imtgui 1.0;

Rectangle
{
    id: redRect;

    width: 300;
    height: width;

    color: "red";

    MouseArea {
        anchors.fill: parent;

        onPressed: {
            console.log("RED onPressed", mouseX);
        }

        onMouseXChanged: {
            console.log("RED onMouseXChanged", mouseX);
        }

        onReleased: {
            console.log("RED onReleased", mouseX);
        }
    }

    Rectangle {
        id: greenRect;

        anchors.centerIn: parent;

        width: 200;
        height: width;

        color: "green";

        MouseArea {
            anchors.fill: parent;

            onPressed: {
                console.log("GREEN onPressed", mouseX);
            }

            onMouseXChanged: {
                console.log("GREEN onMouseXChanged", mouseX);
            }

            onReleased: {
                console.log("GREEN onReleased", mouseX);
            }
        }

        Rectangle {
            id: blueRect;

            anchors.centerIn: parent;

            width: 100;
            height: width;

            color: "blue";

            MouseArea {
                anchors.fill: parent;

                onPressed: {
                    console.log("BLUE pressed", mouseX);
                }

                onMouseXChanged: {
                    console.log("BLUE onMouseXChanged", mouseX);
                }

                onReleased: {
                    console.log("BLUE onReleased", mouseX);
                }
            }
        }
    }
}
